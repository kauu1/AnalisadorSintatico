#pragma once

#include <string>
#include <vector>
#include "lexical.h"

struct syntactic{
    std::vector<lexical> *lexical_analyser_results;
    unsigned int position;
    unsigned int errors = 0;
    std::vector<std::string> error_messages;
};

void syntactic_init(struct syntactic* synt, std::string file_path);

void program(struct syntactic* synt);




