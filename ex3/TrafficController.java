// matheus felipe rodrigues - 93943

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class TrafficController {
    final Lock lock = new ReentrantLock();
    final Condition condition = lock.newCondition();

    int state = 0; // 0 - vazia, 1 - cheia
   
    public void enterLeft();
    public void enterRight();
    public void leaveLeft();
    public void leaveRight();

    public void enterLeft(){
        lock.lock();

        try {
            if ((state != 0)) condition.await();
        } catch (InterruptedException ie) {
            System.err.println(ie.toString());
        }

        state = 1;
        lock.unlock();
    }

    public void enterRight(){
        lock.lock();

        try {ie
            if ((state != 0)) condition.await();
        } catch (InterruptedException ie) {
            System.err.println(ie.toString());
        }

        state = 1;
        lock.unlock();
    }

    public void leaveLeft(){
        lock.lock();
        state = 0;
        condition.signal();
        lock.unlock();
    }

    public void leaveRight(){
        lock.lock();
        state = 0;
        condition.signal();
        lock.unlock();
    }
}