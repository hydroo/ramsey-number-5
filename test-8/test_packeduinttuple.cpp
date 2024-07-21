#include "packeduinttuple.hpp"

#include <set>
#include <unordered_set>

using std::cerr;
using std::endl;

using r5::PackedUIntTuple;
using r5::staticLog2Ceil;
using r5::staticLog2Floor;
using r5::staticPow2;

TEST(PackedUIntTuple, size) {
    PackedUIntTuple< 1> a1_1 ; ASSERT_EQ(sizeof(a1_1 ) , 1); ASSERT_EQ(a1_1.size() , 1);
    PackedUIntTuple< 7> a1_7 ; ASSERT_EQ(sizeof(a1_7 ) , 1);
    PackedUIntTuple< 8> a1_8 ; ASSERT_EQ(sizeof(a1_8 ) , 1);
    PackedUIntTuple< 9> a1_9 ; ASSERT_EQ(sizeof(a1_9 ) , 2);
    PackedUIntTuple<15> a1_15; ASSERT_EQ(sizeof(a1_15) , 2);
    PackedUIntTuple<16> a1_16; ASSERT_EQ(sizeof(a1_16) , 2);
    PackedUIntTuple<17> a1_17; ASSERT_EQ(sizeof(a1_17) , 4);
    PackedUIntTuple<31> a1_31; ASSERT_EQ(sizeof(a1_31) , 4);
    PackedUIntTuple<32> a1_32; ASSERT_EQ(sizeof(a1_32) , 4);
    PackedUIntTuple<33> a1_33; ASSERT_EQ(sizeof(a1_33) , 8);
    PackedUIntTuple<64> a1_64; ASSERT_EQ(sizeof(a1_64) , 8);
}

TEST(PackedUIntTuple, construct) {
    PackedUIntTuple<1> a1_0({0}); ASSERT_EQ(a1_0.data(), 0b0); ASSERT_EQ(a1_0.get<0>(), 0);
    PackedUIntTuple<1> a1_1({1}); ASSERT_EQ(a1_1.data(), 0b1); ASSERT_EQ(a1_1.get<0>(), 1);

    PackedUIntTuple<1,1> a11_00({0,0}); ASSERT_EQ(a11_00.data(), 0b00); ASSERT_EQ(a11_00.get<0>(), 0); ASSERT_EQ(a11_00.get<1>(), 0);
    PackedUIntTuple<1,1> a11_01({0,1}); ASSERT_EQ(a11_01.data(), 0b10); ASSERT_EQ(a11_01.get<0>(), 0); ASSERT_EQ(a11_01.get<1>(), 1);
    PackedUIntTuple<1,1> a11_10({1,0}); ASSERT_EQ(a11_10.data(), 0b01); ASSERT_EQ(a11_10.get<0>(), 1); ASSERT_EQ(a11_10.get<1>(), 0);
    PackedUIntTuple<1,1> a11_11({1,1}); ASSERT_EQ(a11_11.data(), 0b11); ASSERT_EQ(a11_11.get<0>(), 1); ASSERT_EQ(a11_11.get<1>(), 1);

    PackedUIntTuple<2,3,4> a234({0b10,0b110,0b1010}); ASSERT_EQ(a234.data(), 0b101011010);
        ASSERT_EQ(a234.get<0>(), 0b10); ASSERT_EQ(a234.get<1>(), 0b110); ASSERT_EQ(a234.get<2>(), 0b1010);

    PackedUIntTuple<7,11,17> b({127, 567, 16987});
        ASSERT_EQ(b.get<0>(), 127); ASSERT_EQ(b.get<1>(), 567); ASSERT_EQ(b.get<2>(), 16987);
}

