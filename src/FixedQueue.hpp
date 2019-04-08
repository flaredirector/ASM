/**
 * Flare Director
 * ASM
 * FixedQueue.hpp
 * 
 * This file contains the class definition for the FixedQueue class
 * which is a data structure that implements a fixed length FIFO queue.
 */

#include <deque>

class FixedQueue {
    public:
    FixedQueue(int s);
    void push(int data);
    std::deque<int> get();
    bool full();
    void print();
    int average();

    private:
    int size;
    std::deque<int> d;
};