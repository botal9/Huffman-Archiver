//
// Created by vitalya on 05.06.18.
//

#include <iostream>
#include <bitset>
#include <sstream>
#include <cassert>
#include "archiver.h"

size_t const BUFFER_SIZE = 2 * 1024 * 1024;

void archiver::encode(std::istream& in, std::ostream& out) {
    char out_buff[2 * BUFFER_SIZE];
    char in_buff[BUFFER_SIZE];
    std::map<char, unsigned long long> frequency;
    in.seekg(0, std::ios::end);
    auto message_size = (unsigned long long) in.tellg();
    in.seekg(0, std::ios::beg);
    if (message_size == 0) {
        return;
    }
    while (message_size) {
        if (BUFFER_SIZE < message_size) {
            in.read(in_buff, BUFFER_SIZE);
            message_size -= BUFFER_SIZE;
        } else {
            in.read(in_buff, (size_t)message_size);
            message_size = 0;
        }
        size_t const in_size = static_cast<size_t>(in.gcount());
        for (size_t j = 0; j < in_size; ++j) {
            frequency[in_buff[j]]++;
        }
    }
    if (frequency.size() == 1) {
        frequency.insert({char(frequency.begin()->first + 1), 1});
    }

    auto tree = huffman_tree(frequency);
    auto codes = tree.get_codes();
    auto structure = tree.get_tree_structure();
    auto symbols = tree.get_symbols();

    *reinterpret_cast<short int*>(out_buff)  = (short int)frequency.size();
    size_t i = sizeof(short int);
    for (size_t j = 0; j < symbols.size(); ++j, ++i) {
        out_buff[i] = symbols[j];
    }
    for (size_t j = 0; j < structure.size(); j += sizeof(char) * 8) {
        char tmp = 0;
        for (size_t k = 0; k < sizeof(char) * 8; ++k) {
            if (j + k < structure.size()) {
                tmp |= (structure[j + k] << k);
            }
        }
        out_buff[i++] = tmp;
    }
    out.write(out_buff, i);

    in.seekg(0, std::ios::beg);
    while (in) {
        size_t ibit = 0, j, k;
        char tmp = 0;
        in.read(in_buff, BUFFER_SIZE);
        size_t const in_size = static_cast<size_t>(in.gcount());
        unsigned int bit_counter = 0;
        i = sizeof(unsigned int);
        for (j = 0; j < in_size; ++j) {
            auto code = codes[in_buff[j]];
            bit_counter += code.second;
            for (k = 0; k < code.second && ibit < sizeof(char) * 8; ++k, ++ibit) {
                char bit = (code.first >> k) & 1;
                tmp |= (bit << ibit);
            }
            if (ibit == sizeof(char) * 8) {
                out_buff[i++] = tmp;
                tmp = 0;
                ibit = 0;
            }
            for (; k + sizeof(char) * 8 < code.second; k += sizeof(char) * 8) {
                out_buff[i++] = (char)(code.first >> k);
            }
            for (; k < code.second; ++k, ++ibit) {
                char bit = (code.first >> k) & 1;
                tmp |= (bit << ibit);
            }
        }
        if (ibit > 0) {
            out_buff[i++] = tmp;
        }
        unsigned int out_size = (unsigned int)(i - sizeof(unsigned int) - (ibit != 0)) * sizeof(char) * 8 + ibit;
        *reinterpret_cast<unsigned int*>(out_buff) = out_size;
        out.write(out_buff, i);
    }
}

void archiver::decode(std::istream &in, std::ostream &out) {
    size_t const MINIMUM_MESSAGE_SIZE = 10 - 1;
    char in_buff[2 * BUFFER_SIZE];
    char out_buff[BUFFER_SIZE];
    in.seekg(0, std::ios::end);
    auto message_size = (unsigned long long) in.tellg();
    in.seekg(0, std::ios::beg);
    if (message_size == 0) {
        return;
    }
    if (message_size < MINIMUM_MESSAGE_SIZE) {
        throw std::runtime_error("decoding error");
    }
    size_t amount_of_symbols = 0;
    in.read(reinterpret_cast<char*>(&amount_of_symbols), sizeof(short int));
    if (amount_of_symbols <= 0) {
        throw std::runtime_error("decoding error");
    }
    size_t tree_structure_size = (size_t)amount_of_symbols * 2 - 2;
    size_t i = 0;
    std::vector<char> symbols;
    std::vector<bool> structure;
    in.read(in_buff, amount_of_symbols + (tree_structure_size + sizeof(char) * 8 - 1) / (sizeof(char) * 8));
    for (; i < (size_t)amount_of_symbols; ++i) {
        symbols.push_back(in_buff[i]);
    }
    while (tree_structure_size > 0) {
        char tmp = 0;
        if (tree_structure_size >= sizeof(char) * 8) {
            tmp = in_buff[i++];
            for (size_t j = 0; j < sizeof(char) * 8; ++j) {
                structure.push_back((tmp >> j) & 1);
            }
            tree_structure_size -= sizeof(char) * 8;
        } else {
            tmp = in_buff[i++];
            for (size_t j = 0; j < tree_structure_size; ++j) {
                structure.push_back((tmp >> j) & 1);
            }
            tree_structure_size = 0;
        }
    }

    auto tree = huffman_tree(structure, symbols);

    while (in.peek() != std::istream::traits_type::eof()) {
        unsigned int bit_size = 0;
        in.read(reinterpret_cast<char*>(&bit_size), sizeof(unsigned int) / sizeof(char));
        unsigned int size = (bit_size + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
        in.read(in_buff, size);
        size_t out_size = tree.decode(in_buff, bit_size, out_buff);
        out.write(out_buff, out_size);
    }
}