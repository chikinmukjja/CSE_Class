import java.net.*;
import java.nio.ByteBuffer;
import java.util.zip.*;
import java.io.*;
import java.util.Timer;
import java.util.TimerTask;



public class UDPMyEcho {

		static boolean flag;    // timeout의 발생여부를 알기 위한 flag
		
		public static void main(String[] args) {
		// TODO Auto-generated method stub
		if(args.length != 2)
		{
			System.out.println("사용법 : java UDPMyEcho localhost port");   // 인자로 서버의 ip와 port륿 받는다
			System.exit(0);                                               // 인자가 부족할 경우 예외처리
		}
		Timeout tclick = new Timeout();
		Signaling signal = new Signaling();
		int port = Integer.parseInt(args[1]);
		try
		{
			InetAddress inetaddr = InetAddress.getByName(args[0]);
			DatagramSocket socket =  new DatagramSocket();
			Window send = new Window(0,1,3); // 시작 seq, type, windowsize
			Window recv = new Window(0,1,3);
		
			
			
			Thread r1 = new receiveFrameSelc(socket,send,recv,signal,tclick);
			Thread s1 = new sendDataSelc(socket,port,inetaddr,send,signal,tclick);
			
			r1.start();
			s1.start();
			
				
		}catch(UnknownHostException ex){
				System.out.println("Error in the host address");
		}
		catch(IOException e){
				System.out.println(e);
		}
	}



}

class Window
{
	int cur_seq;
	int seq_first;
	int seq_last;
	int typeARQ;
	int windowSize;
	//boolean notify;
	
	boolean[] ackBuffer;
	String[] dataBuffer;
	boolean onlyR;
	
	int count;
	
	Window(int _cur_seq,int _typeARQ,int _windowSize)
	{
		typeARQ = _typeARQ;
		cur_seq = _cur_seq;
		windowSize = _windowSize;
		
		seq_first = cur_seq;
		
		switch(typeARQ){
		
			case  1:    
				seq_last=windowSize-1;// go back
				break;
			case  2:
				seq_last=windowSize-1;// selective
				ackBuffer = new boolean[windowSize];
				onlyR = true;
				break;
			default:    // stop & wait
				break;
		}
	}
	public void InitBuffer()
	{
		ackBuffer = new boolean[20];
		for(int i = 0;i < ackBuffer.length;i++)
			ackBuffer[i] = false;
	}
	public void SetDataBuffer(String[] buffer)
	{
		dataBuffer = buffer;
	}
	public void SetRecv()
	{
		dataBuffer = new String[20];
	}
	public int CheckACK()
	{
		int i=0;
		while(ackBuffer[i] == true)i++;
		return i;
	}
	
}

class SendACK extends Thread                                       // echo send를 위한 독립 thread
{
	
	
	DatagramSocket Dsocket;
	DatagramPacket recv_packet;
	Window window;
	byte flag;
	SendACK(DatagramSocket s, DatagramPacket r,Window _window,byte _flag)                    // receive 받은 socket과 그 packet을 인자로 받는다
	{
		Dsocket = s;
		recv_packet = r;
		window = _window;
		flag = _flag; 
	}
	
