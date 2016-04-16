
import java.io.*;

import java.net.ConnectException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.CancellationException;


public class DVImpl {
	
	static final int nodeNum = 5;
	static final int INFINITE = 100000;
	static  int[][]  DVTable = new int[nodeNum][nodeNum];// distance vector table
	static boolean[] neighbor = new boolean[nodeNum];    // network connected info
	static Socket[] link = new Socket[nodeNum];        // link interface
	static int[] linkCost = new int[nodeNum];

	
	
	private static ServerSocket TCPsocket = null;
	
	public static void showDVTable()
	{
		System.out.println("[ --------------------- 나의 DV Table ---------------------- ] ");
		for(int i=0;i<5;i++){
			for(int j=0;j<5;j++)
			{
				System.out.printf("%7d",DVTable[i][j]);
			}
			System.out.println("");
		}
		System.out.println("[ ---------------------------------------------------------- ]");
	}
	
	public static void main(String[] args)
	{
		int port = 0;
		String fileName = null;
		String routerName = null;
		File file;
		Scanner in = null;
		String[] parsing =null;
		int linkNumber = 0;
		
		
		
		for(int i=0;i<nodeNum;i++){ // 각 변수들 초기화과정
			link[i]= null;
			neighbor[i] = false;
			linkCost[i] = INFINITE;
			
			for(int j=0;j<nodeNum;j++){
				
				DVTable[i][j]=INFINITE;
				if(i == j)DVTable[i][j] =0 ;
				
			}
		}
		
		
		
		if(args.length < 2){
			System.out.println("argument missing");
			return;
		}
		
		
		fileName = new String(args[0]);
		routerName = new String(args[1]);
		int myNum = routerName.charAt(0)-65; // A = 0, B = 1 ... 배열넣기위해
		
	
		
		 //자기자신에 대한 초기화 
		
		file = new File(fileName);  // neighbor.conf 파일 읽기  
		try {
			in = new Scanner(file);
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		
		
		String temp = null;

		while(in.hasNext())   // 백터 테이블 초기화 , 네트워크 연결 맵핑정보 갖기
		{
			temp = in.nextLine();
			if(routerName.equalsIgnoreCase(temp.substring(0, 1)))
			{
				
				parsing = temp.split(" ");
				port = Integer.parseInt(parsing[1]);
				int j = 2;
				
				while( j < parsing.length )
				{
					DVTable[myNum][parsing[j].charAt(0)-65] = Integer.parseInt(parsing[j+1]);
					linkCost[parsing[j].charAt(0)-65] = Integer.parseInt(parsing[j+1]);
					neighbor[parsing[j].charAt(0)-65] = true;
					linkNumber++;
					j=j+2;
				}
			}
		}
		
		showDVTable();
		Socket fromRouter = null;
		
		
		try {
			TCPsocket = new ServerSocket(port);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		for (int i = 0; i < 5; i++) {

			if (neighbor[i]) {
				if (myNum < i) {
					try {

						System.out.println("port " +(port - (myNum - i)) );
						fromRouter = new Socket(InetAddress.getLocalHost(), (port - (myNum - i)));
						// handle(fromRouter)
						link[i] = fromRouter;
						linkNumber--;
						
						//System.out.println(myNum + " -> " + i);
						System.out.println("[연결성공] "+(char)(myNum+65)+ " ----> " +(char)(i+65) +" 이웃노드와 연결되었습니다");
						OutputStream out = link[i].getOutputStream();
						PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
						String Data = new String(myNum + " ");
						pw.println(Data);
						pw.flush();

					} catch (IOException e) {
						// TODO Auto-generated catch block
						System.out.println("[연결실패] "+(char)(myNum+65)+ " ----> " +(char)(i+65) + " 이웃노드가 실행중이지 않습니다 ");
						//e.printStackTrace();
					}

				}

				else {
					try {
						
						System.out.println("port " +(port - (myNum - i)) );
						fromRouter = new Socket(InetAddress.getLocalHost(), (port - (myNum - i)));
						
						link[i] = fromRouter;
						linkNumber--;

						//System.out.println(myNum  + " -> " + i);
						System.out.println("[연결성공] "+(char)(myNum+65)+ " ----> " +(char)(i+65) +" 이웃노드와 연결되었습니다");
						OutputStream out = link[i].getOutputStream();
						PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
						String Data = new String(myNum  + " ");
						pw.println(Data);
						pw.flush();

					} catch (IOException e) {
						// TODO Auto-generated catch block
						//e.printStackTrace();
						System.out.println("[연결실패] "+(char)(myNum+65)+ " ----> " +(char)(i+65) + " 이웃노드가 실행중이지 않습니다 ");
					}
				}
			}
		}

		Socket tempS = null;

		while (linkNumber != 0){
			try {

				tempS = TCPsocket.accept();
				linkNumber--;
				//System.out.println("tcp 연결 !");
				BufferedReader fromSrc = new BufferedReader(new InputStreamReader(tempS.getInputStream()));
				String who = "";
				who = fromSrc.readLine();

				System.out.println("[연결성공] "+(char)(Integer.parseInt(who.substring(0, 1))+65)+" 와 연결되었습니다 ");

				link[Integer.parseInt(who.substring(0, 1))] = tempS;

			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		
		// 모든 연결 마무리
		// distance vector를 수신준비
		receive[] connection = new receive[nodeNum];
		
		System.out.println("[ ----- 나의 이웃 목록  ----- ]");
		for(int i=0;i<nodeNum;i++)
		{
			if(neighbor[i]){
				System.out.println("  이웃노드 : "+(char)(i+65));
				connection[i] = new receive(link,link[i],linkCost,neighbor,DVTable,myNum);
				connection[i].start();
				
			}
		}
		// 자신과 연결된 링크에 자신의 distance vector를 모두 전송
		Scanner stop =new Scanner(System.in);
		System.out.println("[시작대기] 모든 링크연결이 완료되면 enter를 누르세요 ");
		stop.nextLine();
		System.out.println("[교환시작] DV를 이웃노드와 교환하기 시작합니다 ");
		
		for(int i=0;i<nodeNum;i++)
		{
			if(neighbor[i]){
				connection[i].init();
			}
		}
		
		
		
		try {
			Thread.sleep(20000);
			
			
			if(myNum == 1 ){
				System.out.println("[링크증가] cost(B,D)가 12 로 증가 하였습니다 !");
				connection[3].trigger();
			}
			if(myNum == 3 ){
				System.out.println("[링크증가] cost(D,B)가 12 로 증가 하였습니다 !");
				connection[1].trigger();
			}
			
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
}