TEST(PackedUIntTuple, set) {
    PackedUIntTuple<1> a1_0; a1_0.set<0>(0); ASSERT_EQ(a1_0.data(), 0b0); ASSERT_EQ(a1_0.get<0>(), 0);
    PackedUIntTuple<1> a1_1; a1_1.set<0>(1); ASSERT_EQ(a1_1.data(), 0b1); ASSERT_EQ(a1_1.get<0>(), 1);

    PackedUIntTuple<1,1> a11_00; a11_00.set<0>(0); a11_00.set<1>(0); ASSERT_EQ(a11_00.get<0>(), 0); ASSERT_EQ(a11_00.get<1>(), 0);
    PackedUIntTuple<1,1> a11_01; a11_01.set<0>(0); a11_01.set<1>(1); ASSERT_EQ(a11_01.get<0>(), 0); ASSERT_EQ(a11_01.get<1>(), 1);
    PackedUIntTuple<1,1> a11_10; a11_10.set<0>(1); a11_10.set<1>(0); ASSERT_EQ(a11_10.get<0>(), 1); ASSERT_EQ(a11_10.get<1>(), 0);
    PackedUIntTuple<1,1> a11_11; a11_11.set<0>(1); a11_11.set<1>(1); ASSERT_EQ(a11_11.get<0>(), 1); ASSERT_EQ(a11_11.get<1>(), 1);

    PackedUIntTuple<2,3,4> a234; a234.set<0>(0b10); a234.set<1>(0b110); a234.set<2>(0b1010);
        ASSERT_EQ(a234.get<0>(), 0b10); ASSERT_EQ(a234.get<1>(), 0b110); ASSERT_EQ(a234.get<2>(), 0b1010);

    PackedUIntTuple<7,11,17> b; b.set<0>(127); b.set<1>(567); b.set<2>(16987);
        ASSERT_EQ(b.get<0>(), 127); ASSERT_EQ(b.get<1>(), 567); ASSERT_EQ(b.get<2>(), 16987);
}

TEST(PackedUIntTuple, copy_move_constructor_and_assignment) {
    std::size_t v0 = 0b10, v1 = 0b110, v2 = 0b1010;
    PackedUIntTuple<2,3,4> t0({v0, v1, v2}), u0;
    u0 = t0;
    ASSERT_EQ(u0.get<0>(), v0); ASSERT_EQ(u0.get<1>(), v1); ASSERT_EQ(u0.get<2>(), v2);

    PackedUIntTuple<2,3,4> t1({v0, v1, v2}), u1;
    u1 = std::move(t1);
    ASSERT_EQ(u1.get<0>(), v0); ASSERT_EQ(u1.get<1>(), v1); ASSERT_EQ(u1.get<2>(), v2);

    PackedUIntTuple<2,3,4> t2({v0, v1, v2}), u2(t2);
    ASSERT_EQ(u2.get<0>(), v0); ASSERT_EQ(u2.get<1>(), v1); ASSERT_EQ(u2.get<2>(), v2);

    PackedUIntTuple<2,3,4> t3({v0, v1, v2}), u3(std::move(t3));
    ASSERT_EQ(u3.get<0>(), v0); ASSERT_EQ(u3.get<1>(), v1); ASSERT_EQ(u3.get<2>(), v2);
}

TEST(PackedUIntTuple, std_set) {
    using PUT = PackedUIntTuple<4,5,6>;

    std::set<PUT> s{
        PUT({4,5,6}),
        PUT({1,2,3}),
        PUT({7,8,9})};

    ASSERT_TRUE(s.size() == 3);
    ASSERT_TRUE(s.contains(PUT({4,5,6})));
    ASSERT_TRUE(s.contains(PUT({1,2,3})));
    ASSERT_TRUE(s.contains(PUT({7,8,9})));
}

TEST(PackedUIntTuple, std_unordered_set_1) {
    using PUT = PackedUIntTuple<4,5,6>;
    auto v0 = PUT({4,5,6});
    auto v1 = PUT({1,2,3});
    auto v2 = PUT({7,8,9});

    std::unordered_set<PUT> s{v0, v1, v2};

    ASSERT_TRUE(s.size() == 3);
    ASSERT_TRUE(s.contains(v0));
    ASSERT_TRUE(s.contains(v1));
    ASSERT_TRUE(s.contains(v2));
}

