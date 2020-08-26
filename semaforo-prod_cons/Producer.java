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
public class Producer extends SleepWakeup{

    Semaforo empty;
    Semaforo reserva;
    Semaforo full;
    Consumer consumer;
    Buffer buffer;
    
    public Producer(Buffer buffer, Semaforo empty, Semaforo reserva, Semaforo full){
        this.buffer = buffer;
        this.empty = empty;
        this.reserva = reserva;
        this.full = full;
        this.setName("Producer");
    };
    
    void set_consumer(Consumer consumer){
        this.consumer = consumer;
    }
    
    @Override
    public void run(){
        
        int item;
        
        for (int i = 0; i < 10; i++) {
            
            item = buffer.produce_item();

            System.out.println("Producer DOWN empty");
            empty.down(this); //decrementa os spacos vazios do buffer
            System.out.println("Producer DOWN reserva");
            reserva.down(this); //testa se consumer nao esta na regiao critica
            
            System.out.println("Producer entrou na regiao critica-----");
            
            buffer.insert_item(item); //Insere item no buffer
            
            System.out.println("Producer saiu da regiao critica-----");

            System.out.println("Producer UP reserva");
            reserva.up(consumer);//Sai da regiao critica
            System.out.println("Producer UP full");
            full.up(consumer);//Incrementa o contador full, que guarda o numero elementos preenchidos no buffer
        }
    }
    
}