	public void run(){                                               // thread task
		String outData;
		byte[] buffer = new byte[4];
		int ackNum;
		try{
			
			
			
			
			if(window.cur_seq == 0)ackNum = window.windowSize-1;
			else ackNum = window.cur_seq -1;
			
			
			buffer = MakeData(null,window.cur_seq,ackNum,flag);
			DatagramPacket send_packet = new DatagramPacket(buffer,buffer.length,recv_packet.getAddress(),recv_packet.getPort());
			Dsocket.send(send_packet);
		
				
		}catch(IOException e){
			System.out.println(e);
		}
	}
	public static byte[] MakeData(byte[] input,int seq_number,int ack_number,byte flag_f)
	{
		byte[] dataFrame = new byte[135]; //1 1 1 1 4 127
		byte seq = (byte)seq_number;
		byte ack = (byte)ack_number;
		byte flag_field = flag_f;
		byte length;
		if((flag_field&0x0f)==0x02||input == null){
			length = 0;
		}
		else length = (byte) input.length;
		
		dataFrame[0] = seq;
		dataFrame[1] = ack;
		dataFrame[2] = flag_field;
		dataFrame[3] = 0;
		
		
		
		//System.arraycopy(seq,0,dataFrame,0,1);// 추가할 데이터,그위치,저장할어레이,위치,길이
		//System.arraycopy(ack,0,dataFrame,1,1);
		//System.arraycopy(flag_field,0,dataFrame,2,1);
		//System.arraycopy(length,0,dataFrame,3,1);
		
		if((int)length == 0 )return dataFrame;
		
	
		byte[] crc = new byte[4];
		byte[] data_field = input;
		
		Checksum checksum = new CRC32();
		checksum.update(data_field,0,data_field.length);
		long checksumValue = checksum.getValue();
		//System.out.println("crc ="+checksumValue);
		crc[3] = (byte)(int)(checksumValue&255L);
		crc[2] = (byte)(int)(checksumValue>>>8&255L);
		crc[1] = (byte)(int)(checksumValue>>>16&255L);
		crc[0] = (byte)(int)(checksumValue>>>24&255L);
		
		System.arraycopy(crc,0,dataFrame,4,4);
		System.arraycopy(data_field,0,dataFrame,8,data_field.length);
		
		//System.out.println("crc ="+crc[0]+" "+crc[1]+" "+crc[2]+" "+crc[3]);
		System.out.println("data = "+new String(data_field));
	
		return dataFrame; 
	}

}

class sendDataSelc extends Thread                                       // echo send를 위한 독립 thread
{
	
	final static int TIMEOUT = 100;
	DatagramSocket Dsocket;
	int port;
	InetAddress ip;
	Signaling signal;
	Timeout tclick;
	Window send;
	sendDataSelc(DatagramSocket s,int _port, InetAddress _ip,Window _send,Signaling _signal,Timeout _tclick)                    
	{
		Dsocket = s;
		port = _port;
		ip = _ip;
		signal = _signal;
		send = _send;
		tclick = _tclick;
		
	}
	
	public void run(){                                               // thread task
		
		byte[] buffer;
		DatagramPacket send_packet;
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		byte flag;
		int reSend;
		
		 // cur_seq, type ARQ(1->stop, 2-> go back, 3-> selective),window size
		int resendCount =0;
		try{
		
			String[] subData;
			while(true)
			{
			
				resendCount = 0;
				buffer = new byte[512];
				
				
				System.out.print("input Data : ");                
				String data = br.readLine();
				// stop & wait, go back ,selc
				subData = MakeSubData(data);          // 데이터를 10byte씩 쪼개어서 보낸다.
				send.SetDataBuffer(subData);          //data를 공유 변수인 send에 저장해놓았다. receive에서 nak받으면 바로 보내기 위해
				
				reSend =0;
				
				send.cur_seq = 0;
				send.seq_first = 0;
				send.seq_last = send.windowSize-1;
				
				for(int i =0;i < subData.length;){
					if(reSend > 10)break;
					flag = 0x01;  //data flag
				//	System.out.println(" cur, last :"+send.cur_seq+send.seq_last+send.windowSize);
					i = send.cur_seq;
					while(send.cur_seq <= send.seq_last && i < subData.length)
					{	//보내는 데이터의 시작을 알리기위해
						if(i==0)buffer = MakeData(subData[i].getBytes(),send.cur_seq,1,flag);	
						else buffer = MakeData(subData[i].getBytes(),send.cur_seq,0,flag);
						
						send_packet = new DatagramPacket(buffer,buffer.length,ip,port);
						++send.cur_seq; 
						
						Dsocket.send(send_packet);
						tclick.Timeoutset(send_packet.getData()[0],100, signal);
						i++;
						if(i >= subData.length)
						{
							break;
						}
						
					}
					
					signal.waitingACK(); // timeout signal을 기다림 // event가 싸이면 어떻게 되나?
										 // signal이 발생하였을 경우 wait이 없으면 그냥 사라진다. 추측
					if(signal.TIMENOTIFY == true) // 해당 packet만 재전송
					{	
						int reSndSeq = signal.sndseqNo;
						System.out.println("[ Time out ! sequence number : "+send.seq_first+" current sequence number : i : "+(i-1)+" ]");
						reSend++;
						
						
						if(reSndSeq==0)buffer =	 MakeData(subData[reSndSeq].getBytes(),reSndSeq,1,flag);
						else buffer =  MakeData(subData[reSndSeq].getBytes(),reSndSeq,0,flag);
						
						send_packet = new DatagramPacket(buffer,buffer.length,ip,port);
						Dsocket.send(send_packet);
						tclick.Timeoutset(send_packet.getData()[0],100, signal);
					
						
							i  = send.seq_first;
					//	send.cur_seq = send.seq_first;
					}
					
				} //for block
			
				
			}	
		}catch(IOException e){
			System.out.println(e);
	
	
		
			e.printStackTrace();
		}
	}
	public static String[] MakeSubData(String bigString)
	{
		final int subLength = 10;
		String[] subString; 
		int subCount  = bigString.length()/subLength;  // ex 125/10 = 12
		int subCount2 = bigString.length()%subLength;  // ex 125%10 = 5 
		
		if(subCount == 0)
		{
			subString = new String[1];
			subString[0] = bigString;
			return subString;
		}
		
		if(subCount2 != 0)subCount++;                  // ex 5 != 0 => +1
		subString = new String[subCount];
		
		int beginIndex = 0;
		int offset = 9;
		for(int i=0 ;i < subCount; i++)
		{
			subString[i] = bigString.substring(beginIndex, beginIndex + offset);
			beginIndex = beginIndex + 10;
			if(i == subCount -2)offset = subCount2;
		}
		
		return subString;
		
	}
	
