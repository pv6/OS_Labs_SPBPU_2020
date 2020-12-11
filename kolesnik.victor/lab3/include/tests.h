#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED


namespace Tests {
    enum Set_Type {
        SET_FINE,
        SET_OPT
    };

    enum Test_Type {
        RANDOM,
        FIXED
    };

    void writerTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_num, int write_num);
    void readerTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_num, int read_num);
    void generalTest(Set_Type set_type, Test_Type test_type, int tests_num, int threads_read_num, int threads_write_num, int read_num, int write_num);
};

#endif