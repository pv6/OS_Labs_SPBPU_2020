#ifndef LAZY_H
#define LAZY_H

#include "set.h"
#include <pthread.h>

template <typename T>
class LazyList : public Set<T> {
public:
    LazyList();
    
    bool add(T item);

    bool remove(T item);
    
    bool contains(T item);

    bool empty();

    virtual ~LazyList();

private:
    class Node {
    public:
        T item;
        int key;
        Node* next;
        pthread_mutex_t _lock{};
        bool marked = false;

        explicit Node(T val, int code) : item{val}, key{code}, next{nullptr} { _lock = PTHREAD_MUTEX_INITIALIZER; };
        explicit Node(T val) : item{val}, key{(int)std::hash<T>{}(val)}, next{nullptr} { _lock = PTHREAD_MUTEX_INITIALIZER; };
        void lock() {
            pthread_mutex_lock(&_lock);
        }
        void unlock() {
            pthread_mutex_unlock(&_lock);
        }
        ~Node() {
            pthread_mutex_destroy(&_lock);
        }
    };

    bool validate(Node* pred, Node* curr);

    Node* head;
};

#include "Lazy.hpp"
#endif //LAZY_H