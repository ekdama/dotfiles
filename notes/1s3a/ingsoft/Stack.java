import java.util.ArrayList;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Spliterator;
import java.util.function.Consumer;


/**
 *
 * @param <E> elemento dello stack
* */
public class Stack<E> implements Iterable<E> {

public ArrayList<E> elements = new ArrayList<>();

    private class StackIterator implements Iterator{
        private int position;
        public StackIterator(){
            this.position = elements.size()-1;
            /*
            *  Se position si trova alla "fine dello stack" ( o in alto, a seconda di come lo si immagina),
            *  position sarà "l'ultima posizione occupata, quella del prossimo elemento ad uscire dallo stack.
            * */
        }

        @Override
        public boolean hasNext() {
            return position >= 0; // se position è uguale a 0, c'è ancora un elemento, l'ultimo
        }

        @Override
        public Object next() {
            if (!hasNext())
                throw new NoSuchElementException();
            E element = elements.get(position);
            position--;
            return element;
        }
    }

    /**
     *  insert an element
     * @param e the element to insert
     */
    public void push(E e){
        elements.add(e);
    }

    /**
     * push all elements of this stack in stack received in input
     * @param other the stack where to push elements
     */
    public void pushAllIn(Stack<? super E> other){// super: because I have to cast elements of THIS stack in order to insert them in external (input) stack
        for(E e : elements){
            other.push(e);
        }
    }

    /**
     * push all elements of stack received in input, in this stack
     * @param other the stack where to read elements
     */
    public void pushAll(Stack<? extends E> other){// extends: because I have to cast elements of external (input) stack, in order to insert them in THIS stack
        for(E e : other.elements){
            push(e);
        }
    }

    /**
     *  extract one element
     * @return the element extracted
     */
    public E pop()
    {
        try{
            return elements.remove(elements.size()-1);
        }catch(IndexOutOfBoundsException e){
            throw new NoElementException(e);
        }
    }

    /**
     * checks if the stack is empty
     * @return true if the stack is empty
     */
    public boolean isEmpty(){
        return elements.isEmpty();
    }

    @Override
    public Iterator<E> iterator() {
        return new StackIterator();
    }


}
