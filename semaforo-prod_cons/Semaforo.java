/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package semaforo_prod_cons;

/**
 * @author Marcelo
 */
public class Semaforo {
    
    private volatile int S;

    public Semaforo(int init_S) {
        this.S = init_S;
    }
    
    public void down(SleepWakeup process){
        if (S == 0){
            process.sleep();
        }
        S = S - 1;
    }
    
    public void up(SleepWakeup process){
        S = S + 1;
        //System.out.println(name+" of "+process.getName()+" call wakeup");
        process.wakeup();
    }
    
    
}