	public static byte[] MakeData(byte[] input,int seq_number,int ack_number,byte flag_f)
	{
		byte[] dataFrame = new byte[135]; //1 1 1 1 4 127
		byte seq = (byte)seq_number;
		byte ack = (byte)ack_number;
		byte flag_field = flag_f;
		byte length = (byte) input.length;
		byte[] crc = new byte[4];
		byte[] data_field = input;
		
		
		dataFrame[0] = seq;
		dataFrame[1] = ack;
		dataFrame[2] = flag_field;
		dataFrame[3] = length;
//		System.out.println("[ send client -> sever ]---------------------------------------");
//		System.out.println(" 	seq = "+seq);
//		System.out.println("	ack = "+ack);
//		System.out.printf("	flag = %x\n",flag_field);
//		System.out.println("	length = "+length);
		
		
		if((int)length == 0 )return dataFrame;
		Checksum checksum = new CRC32();
		checksum.update(data_field,0,data_field.length);
		long checksumValue = checksum.getValue();
		
		crc[3] = (byte)(int)(checksumValue&255L);
		crc[2] = (byte)(int)(checksumValue>>>8&255L);
		crc[1] = (byte)(int)(checksumValue>>>16&255L);
		crc[0] = (byte)(int)(checksumValue>>>24&255L);
		
		System.arraycopy(crc,0,dataFrame,4,4);
		System.arraycopy(data_field,0,dataFrame,8,data_field.length);
		
//		System.out.println("	crc ="+dataFrame[4]+" "+dataFrame[5]+" "+dataFrame[6]+" "+dataFrame[7]);
		System.out.println("  I : "+new String(data_field));
	
		return dataFrame; 
	}

}






class receiveFrameSelc extends Thread
{
		DatagramSocket Dsocket;
		Window recv;
		Window send;
		Thread sendACK;
		Signaling signal;
		Timeout tclick;
		int ACKR;
		receiveFrameSelc(DatagramSocket s,Window _send,Window _recv,Signaling _signal,Timeout _tclick)                           // DatagramSocket 을 인자로 받는다 
		{
				Dsocket = s;
				recv = _recv;
				send = _send;
				signal = _signal;
				tclick = _tclick;
				ACKR = 0;
		}
		
