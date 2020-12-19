#include <climits>
#include <functional>

template<typename T>
CoarseList<T>::CoarseList() {
    T t1 = T();
    T t2 = T();
    head = new Node(t1, INT_MIN);
    head->next = new Node(t2, INT_MAX);
    lock = PTHREAD_MUTEX_INITIALIZER;
}

template<typename T>
bool CoarseList<T>::add(T item) {
    Node* pred, *curr;
    int key = std::hash<T>{}(item);
    pthread_mutex_lock(&lock);
    pred = head;
    curr = pred->next;
    while (curr->key < key) {
        pred = curr;
        curr = curr->next;
    }
    if (key == curr->key) {
        pthread_mutex_unlock(&lock);
        return false;
    } else {
        Node* node = new Node(item);
        node->next = curr;
        pred->next = node;
        pthread_mutex_unlock(&lock);
        return true;
    }
}

template<typename T>
bool CoarseList<T>::remove(T item) {
    Node* pred, *curr;
    int key = std::hash<T>{}(item);
    pthread_mutex_lock(&lock);
    pred = head;
    curr = pred->next;
    while (curr->key < key) {
        pred = curr;
        curr = curr->next;
    }
    if (key == curr->key) {
        pred->next = curr->next;
        pthread_mutex_unlock(&lock);
        return true;
    } else {
        pthread_mutex_unlock(&lock);
        return false;
    }
}

template<typename T>
bool CoarseList<T>::contains(T item) {
    Node* curr;
    int key = std::hash<T>{}(item);
    pthread_mutex_lock(&lock);
    curr = head->next;
    while (curr->key < key) {
        curr = curr->next;
    }
    pthread_mutex_unlock(&lock);
    return key == curr->key;

}

template <typename T>
bool CoarseList<T>::empty() {
    return head->next->next == nullptr;
}

template <typename T>
CoarseList<T>::~CoarseList() {
    delete head->next;
    delete head;
    pthread_mutex_destroy(&lock);
}