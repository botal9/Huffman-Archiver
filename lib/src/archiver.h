//
// Created by vitalya on 05.06.18.
//

#ifndef HUFFMAN_ARCHIVER_H
#define HUFFMAN_ARCHIVER_H

#include "huffman_tree.h"
#include <fstream>

class archiver {
public:
    static void encode(std::istream& in, std::ostream& out);

    static void decode(std::istream& in, std::ostream& out);
};


#endif //HUFFMAN_ARCHIVER_H
