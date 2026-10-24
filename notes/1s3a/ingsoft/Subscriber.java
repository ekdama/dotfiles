public class Subscriber extends Thread{
    ContoInBanca contoPaga;
    int countReset;
    static int subscribeCounter;
    private int myId;

    public Subscriber(ContoInBanca c){
        contoPaga = c;
        myId = subscribeCounter++;
    }
    public void run(){
        System.out.println("Started Subscriber counter thread " + myId);

        while(true) {
            contoPaga.valore --;
            System.out.println("ho appena SOTTRATTO, il valore del counter è " + Integer.toString(contoPaga.valore));
            if(contoPaga.valore <= 0){
                throw new RuntimeException("valore troppo basso");
            }
        }
    }
    public void decrementa(){
        contoPaga.valore--;
    }
    
    public void reset(){
        contoPaga.valore = 0;
    }
}
