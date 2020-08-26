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
public class SleepWakeup extends Thread {

    private volatile boolean sleeping;

    public void sleep() {
        sleeping = true;
        System.out.println(this.getName() + " indo dormir...");
        while (sleeping);
    }

    public void wakeup() {
        if (sleeping) {
            System.out.println(this.getName() + " acordado");
            sleeping = false;
        }
    }

    public void do_something() {
        for (int i = 0; i < 2; i++) {
            try {
                Thread.sleep(1000);
            } catch (Exception ex) {
                ex.printStackTrace();
            }

        }
    }

}
