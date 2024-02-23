#include <cstddef>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "lexico.h"

int main(){
    std::string file_path = "benchmark-arquivos_testes/Test1.pas";
    std::vector<struct lexical> lexical_analyser_results;

    lexical_analyser_results = lexical_analyser(file_path);
    
}
