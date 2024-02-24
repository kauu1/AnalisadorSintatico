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
void syntactic_init(struct syntactic *synt, std::string file_path){
    std::vector<lexical> aux;
    aux = lexical_analyser(file_path);
    synt->lexical_analyser_results = aux;
    synt->position = 0;
    synt->errors = 0;
    //synt->error_messages.clear();
}

void factor(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        synt->position++;
        if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
            expression_list(synt);

            if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
                synt->position++;
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
        synt->position++;
    }

    if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
        synt->position++;

        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){
            synt->position++;
        }
        else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \")\"\n";
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("not") != std::string::npos){
        synt->position++;
        factor(synt);
    }
      

}

void term(struct syntactic* synt){
    factor(synt);

    if(synt->lexical_analyser_results[synt->position].type == Relational_operator){
        synt->position++;
        term(synt);
    }
}

void simple_expression2(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].type == Aditive_operator){
        synt->position++;
        term(synt);
        simple_expression2(synt);
    }

}

void simple_expression(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("+") != std::string::npos
    || synt->lexical_analyser_results[synt->position].token.find("-") != std::string::npos){

        synt->position++;
        term(synt);
        simple_expression2(synt);
    }else {
        term(synt);
        simple_expression2(synt);
    }

}

void expression(struct syntactic* synt){
    simple_expression(synt);

    if(synt->lexical_analyser_results[synt->position].type == Relational_operator){
        synt->position++;
        simple_expression(synt);
    }

}

/*
void expression_list2(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        return;
    }else {
        expression(synt);
        expression_list2(synt);
    }
}*/

void expression_list(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        return;
    }else {
        expression(synt);
        expression_list(synt);
    }
}

void command(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        synt->position++;

        if(synt->lexical_analyser_results[synt->position].token.find(":=") != std::string::npos){
            synt->position++;
            expression(synt);
        }
        else {
            if(synt->lexical_analyser_results[synt->position].token.find("(")){
                synt->position++;
                expression_list(synt);
            }else {
                
            }
            
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("begin") != std::string::npos){
        compost_command(synt);
    }

    if(synt->lexical_analyser_results[synt->position].token.find("if") != std::string::npos){
        synt->position++;
        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("then") != std::string::npos){
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
        }

        synt->position++;

        command(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("else") != std::string::npos){

            synt->position++;
            command(synt);
        }
    }

    if(synt->lexical_analyser_results[synt->position].token.find("while") != std::string::npos){
        synt->position++;
        expression(synt);

        if(synt->lexical_analyser_results[synt->position].token.find("do") != std::string::npos){}
        else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"do\"\n";
        }

        synt->position++;
        command(synt);
    }
}

void command_list2(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){
        return;
    }

    synt->position++;

    command(synt);

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){}
    else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
    }

    synt->position++;

    command_list2(synt);
}

void command_list(struct syntactic* synt){
    command(synt);
    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){}
    else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
    }

    synt->position++;

    command_list2(synt);
}

void compost_command(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("begin") != std::string::npos){}
    else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"begin\"\n";
    }

    synt->position++;

    if(synt->lexical_analyser_results[synt->position].token.find("end") != std::string::npos){

    }else {
        command_list(synt);
    }
}

void parameter_list2(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find(",") != std::string::npos){

        synt->position++;
        identifier_list(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){

        }else{
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
        }

        synt->position++;
        if(in_array(synt->lexical_analyser_results[synt->position].token, types)){

        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
        }

        synt->position++;

        parameter_list2(synt); 

    }
}

void parameter_list(struct syntactic* synt){
    identifier_list(synt);

    if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){

    }else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
    }

    synt->position++;
    if(in_array(synt->lexical_analyser_results[synt->position].token, types)){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
    }

    synt->position++;

    parameter_list2(synt);
}

void arguments(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].token.find("(") != std::string::npos){
        synt->position++;
        parameter_list(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(")") != std::string::npos){}
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \")\"\n";
        }

        synt->position++;
    }   
}
void subprogram_declaration(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("procedure") != std::string::npos){

        synt->position++;

        if(synt->lexical_analyser_results[synt->position].type == Identifier){
            
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
        }

        synt->position++;

        arguments(synt);

        if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){}
        else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
        }

        synt->position++;

        variable_declaration(synt);

        subprograms_declaration(synt);

        compost_command(synt);


    }
}

void subprograms_declaration(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("procedure") != std::string::npos){
        
        subprogram_declaration(synt);

        subprograms_declaration(synt);
    }
}

void identifier_list2(struct syntactic* synt){
    synt->position++;

    if (synt->lexical_analyser_results[synt->position].token.find(",") != std::string::npos) {

        synt->position++;

        if(synt->lexical_analyser_results[synt->position].type == Identifier){
            
        }else {
            synt->errors++;
            std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
        }
        
        identifier_list2(synt);
    }

}

void identifier_list(struct syntactic* synt){
    if(synt->lexical_analyser_results[synt->position].type == Identifier){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected IDENTIFIER\n";
    }

    identifier_list2(synt);
}



void variable_declaration_list(struct syntactic *synt){
    
    identifier_list(synt);


    if(synt->lexical_analyser_results[synt->position].token.find(":") != std::string::npos){

    }else{
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \":\"\n";
    }

    synt->position++;
    if(in_array(synt->lexical_analyser_results[synt->position].token, types)){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected type\n";
    }

    synt->position++;

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
    }

    synt->position++;

    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        variable_declaration_list(synt);
    }
    
}

void variable_declaration(struct syntactic *synt){ 
    if(synt->lexical_analyser_results[synt->position].token.find("var") != std::string::npos){

        synt->position++;
        variable_declaration_list(synt);
    }
}

void program(struct syntactic *synt){
    if(synt->lexical_analyser_results[synt->position].token.find("program") != std::string::npos){
        synt->position++;
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"program\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].type == Identifier){
        synt->position++;
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"IDENTIFIER\"\n";
    }

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){
        synt->position++;
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
        synt->position++;
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