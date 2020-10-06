

#ifndef __KERNEL_SLAVE_H__
#define __KERNEL_SLAVE_H__


typedef struct {

	unsigned int id;
    unsigned int offset;

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
