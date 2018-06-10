//
// Created by vitalya on 05.06.18.
//

#include <iostream>
#include "archiver.h"

size_t const BUFFER_SIZE = 2 * 1024 * 1024;
int const range = (1u << 8u);
int const shift = range / 2;

void archiver::encode(std::istream& in, std::ostream& out) {
    double timer = clock();

    char out_buff[2 * BUFFER_SIZE];
    char in_buff[BUFFER_SIZE];
    in.seekg(0, std::ios::end);
    auto message_size = (unsigned long long) in.tellg();
    in.seekg(0, std::ios::beg);
    if (message_size == 0) {
        return;
    }
    unsigned long long* frequency = new unsigned long long[range] + shift;
    std::fill(frequency - shift, frequency + shift, 0);
    frequency[0] = frequency[1] = 1;

    while (message_size) {
        if (BUFFER_SIZE < message_size) {
            in.read(in_buff, BUFFER_SIZE);
            message_size -= BUFFER_SIZE;
        } else {
            in.read(in_buff, (size_t)message_size);
            message_size = 0;
        }
        auto in_size = static_cast<size_t>(in.gcount());
        for (size_t j = 0; j < in_size; ++j) {
            ++frequency[in_buff[j]];
        }
    }

    auto tree = huffman_tree(frequency);
    auto codes = tree.get_codes();
    auto structure = tree.get_tree_structure();
    auto symbols = tree.get_symbols();

    out_buff[0] = (char)((int)symbols.size() - 1 - shift);
    size_t i = sizeof(char);
    for (size_t j = 0; j < symbols.size(); ++j, ++i) {
        out_buff[i] = symbols[j];
    }
    for (size_t j = 0; j < structure.size(); j += 8) {
        char tmp = 0;
        for (size_t k = 0; k < 8; ++k) {
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
        auto in_size = static_cast<size_t>(in.gcount());
        unsigned int bit_counter = 0;
        i = sizeof(unsigned int);
        for (j = 0; j < in_size; ++j) {
            auto code = codes[in_buff[j]];
            bit_counter += code.second;
            for (k = 0; k < code && ibit < 8; ++k, ++ibit) {
                char bit = (code >> k) & 1;
                tmp |= (bit << ibit);
            }
            if (ibit == 8) {
                out_buff[i++] = tmp;
                tmp = 0;
                ibit = 0;
            }
            for (; k + 8 < code.second; k += 8) {
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
        unsigned int out_size = (unsigned int)(i - sizeof(unsigned int) - (ibit != 0)) * 8 + (unsigned int)ibit;
        for (unsigned int l = 0; l < sizeof(unsigned int); ++l) {
            out_buff[l] = (char)(out_size >> (l * 8));
        }
        out.write(out_buff, i);
    }

    double executing_time = (clock() - timer) / CLOCKS_PER_SEC;
    std::clog << "Encoding completed in " << executing_time << " seconds" << std::endl;

    delete[] (frequency - shift);
}

void archiver::decode(std::istream &in, std::ostream &out) {
    double timer = clock();

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
    char cnt = 0;
    in.read(&cnt, sizeof(char));
    int amount_of_symbols = (int)cnt + 1 + shift;
    if (amount_of_symbols <= 0) {
        throw std::runtime_error("decoding error");
    }
    size_t tree_structure_size = (size_t)amount_of_symbols * 2 - 2;
    size_t i = 0;
    std::vector<char> symbols;
    std::vector<bool> structure;
    in.read(in_buff, amount_of_symbols + (tree_structure_size + 7) / 8);
    if (in.gcount() != amount_of_symbols + (tree_structure_size + 7) / 8) {
        throw std::runtime_error("decoding error");
    }
    for (; i < (size_t)amount_of_symbols; ++i) {
        symbols.push_back(in_buff[i]);
    }
    while (tree_structure_size > 0) {
        char tmp = 0;
        if (tree_structure_size >= 8) {
            tmp = in_buff[i++];
            for (size_t j = 0; j < 8; ++j) {
                structure.push_back((tmp >> j) & 1);
            }
            tree_structure_size -= 8;
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
        for (unsigned int j = 0; j < sizeof(unsigned int); ++j) {
            char tmp = 0;
            in.read(&tmp, sizeof(char));
            bit_size += (((unsigned int)tmp & 0xff) << (j * 8));
        }
        unsigned int size = (bit_size + 7) / 8;
        if (size == 0) {
            break;
        }
        in.read(in_buff, size);
        if (in.gcount() != size) {
            throw std::runtime_error("decoding error");
        }
        size_t out_size = tree.decode(in_buff, bit_size, out_buff);
        out.write(out_buff, out_size);
    }

    double executing_time = (clock() - timer) / CLOCKS_PER_SEC;
    std::clog << "Decoding completed in " << executing_time << " seconds" << std::endl;
}