#include "../include/tests.h"
#include "../include/set.h"
#include "../include/set_fine.h"
#include "../include/set_opt.h"
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>


using namespace Tests;

namespace {

    struct Thread_Write {
        Set<int> *set;
        std::vector<int> *items;
        int write_num;
        int thread_idx;
    };

    void *_write(void *write_info) {
        Thread_Write *info = (Thread_Write *)write_info;
        for (int i = 0; i < info->write_num; ++i) {
            info->set->add(info->items->at(info->thread_idx * info->write_num + i));
        }
        return nullptr;
    }

    void _createThreadsInfoWrite(std::vector<Thread_Write> &infos, Set<int> *set, std::vector<int> &items, int write_num, int thread_num) {
        for (int i = 0; i < thread_num; ++i) {
            infos[i].set = set;
            infos[i].items = &items;
            infos[i].write_num = write_num;
            infos[i].thread_idx = i;
        }
    }

    void _createThreadsWrite(std::vector<pthread_t> &threads, std::vector<Thread_Write> &infos, int thread_num) {
        for (int i = 0; i < thread_num; ++i) {
            pthread_create(&threads[i], nullptr, _write, &infos[i]);
        }
    }

    void _printWriterTests(Test_Type test_type, Set_Type set_type, int threads_num, int write_num) {
        std::cout << "Writers Tests with ";
        switch (set_type) {
            case Set_Type::SET_FINE:
                std::cout << "Fine-Grained Sync";
                break;
            case Set_Type::SET_OPT:
                std::cout << "Optimistic Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << " and ";
        switch (test_type) {
            case Test_Type::FIXED:
                std::cout << "Fixed Data";
                break;
            case Test_Type::RANDOM:
                std::cout << "Random Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << ". Thread Number: " << threads_num << ". Items to write by one thread: " << write_num << std::endl;
    }

    void _printWriterTestResults(bool result, int time) {
        std::cout << "Tests ";
        if (result) {
            std::cout << "passed: ";
        } else {
            std::cout << "failed: ";
        }
        std::cout << "time: " << time << " milliseconds" << std::endl;
    }

    struct Thread_Read {
        Set<int> *set;
        std::vector<int> *items;
        std::vector<bool> *items_got;
        int read_num;
        int thread_idx;
    };

    void *_read(void *read_info) {
        Thread_Read *info = (Thread_Read *)read_info;
        for (int i = 0; i < info->read_num; ++i) {
            if (info->set->remove(info->items->at(info->thread_idx * info->read_num + i))) {
                info->items_got->at(info->thread_idx * info->read_num + i) = true;
            }
        }
        return nullptr;
    }

    void _createThreadsInfoRead(std::vector<Thread_Read> &infos, std::vector<bool> &items_got, Set<int> *set, std::vector<int> &items, int read_num, int thread_num) {
        for (int i = 0; i < thread_num; ++i) {
            infos[i].set = set;
            infos[i].items = &items;
            infos[i].items_got = &items_got;
            infos[i].read_num = read_num;
            infos[i].thread_idx = i;
        }
    }

    void _createThreadsRead(std::vector<pthread_t> &threads, std::vector<Thread_Read> &infos, int thread_num) {
        for (int i = 0; i < thread_num; ++i) {
            pthread_create(&threads[i], nullptr, _read, &infos[i]);
        }
    }

    void _printReaderTests(Test_Type test_type, Set_Type set_type, int threads_num, int read_num) {
        std::cout << "Reader Tests with ";
        switch (set_type) {
            case Set_Type::SET_FINE:
                std::cout << "Fine-Grained Sync";
                break;
            case Set_Type::SET_OPT:
                std::cout << "Optimistic Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << " and ";
        switch (test_type) {
            case Test_Type::FIXED:
                std::cout << "Fixed Data";
                break;
            case Test_Type::RANDOM:
                std::cout << "Random Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << ". Thread Number: " << threads_num << ". Items to read by one thread: " << read_num << std::endl;
    }

    void _printReaderTestResults(bool result, int time) {
        std::cout << "Tests ";
        if (result) {
            std::cout << "passed: ";
        } else {
            std::cout << "failed: ";
        }
        std::cout << "time: " << time << " milliseconds" << std::endl;
    }

    void _printGeneralTests(Test_Type test_type, Set_Type set_type, int threads_read_num, int read_num, int threads_write_num, int write_num) {
        std::cout << "General Tests with ";
        switch (set_type) {
            case Set_Type::SET_FINE:
                std::cout << "Fine-Grained Sync";
                break;
            case Set_Type::SET_OPT:
                std::cout << "Optimistic Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << " and ";
        switch (test_type) {
            case Test_Type::FIXED:
                std::cout << "Fixed Data";
                break;
            case Test_Type::RANDOM:
                std::cout << "Random Sync";
                break;
            default:
                std::cout << "Unknown Sync";
                break;
        }
        std::cout << std::endl;
        std::cout << "Write Thread Number: " << threads_write_num << ". Items to write by one thread: " << write_num << std::endl;
        std::cout << "Read Thread Number: " << threads_read_num << ". Items to read by one thread: " << read_num << std::endl;
    }

    void _printGeneralTestResults(bool result, int time) {
        std::cout << "Tests ";
        if (result) {
            std::cout << "passed: ";
        } else {
            std::cout << "failed: ";
        }
        std::cout << "time: " << time << " milliseconds" << std::endl;
    }

    Set<int> *_createSet(Set_Type set_type) {
        switch (set_type) {
            case Set_Type::SET_FINE:
                return new Set_Fine<int>();
            case Set_Type::SET_OPT:
                return new Set_Opt<int>();
            default:
                return nullptr;
        }
    }

    void _createData(Test_Type data_type, std::vector<int> &array, int threads_num, int write_num) {
        array.resize(threads_num * write_num);
        int n = 0;
        switch (data_type) {
            case Test_Type::FIXED:
                for (int i = 0; i < write_num; ++i) {
                    for (int j = 0; j < threads_num; ++j) {
                        array[i + j * write_num] = n++;
                    }
                }
                break;
            case Test_Type::RANDOM:
                srand(threads_num * write_num);
                for (int & item: array) {
                    item = rand();
                }
                break;
            default:
                return;
        }
    }

    void _createDataGot(std::vector<bool> &array, int threads_num, int read_num) {
        array.resize(threads_num * read_num);
        for (int i = 0; i < read_num; ++i) {
            for (int j = 0; j < threads_num; ++j) {
                array[i + j * read_num] = false;
            }
        }
    }

    void _joinThreads(std::vector<pthread_t> threads) {
        for (pthread_t thread: threads) {
            pthread_join(thread, nullptr);
        }
    }

    bool _checkWriteTest(Set<int> *set, std::vector<int> items) {
        for (int item: items) {
            if (!set->contains(item)) {
                return false;
            }
        }
        return true;
    }

    bool _checkReadTest(Set<int> *set, std::vector<bool> items_got) {
        if (!set->empty()) {
            return false;
        }
        for (bool item: items_got) {
            if (!item) {
                return false;
            }
        }
        return true;
    }
};


void Tests::writerTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_num, int write_num) {
    Set<int> *set;
    std::vector<int> items;
    std::vector<pthread_t> threads;
    std::vector<Thread_Write> infos;
    bool test = true;
    double time = 0.0f;

    threads.resize(threads_num);
    infos.resize(threads_num);

    _printWriterTests(test_type, set_type, threads_num, write_num);

    for (int i = 0; i < tests_num; ++i) {
        set = _createSet(set_type);
        _createData(test_type, items, threads_num, write_num);
        _createThreadsInfoWrite(infos, set, items, write_num, threads_num);

        auto begin = std::chrono::high_resolution_clock::now();
        _createThreadsWrite(threads, infos, threads_num);
        _joinThreads(threads);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

        time += elapsed.count();

        test &= _checkWriteTest(set, items);

        delete set;
    }    
    _printWriterTestResults(test, time / tests_num);
}
void Tests::readerTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_num, int read_num) {
    Set<int> *set;
    std::vector<int> items;
    std::vector<pthread_t> threads;
    std::vector<Thread_Read> infos;
    std::vector<bool> items_got;
    bool test = true;
    double time = 0.0f;

    threads.resize(threads_num);
    infos.resize(threads_num);

    _printReaderTests(test_type, set_type, threads_num, read_num);

    for (int i = 0; i < tests_num; ++i) {
        set = _createSet(set_type);
        _createData(test_type, items, threads_num, read_num);
        _createDataGot(items_got, threads_num, read_num);
        _createThreadsInfoRead(infos, items_got, set, items, read_num, threads_num);

        auto begin = std::chrono::high_resolution_clock::now();
        _createThreadsRead(threads, infos, threads_num);
        _joinThreads(threads);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

        time += elapsed.count();

        test &= _checkReadTest(set, items_got);

        delete set;
    }    
    _printReaderTestResults(test, time / tests_num);
}
void Tests::generalTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_read_num, int threads_write_num, int read_num, int write_num) {
    if (read_num * threads_read_num != write_num * threads_write_num) {
        return;
    }
    Set<int> *set;
    std::vector<int> items;
    std::vector<pthread_t> threads_read;
    std::vector<Thread_Read> infos_read;
    std::vector<pthread_t> threads_write;
    std::vector<Thread_Write> infos_write;
    std::vector<bool> items_got;
    bool test = true;
    double time = 0.0f;

    threads_read.resize(threads_read_num);
    infos_read.resize(threads_read_num);
    threads_write.resize(threads_write_num);
    infos_write.resize(threads_write_num);

    _printGeneralTests(test_type, set_type, threads_read_num, read_num, threads_write_num, write_num);

    for (int i = 0; i < tests_num; ++i) {
        set = _createSet(set_type);
        _createData(test_type, items, threads_write_num, write_num);
        _createDataGot(items_got, threads_write_num, write_num);
        _createThreadsInfoRead(infos_read, items_got, set, items, read_num, threads_read_num);
        _createThreadsInfoWrite(infos_write, set, items, write_num, threads_write_num);

        auto begin = std::chrono::high_resolution_clock::now();
        _createThreadsWrite(threads_write, infos_write, threads_write_num);
        _createThreadsRead(threads_read, infos_read, threads_read_num);
        _joinThreads(threads_write);
        _joinThreads(threads_read);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

        time += elapsed.count();

        test &= _checkReadTest(set, items_got);

        delete set;
    }    
    _printGeneralTestResults(test, time / tests_num);
}