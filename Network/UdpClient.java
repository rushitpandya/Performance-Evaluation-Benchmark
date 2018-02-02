import java.io.IOException;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import java.io.*;

//import com.sun.xml.internal.ws.api.message.Packet;


public class UdpClient {

	/**
	 * @param args
	 */
	static int no_threads=8;
	static int packet_size=60*1024;//packet size 60KB
	static int mem_size=1073741824;//1GB
	
	public static void main(String[] args) {
		double times[]=new double[no_threads];
		
		// TODO Auto-generated method stub
		try{
			
			
				int no_iterations=mem_size/packet_size;
				Thread thread1[]= new Thread[no_threads];
				for(int j=0;j<no_threads;j++)
				{
					thread1[j] = new Thread(new ExecuteUdpClient(no_threads,j));//creating new thread for connecting to server
					thread1[j].start();
				}
				for(int j=0;j<no_threads;j++)
				{
					thread1[j].join();
				}
			
					ExecuteUdpClient exc =new ExecuteUdpClient();
					//times=exc.getTotalTime();
					times=ExecuteUdpClient.times;
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
				System.out.println("Time taken(seconds): "+totalTime +" Seconds");	//total time in sec
				float throughput = ((mem_size/(1024*1024)))/totalTime;//throughput in MBPS
				//float latency = (totalTime/1000000) ;
				System.out.println("Throughput: "+throughput +" MBPS ");
				float latency = (totalTime/no_iterations)*1000000;//latency in microsecond
				
				System.out.println("Latency: "+latency+" microsecond");
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
	}

	
}

/*class for sending data to server*/
class ExecuteUdpClient implements Runnable
{
	
	int no_threads_i;
	int thread_index;
	static double times[];
	DatagramSocket ds=null;
	DatagramPacket dp=null;
	public double[] getTotalTime()
	{
		return times;
	}
	public ExecuteUdpClient()
	{
		
	}
	public ExecuteUdpClient(int no_threads_i,int thread_index)
	{
		//this.threadName=threadName;
		this.thread_index=thread_index;
		this.no_threads_i=no_threads_i;
		times= new double[no_threads_i];
	}
	
	double startTime=0,totalTime=0;

	@Override
	synchronized public void run() {
		// TODO Auto-generated method stub
		try {
			ds=new DatagramSocket();//creating socket
			InetAddress address = InetAddress.getByName("localhost");
			
			byte buffArr[]=new byte[UdpClient.packet_size];
			for(int j=0;j<UdpClient.packet_size;j++)
			{
				buffArr[j]=(byte)(long)(Math.random()*UdpClient.packet_size);//creating a packet of 60KB
			}
			dp = new DatagramPacket(buffArr, buffArr.length,address, 9000);//creating a packet to send to server
			int no_operation= UdpClient.mem_size/UdpClient.packet_size;
			startTime=System.currentTimeMillis();
			for( int i=0;i<(no_operation/no_threads_i);i++)
			{
				ds.send(dp);//sending to server
				//
			}
			//dp = new DatagramPacket(buffArr, buffArr.length,address, 9000);
			//for( int i=0;i<(no_operation/no_threads_i);i++)
			//{
			//	ds.receive(dp);
				//times[i]=totalTime;
			//}
			
			totalTime=(System.currentTimeMillis()-startTime)/1000;
			
			//System.out.println("Packet Created");
			times[thread_index]=totalTime;
			//System.out.println(i+"");
			
			/*
			double sum=0;
			if(times!=null && times.length>0)
			{
				for(int j=0;j<times.length;j++)
				{
					sum+=times[j];	
				}
			}
			
			totalTime= sum;
			*/
			
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally{
			try {
				ds.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		
	}
	
	
}
