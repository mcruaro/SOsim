#include <api.h>


int main()
{

	print_string("Consumer iniciou\n");
	Message msg;

	for(int i=0; i<1000; i++){
		print_string("Executando");
		print_integer(i);
		//print_string("Mensagem sera enviada");
		
		receive(&msg, hello_world_producer);
	}

	print_string("Terminou");

	return 0;
	//exit();
}