#pragma once 
#include <string>
#include <vector>

struct lexical{
    int line;
    std::string name;
    std::string token;
    int qtd_error;
    std::string error_messages;
};

std::vector<struct lexical> lexical_analyser(std::string);
bool in_array(const std::string &value, const std::vector<std::string> &array);
int check_char(char c, unsigned int line, unsigned int current_state);
void print_lexical_analyser(std::vector<struct lexical> *analysed);