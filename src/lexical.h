#pragma once 
#include <regex>
#include <string>
#include <vector>

enum Type{
    Key_word,
    Multiplicative_operator,
    Aditive_operator,
    Relational_operator,
    Integer,
    Identifier,
    Delimiter,
    Atribution,
    Float,
    Comment_open,
    Comment_closed,
    Boolean,
    Words
};

struct lexical{
    int line;
    std::string token;
    enum Type type;
};

std::vector<struct lexical> lexical_analyser_AFD(std::string);
std::vector<struct lexical> lexical_analyser_ReGex(std::string);
bool in_array(const std::string &value, const std::vector<std::string> &array);
int check_char(char c, unsigned int line, unsigned int current_state);
void print_lexical_analyser(std::vector<struct lexical> *analysed);