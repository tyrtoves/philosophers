#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<time.h> 
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>
#include <sys/timeb.h>


int philosoph(int num, sem_t** semaphors, int n ) {
	int i;
	for(i = 0; i < 2; ++i) {
		srand (time(NULL));
  		struct timeval this_time, this_time_end, res;
  		
  		
		//printf("%u,%u sec: ", this_time.tv_sec, this_time.tv_usec); 
		printf("P%d thinks\n", num + 1);
		sleep( (rand() % 2 + 1) );
	
		//printf("%u,%u sec: ", this_time.tv_sec, this_time.tv_usec); 
		printf("P%d wakes up\n", num + 1);
		gettimeofday(&this_time, 0);           
		while(1) {
		
			sem_wait(semaphors[num%n]);
		
			if (!sem_trywait(semaphors[(num + 1)%n])) {
				gettimeofday(&this_time_end, 0);
				res.tv_sec= this_time_end.tv_sec -this_time.tv_sec;
				res.tv_usec=this_time_end.tv_usec-this_time.tv_usec;
				if(res.tv_usec < 0) {
					res.tv_sec--;
					res.tv_usec+=1000000;
				}
				//printf("%u,%u sec \n", res.tv_sec, res.tv_usec); 
				printf("P%d waited %u,%u sec \n", num + 1, res.tv_sec, res.tv_usec);
				//printf("%u,%u sec: ", this_time.tv_sec, this_time.tv_usec); 
				printf("P%d eats\n", num + 1);
				sleep((rand() % 2 + 1));
	
				sem_post(semaphors[num%n]);
				sem_post(semaphors[(num + 1)%n]);
			
				//printf("%u,%u sec: ", this_time.tv_sec, this_time.tv_usec); 
				printf("P%d finished eating\n", num + 1);
				break;
		
			}
			else {
				sem_post(semaphors[num%n]);
				sleep((rand() % 2 + 1) );
			}
		
		}
	}
	exit(0);
}	

int main(int argc, char** argv) { 
	
	if (argc != 2) {
		printf("Wrong number of arguments");
		exit(0);
	}	
	
	int n = atoi(argv[1]);
	
	pid_t pids[n + 2];
	
	sem_t* semaphors[n + 2];
	char semname[100];
	
	int i, j;

	for (i = 0; i < n; ++i) {
		
		sprintf(semname, "%d", i);
		semaphors[i] = sem_open(semname, O_CREAT | O_TRUNC | O_EXCL, 0777, 1);
		
		if(semaphors[i] == SEM_FAILED) {
			perror("SEMAPHOR");
		}
		
	}

	for (i = 0; i < n; ++i) {

		pids[i] = fork();
		
		if (pids[i] == -1)
			perror("FORK");
			
		if (pids[i] == 0)
			break;

	}	

	if (i == n) {
		int status;
		for(j = 0; j < n; ++j) {
			waitpid(pids[j], &status, WUNTRACED);
		}
		char new_semname[100];
		for(j = 0; j < n; ++j) {
			sprintf(new_semname, "%d", j);	
			printf("%s", new_semname);
			sem_close(semaphors[j]);
			sem_unlink(new_semname);
		}
	}
	else {
		philosoph(i, semaphors, n);
	}

	exit(0);		
	return 0;
}
