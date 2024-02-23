#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
#include "lexical.h"

//checa se um item pertence em uma lista
bool in_array(const std::string &value, const std::vector<std::string> &array)
{
    return std::find(array.begin(), array.end(), value) != array.end();
}

//checa se um char pertence a um alfabeto (nesse caso tabela ASCII - &)
int check_char(char c, unsigned int line, unsigned int current_state){

    for(unsigned char cmp = 0; cmp < 255; cmp++){
        if(cmp == 38){
            continue;
        }
        if(c == cmp){
            return 1;
        }
    }
    if(current_state != 1){
        return -1;
    }   

    return 0;
    /*
    if(alphabet.find(c)==std::string::npos){
        if(current_state != 1){
            std::cerr << "Erro: '" << c << "' does not belong to the language alphabet in line: " << line << std::endl;
        }
        return 0;
    }else{
        return 1;
    }*/
}


std::vector<struct lexical> lexical_analyser(std::string file_path){

    std::vector<struct lexical> result;

    //abertura do arquivo de leitura
    std::ifstream program_template;
    program_template.open(file_path, std::ifstream::in);

    if(program_template.is_open()){
        std::cout << "File opened successfully" << std::endl;
    }else{
        std::cerr << "Cound not find suitable file" << std::endl;
        return result;
    }

    //a leitura sera feita caractere por caractere do arquivo de leitura
    //esse caractere sera denominado "c"
    char c;

    unsigned int line = 1; //contador de linhas

    unsigned int comment_open_line = 0; //linha que um comentario foi aberto

    //comment[0]: representa uma abertura de comentario
    //comment[1]: representa o fechamento de comentario
    //caso comment[0] != comment[1], o comentario nao foi fechado

    unsigned short int comment[2] = {0, 0};

    unsigned short int current_state = 0;

    std::vector<std::string>misc_chars = {" ", "\t" "\n"};

    std::vector<std::string>key_words = {"program", "var", "integer", "real", "boolean", "procedure",
    "begin", "end", "if", "then", "else", "while", "do", "not"};

    std::vector<std::string>delimiters = {";", ".", "(", ")", ","};

    //std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 \t\n{}();:.(),=<>+-*/";

    std::string atribution = ":=";

    std::vector<std::string>relacional_operators= {"=", "<", ">"};

    std::vector<std::string>relacional_operators2= {"<=", ">=", "<>"};

    std::vector<std::string>additive_operators = {"+", "-"};

    std::vector<std::string>multiplicative_operators = {"*", "/"}; 

    std::string word = "\0";

    std::string numbers = "0123456789";

    std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

    std::string numbersletters = letters + numbers;
    
    struct lexical aux;
    
    //cada caractere do arquivo sera lido e armazenado em "c" ate o EOF
    while(program_template.get(c)){
        
        if(current_state == 0){
            word = "\0";
        }
        
        //se um caractere existe no alfabeto e o estado do automato eh diferente de 0, ele sera adicionado a uma palavra
        if(check_char(c, line, current_state)){
            word = word+c;
        }else { 
            continue;
        }

        switch (current_state) { 

            //estado 0: estado inicial
            case 0:
                if (in_array(word, misc_chars)) current_state = 0;

                else if (word.find('{')!=std::string::npos){ 
                    current_state = 1;
                    comment_open_line = line;
                    ++comment[0];
                    aux.line = line;
                    aux.name = "{";
                    aux.token = "comment_open";
                    result.push_back(aux);
                }

                else if (letters.find(word)!=std::string::npos) current_state = 2;
                
                else if (numbers.find(word)!=std::string::npos) current_state = 3;

                else if (in_array(word, delimiters)) current_state = 4;

                else if (in_array(word, relacional_operators)) current_state = 5;

                else if (in_array(word, additive_operators)) current_state = 6;
                 
                else if (in_array(word, multiplicative_operators)) current_state = 7;

                else if (word.find(':')!=std::string::npos) current_state = 8;
                
                break;

            //estado 1: comentário
            case 1:
                if(word.back() == '}') {
                    current_state = 0;
                    ++comment[1];
                    aux.line = line;
                    aux.name = "}";
                    aux.token = "comment_closed";
                    result.push_back(aux);
                }
                break;

            //estado 2: assim que se acaba de ler uma palavra, é verificado sé a palavra é uma palavra reservada, operadores lógicos ou indentificador
            case 2:
                if(numbersletters.find(word.back())!=std::string::npos) current_state = 2;

                else{ 
                    current_state = 0;

                    word.pop_back();

                    if(in_array(word, key_words)){
                        aux.line = line;
                        aux.name = word;
                        aux.token = "key_word";
                        result.push_back(aux);


                    }else if (word.find("and")!=std::string::npos) {
                        aux.line = line;
                        aux.name = word;
                        aux.token = "multiplicative_operator";
                        result.push_back(aux);


                    }else if (word.find("or")!=std::string::npos) {
                        aux.line = line;
                        aux.name = word;
                        aux.token = "aditive_operator";
                        result.push_back(aux);


                    }else{
                        aux.line = line;
                        aux.name = word;
                        aux.token = "identifier";
                        result.push_back(aux);

                    }

                    program_template.unget();
                }
                break;

            //estado 3: se for lido '.', o automato vai para o estado 9
            case 3:
                if (numbers.find(word.back())!=std::string::npos) current_state = 3;

                else if (word.find('.')!=std::string::npos) current_state = 9;

                else{
                    current_state = 0;
                    word.pop_back();
                    aux.line = line;
                    aux.name = word;
                    aux.token = "integer";
                    result.push_back(aux);
                    program_template.unget();
                }
                break;
            
            //estado 4: ';', '.', '(', ')', ','
            case 4:
                current_state = 0;
                word.pop_back();
                program_template.unget();
                aux.line = line;
                aux.name = word;
                aux.token = "delimiter";
                result.push_back(aux);
                break;
            
            //estado 5: ele pega o proximo char para caso resulte em '<=', '>=' ou '<>', se nao ele volta um char do arquivo
            case 5:
                current_state = 0;
                program_template.get(c);
                word += c;
                if(in_array(word, relacional_operators2)){
                    aux.line = line;
                    aux.name = word;
                    aux.token = "relational_operator";
                    result.push_back(aux);
                }else{
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.name = word;
                    aux.token = "relational_operator";
                    result.push_back(aux);
                }
                break;

            //estado 6: '+', '-'
            case 6:
                current_state = 0;
                word.pop_back();
                program_template.unget();
                aux.line = line;
                aux.name = word;
                aux.token = "aditive_operator";
                result.push_back(aux);
                break;
            
            //estado 7: '*', '/'
            case 7:
                current_state = 0;
                word.pop_back();
                program_template.unget();
                aux.line = line;
                aux.name = word;
                aux.token = "multiplicative_operator";
                result.push_back(aux);
                break;
            
            //estado 8:  ':', ele pega o proximo char, caso seja um '=' ele considera uma atribuicao, caso nao, ele considera um delimitador
            case 8:
                current_state = 0;
                program_template.get(c);
                word = word + c;
                if(word.find(":=")!=std::string::npos){
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.name = word;
                    aux.token = "attribution";
                    result.push_back(aux);
                }else {
                    word.pop_back();
                    program_template.unget();
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.name = word;
                    aux.token = "delimiter";
                    result.push_back(aux);
                }
                break;

            //estado 9: float

            case 9:
                if(numbers.find(word.back())!=std::string::npos){
                    current_state = 9;
                }else {
                    current_state = 0;
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.name = word;
                    aux.token = "float";
                    result.push_back(aux);
                }
                break;
        }

        //contador de linhas
        if(word.back() == '\n'){
            ++line;
        }
    }

    if(comment[0]!=comment[1]){
        std::cerr << "Erro: comment unclosed in line  " << comment_open_line << std::endl;
    }

    program_template.close();

    return result;
}

void print_lexical_analyser(std::vector<struct lexical> *analysed){
    if(analysed){
        for(auto element : (*analysed)){
            std::cout << element.line << ' ' << element.name << ' ' << element.token << std::endl;
        }
    }
}