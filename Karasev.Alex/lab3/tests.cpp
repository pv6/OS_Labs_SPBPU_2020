#include "tests.h"
#include "Coarse.h"
#include "Lazy.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

/*
Arg_t::Arg_t(const std::vector<int>& vec, std::vector<bool>& checkList, int index, int num_threads, int n_steps, Set<int>* set) : idx{index}, step{num_threads}, num_steps{n_steps}, set{set} {
    items = new std::vector<int>();
    found = &checkList;
    for (auto n : vec) {
        (*items).push_back(n);
    }
}
*/
void createSet(SetType type, Set<int>** set) {
    std::cout << "Set type: ";
    if (type == SetType::COARSE) {
        std::cout << "Coarse-Grained" << std::endl;
        *set = new CoarseList<int>();
    }
    else {
        std::cout << "Lazy" << std::endl;
        *set = new LazyList<int>();
    }
}

void createArrays(TestType test_type, std::vector<int>& array, std::vector<bool>& found, int size) {
    array.clear();
    found.clear();
    if (test_type == TestType::SEQUENTIAL) {
        std::cout << "Sequential ";
        for (int i = 0; i < size; ++i) {
            array.push_back(i);
            found.push_back(false);
        }
    }
    else {
        std::cout << "Random ";
        for (int i = 0; i < size; ++i) {
            int item = std::rand();
            array.push_back(item);
            found.push_back(false);
        }
    }
    std::cout << "items" << std::endl;
}

void *read(void* args) {
    Arg_t arg = *((Arg_t*)args);
    int pos = arg.idx;
    for (int i = 0, step = arg.step; i < arg.num_steps; i++, pos += step) {
        int item = arg.items->at(pos);
        if (arg.set->contains(item)) {
            arg.found->at(pos) = true;
            std::cout << "Item " << item << " read" << std::endl;
        }
        arg.set->remove(item);

    }
    return nullptr;
}

void *write(void *args) {
    Arg_t arg = *((Arg_t*) args);
    int pos = arg.idx;
    int step = arg.step;
    for (int i = 0; i < arg.num_steps; i++) {
        int item = arg.items->at(pos);
        if (arg.set->add(item)) {
            std::cout << "Item " << item << " added" << std::endl;
        }
        else {
            //std::cout << "Item " << item << " not added" << std::endl;
            //std::cout << pos << std::endl;
        }
        pos += step;
    }
    return nullptr;
}

std::vector<Arg_t> prepareArgs(std::vector<int>& array, std::vector<bool>& found, int num_threads, int num_steps, Set<int>* set) {
    std::vector<Arg_t> args;
    args.resize(num_threads);
    for (int i = 0; i < num_threads; i++) {
        args[i].num_steps = num_steps;
        args[i].items = &array;
        args[i].found = &found;
        args[i].idx = i;
        args[i].set = set;
        args[i].step = num_threads;
    }
    return args;
}

void callExecutor(std::vector<pthread_t>& threads, std::vector<Arg_t> args, int num_threads, Executor exec) {
    void* (*call) (void*);
    if (exec == Executor::READER) {
        call = read;
    }
    else {
        call = write;
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], nullptr, call, &args[i]);
    }
}

bool checkItem(Set<int>* set, int item) {
    bool found = set->contains(item);
    bool del = set->remove(item);
    //std::cout << "Item " << item;
    return (found && del);
    //std::cout << " found" << std::endl;

}

bool checkAllItems(Set<int>* set, std::vector<int>& items) {
    for (auto item : items) {
        if (!checkItem(set, item)) {
            return false;
        }
    }
    return true;
}