		public void run(){                                       //thread task				
				int count = 0;
				while(true)
				{
					boolean debug = true;
					
					byte buffer[]  = new byte[512];              // buffer 생성
					try{
						DatagramPacket recv_packet = new DatagramPacket(buffer,buffer.length);  // DatagramPacket 생성   받은 패킷은 buffer에 저장하기로 설정
						Dsocket.receive(recv_packet);        // wait : packet을 기다린다
							
						if(debug&&recv_packet.getData()[0] == 2&&count == 0&&recv_packet.getData()[1] == 0)
						{
								System.out.println("[ Debug ( true ) : seq = 2 data ignored ( only once ) ]");
								count++;
						}
						else{
						
							if(PrintFrame(recv_packet.getData())){
								//ack ? or Data ?
								if((recv_packet.getData()[2]&0x0f)==0x01)
								{//data -> sendACK
									
									if(recv_packet.getData()[1] == 1 )
									{
										recv.SetRecv(); //data를 저장할 buffer를 새로 만듬
										recv.InitBuffer();
										recv.cur_seq = 0;
										
									}
									//System.out.println("cur_seq "+recv.cur_seq);
									if(recv.cur_seq == recv_packet.getData()[0]){
										//++recv.cur_seq; true가 있는 곳까지 증가되어야함
										recv.ackBuffer[recv_packet.getData()[0]] = true;
										
										recv.cur_seq = recv.CheckACK();
										
										Window temp = new Window(recv.cur_seq,2,3);
										//ack를 보내는데 연속된 true가 있는 곳 까지 보낸다.
										sendACK = new SendACK(Dsocket,recv_packet,temp,(byte) 0x02);
										sendACK.start();
									
									}
									else if(recv_packet.getData()[0] < recv.cur_seq + recv.windowSize &&
											recv_packet.getData()[0] > recv.cur_seq){
										// window 내부인경우 데이터를 저장하고, ackbuffer의 해당부분을 true로
										System.out.println("[ not R seq -> send NAK ]");
										recv.ackBuffer[recv_packet.getData()[0]] = true;
										recv.dataBuffer[recv_packet.getData()[0]] = new String(recv_packet.getData());
										// 현재 seq에 대해 nak를 보낸다.
										Window temp2 = new Window(recv.cur_seq,2,3);
										sendACK = new SendACK(Dsocket,recv_packet,temp2,(byte)0x03); // send NAK
										sendACK.start();
									}
									else {
										System.out.println("[ Discard data : out of receive sequence on window ! ]");
									}
								}
								else if((recv_packet.getData()[2]&0x0f)==0x02){
								// receive ack frame  -> change sequence number of sender
									
									tclick.Timeoutcancel(recv_packet.getData()[1]);
									send.seq_first = recv_packet.getData()[0];
									send.seq_last = (send.seq_first + send.windowSize -1);
									signal.ACKnotifying();
									//다르면 무시하고 timeout 발생
								}else
								{	
									int seq = recv_packet.getData()[0];
									byte[] data_field = new byte[20];
									
									System.arraycopy(data_field,0,recv_packet.getData(),8,data_field.length);
									//System.out.println("seq = "+ seq+ "data : "+new String(data_field));
									byte[] temp_buffer;
									if(seq==0)temp_buffer = MakeData(send.dataBuffer[seq].getBytes(),seq,1,(byte)0x01);	
									else temp_buffer = MakeData(send.dataBuffer[seq].getBytes(),seq,0,(byte)0x01);
									
									DatagramPacket temp_snd;
									temp_snd = new DatagramPacket(temp_buffer,temp_buffer.length,recv_packet.getAddress(),recv_packet.getPort());
									Dsocket.send(temp_snd);
									tclick.Timeoutset(temp_snd.getData()[0],100, signal); // 스케줄이 겹치면 어떻게함?
									
									//System.out.println("receive nak");
									
								  // nak	
								}
							}
							
							else {
								//에러가 있는 경우 아무것도 하지않고 그냥 다시 기다린다 -> 상대방이 타임아웃이 걸림	
									// seq는 그대로
							}//resend!
							
							                                          
						}
					}catch(IOException e){
						System.out.println(e);
					}
				}
			
		}
		
