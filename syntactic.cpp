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

void command(struct syntactic* synt){

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
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"begin\"\n";
    }

    synt->position++;

    command_list2(synt);
}

void command_list(struct syntactic* synt){
    command(synt);
    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){}
    else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"begin\"\n";
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
    synt->position++;
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
    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"program\"\n";
    }
    
    synt->position++;

    if(synt->lexical_analyser_results[synt->position].type == Identifier){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \"IDENTIFIER\"\n";
    }

    synt->position++;

    if(synt->lexical_analyser_results[synt->position].token.find(";") != std::string::npos){

    }else {
        synt->errors++;
        std::cerr << "Line " << synt->lexical_analyser_results[synt->position].line << " ERRO: Expected \";\"\n";
    }

    synt->position++;

    variable_declaration(synt);
    subprograms_declaration(synt);
}