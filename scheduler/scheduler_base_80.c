#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 				2
#define MAX_TIME_SLICE	10
#define MAX_EXEC_TIME	10

enum p_status {READY, WAITING, RUNNING};


struct TCB {
	int ID;
	int status;
	int exec_time;
	//80_wait
	int waiting_time;
};

int t;
int current;
int time_slice;
struct TCB p[N];

void initialize(){
	t = 0;
	current = -1;
	time_slice = MAX_TIME_SLICE;
	
	for(int i=0; i<N; i++){
		p[i].ID = N;
		p[i].status = READY;
		p[i].exec_time = 0;
		//80_wait
		p[i].waiting_time = 0;
	}
	
	printf("System Initialized\n\n");
	
	//Just print the timeline
	for(int i=0; i<30; i++){
		if(i<10)
			printf(" %d|",i);
		else
			printf("%d|",i);
	}
}


int scheduler(){
	static int next_p = 0;
	int scheduled_p;
	
	for(int i=0; i<N; i++){
		
		if (next_p == N)
			next_p = 0;
		
		if (p[next_p].status == READY){
			scheduled_p = next_p;
			next_p++;
			return scheduled_p;
		}
	}
	
	return -1;
}

//80_wait
void simm_IO_time(){
	for(int i=0; i<N; i++){
		if (p[i].status == WAITING){
			
			if (p[i].waiting_time + 1 + p[i].exec_time >= MAX_EXEC_TIME){
				p[i].status = READY;
				p[i].exec_time = 0;
			}
			
			p[i].waiting_time++;
		}
	
	}
}

int main() {
	
	initialize();

	while(1){
		t++;
		
		//Update current task status
		if (current != -1){
			
			//If time slice ended
			if (time_slice == 0){	
				p[current].status = READY;
				p[current].exec_time = 0;
				current = -1;
				
			//80_wait
			} else if (p[current].exec_time >= (MAX_EXEC_TIME*0.2)){
				p[current].status = WAITING;
				p[current].waiting_time = 0;	
				current = -1;
				
				
			} else {
				p[current].exec_time++;
				printf("P%d|", current);
			}
			
			time_slice--;
		} 
		
		//80_wait
		simm_IO_time();
		
		//Schedules a new task to run into CPU
		if (current == -1){
			current = scheduler();
			time_slice = MAX_TIME_SLICE;
			//printf("SC|");
			printf("--|");
		}
		
		usleep(500000);
		//sleep(1);
	}
	
	return 0;
}
