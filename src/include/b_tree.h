#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct Pair {
    std::string key;
    std::uint64_t value;
};

struct Node {
    explicit Node(bool is_leaf) : leaf(is_leaf) {}
    bool leaf;
    std::vector<Pair> pairs;
    std::vector<std::unique_ptr<Node>> children;
};

class BTree {
    std::size_t degree_;
    std::unique_ptr<Node> root_;

    static std::string normalize(const std::string& key);
    void split_child(Node* parent, std::size_t index);
    void insert_non_full(Node* node, Pair pair);
    void borrow_left(Node* node, std::size_t index);
    void borrow_right(Node* node, std::size_t index);
    void merge_children(Node* node, std::size_t index);

public:
    explicit BTree(std::size_t degree = 16);

    std::optional<std::uint64_t> search(const std::string& key) const;
    bool insert(const std::string& key, std::uint64_t value);
    bool remove(const std::string& key);

    friend void save(const BTree& tree, const std::string& path);
    friend void load(BTree& tree, const std::string& path);
};
