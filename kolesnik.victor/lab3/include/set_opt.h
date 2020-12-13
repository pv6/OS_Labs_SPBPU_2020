#ifndef SET_OPT_H_INCLUDED__
#define SET_OPT_H_INCLUDED__
#include "../include/set.h"
#include <pthread.h>
#include <stddef.h>
#include <list>


template <typename T>
class Set_Opt : public Set<T> {
    public:
        Set_Opt();

        bool add(T& item) override;
        bool remove(T& item) override;
        bool contains(T& item) override;
        bool empty() override;

        ~Set_Opt();        

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
        std::list<Node *> _nodes;
        pthread_mutex_t _nodes_lock;

        bool _validate(Set_Opt<T>::Node *pred, Set_Opt<T>::Node *cur);
};

#include "../include/set_opt.hpp"

#endif