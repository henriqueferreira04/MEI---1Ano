package deti.sd.ex1;

import java.util.concurrent.atomic.AtomicInteger;

public class AtomicCounter implements Counter {
    private final AtomicInteger value = new AtomicInteger(0);

    @Override
    public void increment() {
        value.incrementAndGet();
    }

    @Override
    public int getValue() {
        return value.get();
    }

    @Override
    public boolean incrementIfEven() {
        int val;
        do {
            val = value.get();
            if (val % 2 != 0) {
                return false;
            }
        }while(!value.compareAndSet(val, val++));
        
        return true;
    }
}
