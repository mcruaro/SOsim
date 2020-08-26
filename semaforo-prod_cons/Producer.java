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
public class Producer extends Thread implements SleepWakeup{

    boolean sleeping;
    Consumer consumer;
    
    Buffer buffer;
    Semaforo empty;
    Semaforo reserva;
    Semaforo full;
    
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

            empty.down(this); //decrementa os spacos vazios do buffer
            reserva.down(this); //testa se consumer nao esta na regiao critica
            System.out.println("Producer entrou na regiao critica-----");
            //Aqui comeca a regiao critica do produtor
            buffer.insert_item(item); //Insere item no buffer
            System.out.println("Producer saiu da regiao critica-----");

            reserva.up(consumer);//Sai da regiao critica
            full.up(consumer);//Incrementa o contador full, que guarda o numero elementos preenchidos no buffer
        }
    }
    
    @Override
    public void sleep() {
        sleeping = true;
        System.out.println("Producer indo dormir...");
        while(sleeping);
    }

    @Override
    public void wakeup() {
        if(sleeping){
            System.out.println("Producer acordado");
            sleeping = false;
        }
    }
    
}
