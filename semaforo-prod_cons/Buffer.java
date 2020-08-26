/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package semaforo_prod_cons;

import java.util.LinkedList;

/**
 *
 * @author Marcelo
 */
public class Buffer {
    
    private int next_item_value;//Variavel que somente simula a geracao de um novo valor a cada chamada de produce_item
    LinkedList<Integer> buffer;
    
    public Buffer() {
        next_item_value = 0;
        buffer = new LinkedList<>();
    }
    
    
    public int produce_item(){
        //System.out.println("Item produzido");
        return next_item_value++;
    }
    
    public void insert_item(int item){
        buffer.add(item);
        System.out.println("Item inserido = "+item);
        //System.out.println("Inserido - Buffer size = "+ buffer.size());
    }
    
    public int remove_item(){
        int item = buffer.remove();
        System.out.println("Item consumido = "+item);        
        //System.out.println("Consumido - Buffer size = "+ buffer.size());
        return item;
        
    }
    
}