		public static boolean PrintFrame(byte[] rcv_Frame)
		{
			byte[] dataFrame = rcv_Frame; //1 1 1 1 4 127
			byte seq = dataFrame[0];
			byte ack = dataFrame[1];
			byte flag_field = dataFrame[2];
			byte length = dataFrame[3];
		//	System.out.println("[ recv server -> client ]---------------------------------------");
		//	System.out.println("		seq = "+seq);
		//	System.out.println("		ack = "+ack);
		//	System.out.printf ("		flag = %x\n",flag_field);
		//	System.out.println("		length = "+length);
			
			if(length != 0&&flag_field != 0x02){
				boolean error;
				byte[] crc = {dataFrame[4],dataFrame[5],dataFrame[6],dataFrame[7]};
				byte[] data_field = new byte[length];
				for(int i=0;i<length;i++){
					//System.out.println((char)dataFrame[i+8]);
					data_field[i] = dataFrame[i+8];
				}
				byte[] crc2 = new byte[4];
				Checksum checksum = new CRC32();
				checksum.reset();
				checksum.update(data_field,0,data_field.length);
				long checksumValue = checksum.getValue();
				
				crc2[3] = (byte)(int)(checksumValue&255L);
				crc2[2] = (byte)(int)(checksumValue>>>8&255L);
				crc2[1] = (byte)(int)(checksumValue>>>16&255L);
				crc2[0] = (byte)(int)(checksumValue>>>24&255L);
				error = true;
				
				for(int i=0;i<4;i++){
					if(crc[i] != crc2[i])
					{	
						//System.out.println(crc[i]+" == "+crc2[i]);						
						error = false;
					}
				}
				
				if(!error){
					System.out.println("Error detected !");
					return false;
				}
				
				//System.out.println(" 		crc	No error");
				//System.out.println("crc ="+crc[0]+" "+crc[1]+" "+crc[2]+" "+crc[3]);
				//System.out.println("crc2 ="+crc2[0]+" "+crc2[1]+" "+crc2[2]+" "+crc2[3]);
				System.out.println("  other = "+new String(data_field)+" [ crc no error ]");
				
				return true;
			}else if(flag_field == 0x02 ){
				System.out.println("		ACK for sequence number : "+ack);
			}
			else{
				System.out.println("		NAK for sequence number : "+seq);
			}
			return true;
			
			
		}
		
		public static byte[] MakeData(byte[] input,int seq_number,int ack_number,byte flag_f)
		{
			byte[] dataFrame = new byte[135]; //1 1 1 1 4 127
			byte seq = (byte)seq_number;
			byte ack = (byte)ack_number;
			byte flag_field = flag_f;
			byte length = (byte) input.length;
			byte[] crc = new byte[4];
			byte[] data_field = input;
			
			
			dataFrame[0] = seq;
			dataFrame[1] = ack;
			dataFrame[2] = flag_field;
			dataFrame[3] = length;
//			System.out.println("[ send client -> sever ]---------------------------------------");
//			System.out.println(" 	seq = "+seq);
//			System.out.println("	ack = "+ack);
//			System.out.printf("	flag = %x\n",flag_field);
//			System.out.println("	length = "+length);
			
			
			if((int)length == 0 )return dataFrame;
			Checksum checksum = new CRC32();
			checksum.update(data_field,0,data_field.length);
			long checksumValue = checksum.getValue();
			
			crc[3] = (byte)(int)(checksumValue&255L);
			crc[2] = (byte)(int)(checksumValue>>>8&255L);
			crc[1] = (byte)(int)(checksumValue>>>16&255L);
			crc[0] = (byte)(int)(checksumValue>>>24&255L);
			
			System.arraycopy(crc,0,dataFrame,4,4);
			System.arraycopy(data_field,0,dataFrame,8,data_field.length);
			
//			System.out.println("	crc ="+dataFrame[4]+" "+dataFrame[5]+" "+dataFrame[6]+" "+dataFrame[7]);
			System.out.println("  I : "+new String(data_field));
		
			return dataFrame; 
		}
		
}

