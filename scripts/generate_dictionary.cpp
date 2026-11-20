#include <cstdint>
#include <exception>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: generator <count> [seed]\n";
        return 1;
    }

    try {
        std::string count_arg = argv[1];
        if (count_arg.empty() || count_arg.find_first_not_of("0123456789") != std::string::npos) {
            std::cerr << "Count must be non-negative\n";
            return 1;
        }

        std::size_t parsed = 0;
        std::uint64_t count = std::stoull(count_arg, &parsed);
        if (parsed != count_arg.size()) {
            std::cerr << "Invalid count\n";
            return 1;
        }

        std::uint64_t seed = 32;
        if (argc == 3) {
            std::string seed_arg = argv[2];
            if (seed_arg.empty() || seed_arg.find_first_not_of("0123456789") != std::string::npos) {
                std::cerr << "Seed must be non-negative\n";
                return 1;
            }
            parsed = 0;
            seed = std::stoull(seed_arg, &parsed);
            if (parsed != seed_arg.size()) {
                std::cerr << "Invalid seed\n";
                return 1;
            }
        }

        std::mt19937_64 random(seed);
        std::uniform_int_distribution<int> length_distribution(16, 64);
        std::uniform_int_distribution<int> letter_distribution(0, 25);

        std::unordered_set<std::string> words;

        while (words.size() < count) {
            int length = length_distribution(random);
            std::string word;

            for (int i = 0; i < length; ++i) {
                char letter = static_cast<char>('a' + letter_distribution(random));
                word += letter;
            }

            auto result = words.insert(word);
            if (!result.second) {
                continue;
            }

            std::uint64_t value = random();
            std::cout << "+ " << word << ' ' << value << '\n';
            if (!std::cout) {
                std::cerr << "Cannot write generated data\n";
                return 1;
            }
        }
        std::cout.flush();
        if (!std::cout) {
            std::cerr << "Cannot write generated data\n";
            return 1;
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
