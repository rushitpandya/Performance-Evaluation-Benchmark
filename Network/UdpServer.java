import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

/*class for UDP Server*/
public class UdpServer {

	/**
	 * @param args
	 */
	static int[] no_threads={1};
	static int packet_size=60*1024;
	public static void main(String[] args) {
		
		// TODO Auto-generated method stub
		try{
				new ExecuteudpServer().start();//executing server 
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		finally
		{
			
		}
	}
}
/*starting server for UDP client*/
class ExecuteudpServer extends Thread
{
	String threadName;
	int no_threads;
	
	String inputLine=null;
	DatagramSocket ds=null;
	public ExecuteudpServer()
	{

	}
	@Override
	public void run() {
		// TODO Auto-generated method stub
		try {
			
			while(true)
			{
				ds= new DatagramSocket(9000);
				
				System.out.println("datagram server started");
				byte []buff= new byte[UdpServer.packet_size];
				DatagramPacket dp=new DatagramPacket(buff, buff.length);
				ds.receive(dp);//receiving from client
				String info=new String(dp.getData(), 0, dp.getLength());
				InetAddress address = dp.getAddress();
				int port = dp.getPort();
				dp = new DatagramPacket(info.getBytes(), (info.getBytes()).length, address, port);
				//System.out.println("sending data");
				ds.send(dp);
				ds.close();
			}
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally{
			try {
				
				//tcpserver.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			
		}
	}
}
