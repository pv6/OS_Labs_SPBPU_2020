#include <climits>
#include <functional>

template <typename T>
bool LazyList<T>::add(T item) {
    int key = std::hash<T>{}(item);
    while (true) {
        Node* pred = head;
        Node* curr = head->next;
        while (curr->key < key) {
            pred = curr; 
            curr = curr->next;
        }
        pred->lock();
        curr->lock();
        if (validate(pred, curr)) {
            if (curr->key == key) {
                curr->unlock();
                pred->unlock();
                return false;
            } else {
                Node* node = new Node(item);
                node->next = curr;
                pred->next = node;
                curr->unlock();
                pred->unlock();
                return true;
            }
        }
        pred->unlock();
        curr->unlock();
    }
}

template <typename T>
bool LazyList<T>::remove(T item) {
    int key = std::hash<T>{}(item);
    while (true) {
        Node* pred = head;
        Node* curr = head->next;
        while (curr->key < key) {
            pred = curr; 
            curr = curr->next;
        }
        pred->lock();
        curr->lock();
        if (validate(pred, curr)) {
            if (curr->key != key) {
                curr->unlock();
                pred->unlock();
                return false;
            } else {
                curr->marked = true;
                pred->next = curr->next;
                curr->unlock();
                pred->unlock();
                return true;
            }
        }
        pred->unlock();
        curr->unlock();
    }
}

template <typename T>
bool LazyList<T>::contains(T item) {
    int key = std::hash<T>{}(item);
    Node *curr = head;
    while (curr->key < key) {
        curr = curr->next;
    }
    return curr->key == key && !curr->marked;
}

template <typename T>
bool LazyList<T>::validate(Node* pred, Node* curr) {
    return !pred->marked && !curr->marked && pred->next == curr;
}

template<typename T>
LazyList<T>::LazyList() {
    T t1 = T();
    T t2 = T();
    head = new Node(t1, INT_MIN);
    head->next = new Node(t2, INT_MAX);
}

template<typename T>
bool LazyList<T>::empty() {
    return head->next->next == nullptr;
}

template <typename T>
LazyList<T>::~LazyList() {
    Node* curr = head;
    Node* next = head->next;
    while (next != nullptr) {
        Node* to_del = curr;
        curr = next;
        next = next->next;
        delete to_del;
    }
    delete curr;
}