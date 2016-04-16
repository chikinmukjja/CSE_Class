
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
		System.out.println("[ --------------------- ���� DV Table ---------------------- ] ");
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
		
		
		
		for(int i=0;i<nodeNum;i++){ // �� ������ �ʱ�ȭ����
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
		int myNum = routerName.charAt(0)-65; // A = 0, B = 1 ... �迭�ֱ�����
		
	
		
		 //�ڱ��ڽſ� ���� �ʱ�ȭ 
		
		file = new File(fileName);  // neighbor.conf ���� �б�  
		try {
			in = new Scanner(file);
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		
		
		String temp = null;

		while(in.hasNext())   // ���� ���̺� �ʱ�ȭ , ��Ʈ��ũ ���� �������� ����
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
						System.out.println("[���Ἲ��] "+(char)(myNum+65)+ " ----> " +(char)(i+65) +" �̿����� ����Ǿ����ϴ�");
						OutputStream out = link[i].getOutputStream();
						PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
						String Data = new String(myNum + " ");
						pw.println(Data);
						pw.flush();

					} catch (IOException e) {
						// TODO Auto-generated catch block
						System.out.println("[�������] "+(char)(myNum+65)+ " ----> " +(char)(i+65) + " �̿���尡 ���������� �ʽ��ϴ� ");
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
						System.out.println("[���Ἲ��] "+(char)(myNum+65)+ " ----> " +(char)(i+65) +" �̿����� ����Ǿ����ϴ�");
						OutputStream out = link[i].getOutputStream();
						PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
						String Data = new String(myNum  + " ");
						pw.println(Data);
						pw.flush();

					} catch (IOException e) {
						// TODO Auto-generated catch block
						//e.printStackTrace();
						System.out.println("[�������] "+(char)(myNum+65)+ " ----> " +(char)(i+65) + " �̿���尡 ���������� �ʽ��ϴ� ");
					}
				}
			}
		}

		Socket tempS = null;

		while (linkNumber != 0){
			try {

				tempS = TCPsocket.accept();
				linkNumber--;
				//System.out.println("tcp ���� !");
				BufferedReader fromSrc = new BufferedReader(new InputStreamReader(tempS.getInputStream()));
				String who = "";
				who = fromSrc.readLine();

				System.out.println("[���Ἲ��] "+(char)(Integer.parseInt(who.substring(0, 1))+65)+" �� ����Ǿ����ϴ� ");

				link[Integer.parseInt(who.substring(0, 1))] = tempS;

			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		
		// ��� ���� ������
		// distance vector�� �����غ�
		receive[] connection = new receive[nodeNum];
		
		System.out.println("[ ----- ���� �̿� ���  ----- ]");
		for(int i=0;i<nodeNum;i++)
		{
			if(neighbor[i]){
				System.out.println("  �̿���� : "+(char)(i+65));
				connection[i] = new receive(link,link[i],linkCost,neighbor,DVTable,myNum);
				connection[i].start();
				
			}
		}
		// �ڽŰ� ����� ��ũ�� �ڽ��� distance vector�� ��� ����
		Scanner stop =new Scanner(System.in);
		System.out.println("[���۴��] ��� ��ũ������ �Ϸ�Ǹ� enter�� �������� ");
		stop.nextLine();
		System.out.println("[��ȯ����] DV�� �̿����� ��ȯ�ϱ� �����մϴ� ");
		
		for(int i=0;i<nodeNum;i++)
		{
			if(neighbor[i]){
				connection[i].init();
			}
		}
		
		
		
		try {
			Thread.sleep(20000);
			
			
			if(myNum == 1 ){
				System.out.println("[��ũ����] cost(B,D)�� 12 �� ���� �Ͽ����ϴ� !");
				connection[3].trigger();
			}
			if(myNum == 3 ){
				System.out.println("[��ũ����] cost(D,B)�� 12 �� ���� �Ͽ����ϴ� !");
				connection[1].trigger();
			}
			
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
}
