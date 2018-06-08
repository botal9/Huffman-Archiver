//
// Created by vitalya on 08.06.18.
//

#include <iostream>
#include <climits>
#include "gtest/gtest.h"
#include "archiver.h"


TEST(correctness, empty_message) {
std::stringstream in("");
std::stringstream enc;
std::stringstream dec;

archiver::encode(in, enc);
archiver::decode(enc, dec);
EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, one_symbol_message) {
std::stringstream in("a");
std::stringstream enc;
std::stringstream dec;

archiver::encode(in, enc);
archiver::decode(enc, dec);
EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, some_symbols_message) {
    std::stringstream in("abacabadabacabaeabacabadabacabafabacabadabacabaeabacabadabacaba");
    std::stringstream enc;
    std::stringstream dec;

    archiver::encode(in, enc);
    archiver::decode(enc, dec);
    EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, one_sentence_message) {
std::stringstream in("From Russia with love");
std::stringstream enc;
std::stringstream dec;

archiver::encode(in, enc);
archiver::decode(enc, dec);
EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, all_chars_message) {
std::string s = "";
for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
    s += (char)i;
}
std::stringstream in(s);
std::stringstream enc;
std::stringstream dec;
archiver::encode(in, enc);
archiver::decode(enc, dec);
EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, big_message) {
std::string s = "";
for (char i = 0; i < 20; ++i) {
    for (int j = 0; j < (1 << i); ++j) {
        s += i;
    }
}
std::stringstream in(s);
std::stringstream enc;
std::stringstream dec;

archiver::encode(in, enc);
archiver::decode(enc, dec);
EXPECT_EQ(in.str(), dec.str());
}

TEST(correctness, corrupted_message) {
std::stringstream enc("1337");
std::stringstream dec;
EXPECT_ANY_THROW(archiver::decode(enc, dec));
}