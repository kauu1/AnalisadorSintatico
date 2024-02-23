#include "syntactic.h"
#include "lexical.h"
#include <vector>

void syntactic_init(struct syntactic *synt, std::string file_path){
    std::vector<lexical> *aux;
    *aux = lexical_analyser(file_path);
    synt->lexical_analyser_results = aux;
    synt->position = 0;
    synt->errors = 0;
    synt->error_messages.clear();
    
}