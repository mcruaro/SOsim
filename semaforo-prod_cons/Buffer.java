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
    private volatile LinkedList<Integer> buffer;
    
    public Buffer() {
        next_item_value = 0;
        buffer = new LinkedList<>();
    }
    
    public int produce_item(){
        return next_item_value++;
    }
    
    public void insert_item(int item){
        buffer.add(item);
        try {
            //Thread.sleep(500);
        } catch (Exception ex) {}
            
        System.out.println("Item inserido = "+item);
    }
    
    public int remove_item(){
        int item = buffer.remove();
        try {
            //Thread.sleep(500);
        } catch (Exception ex) {}
        
        System.out.println("Item consumido = "+item);        
        return item;
        
    }
    
}
