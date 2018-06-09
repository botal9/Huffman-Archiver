//
// Created by vitalya on 04.06.18.
//

#ifndef HUFFMAN_ARCHIVER_HUFFMAN_TREE_H
#define HUFFMAN_ARCHIVER_HUFFMAN_TREE_H

#include <memory>
#include <map>
#include <vector>
#include <bitset>
#include <unordered_map>

class huffman_tree {
public:

    explicit huffman_tree(std::unordered_map<char, unsigned long long> const& frequency);

    explicit huffman_tree(std::vector<bool> const& structure,
                          std::vector<char> const& symbols);

    std::unordered_map<char,
            std::pair<
                    unsigned long long,
                    unsigned int
            >
    > const& get_codes();

    std::vector<bool> const& get_tree_structure();

    std::vector<char> const& get_symbols();

    std::map<std::pair<unsigned long long, unsigned int>, char> const& get_reverse_codes();

    size_t decode(char const* array, unsigned int size, char* answer);

private:

    struct Node {
        char symbol = 0;
        bool is_terminal = true;

        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;

        Node() = default;

        explicit Node(char _symbol,
             bool _is_terminal = true,
             std::shared_ptr<Node> _left = nullptr,
             std::shared_ptr<Node> _right = nullptr
        );
    };

    std::shared_ptr<Node> root = nullptr;
    std::unordered_map<char, std::pair<unsigned long long, unsigned int>> codes;
    std::vector<bool> tree_structure;
    std::vector<char> symbols;
    std::map<std::pair<unsigned long long, unsigned int>, char> reverse_codes;

    struct comparator {
        bool operator()(std::pair<unsigned long long, std::shared_ptr<Node>> a,
                        std::pair<unsigned long long, std::shared_ptr<Node>> b);
    };

    void init_codes(std::shared_ptr<Node> node,
              unsigned long long code = 0,
              unsigned int size = 0);

    void init_tree_structure(std::shared_ptr<Node> node);

    void init_symbols(std::shared_ptr<Node> node);

    void init_reverse_codes(std::shared_ptr<Node> node,
                             unsigned long long code = 0,
                             unsigned int size = 0);

    void destroy_tree();

    void error();
};


#endif //HUFFMAN_ARCHIVER_HUFFMAN_TREE_H
