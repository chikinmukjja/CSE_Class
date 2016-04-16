package webproxy;

/**
 * ProxyCache.java - Simple caching proxy
 *
 * $Id: ProxyCache.java,v 1.3 2004/02/16 15:22:00 kangasha Exp $
 *
 */

import java.net.*;
import java.io.*;
import java.util.*;

public class ProxyCache {
	/** Port for the proxy */
	private static int port;
	/** Socket for client connections */
	private static ServerSocket socket;

	static LinkedList<HttpResponse> cacheList = new LinkedList<>();
	static int cacheHit = 0;
	static int allRequest = 0;

	/** Create the ProxyCache object and the socket */
	public static void init(int p) {
		port = p;
		try {
			socket = new ServerSocket(port)/* Fill in */;
		} catch (IOException e) {
			System.out.println("Error creating socket: " + e);
			System.exit(-1);
		}
	}

	public static void handle(Socket client) {
		Socket server = null;
		HttpRequest request = null;
		HttpResponse response = null;
		boolean cacheMiss = true;
		HttpResponse hittedData = null;
		/*
		 * Process request. If there are any exceptions, then simply return and
		 * end this request. This unfortunately means the client will hang for a
		 * while, until it timeouts.
		 */

		/* Read request */
		try {
			BufferedReader fromClient = new BufferedReader(
					new InputStreamReader(client.getInputStream()))/* Fill in */;
			request = new HttpRequest(fromClient)/* Fill in */;

			allRequest++;
			for (HttpResponse s : cacheList) {
				if (s.URI.equals(request.URI)) {
					cacheMiss = false;
					hittedData = s;
					cacheHit++;
					break;
				}
			}

		} catch (IOException e) {
			System.out.println("Error reading request from client: " + e);
			return;
		}
		/* Send request to server */
		try {
			/* Open socket and write request to socket */
			if (cacheMiss) {

				server = new Socket(request.getHost(), 80) /* Fill in */;
				DataOutputStream toServer = new DataOutputStream(
						server.getOutputStream()) /* Fill in */;
				toServer.writeBytes(request.toString());
			}
			/* Fill in */
		} catch (UnknownHostException e1) {
			System.out.println("Unknown host: " + request.getHost());
			System.out.println(e1);
			return;
		} catch (IOException e1) {
			System.out.println("Error writing request to server: " + e1);
			return;
		}
		/* Read response and forward it to client */
		try {
			DataOutputStream toClient = new DataOutputStream(
					client.getOutputStream());

			if (cacheMiss) {

				DataInputStream fromServer = new DataInputStream(
						server.getInputStream())/* Fill in */;
				response = new HttpResponse(fromServer)/* Fill in */;
				response.URI = request.URI;
				cacheList.add(response);
				if (cacheList.size() > 50)
					cacheList.removeFirst();
				/* Fill in */
				toClient.writeBytes(response.toString());
				if (response.isBody())
					toClient.write(response.body, 0, response.body.length);

				/* Fill in */
				/* Write response to client. First headers, then body */
			} else {

				toClient.writeBytes(hittedData.toString());
				toClient.write(hittedData.body, 0, hittedData.body.length);

			}
			client.close();
			if (cacheMiss)
				server.close();
			/* Insert object into the cache */
			/* Fill in (optional exercise only) */
		} catch (IOException e1) {
			System.out.println("Error writing response to client: " + e1);
		}
	}

	/** Read command line arguments and start proxy */
	public static void main(String args[]) {
		int myPort = 0;

		try {
			myPort = Integer.parseInt(args[0]);
		} catch (ArrayIndexOutOfBoundsException e) {
			System.out.println("Need port number as argument");
			System.exit(-1);
		} catch (NumberFormatException e) {
			System.out.println("Please give port number as integer.");
			System.exit(-1);
		}

		init(myPort);

		/**
		 * Main loop. Listen for incoming connections and spawn a new thread for
		 * handling them
		 */
		Socket client = null;

		while (true) {
			try {

				client = socket.accept()/* Fill in */;
				handle(client);
				System.out.println("[     cacheRate : " + cacheHit + " / "
						+ allRequest + "     ]");

			} catch (IOException e) {
				System.out.println("Error reading request from client: " + e);
				/*
				 * Definitely cannot continue processing this request, so skip
				 * to next iteration of while loop.
				 */
				continue;
			}
		}

	}
}
