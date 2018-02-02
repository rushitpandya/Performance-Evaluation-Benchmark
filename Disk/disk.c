#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

int mem_size = 1342177280;//file size to be read/write
void perform_random_read(int,int);
void perform_read_write(int,int);
void perform_seq_read(int,int);
void * random_read(void *);	
void * seq_read(void *);	
void * read_write(void *);
void create_testing_file();

/*structure for each thread*/
typedef struct thread_info 
{
	int start,end, blocksize,threadsize;
} thread_info;

static const char* filename = "testing_file";
/*main function*/
int main(int argc, char** argv)
{
	int no_threads[4] = {1,2,4,8},i,j;//no of threads
	int block_sizes[4] = {8,1024*8,1048576*8,1048576*80};//testing block sizes
	
	create_testing_file();//creating a 1GB File
	size_t threads_size=sizeof(no_threads)/sizeof(int);
	size_t no_blocks = sizeof(block_sizes)/sizeof(int);
		
	printf("No_Threads\tBlock_Size\tOperation\tMemory_Accessed\tTime(seconds)\tThroughput(MBPS)\tLatency(ms)\n");
	/*executing for each threads and block sizes combination*/
	for(i=0;i<threads_size;i++)
	{
		for(j=0;j<no_blocks;j++)
		{
			perform_read_write(no_threads[i],block_sizes[j]);
			perform_random_read(no_threads[i],block_sizes[j]);
			perform_seq_read(no_threads[i],block_sizes[j]);
		}
	}
	return 0;
	
}
/*this function creates 1GB testing file*/
void create_testing_file ()
{
	FILE *file=fopen(filename, "w");
	fseek(file, mem_size-1, SEEK_SET);
	fputc('\n', file);
	fclose(file);
}
/*this function creates threads for performing random read on created file*/
void perform_random_read(int no_threads,int block_size)
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
		info[i].start= i*(no_operations/ no_threads);//defining starting block for thread
		info[i].end= (i+1)*(no_operations/ no_threads)-1;//defining ending block for thread
		info[i].blocksize = block_size;
		info[i].threadsize = no_threads;
		pthread_create(&threads[i], NULL, random_read, (void*)&info[i]);// creating multiple threads
	}
	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);//wait all threads finishes
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;//total time
	throughput = ((((double) mem_size)/1048576) / (double)(total_time )) ;//throughput in MBPS
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;//latency in millisecond
	printf("%i\t%d\t%s\t%d\t%f\t%f\t%f\n", no_threads,block_size, "Random Read", mem_size, total_time,throughput,latency);
	free(info);	
}
/*this function creates threads for performing sequence read on created file*/
void perform_seq_read(int no_threads,int block_size)
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
		pthread_create(&threads[i], NULL, seq_read, (void*)&info[i]);
	}
	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;
	throughput = ((((double) mem_size)/1048576) / (double)(total_time )) ;
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;
	printf("%i\t%d\t%s\t%d\t%f\t%f\t%f\n", no_threads,block_size, "Sequen Read", mem_size, total_time,throughput,latency);
	free(info);	
}
/*this function creates threads for performing read adn write on created file*/
void perform_read_write(int no_threads,int block_size)
{
	struct timeval startTime, endTime;
	double total_time,throughput,latency;
	int no_operations=(mem_size/block_size);
	pthread_t threads[no_threads];
	gettimeofday(&startTime, NULL);
	thread_info *info = (thread_info*) malloc(no_threads * sizeof (thread_info));
	
	int i;
	for (i = 0; i < no_threads; i++) 
	{
		info[i].start= i*(no_operations/ no_threads);
		info[i].end= (i+1)*(no_operations/ no_threads)-1;
		info[i].blocksize = block_size;
		info[i].threadsize = no_threads;
		pthread_create(&threads[i], NULL, read_write, (void*)&info[i]);
	}

	for (i = 0; i < no_threads; i++) 
	{
		pthread_join(threads[i], NULL);
	}
	gettimeofday(&endTime, NULL);
	total_time =(double) (endTime.tv_sec - startTime.tv_sec)+(double) (endTime.tv_usec - startTime.tv_usec) / 1000000 ;
	throughput = ((((double) mem_size)/1048576) / (double)(total_time )) ;
	latency = ((double)(total_time)/((double) mem_size/block_size))*1000;
	printf("%i\t%d\t%s\t%d\t%f\t%f\t%f\n", no_threads,block_size, "Read Write", mem_size, total_time,throughput,latency);
	free(info);
}
/*function for performing random read on file*/
void * random_read(void * info)
{
	
	srand((unsigned)time(NULL));
	int fp;
	fp = open(filename, O_RDONLY, 0666);
	if(fp < 0)
	{
		printf("Open test file failed!\n");
		exit(0);
	}	
	int i,random_x;
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize;
	//printf("\nstart: %d, end: %d, blocksize: %d\n",tinfo->start,tinfo->end,b_size);
	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	int no_block= mem_size/b_size;	
	int no_block_threads= no_block/(tinfo->threadsize);
	
	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * b_size);
		random_x = (rand()%(no_block_threads))+(tinfo->start);//fetching block to be accessed randomly
		lseek(fp, random_x*b_size, SEEK_SET);
		read(fp, buffer, b_size);
		free(buffer);
	}
	close(fp);
	free(from);
	free(to);
	
}
/*function for performing sequence read on file*/
void * seq_read(void * info)
{
	
	srand((unsigned)time(NULL));
	int i,random_x;
	int fp;
	fp = open(filename, O_RDONLY, 0666);
	if(fp < 0)
	{
		printf("Open test file failed!\n");
		exit(0);
	}	
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize;

	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	int no_block= mem_size/b_size;	

	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * b_size);
		//random_x = (rand()%(no_block_threads)+(tinfo->start))*b_size;
		lseek(fp, i*b_size, SEEK_SET);
		read(fp, buffer, b_size);
		free(buffer);
	}
	close(fp);
	free(from);
	free(to);
	
}
/*function for performing read write on file*/
void * read_write(void * info)
{
		//printf("in1");
	srand((unsigned)time(NULL));
	int i;
	int fp;
	fp = open(filename, O_RDWR, 0666);
	if(fp < 0)
	{
		printf("Open test file failed!\n");
		exit(0);
	}	
	thread_info *tinfo = (thread_info *) info;
	int b_size = tinfo -> blocksize; 
	char *from = (char*)malloc(sizeof(char)*mem_size);
	char *to = (char*)malloc(sizeof(char)*mem_size);
	
	int no_block= mem_size/b_size;	

	for(i = (tinfo->start); i < (tinfo -> end); i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * b_size);
		//random_x = (rand()%(no_block_threads)+(tinfo->start))*b_size;
		lseek(fp, i*b_size, SEEK_SET);
		read(fp, buffer, b_size);
		write(fp,buffer,b_size);
		free(buffer);
	}
		//printf("out");
	close(fp);
	free(from);
	free(to);	
	
}

