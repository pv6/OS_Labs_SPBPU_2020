#ifndef COARSE_H
#define COARSE_H
#include "set.h"
#include <pthread.h>

template<typename T>
class CoarseList : public Set<T> {
public:
    CoarseList();
    
    bool add(T item);

    bool remove(T item);
    
    bool contains(T item);

    bool empty();

    virtual ~CoarseList();

private:
    class Node {
    public:
        T item;
        int key;
        Node* next;
        explicit Node(T val, int code) : item{val}, key{code}, next{nullptr} {};
        explicit Node(T val) : item{val}, key{(int)std::hash<T>{}(val)}, next{nullptr} {};
    };

    Node* head{};
    pthread_mutex_t lock{};
};

#include "Coarse.hpp"

#endif //COARSE_H