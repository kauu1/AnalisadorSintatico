#pragma once

#include <iterator>
#include <string>
#include <vector>
#include "lexical.h"

struct syntactic{
    std::vector<lexical> lexical_analyser_results;
    unsigned int position;
    unsigned int errors = 0;
};

void next(struct syntactic* synt);

void procedure_activation(struct syntactic* synt);

void factor2(struct syntactic* synt);

void factor(struct syntactic* synt);

void term2(struct syntactic* synt);

void term(struct syntactic* synt);

void simple_expression2(struct syntactic* synt);

void simple_expression(struct syntactic* synt);

void expression_list2(struct syntactic* synt);

void expression_list(struct syntactic* synt);

void expression(struct syntactic* synt);

void command(struct syntactic* synt);

void command_list2(struct syntactic* synt);

void command_list(struct syntactic* synt);

void compost_command(struct syntactic* synt);

void parameter_list2(struct syntactic* synt);

void parameter_list(struct syntactic* synt);

void arguments(struct syntactic* synt);

void subprogram_declaration(struct syntactic *synt);

void subprograms_declaration(struct syntactic* synt);

void subprograms_declaration2(struct syntactic *synt);

void syntactic_init(struct syntactic* synt, std::string file_path, int option);

void program(struct syntactic* synt);

void variable_declaration(struct syntactic* synt);

void variable_declaration_list(struct syntactic* synt);

void variable_declaration_list2(struct syntactic* synt);

void identifier_list(struct syntactic* synt);

void identifier_list2(struct syntactic* synt);

bool in_array(const std::string &value, const std::vector<std::string> &array);


