#include <api.h>


int main()
{

	print_string("Producer iniciou\n");
	Message msg;

	msg.msg_size = 100;
	for(int i=0; i<100; i++){
		msg.data[i] = i;
	}

	for(int i=0; i<1000; i++){
		print_string("Executando");
		print_integer(i);
		send(&msg, hello_world_consumer);
	}

	print_string("Terminou");

	return 0;
	//exit();
}