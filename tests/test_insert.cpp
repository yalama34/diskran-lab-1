#include "b_tree.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace {
std::string word(int i) {
    std::string result;
    do {
        result += static_cast<char>('a' + i % 26);
        i /= 26;
    } while (i);
    return result;
}
}

TEST(BTreeInsert, EmptyTreeAndDegree) {
    BTree tree(2);
    EXPECT_FALSE(tree.search("missing").has_value());
    EXPECT_THROW(BTree(1), std::invalid_argument);
}

TEST(BTreeInsert, SplitsAndCaseInsensitiveDuplicates) {
    BTree tree(2);
    for (int i = 0; i < 5000; ++i) {
        ASSERT_TRUE(tree.insert("Key" + word(i), static_cast<std::uint64_t>(i))) << i;
    }
    for (int i = 0; i < 5000; ++i) {
        const auto key = "kEY" + word(i);
        ASSERT_EQ(tree.search(key), static_cast<std::uint64_t>(i)) << i;
        EXPECT_FALSE(tree.insert(key, 42)) << i;
        EXPECT_EQ(tree.search(key), static_cast<std::uint64_t>(i)) << i;
    }
}

TEST(BTreeInsert, BoundaryKeysAndValues) {
    BTree tree(2);
    const auto max = std::numeric_limits<std::uint64_t>::max();
    ASSERT_TRUE(tree.insert("max", max));
    EXPECT_EQ(tree.search("MAX"), max);
    ASSERT_TRUE(tree.insert(std::string(256, 'A'), 0));
    EXPECT_EQ(tree.search(std::string(256, 'a')), 0);
    EXPECT_FALSE(tree.insert("", 1));
    EXPECT_FALSE(tree.insert(std::string(257, 'a'), 1));
    EXPECT_FALSE(tree.insert("abc1", 1));
    EXPECT_FALSE(tree.search("abc1").has_value());
}

