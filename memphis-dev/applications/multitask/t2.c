#include <api.h>

int main()
{
    volatile int i = 0;
    Message msg;

    while(1){
        print_string("Tarefa 2 executando...\n");

        receive(&msg, t1);

        print_string("Mensagem recebida, tamanho:\n");
        print_integer(msg.msg_size);
        print_string("Valores:\n");
        print_integer(msg.data[0]);
        print_integer(msg.data[1]);
        print_integer(msg.data[2]);

        for (i=0; i<99; i++);
    }

	exit();
}