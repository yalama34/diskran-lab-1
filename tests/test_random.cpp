#include "b_tree.h"

#include <gtest/gtest.h>

#include <map>
#include <random>
#include <string>

namespace {
std::string word(unsigned i) {
    std::string result;
    do {
        result += static_cast<char>('a' + i % 26);
        i /= 26;
    } while (i);
    return result;
}
}

TEST(BTreeRandom, MatchesStdMapAcrossDegrees) {
    for (std::size_t degree : {2, 3, 16}) {
        SCOPED_TRACE(degree);
        BTree tree(degree);
        std::map<std::string, std::uint64_t> expected;
        std::mt19937 random(1234);
        for (int i = 0; i < 100000; ++i) {
            std::string key = "key" + word(random() % 3000);
            auto value = static_cast<std::uint64_t>(random());
            switch (random() % 3) {
            case 0:
                ASSERT_EQ(tree.insert(key, value), expected.emplace(key, value).second) << i;
                break;
            case 1:
                ASSERT_EQ(tree.remove(key), expected.erase(key) != 0) << i;
                break;
            default: {
                auto it = expected.find(key);
                auto actual = tree.search(key);
                ASSERT_EQ(actual.has_value(), it != expected.end()) << i;
                if (it != expected.end()) {
                    EXPECT_EQ(*actual, it->second) << i;
                }
            }
            }
        }
        for (const auto& pair : expected) {
            ASSERT_EQ(tree.search(pair.first), pair.second);
        }
        for (const auto& pair : expected) {
            ASSERT_TRUE(tree.remove(pair.first));
            EXPECT_FALSE(tree.search(pair.first).has_value());
        }
        EXPECT_FALSE(tree.remove("missing"));
    }
}
