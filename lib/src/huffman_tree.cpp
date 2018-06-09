//
// Created by vitalya on 04.06.18.
//

#include <queue>
#include "huffman_tree.h"

huffman_tree::Node::Node(char _symbol,
                         bool _is_terminal,
                         std::shared_ptr<Node> _left,
                         std::shared_ptr<Node> _right) :
        symbol(_symbol),
        is_terminal(_is_terminal),
        left(std::move(_left)),
        right(std::move(_right))
{}

bool huffman_tree::comparator::operator()(std::pair<unsigned long long int, std::shared_ptr<huffman_tree::Node>> a,
                                          std::pair<unsigned long long int, std::shared_ptr<huffman_tree::Node>> b) {
    return a.first > b.first;
}

huffman_tree::huffman_tree(unsigned long long const* frequency) {
    std::priority_queue<std::pair<unsigned long long, std::shared_ptr<Node>>,
            std::vector<std::pair<unsigned long long, std::shared_ptr<Node>>>, comparator> nodes_pool;
    for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
        if (frequency[i] > 0) {
            nodes_pool.emplace(frequency[i], std::make_shared<Node>(Node(char(i))));
        }
    }
    while(nodes_pool.size() > 1) {
        auto [left_weight, left_node] = nodes_pool.top();
        nodes_pool.pop();
        auto [right_weight, right_node] = nodes_pool.top();
        nodes_pool.pop();
        nodes_pool.emplace(left_weight + right_weight, std::make_shared<Node>(Node(0, false, left_node, right_node)));
    }
    huffman_tree::root = nodes_pool.top().second;

    huffman_tree::init_codes(root);
    huffman_tree::init_tree_structure(root);
    huffman_tree::init_symbols(root);
}

huffman_tree::huffman_tree(std::vector<bool> const& structure, std::vector<char> const& symbols) {
    if (symbols.empty() || structure.empty()) {
        error();
    }
    root = std::make_shared<Node>(Node(0, false));
    std::vector<std::shared_ptr<Node>> parents;
    auto node = root;
    size_t i = 0;
    for (bool tmp : structure) {
        if (parents.empty() && tmp == 1) {
            error();
        }
        if (tmp == 0) {
            node->left = std::make_shared<Node>(Node(0, false));
            parents.push_back(node);
            node = node->left;
        } else {
            if (i >= symbols.size()) {
                error();
            }
            node.get()->symbol = symbols[i++];
            node.get()->is_terminal = true;
            do {
                node = parents.back();
                parents.pop_back();
            } while (node->right != nullptr);
            node->right = std::make_shared<Node>(Node(0, false));
            parents.push_back(node);
            node = node->right;
        }
    }

    if (i >= symbols.size()) {
        error();
    }
    node.get()->symbol = symbols[i++];
    node.get()->is_terminal = true;

    if (i < symbols.size()) {
        error();
    }

    huffman_tree::tree_structure = structure;
    huffman_tree::symbols = symbols;
    huffman_tree::init_codes(root);
}

huffman_tree::~huffman_tree() {
    delete[] (codes - shift);
}

std::pair<unsigned long long, unsigned int> const* huffman_tree::get_codes() {
    return huffman_tree::codes;
}

std::vector<bool> const& huffman_tree::get_tree_structure() {
    return tree_structure;
}

std::vector<char> const& huffman_tree::get_symbols() {
    return symbols;
}

void huffman_tree::init_codes(std::shared_ptr<huffman_tree::Node> node,
                              unsigned long long code,
                              unsigned int size) {
    if (node->is_terminal) {
        codes[node->symbol] = {code, size};
        return;
    }
    if (node->left != nullptr) {
        init_codes(node->left, code, size + 1);
    }
    if (node->right != nullptr) {
        code |= (1u << size);
        init_codes(node->right, code, size + 1);
    }
}

void huffman_tree::init_tree_structure(std::shared_ptr<Node> node) {
    if (node->left != nullptr) {
        tree_structure.push_back(0);
        init_tree_structure(node->left);
        init_tree_structure(node->right);
    } else {
        tree_structure.push_back(1);
    }
    if (node == root) {
        tree_structure.pop_back();
    }
}

void huffman_tree::init_symbols(std::shared_ptr<huffman_tree::Node> node) {
    if(node->left != nullptr) {
        init_symbols(node->left);
    }
    if (node->is_terminal) {
        symbols.push_back(node->symbol);
    }
    if (node->right != nullptr) {
        init_symbols(node->right);
    }
}

void huffman_tree::error(std::string message) {
    if (!message.empty()) {
        message = " (" + message + ")";
    }
    throw std::runtime_error("decoding error" + message);
}

size_t huffman_tree::decode(char const* array, unsigned int size, char* answer) {
    if (!root) {
        error();
    }
    Node* node = root.get();
    unsigned int i = 0, j = 0, pos = 0;
    while (i * 8 + j < size) {
        if (node->is_terminal) {
            *(answer + pos++) = node->symbol;
            node = root.get();
        } else if ((array[i] >> j) & 1) {
            if (!node->right) {
                error();
            }
            node = node->right.get();

            if (j == 7)
                ++i, j = 0;
            else
                ++j;
        } else {
            if (!node->left) {
                error();
            }
            node = node->left.get();

            if (j == 7)
                ++i, j = 0;
            else
                ++j;
        }
    }
    if (node->is_terminal) {
        *(answer + pos++) = node->symbol;
    } else {
        error();
    }
    return pos;
}

