#include <stddef.h>
#include <pthread.h>
#include <functional>
#include <list>

template <typename T>
Set_Opt<T>::Node::Node(T& item, size_t key, Node *next)
    : _next(next), _key(key), _item(item)
{
    _lock = PTHREAD_MUTEX_INITIALIZER;
}
template <typename T>
void Set_Opt<T>::Node::lock() {
    pthread_mutex_lock(&_lock);
}
template <typename T>
void Set_Opt<T>::Node::unlock() {
    pthread_mutex_unlock(&_lock);
}
template <typename T>
T Set_Opt<T>::Node::getItem() {
    return _item;
}
template <typename T>
size_t Set_Opt<T>::Node::getKey() {
    return _key;
}
template <typename T>
void Set_Opt<T>::Node::setNext(Set_Opt<T>::Node* next) {
    _next = next;
}
template <typename T>
typename Set_Opt<T>::Node *Set_Opt<T>::Node::getNext() {
    return _next;
}
template <typename T>
Set_Opt<T>::Node::~Node() {
    pthread_mutex_destroy(&_lock);
}

template <typename T>
Set_Opt<T>::Set_Opt() {
    T t1 = T();
    T t2 = T();
    _head = new Set_Opt<T>::Node(t1, 0, nullptr);
    _nodes_lock = PTHREAD_MUTEX_INITIALIZER;
    _head->setNext(new Set_Opt<T>::Node(t2, SIZE_MAX, nullptr));
    _nodes.push_back(_head);
    _nodes.push_back(_head->getNext());
}
template <typename T>
bool Set_Opt<T>::add(T& item) {
    size_t key = std::hash<T>()(item);

    Set_Opt<T>::Node *pred;
    Set_Opt<T>::Node *cur;
    while (true) {
        pred = _head;
        cur = pred->getNext();
        while (cur->getKey() < key) {
            pred = cur;
            cur = cur->getNext();
        }
        pred->lock();
        cur->lock();
        if (_validate(pred, cur)) {
            if (cur->getKey() == key) {
                pred->unlock();
                cur->unlock();
                return false;
            } else {
                Set_Opt<T>::Node *newNode = new Set_Opt<T>::Node(item, key, cur);
                pred->setNext(newNode);
                pthread_mutex_lock(&_nodes_lock);
                _nodes.push_back(newNode);
                pthread_mutex_unlock(&_nodes_lock);
                pred->unlock();
                cur->unlock();
                return true;
            }
        } else {
            pred->unlock();
            cur->unlock();
        }
    }
}
template <typename T>
bool Set_Opt<T>::remove(T& item) {
    size_t key = std::hash<T>()(item);

    Set_Opt<T>::Node *pred;
    Set_Opt<T>::Node *cur;
    while (true) {
        pred = _head;
        cur = pred->getNext();
        while (cur->getKey() < key) {
            pred = cur;
            cur = cur->getNext();
        }
        pred->lock();
        cur->lock();
        if (_validate(pred, cur)) {
            if (cur->getKey() == key) {
                pred->setNext(cur->getNext());
                pred->unlock();
                cur->unlock();
                return true;
            } else {
                pred->unlock();
                cur->unlock();
                return false;
            }
        } else {
            pred->unlock();
            cur->unlock();
        }
    }
}
template <typename T>
bool Set_Opt<T>::contains(T& item) {
    size_t key = std::hash<T>()(item);

    Set_Opt<T>::Node *pred;
    Set_Opt<T>::Node *cur;
    while (true) {
        pred = _head;
        cur = pred->getNext();
        while (cur->getKey() < key) {
            pred = cur;
            cur = cur->getNext();
        }
        pred->lock();
        cur->lock();
        if (_validate(pred, cur)) {
            bool contains_cond = cur->getItem() == item;
            pred->unlock();
            cur->unlock();
            return contains_cond;
        } else {
            pred->unlock();
            cur->unlock();
        }
    }
}
template <typename T>
bool Set_Opt<T>::empty() {
    if (_head->getNext()->getNext() == nullptr) {
        return true;
    } else {
        return false;
    }
}
template <typename T>
Set_Opt<T>::~Set_Opt() {
    for (Set_Opt<T>::Node *node: _nodes) {
        delete node;
    }
    pthread_mutex_destroy(&_nodes_lock);
}
template <typename T>
bool Set_Opt<T>::_validate(Set_Opt<T>::Node *pred, Set_Opt<T>::Node *cur) {
    Set_Opt<T>::Node *node = _head;
    while (node->getKey() <= pred->getKey()) {
        node = node->getNext();
    }
    return node == pred;
}