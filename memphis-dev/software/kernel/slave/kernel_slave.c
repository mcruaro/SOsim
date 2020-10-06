
#include "kernel_slave.h"

#include "../../../include/kernel_pkg.h"
#include "../../include/api.h"
#include "../../include/plasma.h"
#include "../../include/services.h"
#include "../../modules/driver_rede.h"
#include "../../modules/utils.h"


unsigned int 	schedule_after_syscall;		//!< Signals the syscall function (assembly implemented) to call the scheduler after the syscall
TCB *			current;	
unsigned int	my_cpu_address;
unsigned int 	cluster_master_address;


/*Implementa chamadas de sistemas*/
int Syscall(unsigned int service, unsigned int arg0, unsigned int arg1, unsigned int arg2) {

	
	schedule_after_syscall = 0;


	return 0;
}


void Scheduler() {


}

int handle_packet(volatile ServiceHeader * p) {
	
	int need_scheduling = 0;
	
	switch (p->service) {

		case INITIALIZE_SLAVE:

			cluster_master_address = p->source_PE;

			putsv("Slave initialized by cluster address: ", cluster_master_address);

		break;

		case TASK_ALLOCATION:
		while(1) puts("TASK_ALLOCATION\n");

		break;

		case TASK_RELEASE:
		break;

	}
}

/*Chamada quando uma interrupcao ocorre*/
void OS_InterruptServiceRoutine(unsigned int status) {

	volatile ServiceHeader p;
	unsigned int call_scheduler = 0;
	
	//***** Check if interruption comes from NoC
	if ( status & IRQ_NOC ){

		puts("Handle packet\n");
		read_packet((ServiceHeader *)&p);

		call_scheduler = handle_packet(&p);

			//**** Handles remaining packets
	} 

		
	if ( status & IRQ_SCHEDULER ){

		call_scheduler = 1;
		
	}

	while(1) puts("idle 2....\n");

    /*runs the scheduled task*/
    //ASM_RunScheduledTask(current);
}




/** Idle function
 */
void OS_Idle() {
	for (;;){
		MemoryWrite(CLOCK_HOLD, 1);
	}
}

int main(){

	ASM_SetInterruptEnable(FALSE);

	puts("Runnign kernel slave\n");

	/*Configura o tipo de interrupcao para ESCALONADOR E NOC*/
	MemoryWrite(IRQ_MASK, (IRQ_SCHEDULER | IRQ_NOC));

	/*runs the scheduled task -- this function also set interruption to true*/
	//ASM_RunScheduledTask(current);

	ASM_SetInterruptEnable(TRUE);

	while(1){
		puts("idle...\n");
	}

	return 0;
}
