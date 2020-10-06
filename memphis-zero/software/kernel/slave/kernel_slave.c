/*!\file kernel_slave.c
 * MEMPHIS VERSION - 8.0 - support for RT applications
 *
 * Distribution:  June 2016
 *
 * Edited by: Marcelo Ruaro - contact: marcelo.ruaro@acad.pucrs.br
 *
 * Research group: GAPH-PUCRS   -  contact:  fernando.moraes@pucrs.br
 *
 * \brief
 * Kernel slave is the system slave used to execute user's tasks.
 *
 * \detailed
 * kernel_slave is the core of the OS running into the slave processors.
 * Its job is to runs the user's task. It communicates whit the kernel_master to receive new tasks
 * and also notifying its finish.
 * The kernel_slave file uses several modules that implement specific functions
 */

#include "kernel_slave.h"

#include "../../../include/kernel_pkg.h"
#include "../../include/api.h"
#include "../../include/plasma.h"
#include "../../include/services.h"
#include "../../modules/packet.h"
#include "../../modules/utils.h"


unsigned int 	schedule_after_syscall;		//!< Signals the syscall function (assembly implemented) to call the scheduler after the syscall



/** Assembles and sends a TASK_ALLOCATED packet to the master kernel
 *  \param allocated_task Allocated task TCB pointer
 */
void send_task_allocated(TCB * allocated_task){

	ServiceHeader *p = get_service_header_slot();

	p->header = allocated_task->master_address;

	p->service = TASK_ALLOCATED;

	p->task_ID = allocated_task->id;

	p->master_ID = cluster_master_address;

	send_packet(p, 0, 0);
}




/** Useful function to writes a message into the task page space
 * \param task_tcb_ptr TCB pointer of the task
 * \param msg_lenght Lenght of the message to be copied
 * \param msg_data Message data
 */
void write_local_msg_to_task(TCB * task_tcb_ptr, int msg_lenght, int * msg_data){

	Message * msg_ptr;

	msg_ptr = (Message*)((task_tcb_ptr->offset) | ((unsigned int)task_tcb_ptr->reg[3])); //reg[3] = address message

	msg_ptr->length = msg_lenght;

	for (int i=0; i<msg_ptr->length; i++)
		msg_ptr->msg[i] = msg_data[i];

	//Unlock the blocked task
	task_tcb_ptr->reg[0] = TRUE;

	//Release task to execute
	task_tcb_ptr->scheduling_ptr->waiting_msg = 0;
}

/** Syscall handler. It is called when a task calls a function defined into the api.h file
 * \param service Service of the syscall
 * \param arg0 Generic argument
 * \param arg1 Generic argument
 * \param arg2 Generic argument
 */
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

			puts("$$$_");
			puts(itoa(net_address>>8));puts("x");puts(itoa(net_address&0xFF)); puts("_");
			puts(itoa(current->id >> 8)); puts("_");
			puts(itoa(current->id & 0xFF)); puts("_");
			puts((char *)((current->offset) | (unsigned int) arg0));
			puts("\n");

		break;


	}

	return 0;
}



/** Handles a new packet from NoC
 */
int handle_packet(volatile ServiceHeader * p) {

	int need_scheduling, code_lenght, app_ID, task_loc;
	unsigned int app_tasks_location[MAX_TASKS_APP];
	PipeSlot * slot_ptr;
	Message * msg_ptr;
	TCB * tcb_ptr = 0;

	need_scheduling = 0;

	switch (p->service) {


	case TASK_RELEASE:

		tcb_ptr = searchTCB(p->task_ID);

		app_ID = p->task_ID >> 8;

		//putsv("-> TASK RELEASE received to task ", p->task_ID);

		if (tcb_ptr->scheduling_ptr->status == BLOCKED)
			tcb_ptr->scheduling_ptr->status = READY;

		DMNI_read_data( (unsigned int) app_tasks_location, p->app_task_number);

		for (int i = 0; i < p->app_task_number; i++){
			add_task_location(app_ID << 8 | i, app_tasks_location[i]);
		}

		if (current == &idle_tcb){
			need_scheduling = 1;
		}

		break;

	case INITIALIZE_SLAVE:

		cluster_master_address = p->source_PE;

		putsv("Slave initialized by cluster address: ", cluster_master_address);

		break;



	default:
		putsv("ERROR: service unknown: ", MemoryRead(TICK_COUNTER));
		break;
	}

	return need_scheduling;
}

/** Generic task scheduler call
 */
void Scheduler() {


	OS_InterruptMaskSet(IRQ_SCHEDULER);

}

/** Function called by assembly (into interruption handler). Implements the routine to handle interruption in Memphis
 * This function must implement a important rule: it cannot send a packet when the DMNI is already send a packet.
 * The interruption triggers according to the DMNI status, and the if-else statements inside this function ensure this
 * behavior.
 * \param status Status of the interruption. Signal the interruption type
 */
void OS_InterruptServiceRoutine(unsigned int status) {

	//***** Check if interruption comes from NoC
	if ( status & IRQ_NOC ){

		read_packet((ServiceHeader *)&p);

			//**** Handles remaining packets
	} 

		
	if ( status & IRQ_SCHEDULER ){

		call_scheduler = 1;
	}

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

	/*enables timeslice counter and wrapper interrupts*/
	OS_InterruptMaskSet(IRQ_SCHEDULER | IRQ_NOC | IRQ_SLACK_TIME);

	/*runs the scheduled task*/
	ASM_RunScheduledTask(current);

	return 0;
}
