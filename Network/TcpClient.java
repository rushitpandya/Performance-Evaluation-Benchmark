import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

import java.io.*;

//import com.sun.xml.internal.ws.api.message.Packet;


public class TcpClient {

	/**
	 * @param args
	 */
	static int no_threads=8;
	static int packet_size=64*1024;//packet size to be sent
	static int mem_size=1073741824;
	
	public static void main(String[] args) {
		double times[]=new double[no_threads];
		// TODO Auto-generated method stub
		try{
			
				int no_iterations=mem_size/packet_size;

				Thread thread1[]= new Thread[no_threads];
				/*creating threads for sending data*/
				for(int j=0;j<no_threads;j++)
				{
					thread1[j] = new Thread(new ExecuteTcpClient(no_threads,j));
					thread1[j].start();
				}
				for(int j=0;j<no_threads;j++)
				{
					thread1[j].join();
				}
			
					ExecuteTcpClient exc =new ExecuteTcpClient();
					//times=exc.getTotalTime();
					times=ExecuteTcpClient.times;
				double sum=0;
				//System.out.println("time1"+times.length);
				if(times!=null && times.length>0)
				{
					for(int j=0;j<times.length;j++)
					{
						
						sum+=times[j];	
					}
				}
				//System.out.println("Sum:"+sum);
				System.out.println("Number of Threads: "+no_threads);
				float totalTime= (float)sum/no_threads;
				System.out.println("Time taken(seconds): "+totalTime +" Seconds");	//total time
				float throughput = ((mem_size/(1024*1024)))/totalTime;//throughput
				System.out.println("Throughput: "+throughput +" MBPS ");

				float latency = (totalTime/no_iterations)*1000000;//latency
				
				System.out.println("Latency: "+latency+" microsecond");
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
	}

	
}
/*class for creating new thread for TCP Client*/
class ExecuteTcpClient implements Runnable
{
	String threadName;
	int no_threads_i;
	int thread_index;
	static double times[];
	/*returning time of all threads*/
	public double[] getTotalTime()
	{
		return times;
	}
	public ExecuteTcpClient()
	{
		
	}
	public ExecuteTcpClient(int no_threads_i,int thread_index)
	{
		//this.threadName=threadName;
		this.thread_index=thread_index;
		this.no_threads_i=no_threads_i;
		times= new double[no_threads_i];
	}
	
	Socket tcpsocket;
	double startTime=0,totalTime=0;

	@Override
	synchronized public void run() {
		// TODO Auto-generated method stub
		try {
			tcpsocket= new Socket("localhost",9000);//connecting socket connection at 9000 port
			OutputStream os= (java.io.OutputStream)tcpsocket.getOutputStream();
			PrintWriter out=new PrintWriter(os,true);
			InputStream in = tcpsocket.getInputStream();
			BufferedReader bufin =new BufferedReader(new InputStreamReader(in));
			
			byte buffArr[]=new byte[TcpClient.packet_size];
			for(int j=0;j<TcpClient.packet_size;j++)
			{
				buffArr[j]=(byte)(long)(Math.random()*TcpClient.packet_size);//creating a packet of packet size 64KB
			}
			
			int i;
			int no_operation= TcpClient.mem_size/TcpClient.packet_size;
			startTime=System.currentTimeMillis();
			for( i=0;i<(no_operation/no_threads_i);i++)
			{
				out.println(buffArr);//sending data
				bufin.readLine();//receiving data
			}
			totalTime=(System.currentTimeMillis()-startTime)/1000;//calculating total time for thread
			times[thread_index]=totalTime;
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally{
			try {
				tcpsocket.close();//closing socket
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		
	}
	
	
}
