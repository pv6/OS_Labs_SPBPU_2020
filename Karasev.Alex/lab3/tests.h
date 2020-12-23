#ifndef TESTS_H
#define TESTS_H
#include "set.h"
#include <vector>

enum class SetType {
    COARSE,
    LAZY
};

enum class TestType {
    SEQUENTIAL,
    RANDOM
};

enum class Executor {
    READER,
    WRITER
};

struct Arg_t {
    int idx;
    int step;
    int num_steps;
    std::vector<int>* items;
    std::vector<bool>* found;
    Set<int>* set;
};


void WritersTest(SetType set_type, TestType test_type, int num_records, int num_writers);
void ReadersTest(SetType set_type, TestType test_type, int num_records, int num_readers);
void TotalTest(SetType set_type, TestType test_type, int readers_mult, int num_readers, int writers_mult, int num_writers);
#endif //TESTS_H