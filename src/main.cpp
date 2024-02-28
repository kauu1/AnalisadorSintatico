#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "lexical.h"
#include "syntactic.h"

int main(int argc, char *argv[]){
    std::string file_path = argv[2];

    struct syntactic *analyser = (struct syntactic*) malloc(sizeof(struct syntactic));

    syntactic_init(analyser, file_path, atoi(argv[1]));

    program(analyser);

    if(analyser->errors > 0){
        std::cerr << "Reported " << analyser->errors << " syntactic errors" << std::endl;
    }

    //print_lexical_analyser(&analyser->lexical_analyser_results);
    free(analyser);
    
}
