#include "syntactic.h"
#include "lexical.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> types = {"real", "integer", "boolean"};

bool in_array(std::string &value, std::vector<std::string> &array)
{
    return std::find(array.begin(), array.end(), value) != array.end();
}
void syntactic_init(struct syntactic *synt, std::string file_path, int option){
    std::vector<lexical> aux;

    if (option == 1) {
        aux = lexical_analyser_AFD(file_path);

    }else if (option == 2){
        aux = lexical_analyser_ReGex(file_path);
    }
    
    synt->lexical_analyser_results = aux;
    synt->position = 0;
    synt->errors = 0;

}

void next(struct syntactic* synt){
    if(synt->position < synt->lexical_analyser_results.size() - 1){
        synt->position++;
    }
}

void factor(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### factor" << std::endl;
    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        next(synt);
        if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
            expression_list(synt);

            if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
                next(synt);
            }
            else{
                synt->errors++;
                std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \")\"\n";
            }
        }
    }

    if(synt->lexical_analyser_results[synt->position].type == Integer ||
    synt->lexical_analyser_results[synt->position].type == Float ||
    synt->lexical_analyser_results[synt->position].type == Boolean){
        next(synt);
    }

    if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
        next(synt);

        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
            next(synt);
        }
        else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \")\"\n";
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("not") != std::string::npos){
        next(synt);
        factor(synt);
    }
      

}

void term(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### term" << std::endl;
    factor(synt);

    if(synt->lexical_analyser_results[synt->position].type == Multiplicative_operator){
        next(synt);
        term(synt);
    }
}

void simple_expression2(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### simple_expression2" << std::endl;
    if(synt->lexical_analyser_results[synt->position].type == Aditive_operator){
        next(synt);
        term(synt);
        simple_expression2(synt);
    }

}

void simple_expression(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### simple_expression" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find("+") != std::string::npos
    || synt->lexical_analyser_results[synt->position].token.find("-") != std::string::npos){

        next(synt);
        term(synt);
        simple_expression2(synt);
    }else {
        term(synt);
        simple_expression2(synt);
    }

}

void expression(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### expression" << std::endl;
    simple_expression(synt);

    if(synt->lexical_analyser_results[synt->position].type == Relational_operator){
        next(synt);
        simple_expression(synt);
    }
    
}

void expression_list2(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### expression_list2" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find(",") != std::string::npos){
        next(synt);
        expression(synt);
        expression_list2(synt);
    }else if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
        next(synt);
        return;
    }else{
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position-1].line << " ERRO: Expected \")\"\n";
    }

}

void expression_list(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### expression_list" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
        next(synt);
        return;
    }else {
        expression(synt);
        expression_list2(synt);
    }
}

void command(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### command" << std::endl;
    
    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        next(synt);
        if(synt->lexical_analyser_results[synt->position].token.find(":=") != std::string::npos){
            next(synt);
            expression(synt);
        }
        else if (synt->lexical_analyser_results[synt->position].token.find("=") != std::string::npos){
            next(synt);
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\" before \"=\"\n";
            expression(synt);

        }else if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
            next(synt);
            expression_list(synt);
                
        }
        
        if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
            next(synt);
        }
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position-1].line << " ERRO: Expected \";\"\n";
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("begin") != std::string::npos){
        compost_command(synt);
    }

    if(synt->lexical_analyser_results[synt->position].token.find("if") != std::string::npos){
        next(synt);
        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("then") != std::string::npos){
            next(synt);
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"then\"\n";
        }

        command(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("else") != std::string::npos){

            next(synt);
            command(synt);
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("while") != std::string::npos){
        next(synt);
        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("do") != std::string::npos){}
        else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"do\"\n";
        }

        next(synt);
        command(synt);
    }
}

void command_list2(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### command_list2" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        return;
    }

    command(synt);

    command_list2(synt);
}

void command_list(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### command_list" << std::endl;
    command(synt);

    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        return;
    }

    command_list2(synt);
}

void compost_command(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### compost_command" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find("begin") != std::string::npos){
        next(synt);
    }
    else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"begin\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        next(synt);
        return;
    }
    
    command_list(synt);

    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        next(synt);
    }else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"end\"\n";
    }
}

void parameter_list2(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### program" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find(",") != std::string::npos){

        next(synt);
        identifier_list(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){

        }else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
        }

        next(synt);
        if(in_array(synt->lexical_analyser_results[synt->position].token, types)){

        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
        }

        next(synt);

        parameter_list2(synt); 

    }
}

void parameter_list(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### program" << std::endl;
    identifier_list(synt);

    if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){

    }else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
    }

    next(synt);
    if(in_array(synt->lexical_analyser_results[synt->position].token, types)){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
    }

    next(synt);

    parameter_list2(synt);
}

void arguments(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### program" << std::endl;
    if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
        next(synt);
        parameter_list(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){}
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \")\"\n";
        }

        next(synt);
    }   
}
void subprogram_declaration(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("procedure") != std::string::npos){

        next(synt);

        if(synt->lexical_analyser_results[synt->position].type == Identifier){
            next(synt);
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
        }

        arguments(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
            next(synt);
        }
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
        }

        variable_declaration(synt);

        subprograms_declaration(synt);

        compost_command(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
            next(synt);
        }
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position-1].line << " ERRO: Expected \";\"\n";
        }
    }
}

void subprograms_declaration(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("procedure") != std::string::npos){
        
        subprogram_declaration(synt);

        subprograms_declaration(synt);
    }
}

void identifier_list2(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### program" << std::endl;

    if (synt->lexical_analyser_results[synt->position].token.find(",") != std::string::npos) {

        next(synt);

        if(synt->lexical_analyser_results[synt->position].type == Identifier){
            next(synt);
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
        }
        
        identifier_list2(synt);
    }

}

void identifier_list(struct syntactic* synt){
    //std::cout << synt->lexical_analyser_results[synt->position].token << ' ' << "### program" << std::endl;
    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
    }

    identifier_list2(synt);
}



void variable_declaration_list(struct syntactic *synt){
    
    identifier_list(synt);

    if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){
        next(synt);
    }else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
    }

    if(in_array(synt->lexical_analyser_results[synt->position].token, types)){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
    }

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        variable_declaration_list(synt);
    }
    
}

void variable_declaration(struct syntactic *synt){ 
    if(synt->lexical_analyser_results[synt->position].token.find("var") != std::string::npos){
        next(synt);
        variable_declaration_list(synt);
    }
}

void program(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("program") != std::string::npos){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"program\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"IDENTIFIER\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
        next(synt);
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
        next(synt);
    }

    variable_declaration(synt);
    subprograms_declaration(synt);
    compost_command(synt);

    if(synt->lexical_analyser_results[synt->position].token.find(".") != std::string::npos){}
    else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \".\"\n";
    }
}