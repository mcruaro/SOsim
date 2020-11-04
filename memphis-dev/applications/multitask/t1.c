#include <api.h>

int main()
{
    volatile int i = 0;
    Message msg;

    while(1){
        print_string("Tarefa 1 executando...\n");
        msg.msg_size = 3;
        msg.data[0] = 99;
        msg.data[1] = 88;
        msg.data[2] = 77;

        send(&msg, t2);

        for (i=0; i<99; i++);
    }

	exit();
}