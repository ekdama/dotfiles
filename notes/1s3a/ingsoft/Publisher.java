public class Publisher extends Thread{

    ContoInBanca contoPaga;
    static int publishCounter;

    private int myId;

    public Publisher(ContoInBanca c){
        this.contoPaga=c;

        myId = publishCounter++;
    }

    public void run(){
        System.out.println("Started Publisher counter thread " + myId);

            while(true){
                incrementa();
                System.out.println("ho appena AGGIUNTO, il valore del counter è " + Integer.toString(contoPaga.valore));
                if(contoPaga.valore>=10)
                    throw new RuntimeException("Valore troppo alto");

            }
    }

    public void incrementa(){
        contoPaga.valore++;
    }
}