TEST(PackedUIntTuple, std_unordered_set_2) {
    using PUT = PackedUIntTuple<4,5,6>;

    auto v0 = std::make_tuple(PUT({4,5,6}), 1);
    auto v1 = std::make_tuple(PUT({1,2,3}), 2);
    auto v2 = std::make_tuple(PUT({7,8,9}), 3);

    std::unordered_set<std::tuple<PUT, std::size_t>> s{v0, v1, v2};

    ASSERT_TRUE(s.size() == 3);
    ASSERT_TRUE(s.contains(v0));
    ASSERT_TRUE(s.contains(v1));
    ASSERT_TRUE(s.contains(v2));
}

TEST(PackedUIntTuple_helpers, staticLog2Floor) {
    ASSERT_EQ(staticLog2Floor(-1) ,    -1);
    ASSERT_EQ(staticLog2Floor( 0) ,     0);
    ASSERT_EQ(staticLog2Floor( 1) ,     0);
    ASSERT_EQ(staticLog2Floor( 2) ,     1);
    ASSERT_EQ(staticLog2Floor( 3) ,     1);
    ASSERT_EQ(staticLog2Floor( 4) ,     2);
    ASSERT_EQ(staticLog2Floor( 5) ,     2);
    ASSERT_EQ(staticLog2Floor( 6) ,     2);
    ASSERT_EQ(staticLog2Floor( 7) ,     2);
    ASSERT_EQ(staticLog2Floor( 8) ,     3);
    ASSERT_EQ(staticLog2Floor( 9) ,     3);
    ASSERT_EQ(staticLog2Floor(15) ,     3);
    ASSERT_EQ(staticLog2Floor(16) ,     4);
    ASSERT_EQ(staticLog2Floor(17) ,     4);
}

TEST(PackedUIntTuple_helpers, staticLog2Ceil) {
    ASSERT_EQ(staticLog2Ceil(-1) ,    -1);
    ASSERT_EQ(staticLog2Ceil( 0) ,     0);
    ASSERT_EQ(staticLog2Ceil( 1) ,     0);
    ASSERT_EQ(staticLog2Ceil( 2) ,     1);
    ASSERT_EQ(staticLog2Ceil( 3) ,     2);
    ASSERT_EQ(staticLog2Ceil( 4) ,     2);
    ASSERT_EQ(staticLog2Ceil( 5) ,     3);
    ASSERT_EQ(staticLog2Ceil( 6) ,     3);
    ASSERT_EQ(staticLog2Ceil( 7) ,     3);
    ASSERT_EQ(staticLog2Ceil( 8) ,     3);
    ASSERT_EQ(staticLog2Ceil( 9) ,     4);
    ASSERT_EQ(staticLog2Ceil(15) ,     4);
    ASSERT_EQ(staticLog2Ceil(16) ,     4);
    ASSERT_EQ(staticLog2Ceil(17) ,     5);
}

TEST(PackedUIntTuple_helpers, staticPow2) {
    ASSERT_EQ(staticPow2(0) ,    1);
    ASSERT_EQ(staticPow2(1) ,    2);
    ASSERT_EQ(staticPow2(2) ,    4);
    ASSERT_EQ(staticPow2(3) ,    8);
    ASSERT_EQ(staticPow2(4) ,   16);
    ASSERT_EQ(staticPow2(5) ,   32);
    ASSERT_EQ(staticPow2(6) ,   64);
    ASSERT_EQ(staticPow2(7) ,  128);
    ASSERT_EQ(staticPow2(8) ,  256);
    ASSERT_EQ(staticPow2(9) ,  512);
    ASSERT_EQ(staticPow2(10), 1024);
}

TEST(PackedUIntTuple_helpers, staticPow2_staticLog2Floor) {
    ASSERT_EQ(staticPow2(staticLog2Ceil( 1)) ,     1);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 2)) ,     2);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 3)) ,     4);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 4)) ,     4);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 5)) ,     8);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 6)) ,     8);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 7)) ,     8);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 8)) ,     8);
    ASSERT_EQ(staticPow2(staticLog2Ceil( 9)) ,    16);
    ASSERT_EQ(staticPow2(staticLog2Ceil(15)) ,    16);
    ASSERT_EQ(staticPow2(staticLog2Ceil(16)) ,    16);
    ASSERT_EQ(staticPow2(staticLog2Ceil(17)) ,    32);
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}