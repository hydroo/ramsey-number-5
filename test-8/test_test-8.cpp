#include "prereqs.hpp"

#include "test-8.hpp"

using std::cerr;
using std::endl;

using r5::AdjacencyMatrix;

TEST(Test8, ramseyGraphs) {

    ASSERT_EQ((RamseyGraphs<1, 1,  1>::f().size()),   0); ASSERT_EQ((RamseyGraphs<1, 1,  1>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<1, 2,  1>::f().size()),   0); ASSERT_EQ((RamseyGraphs<2, 1,  1>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<1, 3,  1>::f().size()),   0); ASSERT_EQ((RamseyGraphs<3, 1,  1>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<1, 4,  1>::f().size()),   0); ASSERT_EQ((RamseyGraphs<4, 1,  1>::f().size()),   0);

    ASSERT_EQ((RamseyGraphs<2, 2,  2>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<2, 3,  3>::f().size()),   0); ASSERT_EQ((RamseyGraphs<3, 2,  3>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<2, 4,  4>::f().size()),   0); ASSERT_EQ((RamseyGraphs<4, 2,  4>::f().size()),   0);

    ASSERT_EQ((RamseyGraphs<3, 3,  6>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<3, 4,  9>::f().size()),   0); ASSERT_EQ((RamseyGraphs<4, 3,  9>::f().size()),   0);

    ASSERT_EQ((RamseyGraphs<3, 4,  1>::f().size()),   1); ASSERT_EQ((RamseyGraphs<4, 3,  1>::f().size()),   1);
    ASSERT_EQ((RamseyGraphs<3, 4,  2>::f().size()),   2); ASSERT_EQ((RamseyGraphs<4, 3,  2>::f().size()),   2);
    ASSERT_EQ((RamseyGraphs<3, 4,  3>::f().size()),   3); ASSERT_EQ((RamseyGraphs<4, 3,  3>::f().size()),   3);
    ASSERT_EQ((RamseyGraphs<3, 4,  4>::f().size()),   6); ASSERT_EQ((RamseyGraphs<4, 3,  4>::f().size()),   6);
    ASSERT_EQ((RamseyGraphs<3, 4,  5>::f().size()),   9); ASSERT_EQ((RamseyGraphs<4, 3,  5>::f().size()),   9);
    ASSERT_EQ((RamseyGraphs<3, 4,  6>::f().size()),  15); ASSERT_EQ((RamseyGraphs<4, 3,  6>::f().size()),  15);
    ASSERT_EQ((RamseyGraphs<3, 4,  7>::f().size()),   9); ASSERT_EQ((RamseyGraphs<4, 3,  7>::f().size()),   9);
    ASSERT_EQ((RamseyGraphs<3, 4,  8>::f().size()),   3); ASSERT_EQ((RamseyGraphs<4, 3,  8>::f().size()),   3);
    ASSERT_EQ((RamseyGraphs<3, 4,  9>::f().size()),   0); ASSERT_EQ((RamseyGraphs<4, 3,  9>::f().size()),   0);
    ASSERT_EQ((RamseyGraphs<3, 4, 10>::f().size()),   0); ASSERT_EQ((RamseyGraphs<4, 3, 10>::f().size()),   0);

    ASSERT_EQ((RamseyGraphs<3, 5,  1>::f().size()),   1); ASSERT_EQ((RamseyGraphs<5, 3,  1>::f().size()),   1);
    ASSERT_EQ((RamseyGraphs<3, 5,  2>::f().size()),   2); ASSERT_EQ((RamseyGraphs<5, 3,  2>::f().size()),   2);
    ASSERT_EQ((RamseyGraphs<3, 5,  3>::f().size()),   3); ASSERT_EQ((RamseyGraphs<5, 3,  3>::f().size()),   3);
    ASSERT_EQ((RamseyGraphs<3, 5,  4>::f().size()),   7); ASSERT_EQ((RamseyGraphs<5, 3,  4>::f().size()),   7);
    ASSERT_EQ((RamseyGraphs<3, 5,  5>::f().size()),  13); ASSERT_EQ((RamseyGraphs<5, 3,  5>::f().size()),  13);
    ASSERT_EQ((RamseyGraphs<3, 5,  6>::f().size()),  32); ASSERT_EQ((RamseyGraphs<5, 3,  6>::f().size()),  32);
    ASSERT_EQ((RamseyGraphs<3, 5,  7>::f().size()),  71); ASSERT_EQ((RamseyGraphs<5, 3,  7>::f().size()),  71);
    ASSERT_EQ((RamseyGraphs<3, 5,  8>::f().size()), 179); ASSERT_EQ((RamseyGraphs<5, 3,  8>::f().size()), 179);

    ASSERT_EQ((RamseyGraphs<3, 6,  1>::f().size()),   1); ASSERT_EQ((RamseyGraphs<6, 3,  1>::f().size()),   1);
    ASSERT_EQ((RamseyGraphs<3, 6,  2>::f().size()),   2); ASSERT_EQ((RamseyGraphs<6, 3,  2>::f().size()),   2);
    ASSERT_EQ((RamseyGraphs<3, 6,  3>::f().size()),   3); ASSERT_EQ((RamseyGraphs<6, 3,  3>::f().size()),   3);
    ASSERT_EQ((RamseyGraphs<3, 6,  4>::f().size()),   7); ASSERT_EQ((RamseyGraphs<6, 3,  4>::f().size()),   7);
    ASSERT_EQ((RamseyGraphs<3, 6,  5>::f().size()),  14); ASSERT_EQ((RamseyGraphs<6, 3,  5>::f().size()),  14);
    ASSERT_EQ((RamseyGraphs<3, 6,  6>::f().size()),  37); ASSERT_EQ((RamseyGraphs<6, 3,  6>::f().size()),  37);
    ASSERT_EQ((RamseyGraphs<3, 6,  7>::f().size()), 100); ASSERT_EQ((RamseyGraphs<6, 3,  7>::f().size()), 100);
    ASSERT_EQ((RamseyGraphs<3, 6,  8>::f().size()), 356); ASSERT_EQ((RamseyGraphs<6, 3,  8>::f().size()), 356);

    ASSERT_EQ((RamseyGraphs<4, 4,  1>::f().size()),   1);
    ASSERT_EQ((RamseyGraphs<4, 4,  2>::f().size()),   2);
    ASSERT_EQ((RamseyGraphs<4, 4,  3>::f().size()),   4);
    ASSERT_EQ((RamseyGraphs<4, 4,  4>::f().size()),   9);
    ASSERT_EQ((RamseyGraphs<4, 4,  5>::f().size()),  24);
    ASSERT_EQ((RamseyGraphs<4, 4,  6>::f().size()),  84);
    ASSERT_EQ((RamseyGraphs<4, 4,  7>::f().size()), 362);
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}

