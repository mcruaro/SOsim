#ifndef __TASK_H__
#define __TASK_H__

/* Syscalls*/
#define EXIT      			        0
#define PRINT_STRING                1
#define PRINT_INTEGER               2
#define SEND_MSG                    3
#define RECEIVE_MSG                 4

//Chamadas de Sistema
#define exit()                  SystemCall(EXIT, 0, 0, 0)
#define print_string(str)       SystemCall(PRINT_STRING, (char *) str, 0, 0)
#define print_integer(integer)  SystemCall(PRINT_INTEGER, integer, 0, 0)
#define send(msg, target)       SystemCall(SEND_MSG, (unsigned int*)msg, target, 0)
#define receive(msg, source)    SystemCall(RECEIVE_MSG, (unsigned int*)msg, source, 0)


#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)


extern int SystemCall();


#define MSG_SIZE  128
typedef struct {
    unsigned int data[MSG_SIZE];
    unsigned int msg_size;
}Message;


#endif /*__TASK_H__*/

