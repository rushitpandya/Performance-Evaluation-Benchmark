import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;


public class TcpServer {

	/**
	 * @param args
	 */
	static int[] no_threads={2};
	static int packet_size=64*1024;//64KB packet size
	public static void main(String[] args) {
		ServerSocket tcpserver=null;
		// TODO Auto-generated method stub
		try{
		
			
			tcpserver= new ServerSocket(9000);//staring server at 9000 port
			System.out.println("Server Started ");
			
			while(true)
			{
				new ExecuteTcpServer(tcpserver.accept()).start();//starting new thread to accept connection
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		finally
		{
			try {
				tcpserver.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

}
/*class for executing server*/
class ExecuteTcpServer extends Thread
{
	String threadName;
	int no_threads;
	ServerSocket tcpserver=null;
	Socket tcpclient=null;
	String inputLine=null;
	
	public ExecuteTcpServer(Socket tcpclient)
	{
	//	this.threadName=threadName;
	//	this.no_threads=no_threads;
		this.tcpclient=tcpclient;
	}
	@Override
	public void run() {
		// TODO Auto-generated method stub
		try {
			System.out.println("Connection Accepted ");
			OutputStream os= (java.io.OutputStream)tcpclient.getOutputStream();
			PrintWriter out=new PrintWriter(os,true);
			InputStream in = tcpclient.getInputStream();
			BufferedReader bufin =new BufferedReader(new InputStreamReader(in));
			 while ((inputLine = bufin.readLine()) != null) {
			    	out.println(inputLine);//receiving from client
			    }
			System.out.println("Ended");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally{
			try {
				tcpclient.close();//closing connection
				//tcpserver.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			
		}
	}
}
