import java.io.*;

import java.net.Socket;

public class receive extends Thread {

	static final int INFINITE = 100000;
	static final int nodeNum =5;
	Socket[] linkAll =null; 
	Socket link = null;
	int[] linkCost =null;
	boolean[] neighbor; 
	static int[][] DVTable;
	int myNum;
	
	public receive(Socket[] _linkAll,Socket _link,int[] _linkCost, boolean[] _neighbor, int[][] _DVTable,int _myNum)
	{
		link = _link;
		linkAll = _linkAll;
		neighbor = _neighbor;
		DVTable = _DVTable;
		myNum = _myNum;
		linkCost = _linkCost;
	}
	
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
	
	public void init() {

		for (int j = 0; j < nodeNum; j++) { // 내가 가진 알고있는 벡터모두를 전달
			// System.out.println("나의 이웃 : "+dest+" "+j+" "+(DVTable[myNum][j]
			// != INFINITE));
			if (DVTable[myNum][j] != INFINITE) {
				if (DVTable[myNum][j] != 0){
					try {
						Thread.sleep(300);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					//System.out.println("Source : "+(char)(myNum+65)+" -----> Dest : " +(char)(j+65)+" !");
					System.out.println("give DV! Source -----> Dest : "+(char)(myNum+65)+" -----> " +(char)(j+65)+
							" DVvalue["+(char)(myNum+65)+"]["+j+"] = "+DVTable[myNum][j]);
					sendDV(link, myNum, j, DVTable[myNum][j]); // 초기에 연결시 자신의
											
				}// DV값을 다 전달
			}

		}

	}
	
	public void sendDV(Socket link,int src,int dest,int cost)
	{
		try {
			String Data = new String(src + " " + dest + " " + cost);
			OutputStream out = link.getOutputStream();
			PrintWriter pw = new PrintWriter(new OutputStreamWriter(out));
			pw.println(Data);
			pw.flush();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void DVAlgorithm() {
		
		int[] store = new int[nodeNum];

		for (int dst = 0; dst < nodeNum; dst++) // 모든 목적지에 대해서
		{
			store[dst] = INFINITE; // 현재 dvtable을 이용하여 dv 계산

			if (dst == myNum)
				store[dst] = 0;
			if (dst != myNum) {
				for (int node = 0; node < nodeNum; node++) // 이웃을 통해 가는 방법중에 가장
															// 작은방법
					if (neighbor[node]) {
						if (store[dst] > linkCost[node] + DVTable[node][dst]) {
							store[dst] = linkCost[node] + DVTable[node][dst];

						}

					}
			}

			if (store[dst] != DVTable[myNum][dst]) {
				DVTable[myNum][dst] = store[dst];
				showDVTable();
				for (int ackToNode = 0; ackToNode < nodeNum; ackToNode++) {
					if (neighbor[ackToNode]) {
						System.out.println("give DV! Source -----> Dest : " + (char) (myNum + 65) + " -----> "
								+ (char) (ackToNode + 65) + " DVvalue[" + (char) (myNum + 65) + "][" + dst + "] = "
								+ DVTable[myNum][dst]);

						sendDV(linkAll[ackToNode], myNum, dst, DVTable[myNum][dst]);
					}
				}
			}
		}
	}
	
	public String parseData(BufferedReader Data)
	{
		String temp = null;
		String[] parsed;
		int src,dest,cost;

		try {
			temp = new String(Data.readLine());
			parsed = temp.split(" ");
			if(parsed.length !=  3) System.out.println("this is over!");
			src = Integer.parseInt(parsed[0]);
			dest = Integer.parseInt(parsed[1]);
			cost = Integer.parseInt(parsed[2]);
			
			DVTable[src][dest]=cost;
			showDVTable();
			
			DVAlgorithm();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return null;
	}

	
	public  void trigger()
	{
		if(myNum == 1){
			linkCost[3] = 12;
			DVTable[myNum][3] = 12;
			DVAlgorithm();// Db(D) =12 
		}
		else {
			linkCost[1] = 12;
			DVTable[myNum][1] = 12;
			DVAlgorithm(); // Dd(B) = 12
		}
		
	}
	
	
	public void run()
	{
		try {
			while(true){
				
				BufferedReader fromSrc = new BufferedReader(new InputStreamReader(link.getInputStream()));
				parseData(fromSrc);
			}
			
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	
	
}
