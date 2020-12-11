#include <stddef.h>
#include <pthread.h>
#include <functional>

template <typename T>
Set_Fine<T>::Node::Node(T& item, size_t key, Node *next)
    : _next(next), _key(key), _item(item)
{
    _lock = PTHREAD_MUTEX_INITIALIZER;
}
template <typename T>
void Set_Fine<T>::Node::lock() {
    pthread_mutex_lock(&_lock);
}
template <typename T>
void Set_Fine<T>::Node::unlock() {
    pthread_mutex_unlock(&_lock);
}
template <typename T>
T Set_Fine<T>::Node::getItem() {
    return _item;
}
template <typename T>
size_t Set_Fine<T>::Node::getKey() {
    return _key;
}
template <typename T>
void Set_Fine<T>::Node::setNext(Set_Fine<T>::Node* next) {
    _next = next;
}
template <typename T>
typename Set_Fine<T>::Node *Set_Fine<T>::Node::getNext() {
    return _next;
}
template <typename T>
Set_Fine<T>::Node::~Node() {
    pthread_mutex_destroy(&_lock);
}

template <typename T>
Set_Fine<T>::Set_Fine() {
    T t1 = T();
    T t2 = T();
    _head = new Set_Fine<T>::Node(t1, 0, nullptr);
    _head->setNext(new Set_Fine<T>::Node(t2, SIZE_MAX, nullptr));
}
template <typename T>
bool Set_Fine<T>::add(T& item) {
    size_t key = std::hash<T>()(item);

    _head->lock();
    Set_Fine<T>::Node *pred = _head;
    Set_Fine<T>::Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    if (cur->getKey() == key) {
        pred->unlock();
        cur->unlock();
        return false;
    } else {
        Set_Fine<T>::Node *newNode = new Set_Fine<T>::Node(item, key, cur);
        pred->setNext(newNode);
        pred->unlock();
        cur->unlock();
        return true;
    }
}
template <typename T>
bool Set_Fine<T>::remove(T& item) {
    size_t key = std::hash<T>()(item);

    _head->lock();
    Set_Fine<T>::Node *pred = _head;
    Set_Fine<T>::Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    if (cur->getKey() == key) {
        pred->setNext(cur->getNext());
        cur->unlock();
        delete cur;
        pred->unlock();
        return true;
    } else {
        pred->unlock();
        cur->unlock();
        return false;
    }
}
template <typename T>
bool Set_Fine<T>::contains(T& item) {
    size_t key = std::hash<T>()(item);
    _head->lock();
    Set_Fine<T>::Node *pred = _head;
    Set_Fine<T>::Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    bool contains_cond = cur->getKey() == key;
    pred->unlock();
    cur->unlock();
    return contains_cond;
}
template <typename T>
bool Set_Fine<T>::empty() {
    if (_head->getNext()->getNext() == nullptr) {
        return true;
    } else {
        return false;
    }
}
template <typename T>
Set_Fine<T>::~Set_Fine() {
    Set_Fine<T>::Node *pred = _head;
    Set_Fine<T>::Node *cur = pred->getNext();
    while (cur != nullptr) {
        delete pred;
        pred = cur;
        cur = cur->getNext();
    }
    delete pred;
}