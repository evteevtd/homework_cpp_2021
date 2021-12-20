#include <gtest/gtest.h>
#include <iostream>
#include <cstdio>

#include "mipt/set/set.h"
#include "mipt/log/log.h"

TEST(FlatTest, SimpleInclude) {
    mipt::FlatSet<int> test_set("./journal", mipt::OptimizeLevel::NONE);
    ASSERT_EQ(test_set.is_empty(), true);
    ASSERT_EQ(test_set.size(), 0);

    ASSERT_EQ(test_set.insert(0), true);
    ASSERT_EQ(test_set.insert(1), true);
    
    ASSERT_EQ(test_set.is_empty(), false);

    ASSERT_EQ(test_set.insert(1), false);
    ASSERT_EQ(test_set.erase(1), true);
    ASSERT_EQ(test_set.size(), 1);

    std::filesystem::remove_all("./journal");
}

void multitest(int times, int questions, int max_value, mipt::OptimizeLevel optlevel) {
    std::set<int> my_set;
    for (int t = 0; t < times; ++t) {
        mipt::FlatSet<int> test_set("./journal", optlevel);
        for (int i = 0; i < questions; ++i) {
            int type = rand() % 5;
            int x = rand() % max_value - max_value / 2;
            if (rand() % 2 && !my_set.empty()) {
                auto tmp = my_set.begin();
                std::advance(tmp, rand() % my_set.size());
                x = *tmp;
            }
            if (type <= 2) { ASSERT_EQ(my_set.insert(x).second, test_set.insert(x)); }
            if (type == 3) { ASSERT_EQ(my_set.erase(x), test_set.erase(x)); }
            if (type == 4) { ASSERT_EQ(my_set.count(x), test_set.exists(x)); }
        }
    }
    std::filesystem::remove_all("./journal");
}

TEST(FlatTest, DataLoss) {
    multitest(10, 1000, 1000, mipt::OptimizeLevel::NONE);   
}

TEST(FlatTest, StingOptimize) {
    multitest(10, 1000, 1000, mipt::OptimizeLevel::STRONG);   
}

TEST(FlatTest, gInfoLevelCoverage) {
    set_loglevel(InfoLevel::VERBOSE);
    multitest(2, 300, 1000, mipt::OptimizeLevel::NONE);
    multitest(2, 300, 1000, mipt::OptimizeLevel::STRONG);
    set_loglevel(InfoLevel::DEFAULT);
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    std::filesystem::remove_all("./journal");
    return RUN_ALL_TESTS();
}
