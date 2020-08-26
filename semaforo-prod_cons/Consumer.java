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
public class Consumer extends Thread implements SleepWakeup{

    boolean sleeping;
    Producer producer;
    
    Buffer buffer;
    Semaforo empty;
    Semaforo reserva;
    Semaforo full;
    
    public Consumer(Buffer buffer, Semaforo empty, Semaforo reserva, Semaforo full){
        this.buffer = buffer;
        this.empty = empty;
        this.reserva = reserva;
        this.full = full;
        this.setName("Consumer");
    };
    
    public void set_producer(Producer producer){
        this.producer = producer;
    }
    
    @Override
    public void run(){
        int item;
                
        while(true){
                
            full.down(this);
            reserva.down(this);

            System.out.println("Consumer entrou na regiao critica******");
            
            item = buffer.remove_item();
            
            System.out.println("Consumer saiu na regiao critica********");

            
            reserva.up(producer);
            empty.up(producer);
            
        }
    }
    
     @Override
    public void sleep() {
        sleeping = true;
        System.out.println("Consumer indo dormir...");
        while(sleeping);
    }

    @Override
    public void wakeup() {
        if(sleeping){
            System.out.println("Consumer acordado");
            sleeping = false;
        }
    }
    
}