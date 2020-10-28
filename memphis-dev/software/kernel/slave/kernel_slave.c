
#include "kernel_slave.h"


#include "../../include/api.h"
#include "../../include/plasma.h"
#include "../../include/services.h"
#include "../../modules/driver_rede.h"
#include "../../modules/task_location.h"
#include "../../modules/message_exanche.h"
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

void send_message_request(int producer_task, int consumer_task, unsigned int targetPE){

	ServiceHeader *p = get_service_header_slot();

	p->header = targetPE;

	p->service = MESSAGE_REQUEST;

	p->producer_task = producer_task;

	p->consumer_task = consumer_task;

	send_packet(p, 0, 0);
}

void send_message_delivery(int producer_task, int consumer_task, int consumer_PE, Message * msg_ptr){

	ServiceHeader *p = get_service_header_slot();

	p->header = consumer_PE;

	p->service = MESSAGE_DELIVERY;

	p->producer_task = producer_task;

	p->consumer_task = consumer_task;

	p->msg_lenght = msg_ptr->msg_size;

	while (msg_ptr->msg_size > MSG_SIZE)
		puts("ERROR: message lenght higher than MSG_SIZE\n");

	send_packet(p, (unsigned int)msg_ptr->data, msg_ptr->msg_size);

}


/*Implementa chamadas de sistemas*/
int Syscall(unsigned int service, unsigned int arg0, unsigned int arg1, unsigned int arg2) {

	//puts("Chamada de sistema\n");
	int consumer_task, producer_task, producer_PE, consumer_PE;
	int appID;
	MessageRequest * msg_req_ptr;
	Message *msg_read;
	
	schedule_after_syscall = 0;

	switch (service) {

		case EXIT:
			puts("EXIT\n");
			current->id = -1;
		break;

		case PRINT_STRING:
			puts((char *)((current->offset) | (unsigned int) arg0)); puts("\n");
			break;

		case PRINT_INTEGER:
			puts(itoa(arg0)); puts("\n");
			break;

		case SEND_MSG:
			
			puts("SEND_MSG\n");
			producer_task =  current->id;
			consumer_task = (int) arg1;
			appID = producer_task >> 8;
			consumer_task = (appID << 8) | consumer_task;

			//1. Verificar se ha um registro de requisicao de mesnagem (MESSGE_REQUEST)
			msg_req_ptr = remove_message_request(producer_task, consumer_task);

			//2. Se sim, enviar a mensagem do send
			if (msg_req_ptr){

				consumer_PE = get_task_location(consumer_task);

				msg_read = (Message *)((current->offset) | arg0);

				send_message_delivery(producer_task, consumer_task, consumer_PE, msg_read);
			//3. Se nao, deixar a produra em WAITING, aguardando sua mensagem ser requisitada
			} else {
				puts("Produra entroi em WAITING\n");
				current->status = WAITING;
				schedule_after_syscall = 1;
			}

			break;

		case RECEIVE_MSG:
			puts("RECEIVE MSG\n");
			//1. Enviar o MESSAGE_REQUEST para a TAREFA PRODUTORA
			consumer_task =  current->id;
			producer_task = (int) arg1;
			appID = producer_task >> 8;
			consumer_task = (appID << 8) | consumer_task;

			producer_PE = get_task_location(producer_task);

			putsv("producer_task: ", producer_task);
			putsv("consumer_task: ", consumer_task);
			puts("producer_PE: "); puts(itoh(producer_PE)); puts("\n");

			send_message_request(producer_task, consumer_task, producer_PE);

			//2. Deixar a tarefa atual (consumidora) em WAITING
			current->status = WAITING;
			schedule_after_syscall = 1;
			
			//3. (nao eh impletado aqui) o kernel, ao recebe o MESSAGE_DELIVERU da produra, libera a consumidora.

			break;
		
		default:
			break;
	}


	return 0;
}




