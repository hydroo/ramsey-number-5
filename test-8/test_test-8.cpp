#include "prereqs.hpp"

#include "test-8.hpp"

using std::cerr;
using std::endl;

using r5::AdjacencyMatrix;

//#define R5_TEST_8_FAST_COMPILE

TEST(Test8, ramseyGraphs) {

    ASSERT_EQ((ramseyGraphs<1, 1,  1>().size()),   0); ASSERT_EQ((ramseyGraphs<1, 1,  1>().size()),   0);
    ASSERT_EQ((ramseyGraphs<1, 2,  1>().size()),   0); ASSERT_EQ((ramseyGraphs<2, 1,  1>().size()),   0);
    ASSERT_EQ((ramseyGraphs<1, 3,  1>().size()),   0); ASSERT_EQ((ramseyGraphs<3, 1,  1>().size()),   0);
    ASSERT_EQ((ramseyGraphs<1, 4,  1>().size()),   0); ASSERT_EQ((ramseyGraphs<4, 1,  1>().size()),   0);

    ASSERT_EQ((ramseyGraphs<2, 2,  2>().size()),   0);
    ASSERT_EQ((ramseyGraphs<2, 3,  3>().size()),   0); ASSERT_EQ((ramseyGraphs<3, 2,  3>().size()),   0);
    ASSERT_EQ((ramseyGraphs<2, 4,  4>().size()),   0); ASSERT_EQ((ramseyGraphs<4, 2,  4>().size()),   0);

    ASSERT_EQ((ramseyGraphs<3, 3,  6>().size()),   0);

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<3, 4,  1>().size()),   1); ASSERT_EQ((ramseyGraphs<4, 3,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<3, 4,  2>().size()),   2); ASSERT_EQ((ramseyGraphs<4, 3,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<3, 4,  3>().size()),   3); ASSERT_EQ((ramseyGraphs<4, 3,  3>().size()),   3);
    ASSERT_EQ((ramseyGraphs<3, 4,  4>().size()),   6); ASSERT_EQ((ramseyGraphs<4, 3,  4>().size()),   6);
    ASSERT_EQ((ramseyGraphs<3, 4,  5>().size()),   9); ASSERT_EQ((ramseyGraphs<4, 3,  5>().size()),   9);
    ASSERT_EQ((ramseyGraphs<3, 4,  6>().size()),  15); ASSERT_EQ((ramseyGraphs<4, 3,  6>().size()),  15);
    ASSERT_EQ((ramseyGraphs<3, 4,  7>().size()),   9); ASSERT_EQ((ramseyGraphs<4, 3,  7>().size()),   9);
    ASSERT_EQ((ramseyGraphs<3, 4,  8>().size()),   3); ASSERT_EQ((ramseyGraphs<4, 3,  8>().size()),   3);
    ASSERT_EQ((ramseyGraphs<3, 4,  9>().size()),   0); ASSERT_EQ((ramseyGraphs<4, 3,  9>().size()),   0);
#endif /* R5_TEST_8_FAST_COMPILE */
    ASSERT_EQ((ramseyGraphs<3, 4, 10>().size()),   0); ASSERT_EQ((ramseyGraphs<4, 3, 10>().size()),   0);

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<3, 5,  1>().size()),   1); ASSERT_EQ((ramseyGraphs<5, 3,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<3, 5,  2>().size()),   2); ASSERT_EQ((ramseyGraphs<5, 3,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<3, 5,  3>().size()),   3); ASSERT_EQ((ramseyGraphs<5, 3,  3>().size()),   3);
    ASSERT_EQ((ramseyGraphs<3, 5,  4>().size()),   7); ASSERT_EQ((ramseyGraphs<5, 3,  4>().size()),   7);
    ASSERT_EQ((ramseyGraphs<3, 5,  5>().size()),  13); ASSERT_EQ((ramseyGraphs<5, 3,  5>().size()),  13);
    ASSERT_EQ((ramseyGraphs<3, 5,  6>().size()),  32); ASSERT_EQ((ramseyGraphs<5, 3,  6>().size()),  32);
    ASSERT_EQ((ramseyGraphs<3, 5,  7>().size()),  71); ASSERT_EQ((ramseyGraphs<5, 3,  7>().size()),  71);
    ASSERT_EQ((ramseyGraphs<3, 5,  8>().size()), 179); ASSERT_EQ((ramseyGraphs<5, 3,  8>().size()), 179);
