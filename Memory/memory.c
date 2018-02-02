#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

int mem_size = 1342177280;//memory size to be read/write
void perform_random_write(int,int);
void perform_seq(int,int);
void perform_seq_write(int,int);
void * random_write(void *);	
void * seq_write(void *);	
void * seq_access(void *);
/*structure for thread*/
typedef struct thread_info 
{
	int start,end, blocksize,threadsize;
} thread_info;

  /*main function*/
int main(int argc, char** argv)
{
	int no_threads[4] = {1,2,4,8},i,j;
	int block_sizes[4] = {1*8, 1024*8,1048576*8,1048576*80};
	
	size_t threads_size=sizeof(no_threads)/sizeof(int);
	size_t no_blocks = sizeof(block_sizes)/sizeof(int);
	printf("No_Threads\tBlock_Size\tOperation\tTime(seconds)\tThroughput(MBPS)\tLatency(ms)\n");
	/*performing all combination of threads and block size*/
	for(i=0;i<threads_size;i++)
	{
		for(j=0;j<no_blocks;j++)
		{
		   	perform_seq(no_threads[i],block_sizes[j]);
			perform_random_write(no_threads[i],block_sizes[j]);
			perform_seq_write(no_threads[i],block_sizes[j]);
		}
	}
	return 0;
	
}
/*function for creating threads for random write*/
void perform_random_write(int no_threads,int block_size)
{
	struct timeval startTime, endTime;
	double total_time,throughput,latency;
	int no_operations=(mem_size/block_size);
	pthread_t threads[no_threads];	
	//pthread_t *threads = (pthread_t*) malloc(no_threads * sizeof (pthread_t));
	int i;
	
	thread_info *info = (thread_info*) malloc(no_threads * sizeof (thread_info));
	gettimeofday(&startTime, NULL);
	for (i = 0; i < no_threads; i++) 
	{
		info[i].start= i*(no_operations/ no_threads);//allocating starting block
		info[i].end= (i+1)*(no_operations/ no_threads)-1;//allocating ending block
		info[i].blocksize = block_size;
		info[i].threadsize = no_threads;
		pthread_create(&threads[i], NULL, random_write, (void*)&info[i]);//creating threads
	}
	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);//wait for all threads to finishes
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;//total time
	throughput = ((((double) mem_size)/1048576) / (double)(total_time ));//throughput
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;//latency
	printf("%i\t\t%d\t%s\t%f\t%f\t%f\n", no_threads,block_size, "Random Write", total_time,throughput,latency);
	free(info);	
}
/*function for creating threads for sequence write*/
void perform_seq_write(int no_threads,int block_size)
{
	struct timeval startTime, endTime;
	double total_time,throughput,latency;
	int no_operations=(mem_size/block_size);
	pthread_t threads[no_threads];	
	//pthread_t *threads = (pthread_t*) malloc(no_threads * sizeof (pthread_t));
	int i;
	gettimeofday(&startTime, NULL);
	thread_info *info = (thread_info*) malloc(no_threads * sizeof (thread_info));

	for (i = 0; i < no_threads; i++) 
	{
		info[i].start= i*(no_operations/ no_threads);
		info[i].end= (i+1)*(no_operations/ no_threads)-1;
		info[i].blocksize = block_size;
		info[i].threadsize = no_threads;
		pthread_create(&threads[i], NULL, seq_write, (void*)&info[i]);
	}
	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;
	throughput = ((((double) mem_size)/1048576) / (double)(total_time ));
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;
	printf("%i\t\t%d\t%s\t%f\t%f\t%f\n", no_threads,block_size, "Sequen Write", total_time,throughput,latency);
	free(info);	
}
/*function for creating threads for read write*/
void perform_seq(int no_threads,int block_size)
{
	struct timeval startTime, endTime;
	double total_time,throughput,latency;
	int no_operations=(mem_size/block_size);
	pthread_t threads[no_threads];
	//pthread_t *threads = (pthread_t*) malloc(no_threads * sizeof (pthread_t));
	gettimeofday(&startTime, NULL);
	thread_info *info = (thread_info*) malloc(no_threads * sizeof (thread_info));
	
	int i;
	for (i = 0; i < no_threads; i++) 
	{
		info[i].start= i*(no_operations/ no_threads);
		info[i].end= (i+1)*(no_operations/ no_threads)-1;
		info[i].blocksize = block_size;
		info[i].threadsize = no_threads;
		pthread_create(&threads[i], NULL, seq_access, (void*)&info[i]);
	}

	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;
	throughput = ((((double) mem_size)/1048576) / (double)(total_time )) ;
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;
	printf("%i\t\t%d\t%s\t%f\t%f\t%f\n", no_threads,block_size, "Read Write", total_time,throughput,latency);
	free(info);
}
/*function for perfoming random write*/
void * random_write(void * info)
{
	
	srand((unsigned)time(NULL));
	int i,random_x;
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize;

	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	int no_block= mem_size/b_size;	
	int no_block_threads= no_block/(tinfo->threadsize);
	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		random_x = (rand()%(no_block_threads))+(tinfo->start);
		memset(to+((random_x)*b_size), 'r', b_size);//writing to memory
	}
	free(from);
	free(to);
	
}
/*function for perfoming sequence write*/
void * seq_write(void * info)
{
	
	srand((unsigned)time(NULL));
	int i,random_x;
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize;

	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	int no_block= mem_size/b_size;	

	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		memset(to+(i*b_size), 'r', b_size);//writing to memory
	}
	free(from);
	free(to);
	
}
/*function for perfoming read write*/
void * seq_access(void * info)
{
	
	srand((unsigned)time(NULL));
	int i;
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize; 
	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	
	//int no_block= mem_size/b_size;	

	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		memcpy(to+(i*b_size), from+(i*b_size), b_size);//reading and writing from memory
	}
	free(from);
	free(to);	
	
}

