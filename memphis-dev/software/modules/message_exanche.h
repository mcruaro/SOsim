#define REQUEST_SIZE	 MAX_LOCAL_TASKS*(MAX_TASKS_APP-1) //50	//!< Size of the message request array in fucntion of the maximum number of local task and max task per app


typedef struct {
    int requester;             	//!< Store the requested task id ( task that performs the Receive() API )
    int requested;             	//!< Stores the requested task id ( task that performs the Send() API )
} MessageRequest;

MessageRequest message_request[REQUEST_SIZE];	//!< message request array


void init_message_request_table(){

	for(int i=0; i<REQUEST_SIZE; i++){
		message_request[i].requested = -1;
		message_request[i].requester = -1;
	}
}

int insert_message_request(int producer_task, int consumer_task) {

    int i;

    for (i=0; i<REQUEST_SIZE; i++)
    	if ( message_request[i].requester == -1 ) {
    		message_request[i].requester  = consumer_task;
    		message_request[i].requested  = producer_task;


    		return 1;
		}

    //puts("ERROR - request table if full\n");
	return 0;	/*no space in table*/
}

MessageRequest * remove_message_request(int producer_task, int consumer_task) {

    for(int i=0; i<REQUEST_SIZE; i++) {
        if( message_request[i].requested == producer_task && message_request[i].requester == consumer_task){
        	message_request[i].requester = -1;
        	message_request[i].requested = -1;


            //return message_request[i].requester_proc;
        	return &message_request[i];
        }
    }

    return 0;
}

int search_message_request(int producer_task, int consumer_task) {

    for(int i=0; i<REQUEST_SIZE; i++) {
        if( message_request[i].requested == producer_task && message_request[i].requester == consumer_task){
            return 1;
        }
    }

    return 0;
}