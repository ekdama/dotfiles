public class ContoInBancaSync {
    private Conto conto = new Conto();

    public synchronized void increment() {
            while(conto.value >= 10) {
                try {
                    wait();
                } catch (InterruptedException e) {

                }
            }
            conto.value++;
            if(conto.value == 1)
                notifyAll();

    }


    public synchronized void decrement() {
            while(conto.value <= 0) {
                try {
                    wait();
                } catch (InterruptedException e) {

                }
            }
            conto.value--;
            if(conto.value == 9)
                notifyAll();
        }


    public int getCount() {
        return conto.value;
    }

    public void reset() { conto.value = 0; }
}
