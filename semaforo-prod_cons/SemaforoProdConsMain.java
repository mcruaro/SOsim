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
public class SemaforoProdConsMain {

    static final int N = 3; //numero de lugares no buffer
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        Buffer buffer = new Buffer();
        
        Semaforo reserva = new Semaforo(1);
        Semaforo empty = new Semaforo(N);
        Semaforo full = new Semaforo(0);
        
        Producer producer = new Producer(buffer, empty, reserva, full);
        Consumer consumer = new Consumer(buffer, empty, reserva, full);
        
        producer.set_consumer(consumer);
        consumer.set_producer(producer);
        
        producer.start();
        consumer.start();
    }
    
}