int handle_packet(volatile ServiceHeader * p) {
	
	TCB * tcb_ptr;
	int app_ID;
	Message *msg_read;
	unsigned int app_tasks_location[MAX_TASKS_APP];

	int need_scheduling = 0;
	
	switch (p->service) {

		case MESSAGE_REQUEST:

			putsv("Produra: ", p->producer_task);
			putsv("Consumidora: ", p->consumer_task);

			tcb_ptr = search_TCB(p->producer_task);

			while(tcb_ptr == 0) puts("ERROR: tcb not found!\n");

			//Se a produtora esta em WAITING ela ja produziu a mensagem
			if (tcb_ptr->status == WAITING){
				
				//Pega o pointeiro da mensagem da produtora
				msg_read = (Message*)((tcb_ptr->offset) | ((unsigned int)tcb_ptr->reg[3]));

				puts("Mensagem encontrada\n");
				puts("PE da Consumidora: "); puts(itoh(p->source_PE)); puts("\n");
				putsv("Tamanho da mensagem: ", msg_read->msg_size);

				send_message_delivery(p->producer_task, p->consumer_task, p->source_PE, msg_read);

				tcb_ptr->status = READY;
				
				need_scheduling = 1;

			} else {
				puts("Mensagem gravada na tabela de msg request\n");
				insert_message_request(p->producer_task, p->consumer_task);
			}

		break;

		case MESSAGE_DELIVERY:

			tcb_ptr = search_TCB(p->consumer_task);

			msg_read = (Message*)((tcb_ptr->offset) | ((unsigned int)tcb_ptr->reg[3]));

			msg_read->msg_size = p->msg_lenght;

			DMNI_read_data((unsigned int)msg_read->data, msg_read->msg_size);

			tcb_ptr->status = READY;

			need_scheduling = 1;

		break;

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

			tcb_ptr->status = WAITING;

			//Se a tarefa IDLE estava executando, entao eu preciso chamar o escalonador
			if (current == &idle_tcb){
				need_scheduling = 1;
			}

			add_task_location(p->task_ID, my_cpu_address);

			send_task_allocated(tcb_ptr);

		
			puts("TASK_ALLOCATION\n");

		break;

		case TASK_RELEASE:

			puts("TASK REALEASE");

			tcb_ptr = search_TCB(p->task_ID);
			app_ID = p->task_ID >> 8;

			tcb_ptr->status = READY;

			DMNI_read_data( (unsigned int) app_tasks_location, p->app_task_number);

			for (int i = 0; i < p->app_task_number; i++){
				add_task_location(app_ID << 8 | i, app_tasks_location[i]);
			}

			if (current == &idle_tcb)
				need_scheduling = 1;

		break;

	

	}

	return need_scheduling;
}

void Scheduler() {
	static int next_t = 0;

	MemoryWrite(SCHEDULING_REPORT, SCHEDULER);
	
	for(int i=0; i<MAX_LOCAL_TASKS; i++){
		
		if (next_t == MAX_LOCAL_TASKS)
			next_t = 0;
		
		//Procura a proxima tarefa valida cujo status seja READY
		if (tcbs[next_t].id != -1 && tcbs[next_t].status == READY){
			current = &tcbs[next_t];
			putsv("Escalonou tarefa: ", tcbs[next_t].id);
			putsv("Tempo: ", MemoryRead(TICK_COUNTER));
			next_t++;
			MemoryWrite(TIME_SLICE, 16384);//10000=0,16ms  100000=1ms (1s == 1000ms)
			MemoryWrite(SCHEDULING_REPORT, current->id);
			return;
		}

		next_t++;
	}
	
	MemoryWrite(TIME_SLICE, 16384);//10000=0,16ms  100000=1ms (1s == 1000ms)
	current = &idle_tcb;
	MemoryWrite(SCHEDULING_REPORT, IDLE);
	puts("Idle running...\n");
}

/*Chamada quando uma interrupcao ocorre*/
void OS_InterruptServiceRoutine(unsigned int status) {

	volatile ServiceHeader p;
	unsigned int call_scheduler = 0;

	MemoryWrite(SCHEDULING_REPORT, INTERRUPTION);

	puts("\nNew interruption\n");
	
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
	} else if (current == &idle_tcb){
		MemoryWrite(SCHEDULING_REPORT, IDLE);
	} else{
		MemoryWrite(SCHEDULING_REPORT, current->id);
	}

	//while(1) puts("idle 2....\n");

    /*runs the scheduled task*/
    ASM_RunScheduledTask(current);
}




/** Idle implementa o comportamento de IDLE do kernel, neste caso, chama um gatilho em hardware
 * para o processador ir dorimir (MemoryWrite(CLOCK_HOLD, 1))
 */
void OS_Idle() {
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

	init_task_location();

	init_message_request_table();
	
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
