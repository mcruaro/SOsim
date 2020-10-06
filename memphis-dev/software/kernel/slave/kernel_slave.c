
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


int Syscall(unsigned int service, unsigned int arg0, unsigned int arg1, unsigned int arg2) {

	
	schedule_after_syscall = 0;

	switch (service) {

		case EXIT:

			schedule_after_syscall = 1;
			
			puts("Task id: "); puts(itoa(current->id)); putsv(" terminated at ", MemoryRead(TICK_COUNTER));

			
		return 1;

		case WRITEPIPE:

			
		return 1;

		case READPIPE:


			return 0;

		case GETTICK:

			return MemoryRead(TICK_COUNTER);

		break;

		case ECHO:

			/*puts("$$$_");
			puts(itoa(my_cpu_address>>8));puts("x");puts(itoa(my_cpu_address&0xFF)); puts("_");
			puts(itoa(current->id >> 8)); puts("_");
			puts(itoa(current->id & 0xFF)); puts("_");
			puts((char *)((current->offset) | (unsigned int) arg0));
			puts("\n");*/

		break;


	}

	return 0;
}


void Scheduler() {


	OS_InterruptMaskSet(IRQ_SCHEDULER);

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

/** Function called by assembly (into interruption handler). Implements the routine to handle interruption in Memphis
 * This function must implement a important rule: it cannot send a packet when the DMNI is already send a packet.
 * The interruption triggers according to the DMNI status, and the if-else statements inside this function ensure this
 * behavior.
 * \param status Status of the interruption. Signal the interruption type
 */
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



/** Set a interruption mask
 * \param Mask Interruption mask set
 */
unsigned int OS_InterruptMaskSet(unsigned int Mask) {

    unsigned int mask;

    mask = MemoryRead(IRQ_MASK) | Mask;
    MemoryWrite(IRQ_MASK, mask);

    return mask;
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

	/*enables timeslice counter and wrapper interrupts*/
	OS_InterruptMaskSet(IRQ_SCHEDULER | IRQ_NOC);

	/*runs the scheduled task -- this function also set interruption to true*/
	//ASM_RunScheduledTask(current);

	ASM_SetInterruptEnable(TRUE);

	while(1){
		puts("idle...\n");
	}

	return 0;
}
