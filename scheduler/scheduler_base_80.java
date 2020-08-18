/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package sosim80;


enum ProcStatus {
    WAITING,
    READY,
    RUNNING
}

class TCB {
    int ID;
    ProcStatus status;
    int exec_time;

    //80_wait
    int waiting_time;
};

/**
 *
 * @author Marcelo
 */
public class SOsim80 {

    //Constantes
      static final int N = 2;
    static final int MAX_TIME_SLICE = 10;
    static final int MAX_EXEC_TIME = 10;

    static int t;
    static int current;
    static int time_slice;
    static TCB[] p = new TCB[4];
    
    //auxiliar variavel usada somente dentro do escalonador
    static int next_p = 0;

    static void initialize() {
        t = 0;
        current = -1;
        time_slice = MAX_TIME_SLICE;

        for (int i = 0; i < N; i++) {
            p[i] = new TCB();
            p[i].ID = i;
            p[i].status = ProcStatus.READY;
            p[i].exec_time = 0;

            //80_wait
		    p[i].waiting_time = 0;
        }
        
        System.out.print("System Initialized\n\n");
	
	    for (int i = 0; i < 30; i++) {
            if (i < 10) {
                System.out.print(" "+i+ "|");
            } else {
                System.out.print(i + "|");
            }
        }
        System.out.print("\n");
    }
    
    
    static int scheduler() {
        int scheduled_p;

        for (int i = 0; i < N; i++) {

            if (next_p == N) {
                next_p = 0;
            }

            if (p[next_p].status == ProcStatus.READY) {
                scheduled_p = next_p;
                next_p++;
                return scheduled_p;
            }
        }

        return -1;
    }

    //80_wait
    static void simm_IO_time(){
        for(int i=0; i<N; i++){
            if (p[i].status == ProcStatus.WAITING){
                
                if (p[i].waiting_time + 1 + p[i].exec_time >= MAX_EXEC_TIME){
                    p[i].status = ProcStatus.READY;
                    p[i].exec_time = 0;
                }
                
                p[i].waiting_time++;
            }
        
        }
    }

    public static void main(String[] args) {
        initialize();
       
        while (true) {
            //Update current task status
            if (current != -1) {
                //If time slice ended
                if (time_slice == 0) {
                    p[current].status = ProcStatus.READY;
                    p[current].exec_time = 0;
                    current = -1;
                //80_wait
                } else if (p[current].exec_time >= (MAX_EXEC_TIME*0.2)){
                    p[current].status = ProcStatus.WAITING;
                    p[current].waiting_time = 0;	
                    current = -1;
                } else {
                    p[current].exec_time++;
                    System.out.print("P" + current+"|");
                }

                time_slice--;
            }

            //80_wait
		    simm_IO_time();

            //Schedules a new task to run into CPU
            if (current == -1) {
                current = scheduler();
                time_slice = MAX_TIME_SLICE;
                System.out.print("--|");
            }

            try {
                Thread.sleep(500);
            } catch (Exception ex) {
            }

        }

    }

}
