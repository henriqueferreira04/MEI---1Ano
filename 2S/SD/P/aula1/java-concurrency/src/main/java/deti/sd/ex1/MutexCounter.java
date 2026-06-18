package deti.sd.ex1;

import java.util.concurrent.locks.ReentrantLock;

public class MutexCounter implements Counter {
    private int value = 0;
    private final ReentrantLock lock = new ReentrantLock();

    @Override
    public void increment() {
        lock.lock();
        try {
            value++;
        } finally {            
            lock.unlock();
        }
    }

    @Override
    public int getValue() {
        lock.lock();

        int val;
        try{
            val = value;
            
        } finally {
            lock.unlock();
        }
        
        return val;
    }

    @Override
    public boolean incrementIfEven() {
        lock.lock();
        boolean isEven = false;
        try{
            if (value % 2 == 0) {
                isEven = true;
            }
        } finally {
            lock.unlock();
        }
        return isEven;
    }
}