#endif /* R5_TEST_8_FAST_COMPILE */

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<3, 6,  1>().size()),   1); ASSERT_EQ((ramseyGraphs<6, 3,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<3, 6,  2>().size()),   2); ASSERT_EQ((ramseyGraphs<6, 3,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<3, 6,  3>().size()),   3); ASSERT_EQ((ramseyGraphs<6, 3,  3>().size()),   3);
    ASSERT_EQ((ramseyGraphs<3, 6,  4>().size()),   7); ASSERT_EQ((ramseyGraphs<6, 3,  4>().size()),   7);
    ASSERT_EQ((ramseyGraphs<3, 6,  5>().size()),  14); ASSERT_EQ((ramseyGraphs<6, 3,  5>().size()),  14);
    ASSERT_EQ((ramseyGraphs<3, 6,  6>().size()),  37); ASSERT_EQ((ramseyGraphs<6, 3,  6>().size()),  37);
    ASSERT_EQ((ramseyGraphs<3, 6,  7>().size()), 100); ASSERT_EQ((ramseyGraphs<6, 3,  7>().size()), 100);
#endif /* R5_TEST_8_FAST_COMPILE */
    ASSERT_EQ((ramseyGraphs<3, 6,  8>().size()), 356); ASSERT_EQ((ramseyGraphs<6, 3,  8>().size()), 356);

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<4, 4,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<4, 4,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<4, 4,  3>().size()),   4);
    ASSERT_EQ((ramseyGraphs<4, 4,  4>().size()),   9);
    ASSERT_EQ((ramseyGraphs<4, 4,  5>().size()),  24);
    ASSERT_EQ((ramseyGraphs<4, 4,  6>().size()),  84);
#endif /* R5_TEST_8_FAST_COMPILE */
    ASSERT_EQ((ramseyGraphs<4, 4,  7>().size()), 362);

    // Note: the following results are not confirmed by outside information, yet. TODO

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<4, 5,  1>().size()),   1); ASSERT_EQ((ramseyGraphs<5, 4,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<4, 5,  2>().size()),   2); ASSERT_EQ((ramseyGraphs<5, 4,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<4, 5,  3>().size()),   4); ASSERT_EQ((ramseyGraphs<5, 4,  3>().size()),   4);
    ASSERT_EQ((ramseyGraphs<4, 5,  4>().size()),  10); ASSERT_EQ((ramseyGraphs<5, 4,  4>().size()),  10);
    ASSERT_EQ((ramseyGraphs<4, 5,  5>().size()),  28); ASSERT_EQ((ramseyGraphs<5, 4,  5>().size()),  28);
    ASSERT_EQ((ramseyGraphs<4, 5,  6>().size()), 114); ASSERT_EQ((ramseyGraphs<5, 4,  6>().size()), 114);
#endif /* R5_TEST_8_FAST_COMPILE */
    ASSERT_EQ((ramseyGraphs<4, 5,  7>().size()), 627); ASSERT_EQ((ramseyGraphs<5, 4,  7>().size()), 627);

#ifndef R5_TEST_8_FAST_COMPILE
    ASSERT_EQ((ramseyGraphs<5, 5,  1>().size()),   1);
    ASSERT_EQ((ramseyGraphs<5, 5,  2>().size()),   2);
    ASSERT_EQ((ramseyGraphs<5, 5,  3>().size()),   4);
    ASSERT_EQ((ramseyGraphs<5, 5,  4>().size()),  11);
    ASSERT_EQ((ramseyGraphs<5, 5,  5>().size()),  32);
    ASSERT_EQ((ramseyGraphs<5, 5,  6>().size()), 144);
    ASSERT_EQ((ramseyGraphs<5, 5,  7>().size()), 928);
#endif /* R5_TEST_8_FAST_COMPILE */
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}

