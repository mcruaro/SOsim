/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package semaforo_prod_cons;

/**
 *
 * @author Marcelo
 */
public class Semaforo {
    
    private int S;
    SleepWakeup process;

    public Semaforo(int init_S) {
        S = init_S;
    }
    
    public synchronized void down(SleepWakeup process){
        if (S <= 0){
            process.sleep();
        }
        S = S - 1;
    }
    
    public synchronized void up(SleepWakeup process){
        S = S + 1;
        process.wakeup();
    }
    
    
}