void WritersTest(SetType set_type, TestType test_type, int num_records, int num_writers) {
    std::cout << "Writers test:" << std::endl;
    Set<int>* set;
    std::vector<int> array;
    std::vector<bool> found;
    std::vector<pthread_t> threads;
    threads.resize(num_writers);
    int num_tests = num_writers;
    unsigned int time = 0;
    for (int i = 0; i < num_tests; i++) {
        createSet(set_type, &set);
        createArrays(test_type, array, found, num_records * num_writers);
        std::vector<Arg_t> args = prepareArgs(array, found, num_writers, num_records, set);
        unsigned int start_time = clock();
        callExecutor(threads, args, num_writers, Executor::WRITER);
        for (int j = 0; j < num_writers; ++j) {
            pthread_join(threads[j], nullptr);
        }
        unsigned int end_time = clock();
        std::cout << "Writers test ";
        if (!checkAllItems(set, array)) {
            std::cout << "NOT ";
        }
        std::cout << "passed" << std::endl;
        time += end_time - start_time;
        delete set;
    }
    std::cout << "Execution time " << time / num_tests << "ms" << std::endl << std::endl;
}

void fillSet(TestType test_type, Set<int>* set, std::vector<int>& array, std::vector<bool>& found, int size) {
    createArrays(test_type, array, found, size);
    for (int i  = 0; i < size; ++i) {
        set->add(array[i]);
    }
}

bool checkFound(const std::vector<bool>& found) {
    for (auto f : found) {
        if (!f) {
            return false;
        }
    }
    return true;
}

void ReadersTest(SetType set_type, TestType test_type, int num_records, int num_readers) {
    std::cout << "Readers test: " << std::endl;
    Set<int>* set;
    std::vector<int> array;
    std::vector<pthread_t> threads;
    std::vector<bool> found;
    threads.resize(num_readers);
    unsigned int time = 0;
    int num_tests = num_readers;
    for (int i = 0; i < num_tests; i++) {
        createSet(set_type, &set);
        fillSet(test_type, set, array, found, num_records * num_readers);
        std::vector<Arg_t> args = prepareArgs(array, found, num_readers, num_records, set);
        unsigned int start_time = clock();
        callExecutor(threads, args, num_readers, Executor::READER);
        for (int j = 0; j < num_readers; ++j) {
            pthread_join(threads[j], nullptr);
        }
        unsigned int end_time = clock();
        std::cout << "Readers test ";
        if (!set->empty()) { //if (!checkFound(found) || !set->empty()) {
            std::cout << "NOT ";
        }
        std::cout << "passed" << std::endl;
        time += end_time - start_time;
        delete set;
    }
    std::cout << "Execution time " << time / num_tests << "ms" << std::endl << std::endl;
}

void TotalTest(SetType set_type, TestType test_type, int readers_mult, int num_readers, int writers_mult, int num_writers) {
    std::cout << "Total test:\n";
    if (num_readers * readers_mult != num_writers * writers_mult) { return; }
    int num_tests = num_writers * num_readers;
    Set<int>* set;
    std::vector<int> array;
    std::vector<bool> found;
    std::vector<pthread_t> readers_threads;
    readers_threads.resize(num_readers);
    std::vector<pthread_t> writers_threads;
    writers_threads.resize(num_writers);
    unsigned int total_time = 0;
    for (int n = 0; n < num_tests; ++n) {
        createSet(set_type, &set);
        createArrays(test_type, array, found, writers_mult * num_writers);
        std::vector<Arg_t> args = prepareArgs(array, found, num_writers, writers_mult, set);
        unsigned int start_time = clock();
        callExecutor(writers_threads, args, num_writers, Executor::WRITER);
        for (int i = 0; i < num_writers; ++i) {
            pthread_join(writers_threads[i], nullptr);
        }

        callExecutor(readers_threads, args, num_readers, Executor::READER);
        for (int i = 0; i < num_readers; ++i) {
            pthread_join(readers_threads[i], nullptr);
        }
        unsigned int end_time = clock();
        std::cout << "Total test " << n + 1;
        if (!checkFound(found) || !set->empty()) {
            std::cout << " NOT";
        }
        std::cout << " passed" << std::endl;
        total_time += end_time - start_time;
        delete set;
    }
    std::cout << "Execution time " << total_time / num_tests << "ms" << std::endl << std::endl;
}