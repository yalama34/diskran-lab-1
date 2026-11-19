#include "include/b_tree.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    BTree tree;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        if (!(input >> command)) continue;
        try {
            if (command == "+") {
                std::string key;
                std::uint64_t value;
                if (!(input >> key >> value)) break;
                std::cout << (tree.insert(key, value) ? "OK\n" : "Exist\n");
            } else if (command == "-") {
                std::string key;
                if (!(input >> key)) break;
                std::cout << (tree.remove(key) ? "OK\n" : "NoSuchWord\n");
            } else if (command == "!") {
                std::string action, path;
                if (!(input >> action) || !std::getline(input >> std::ws, path) || path.empty()) {
                    std::cout << "ERROR: Invalid command\n";
                    continue;
                }
                if (action == "Save") {
                    save(tree, path);
                } else if (action == "Load") {
                    load(tree, path);
                } else {
                    std::cout << "ERROR: Unknown command\n";
                    continue;
                }
                std::cout << "OK\n";
            } else {
                auto value = tree.search(command);
                if (value) std::cout << "OK: " << *value << '\n';
                else std::cout << "NoSuchWord\n";
            }
        } catch (const std::exception& error) {
            std::cout << "ERROR: " << error.what() << '\n';
        }
    }
}

