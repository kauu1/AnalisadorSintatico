#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "lexical.h"
#include "syntactic.h"

int main(){
    std::string file_path = "benchmark-arquivos_testes/Test1_copy.pas";

    struct syntactic *analyser = (struct syntactic*) malloc(sizeof(struct syntactic));

    syntactic_init(analyser, file_path);

    //print_lexical_analyser(&(analyser->lexical_analyser_results));

    program(analyser);
}
