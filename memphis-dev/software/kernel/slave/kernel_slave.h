

#ifndef __KERNEL_SLAVE_H__
#define __KERNEL_SLAVE_H__


//Estrutura Task Control Block
typedef struct {

    //###### Variaveis utililizadas na troca de contexto ######
    unsigned int reg[30]; //Armazena o conteudo dos registradores quando ha troca de contexto
    unsigned int pc; //Armazena o conteudo do program counter (PC) - guarda a linha atual de execucao
    unsigned int offset; //Armazena o endereco inicial da pagina de memoria 
   
    //####Outras variaveis
    unsigned int id; //Cada tarefa/thread vai ter um ID unico

}TCB;



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
