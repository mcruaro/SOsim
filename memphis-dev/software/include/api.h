/*!\file api.h
 * MEMPHIS VERSION - 8.0 - support for RT applications
 *
 * Distribution:  June 2016
 *
 * Edited by: Marcelo Ruaro - contact: marcelo.ruaro@acad.pucrs.br
 *
 * Research group: GAPH-PUCRS   -  contact:  fernando.moraes@pucrs.br
 *
 * \brief
 * Implements the API for the user's task and defines the structure Message,
 * used by tasks to exchange messages
 */

#ifndef __TASK_H__
#define __TASK_H__

/* Syscalls*/
#define EXIT      			0
#define WRITEPIPE 			1
#define READPIPE  			2
#define GETTICK   			3
#define ECHO      			4

#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)
#define TRUE	1
#define FALSE	0

extern int SystemCall();

//#define Send(msg, target) while(!SystemCall(WRITEPIPE, (unsigned int*)msg, target,0))
//#define Echo(str) SystemCall(ECHO, (char*)str,0,0)


#endif /*__TASK_H__*/

