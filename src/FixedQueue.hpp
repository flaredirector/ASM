#include <deque>
#include <iostream>

class FixedQueue {
    public:
    FixedQueue(int s) {
        this->size = s;
    }

    void push(int data) {
        if (this->d.size() == this->size)
            this->d.pop_back();
        this->d.push_front(data);
    }

    std::deque<int> get() {
        return this->d;
    }

    bool full() {
        return this->d.size() == this->size;
    }

    void print() {
        std::deque<int> copy = this->d;
        while (!copy.empty()) {
            std::cout << copy.front() << " ";
            copy.pop_front();
        }

        std::cout << std::endl;
    }

    int average() {
        int total = 0;
        std::deque<int> copy = this->d;
        while (!copy.empty()) {
            total += copy.front();
            copy.pop_front();
        }
        return (int)(total / this->size);
    }

    private:
    int size;
    std::deque<int> d;
};