#include "../include/tests.h"
#include <iostream>

using namespace Tests;

int main(int argc, char *argv[]) {

    if (argc != 10) {
        return -1;
    }

    int tests_num = std::stoi(argv[1]);

    int reader_test_threads_num = std::stoi(argv[2]);
    int reader_test_read_num = std::stoi(argv[3]);

    int writer_test_threads_num = std::stoi(argv[4]);
    int writer_test_write_num = std::stoi(argv[5]);

    int general_test_reader_threads_num = std::stoi(argv[6]);
    int general_test_read_num = std::stoi(argv[7]);
    int general_test_writer_threads_num = std::stoi(argv[8]);
    int general_test_write_num = std::stoi(argv[9]);


    readerTest(Set_Type::SET_FINE, Test_Type::FIXED, tests_num, reader_test_threads_num, reader_test_read_num);
    readerTest(Set_Type::SET_FINE, Test_Type::RANDOM, tests_num, reader_test_threads_num, reader_test_read_num);

    readerTest(Set_Type::SET_OPT, Test_Type::FIXED, tests_num, reader_test_threads_num, reader_test_read_num);
    readerTest(Set_Type::SET_OPT, Test_Type::RANDOM, tests_num, reader_test_threads_num, reader_test_read_num);


    writerTest(Set_Type::SET_FINE, Test_Type::FIXED, tests_num, writer_test_threads_num, writer_test_write_num);
    writerTest(Set_Type::SET_FINE, Test_Type::RANDOM, tests_num, writer_test_threads_num, writer_test_write_num);

    writerTest(Set_Type::SET_OPT, Test_Type::FIXED, tests_num, writer_test_threads_num, writer_test_write_num);
    writerTest(Set_Type::SET_OPT, Test_Type::RANDOM, tests_num, writer_test_threads_num, writer_test_write_num);


    generalTest(Set_Type::SET_FINE, Test_Type::FIXED, tests_num, general_test_reader_threads_num, general_test_writer_threads_num, general_test_read_num, general_test_write_num);
    generalTest(Set_Type::SET_FINE, Test_Type::RANDOM, tests_num, general_test_reader_threads_num, general_test_writer_threads_num, general_test_read_num, general_test_write_num);

    generalTest(Set_Type::SET_OPT, Test_Type::FIXED, tests_num, general_test_reader_threads_num, general_test_writer_threads_num, general_test_read_num, general_test_write_num);
    generalTest(Set_Type::SET_OPT, Test_Type::RANDOM, tests_num, general_test_reader_threads_num, general_test_writer_threads_num, general_test_read_num, general_test_write_num);  


    return 0;
}