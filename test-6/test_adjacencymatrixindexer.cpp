#include "prereqs.hpp"

#include <gtest/gtest.h>

#include "adjacencymatrixindexer.hpp"

using std::make_pair;

TEST(AdjacencyMatrixIndexer, index_triangular) {
    using I = AdjacencyMatrixIndexer<4>;

    STATIC_ASSERT(I::indexChecked(0, 0) == -1);
    STATIC_ASSERT(I::indexChecked(0, 1) ==  0);
    STATIC_ASSERT(I::indexChecked(0, 2) ==  1);
    STATIC_ASSERT(I::indexChecked(0, 3) ==  2);
    STATIC_ASSERT(I::indexChecked(1, 0) ==  0);
    STATIC_ASSERT(I::indexChecked(1, 1) == -1);
    STATIC_ASSERT(I::indexChecked(1, 2) ==  3);
    STATIC_ASSERT(I::indexChecked(1, 3) ==  4);
    STATIC_ASSERT(I::indexChecked(2, 0) ==  1);
    STATIC_ASSERT(I::indexChecked(2, 1) ==  3);
    STATIC_ASSERT(I::indexChecked(2, 2) == -1);
    STATIC_ASSERT(I::indexChecked(2, 3) ==  5);
    STATIC_ASSERT(I::indexChecked(3, 0) ==  2);
    STATIC_ASSERT(I::indexChecked(3, 1) ==  4);
    STATIC_ASSERT(I::indexChecked(3, 2) ==  5);
    STATIC_ASSERT(I::indexChecked(3, 3) == -1);
}

TEST(AdjacencyMatrixIndexer, index_nontriangular) {
    using I = AdjacencyMatrixIndexer<4, false>;

    STATIC_ASSERT(I::indexChecked(0, 0) ==  0);
    STATIC_ASSERT(I::indexChecked(0, 1) ==  1);
    STATIC_ASSERT(I::indexChecked(0, 2) ==  2);
    STATIC_ASSERT(I::indexChecked(0, 3) ==  3);
    STATIC_ASSERT(I::indexChecked(1, 0) ==  4);
    STATIC_ASSERT(I::indexChecked(1, 1) ==  5);
    STATIC_ASSERT(I::indexChecked(1, 2) ==  6);
    STATIC_ASSERT(I::indexChecked(1, 3) ==  7);
    STATIC_ASSERT(I::indexChecked(2, 0) ==  8);
    STATIC_ASSERT(I::indexChecked(2, 1) ==  9);
    STATIC_ASSERT(I::indexChecked(2, 2) == 10);
    STATIC_ASSERT(I::indexChecked(2, 3) == 11);
    STATIC_ASSERT(I::indexChecked(3, 0) == 12);
    STATIC_ASSERT(I::indexChecked(3, 1) == 13);
    STATIC_ASSERT(I::indexChecked(3, 2) == 14);
    STATIC_ASSERT(I::indexChecked(3, 3) == 15);
}

TEST(AdjacencyMatrixIndexer, reverse_triangular) {
    using I = AdjacencyMatrixIndexer<4>;

    STATIC_ASSERT((I::reverseChecked(0) == make_pair<s64, s64>(0, 1)));
    STATIC_ASSERT((I::reverseChecked(1) == make_pair<s64, s64>(0, 2)));
    STATIC_ASSERT((I::reverseChecked(2) == make_pair<s64, s64>(0, 3)));
    STATIC_ASSERT((I::reverseChecked(3) == make_pair<s64, s64>(1, 2)));
    STATIC_ASSERT((I::reverseChecked(4) == make_pair<s64, s64>(1, 3)));
    STATIC_ASSERT((I::reverseChecked(5) == make_pair<s64, s64>(2, 3)));
}

TEST(AdjacencyMatrixIndexer, reverse_nontriangular) {
    using I = AdjacencyMatrixIndexer<4, false>;

    STATIC_ASSERT((I::reverseChecked( 0) == make_pair<s64, s64>(0, 0)));
    STATIC_ASSERT((I::reverseChecked( 1) == make_pair<s64, s64>(0, 1)));
    STATIC_ASSERT((I::reverseChecked( 2) == make_pair<s64, s64>(0, 2)));
    STATIC_ASSERT((I::reverseChecked( 3) == make_pair<s64, s64>(0, 3)));
    STATIC_ASSERT((I::reverseChecked( 4) == make_pair<s64, s64>(1, 0)));
    STATIC_ASSERT((I::reverseChecked( 5) == make_pair<s64, s64>(1, 1)));
    STATIC_ASSERT((I::reverseChecked( 6) == make_pair<s64, s64>(1, 2)));
    STATIC_ASSERT((I::reverseChecked( 7) == make_pair<s64, s64>(1, 3)));
    STATIC_ASSERT((I::reverseChecked( 8) == make_pair<s64, s64>(2, 0)));
    STATIC_ASSERT((I::reverseChecked( 9) == make_pair<s64, s64>(2, 1)));
    STATIC_ASSERT((I::reverseChecked(10) == make_pair<s64, s64>(2, 2)));
    STATIC_ASSERT((I::reverseChecked(11) == make_pair<s64, s64>(2, 3)));
    STATIC_ASSERT((I::reverseChecked(12) == make_pair<s64, s64>(3, 0)));
    STATIC_ASSERT((I::reverseChecked(13) == make_pair<s64, s64>(3, 1)));
    STATIC_ASSERT((I::reverseChecked(14) == make_pair<s64, s64>(3, 2)));
    STATIC_ASSERT((I::reverseChecked(15) == make_pair<s64, s64>(3, 3)));
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
