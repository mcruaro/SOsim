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
public class Consumer extends SleepWakeup{

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
                
        for (int i = 0; i < 10; i++) {
                
            System.out.println("Consumer DOWN full");
            full.down(this);
            System.out.println("Consumer DOWN reserva");
            reserva.down(this);

            System.out.println("Consumer entrou na regiao critica******");
            
            item = buffer.remove_item();
            
            System.out.println("Consumer saiu na regiao critica********");
            
            System.out.println("Consumer UP reserva");
            reserva.up(producer);
            System.out.println("Consumer UP empty");
            empty.up(producer);
            
            //do_something();
            
        }
    }
}