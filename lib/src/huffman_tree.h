//
// Created by vitalya on 04.06.18.
//

#ifndef HUFFMAN_ARCHIVER_HUFFMAN_TREE_H
#define HUFFMAN_ARCHIVER_HUFFMAN_TREE_H

#include <memory>
#include <vector>
#include <bitset>
#include <climits>

class huffman_tree {
public:

    explicit huffman_tree(unsigned long long const* frequency);

    explicit huffman_tree(std::vector<bool> const& structure, std::vector<char> const& symbols);

    ~huffman_tree();

    std::pair<unsigned long long, unsigned int> const* get_codes();

    std::vector<bool> const& get_tree_structure();

    std::vector<char> const& get_symbols();

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

    int const range = (1 << (sizeof(char) * 8));
    int const shift = range / 2;

    std::pair<unsigned long long, unsigned int>* codes =
            new std::pair<unsigned long long, unsigned int>[range] + shift;
    std::shared_ptr<Node> root = nullptr;
    std::vector<bool> tree_structure;
    std::vector<char> symbols;

    struct comparator {
        bool operator()(std::pair<unsigned long long, std::shared_ptr<Node>> a,
                        std::pair<unsigned long long, std::shared_ptr<Node>> b);
    };

    void init_codes(std::shared_ptr<Node> node,
                    unsigned long long code = 0,
                    unsigned int size = 0);

    void init_tree_structure(std::shared_ptr<Node> node);

    void init_symbols(std::shared_ptr<Node> node);

    void error(std::string message = "");
};


#endif //HUFFMAN_ARCHIVER_HUFFMAN_TREE_H