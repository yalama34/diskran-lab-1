#include "b_tree.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {
class PersistenceTest : public ::testing::Test {
protected:
    std::string path;

    void SetUp() override {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        path = std::string("test-btree-") + info->name() + ".bin";
        std::remove(path.c_str());
    }

    void TearDown() override {
        std::remove(path.c_str());
    }
};
}

TEST_F(PersistenceTest, EmptyTreeReplacesExistingContents) {
    BTree tree(2);
    EXPECT_NO_THROW(save(tree, path));
    ASSERT_TRUE(tree.insert("Apple", 0));
    EXPECT_NO_THROW(load(tree, path));
    EXPECT_FALSE(tree.search("apple").has_value());
}

TEST_F(PersistenceTest, RoundTripReplacesTreeAndKeepsValues) {
    BTree tree(2);
    ASSERT_TRUE(tree.insert("Apple", 0));
    ASSERT_TRUE(tree.insert("BANANA", std::numeric_limits<std::uint64_t>::max()));
    EXPECT_NO_THROW(save(tree, path));
    ASSERT_TRUE(tree.remove("apple"));
    ASSERT_TRUE(tree.insert("cherry", 7));
    EXPECT_NO_THROW(load(tree, path));
    EXPECT_EQ(tree.search("APPLE"), 0);
    EXPECT_EQ(tree.search("banana"), std::numeric_limits<std::uint64_t>::max());
    EXPECT_FALSE(tree.search("cherry").has_value());
}

TEST_F(PersistenceTest, BrokenOrMissingFileKeepsExistingTree) {
    BTree tree(2);
    ASSERT_TRUE(tree.insert("apple", 0));
    ASSERT_TRUE(tree.insert("banana", std::numeric_limits<std::uint64_t>::max()));
    {
        std::ofstream damaged(path, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(damaged.is_open());
        damaged << "BTREE001";
    }
    EXPECT_THROW(load(tree, path), std::runtime_error);
    EXPECT_EQ(tree.search("apple"), 0);
    EXPECT_EQ(tree.search("banana"), std::numeric_limits<std::uint64_t>::max());
    std::remove(path.c_str());
    EXPECT_THROW(load(tree, path), std::runtime_error);
    EXPECT_EQ(tree.search("apple"), 0);
    EXPECT_EQ(tree.search("banana"), std::numeric_limits<std::uint64_t>::max());
}

TEST_F(PersistenceTest, InvalidOutputPathThrows) {
    BTree tree(2);
    // The regular file cannot act as a directory.
    { std::ofstream file(path); ASSERT_TRUE(file.is_open()); }
    EXPECT_THROW(save(tree, path + "/tree.bin"), std::runtime_error);
}
