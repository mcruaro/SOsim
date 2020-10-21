
#include "kernel_slave.h"


#include "../../include/api.h"
#include "../../include/plasma.h"
#include "../../include/services.h"
#include "../../modules/driver_rede.h"
#include "../../modules/task_location.h"
#include "../../modules/utils.h"


unsigned int 	schedule_after_syscall;		//!< Signals the syscall function (assembly implemented) to call the scheduler after the syscall
TCB *			current;	
TCB 			idle_tcb;
unsigned int	my_cpu_address;
unsigned int 	master_address;


/** Assembles and sends a TASK_ALLOCATED packet to the master kernel
 *  \param allocated_task Allocated task TCB pointer
 */
void send_task_allocated(TCB * allocated_task){

	ServiceHeader *p = get_service_header_slot();
	p->header = master_address;
	p->service = TASK_ALLOCATED;
	p->task_ID = allocated_task->id;
	p->master_ID = 0;
	send_packet(p, 0, 0);
}


/*Implementa chamadas de sistemas*/
int Syscall(unsigned int service, unsigned int arg0, unsigned int arg1, unsigned int arg2) {

	
	schedule_after_syscall = 0;


	return 0;
}




int handle_packet(volatile ServiceHeader * p) {
	
	TCB * tcb_ptr;

	int need_scheduling = 0;
	
	switch (p->service) {

		case INITIALIZE_SLAVE:

			master_address = p->source_PE;

			putsv("Slave initialized by master at address: ", master_address);

		break;

		case TASK_ALLOCATION:

			tcb_ptr = search_free_TCB();

			tcb_ptr->pc = 0;
			
			tcb_ptr->id = p->task_ID;

			puts("Task id: "); puts(itoa(tcb_ptr->id)); putsv(" allocated at ", MemoryRead(TICK_COUNTER));

			DMNI_read_data(tcb_ptr->offset, p->code_size);

			tcb_ptr->status = READY;

			//Se a tarefa IDLE estava executando, entao eu preciso chamar o escalonador
			if (current == &idle_tcb){
				need_scheduling = 1;
			}

			add_task_location(p->task_ID, my_cpu_address);

		
			puts("TASK_ALLOCATION\n");

		break;

		case TASK_RELEASE:
		break;

	}

	return need_scheduling;
}

void Scheduler() {
	static int next_t = 0;
	
	for(int i=0; i<MAX_LOCAL_TASKS; i++){
		
		if (next_t == MAX_LOCAL_TASKS)
			next_t = 0;
		
		if (tcbs[next_t].status == READY){
			current = &tcbs[next_t];
			putsv("Escalonou tarefa: ", tcbs[next_t].id);
			next_t++;
		}

		next_t++;
	}

	current = &idle_tcb;
}

/*Chamada quando uma interrupcao ocorre*/
void OS_InterruptServiceRoutine(unsigned int status) {

	volatile ServiceHeader p;
	unsigned int call_scheduler = 0;
	
	//***** Check if interruption comes from interface de rede intra chip (NoC)
	if ( status & IRQ_NOC ){

		//Pacote eh lido da rede (driver de rede) e o cabecalho eh gravado em p (ServiceHeader)
		read_packet((ServiceHeader *)&p);

		call_scheduler = handle_packet(&p);

			//**** Handles remaining packets
	} 

		
	if ( status & IRQ_SCHEDULER ){

		call_scheduler = 1;
		
	}

	if (call_scheduler){
		Scheduler();
	}

	//while(1) puts("idle 2....\n");

    /*runs the scheduled task*/
    ASM_RunScheduledTask(current);
}




/** Idle implementa o comportamento de IDLE do kernel, neste caso, chama um gatilho em hardware
 * para o processador ir dorimir (MemoryWrite(CLOCK_HOLD, 1))
 */
void OS_Idle() {
	puts("Idle running...\n");
	for (;;){
		MemoryWrite(CLOCK_HOLD, 1);
	}
}

int main(){

	//Desabilita interrupcoes
	ASM_SetInterruptEnable(FALSE);

	//Inicializando a tarefa Idle
	idle_tcb.pc = (unsigned int) &OS_Idle;
	idle_tcb.id = 0;
	idle_tcb.offset = 0;
	
	current = &idle_tcb; //A TCB current recebe a TCB idle 

	//Init a estrutura TCB
	init_tcbs();
	
	//Le o endereco atual do core/CPU
	my_cpu_address = MemoryRead(NI_CONFIG);

	puts("Runnign kernel slave\n");
	puts("My address is: "); puts(itoh(my_cpu_address)); puts("\n");

	/*Configura o tipo de interrupcao: ESCALONADOR E NOC*/
	MemoryWrite(IRQ_MASK, (IRQ_SCHEDULER | IRQ_NOC));

	/*runs the scheduled task -- this function also set interruption to true*/
	ASM_RunScheduledTask(current);

	return 0;
}
