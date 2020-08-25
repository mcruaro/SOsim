/*
 Esse programa simula um exemplo monothread e multithread em java

Faça um programa em Java que possui 3 funções:
P = processamento
D = acesso a disco, 
N = acesso a rede.
A ideia central do programa é ter um momento de processamento, em seguida o dado processado é salvo no disco e transmitido para a rede (para um servidor, por exemplo).
Cada função executa por 1 segundo. Execute as funções na seguinte ordem:
P, D, N
Faça um laço que execute por 3 vezes a chamada dessa sequencia de funções
Observe que o tempo total de execução do programa será de 9 segundos
Agora vamos a parte principal do exercício, transforme esse programa de monothread para multithread
Para isso, faça com que as funções D e N virem um thread. A ideia é paralelizar a execução das funções D e N, não fazendo com que o atraso ao disco impacte no atraso ao acesso a rede.
Um detalhe importante: para poder chamar novamente uma rodada de processamento, garanta que as funções D e N (implementadas como thread) terminem sua execução. Dessa forma, evita-se do processamento sobrescrever um possível dado que estava sendo usado tanto para ser gravado em disco quanto para ser enviado para a rede

A ideia com esse exercício é representar o comportamento tal como a figura abaixo apresenta.
Note que no cenário com multithread há uma melhora de 1/3 % de tempo de execução comparado ao caso monothread. 

 */
package threadjava;

public class MonoMulti {
    
    
    public static void process() {
        System.out.println("\nProcessing...");
        try {
            Thread.sleep(1000);
        } catch (Exception ex) {
        }
        System.out.println("Processing completed!\n");
    }
    
    //DESCOMENTE OU COMENTE OS BLOCOS ABAIXO PARA REPRESENTAR O COMPORTAMENTO DESEJADO

    //-------------------------------------------------------------- MONOTHREAD
    public static void sendToNet(){
        save_working = true;
        try {
            System.out.println("Acessing disk...");
            Thread.sleep(1000);
            save_working = false;
            System.out.println("Save completed!\n");
        } catch (Exception ex) {
        }
    }
    
   public static void saveToDisk() {
        save_working = true;
        try {
            System.out.println("Acessing disk...");
            Thread.sleep(1000);
            save_working = false;
            System.out.println("Save completed!\n");
        } catch (Exception ex) {
        }
    }
    //-------------------------------------------------------------- MONOTHREAD
    

    
    //-------------------------------------------------------------- MULTITHREAD
    /*public static void saveToDisk() {
        save_working = true;
        
        new Thread("Thread 1: SaveToDisk") {
            @Override
            public void run() {
                try {
                    System.out.println("Acessing disk...");
                    Thread.sleep(1000);
                    save_working = false;
                    System.out.println("Save completed!");
                } catch (Exception ex) {
                }
                
            }
        }.start();

    }

    public static void sendToNet() {
        sent_working = true;
        
        new Thread("Thread 2: sendToNet") {
            @Override
            public void run() {
                try {
                    System.out.println("Connecting to Server...");
                    Thread.sleep(1000);
                    sent_working = false;
                    System.out.println("Upload completed!");

                } catch (Exception ex) {
                }
                
            }
        }.start();

    }*/
    //-------------------------------------------------------------- MULTITHREAD
    
    //The Java volatile keyword guarantees visibility of changes to variables across threads. See more in: http://tutorials.jenkov.com/java-concurrency/volatile.html
    volatile static boolean save_working = false;
    volatile static boolean sent_working = false;
    
    public static void main(String[] args) {
        
         int rodadas = 0;
        
         while (rodadas < 3) {
        
            if (save_working == false && sent_working == false){
                process();
                saveToDisk();
                sendToNet();
                rodadas++;
            }
         }
        
    }
}
