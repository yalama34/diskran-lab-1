#include "include/b_tree.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace {

void write_number(std::ostream& out, std::uint64_t value, unsigned bytes) {
    for (unsigned i = 0; i < bytes; ++i) {
        out.put(static_cast<char>((value >> (8 * i)) & 0xff));
    }
}

std::uint64_t read_number(std::istream& in, unsigned bytes) {
    std::uint64_t value = 0;
    for (unsigned i = 0; i < bytes; ++i) {
        int byte = in.get();
        if (byte == std::char_traits<char>::eof()) {
            throw std::runtime_error("Invalid file format");
        }
        value |= static_cast<std::uint64_t>(byte) << (8 * i);
    }
    return value;
}

std::uint64_t count_pairs(const Node* node) {
    if (node == nullptr) {
        return 0;
    }
    std::uint64_t count = node->pairs.size();
    for (const auto& child : node->children) {
        count += count_pairs(child.get());
    }
    return count;
}

void write_node(std::ostream& out, const Node* node) {
    if (node == nullptr) {
        return;
    }
    for (std::size_t i = 0; i < node->pairs.size(); ++i) {
        if (!node->leaf) {
            write_node(out, node->children[i].get());
        }
        const Pair& pair = node->pairs[i];
        write_number(out, pair.key.size(), 2);
        out.write(pair.key.data(), static_cast<std::streamsize>(pair.key.size()));
        write_number(out, pair.value, 8);
    }
    if (!node->leaf) {
        write_node(out, node->children.back().get());
    }
}

}

void save(const BTree& tree, const std::string& path) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Cannot open file for writing");
    }

    out.write("BTREE001", 8);
    write_number(out, count_pairs(tree.root_.get()), 8);
    write_node(out, tree.root_.get());
    out.close();
    if (!out) {
        throw std::runtime_error("Cannot write file");
    }
}

void load(BTree& tree, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open file for reading");
    }

    char magic[8];
    if (!in.read(magic, sizeof(magic)) || std::string(magic, sizeof(magic)) != "BTREE001") {
        throw std::runtime_error("Invalid file format");
    }
    std::uint64_t count = read_number(in, 8);
    BTree replacement(tree.degree_);
    std::string previous;

    for (std::uint64_t i = 0; i < count; ++i) {
        std::uint64_t length = read_number(in, 2);
        if (length == 0 || length > 256) {
            throw std::runtime_error("Invalid file format");
        }

        std::string key(static_cast<std::size_t>(length), '\0');
        if (!in.read(&key[0], static_cast<std::streamsize>(length))) {
            throw std::runtime_error("Invalid file format");
        }
        std::uint64_t value = read_number(in, 8);
        for (char letter : key) {
            if (letter < 'a' || letter > 'z') {
                throw std::runtime_error("Invalid file format");
            }
        }
        if (i > 0 && previous >= key) {
            throw std::runtime_error("Invalid file format");
        }

        replacement.insert(key, value);
        previous = std::move(key);
    }
    if (in.peek() != std::char_traits<char>::eof() || in.bad()) {
        throw std::runtime_error("Invalid file format");
    }
    tree.root_ = std::move(replacement.root_);
}
