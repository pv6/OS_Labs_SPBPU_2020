#include <string>
#include "tests.h"

int main(int argc, char* argv[]) {
    if (argc < 9) {
        return 1;
    }
    int num_w = std::stoi(argv[1]);     // writers
    int num_wm = std::stoi(argv[2]);    // writers multiplier
    int num_r = std::stoi(argv[3]);     // readers
    int num_rm = std::stoi(argv[4]);    // readers multiplier
    int num_w_t = std::stoi(argv[5]);   // writers totalTest
    int num_wm_t = std::stoi(argv[6]);  // writers totalTest multiplier
    int num_r_t = std::stoi(argv[7]);   // readers totalTest
    int num_rm_t = std::stoi(argv[8]);  // readers totalTest multiplier

    // Coarse set testing
    //      Writers
    WritersTest(SetType::COARSE, TestType::SEQUENTIAL, num_wm, num_w);
    WritersTest(SetType::COARSE, TestType::RANDOM, num_wm, num_w);
    //      Readers
    ReadersTest(SetType::COARSE, TestType::SEQUENTIAL, num_rm, num_r);
    ReadersTest(SetType::COARSE, TestType::RANDOM, num_rm, num_r);

    // Lazy set testing
    //      Writers
    WritersTest(SetType::LAZY, TestType::SEQUENTIAL, num_wm, num_w);
    WritersTest(SetType::LAZY, TestType::RANDOM, num_wm, num_w);
    //      Readers
    ReadersTest(SetType::LAZY, TestType::SEQUENTIAL, num_rm, num_r);
    ReadersTest(SetType::LAZY, TestType::RANDOM, num_rm, num_r);

    // Total test
    //      Coarse
    TotalTest(SetType::COARSE, TestType::SEQUENTIAL, num_rm_t, num_r_t, num_wm_t, num_w_t);
    TotalTest(SetType::COARSE, TestType::RANDOM, num_rm_t, num_r_t, num_wm_t, num_w_t);
    //      Lazy
    TotalTest(SetType::LAZY, TestType::SEQUENTIAL, num_rm_t, num_r_t, num_wm_t, num_w_t);
    TotalTest(SetType::LAZY, TestType::RANDOM, num_rm_t, num_r_t, num_wm_t, num_w_t);

    return 0;
}