//
// Created by vitalya on 08.06.18.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "lib/src/archiver.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Wrong number of arguments" << std::endl;
        return 0;
    }
    auto parameter = argv[1];
    auto source = argv[2];
    auto target = argv[3];

    std::ifstream in(source, std::ifstream::binary);
    std::ofstream out(target, std::ofstream::binary);
    if (!in.is_open() || !out.is_open()) {
        std::cout << "Can't open input/output file" << std::endl;
        return 0;
    }

    if (strcmp(parameter, "-e") == 0) {
        try {
            archiver::encode(in, out);
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    } else if (strcmp(parameter, "-d") == 0) {
        try {
            archiver::decode(in, out);
        } catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    } else {
        std::cout << "Wrong arguments" << std::endl;
    }
    in.close();
    out.close();
}