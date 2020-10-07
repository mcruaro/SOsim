
#include "kernel_slave.h"

#include "../../../include/kernel_pkg.h"
#include "../../include/api.h"
#include "../../include/plasma.h"
#include "../../include/services.h"
#include "../../modules/driver_rede.h"
#include "../../modules/utils.h"


unsigned int 	schedule_after_syscall;		//!< Signals the syscall function (assembly implemented) to call the scheduler after the syscall
TCB *			current;	
TCB 			idle_tcb;
unsigned int	my_cpu_address;
unsigned int 	master_address;


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

			master_address = p->source_PE;

			putsv("Slave initialized by master at address: ", master_address);

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

	while(1) puts("idle 2....\n");

    /*runs the scheduled task*/
    //ASM_RunScheduledTask(current);
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
