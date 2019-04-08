/**
 * Flare Director
 * ASM
 * FixedQueue.hpp
 * 
 * This file contains the implementaion details for the FixedQueue
 * class.
 */

#include "FixedQueue.hpp"
#include <iostream>

using namespace std;

/**
 * FixedQueue
 ** Instantiates new FixedQueue instance with passed in size.
 * @param {s} The size of the queue to initialize.
 */
FixedQueue::FixedQueue(int s) {
    this->size = s;
}

/**
 * push
 ** Pushes data onto the queue.
 * @param {data} The data to push onto the queue.
 */
void FixedQueue::push(int data) {
    if (this->d.size() == this->size)
        this->d.pop_back();
    this->d.push_front(data);
}

/**
 * get
 ** Provides access to the queue
 */
deque<int> FixedQueue::get() {
    return this->d;
}

/**
 * full
 ** Returns whether the queue is full or not.
 */
bool FixedQueue::full() {
    return this->d.size() == this->size;
}

/**
 * print
 ** Prints the contents of the queue
 */
void FixedQueue::print() {
    deque<int> copy = this->d;
    while (!copy.empty()) {
        cout << copy.front() << " ";
        copy.pop_front();
    }

    cout << endl;
}

/**
 * average
 ** Returns the average of the values in the queue. 
 */
int FixedQueue::average() {
    int total = 0;
    deque<int> copy = this->d;
    while (!copy.empty()) {
        total += copy.front();
        copy.pop_front();
    }
    return (int)(total / this->size);
}
