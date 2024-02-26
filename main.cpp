#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "lexical.h"
#include "syntactic.h"

int main(int argc, char *argv[]){
    std::string file_path = argv[1];

    struct syntactic *analyser = (struct syntactic*) malloc(sizeof(struct syntactic));

    syntactic_init(analyser, file_path);

    program(analyser);

    if(analyser->errors > 0){
        std::cerr << "Reported " << analyser->errors << " errors" << std::endl;
    }
}
