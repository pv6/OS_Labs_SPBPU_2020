#ifndef SET_FINE_H_INCLUDED__
#define SET_FINE_H_INCLUDED__
#include "../include/set.h"
#include <pthread.h>
#include <stddef.h>


template <typename T>
class Set_Fine : public Set<T> {
    public:
        Set_Fine();

        bool add(T& item) override;
        bool remove(T& item) override;
        bool contains(T& item) override;
        bool empty() override;

        ~Set_Fine();        

    private:
        class Node {
            public:
                Node(T& item, size_t key, Node *next);

                void lock();
                void unlock();
                T getItem();
                size_t getKey();
                void setNext(Node* next);
                Node *getNext();

                ~Node();

            private:
                pthread_mutex_t _lock;
                Node *_next;
                size_t _key;
                T _item;
        };

        Node* _head;
};

#include "../include/set_fine.hpp"
#endif