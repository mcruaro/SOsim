

#ifndef __KERNEL_SLAVE_H__
#define __KERNEL_SLAVE_H__

#include "../../../include/kernel_pkg.h"
//Estrutura Task Control Block
typedef struct {

    //###### Variaveis utililizadas na troca de contexto ######
    unsigned int reg[30]; //Armazena o conteudo dos registradores quando ha troca de contexto
    unsigned int pc; //Armazena o conteudo do program counter (PC) - guarda a linha atual de execucao
    unsigned int offset; //Armazena o endereco inicial da pagina de memoria 
   
    //####Outras variaveis
    unsigned int id; //Cada tarefa/thread vai ter um ID unico
    
    unsigned int status;

}TCB;

enum scheduling_status {READY, WAITING, RUNNING};

TCB tcbs[MAX_LOCAL_TASKS];

void init_tcbs(){

    for(int i=0; i<MAX_LOCAL_TASKS; i++){
        tcbs[i].pc = 0;
        tcbs[i].id = -1; //LIVRE
        tcbs[i].offset = PAGE_SIZE * (i + 1);

        tcbs[i].status = READY;
    }
}

TCB * search_free_TCB() {

    for(int i=0; i<MAX_LOCAL_TASKS; i++){
		if(tcbs[i].id == -1){
			return &tcbs[i];
		}
	}

    puts("ERROR - no FREE TCB\n");
    while(1);
    return 0;
}


TCB * search_TCB(int id) {

    for(int i=0; i<MAX_LOCAL_TASKS; i++){
		if(tcbs[i].id == id){
			return &tcbs[i];
		}
	}
    
    return 0;
}




extern unsigned int ASM_SetInterruptEnable(unsigned int);
extern void ASM_SaveRemainingContext(TCB*);
extern void ASM_RunScheduledTask(TCB*);
void OS_InterruptServiceRoutine(unsigned int);

// ISR
unsigned int OS_InterruptMaskSet(unsigned int);

void OS_Init();
void OS_Idle();

void Scheduler();


#endif
