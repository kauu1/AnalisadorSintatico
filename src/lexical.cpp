#include <cstddef>
#include <cstdio>
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
        std::cerr << "Line " << line << " "<< c << " does not belong to the language alphabet\n";
    }

    return 0;
}


std::vector<struct lexical> lexical_analyser_AFD(std::string file_path){

    std::vector<struct lexical> result;

    //abertura do arquivo de leitura
    std::ifstream program_template;
    program_template.open(file_path, std::ifstream::in);

    if(program_template.is_open()){}
    else{
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
    std::vector<std::string> boolean = {"true", "false"};

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
                }
                break;

            //estado 2: assim que se acaba de ler uma palavra, é verificado sé a palavra é uma palavra reservada, operadores lógicos ou indentificador
            case 2:
                if(numbersletters.find(word.back())!=std::string::npos) current_state = 2;

                else{ 
                    current_state = 0;

                    word.pop_back();
                    if(in_array(word, boolean)){
                        aux.line = line;
                        aux.token = word;
                        aux.type = Boolean;
                        result.push_back(aux);

                    }
                    else if(in_array(word, key_words)){
                        aux.line = line;
                        aux.token = word;
                        aux.type = Key_word;
                        result.push_back(aux);


                    }else if (word.find("and")!=std::string::npos) {
                        aux.line = line;
                        aux.token = word;
                        aux.type = Multiplicative_operator;
                        result.push_back(aux);


                    }else if (word.find("or")!=std::string::npos) {
                        aux.line = line;
                        aux.token = word;
                        aux.type = Aditive_operator;
                        result.push_back(aux);


                    }else{
                        aux.line = line;
                        aux.token = word;
                        aux.type = Identifier;
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
                    aux.token = word;
                    aux.type = Integer;
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
                aux.token = word;
                aux.type = Delimiter;
                result.push_back(aux);
                break;
            
            //estado 5: ele pega o proximo char para caso resulte em '<=', '>=' ou '<>', se nao ele volta um char do arquivo
            case 5:
                current_state = 0;
                program_template.get(c);
                word += c;
                if(in_array(word, relacional_operators2)){
                    aux.line = line;
                    aux.token = word;
                    aux.type = Relational_operator;
                    result.push_back(aux);
                }else{
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.token = word;
                    aux.type = Relational_operator;
                    result.push_back(aux);
                }
                break;

            //estado 6: '+', '-'
            case 6:
                current_state = 0;
                word.pop_back();
                program_template.unget();
                aux.line = line;
                aux.token = word;
                aux.type = Aditive_operator;
                result.push_back(aux);
                break;
            
            //estado 7: '*', '/'
            case 7:
                current_state = 0;
                word.pop_back();
                program_template.unget();
                aux.line = line;
                aux.token = word;
                aux.type = Multiplicative_operator;
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
                    aux.token = word;
                    aux.type = Atribution;
                    result.push_back(aux);
                }else {
                    word.pop_back();
                    program_template.unget();
                    word.pop_back();
                    program_template.unget();
                    aux.line = line;
                    aux.token = word;
                    aux.type = Delimiter;
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
                    aux.token = word;
                    aux.type = Float;
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

    program_template.get(c);

    if(c == '.'){
        aux.line = line;
        aux.token = ".";
        aux.type = Delimiter;
        result.push_back(aux);
    }

    return result;
}


typedef struct
{
    const Type type;
    const std::regex pattern;
    
} Pattern;

std::vector<struct lexical> lexical_analyser_ReGex(std::string file_path){

    std::vector<struct lexical> result;

    //abertura do arquivo de leitura
    std::ifstream program_template;
    program_template.open(file_path, std::ifstream::in);

    if(program_template.is_open()){}
    else{
        std::cerr << "Cound not find suitable file" << std::endl;
        return result;
    }

    //conjunto de regras regex indentificadoras
    std::vector<Pattern> rules = {
        {Words, std::regex("[a-zA-Z]\\w*")}, //words
        {Float, std::regex("\\d+[.]\\d*")}, //float
        {Integer, std::regex("\\d+")}, //integers
        {Atribution, std::regex(":=")}, //attribuition
        {Delimiter, std::regex(":(?!=)|[;]|[.]|[,]|[(]|[)]")}, //delimiters
        {Relational_operator, std::regex("(?![<|>])=|[<|>][=]|[<][>]|(?!<)>|[<]")}, //relational operators
        {Aditive_operator, std::regex("[+]|[-]")}, //additive operators
        {Multiplicative_operator, std::regex("[*]|[/]")} //multiplicative operators
    };

    //regras regex e variaveis para o tratamento de comentarios

    std::regex comment_same_line ("\\{.*\\}");

    std::regex comment_other_line ("\\{.*[^\\}]");

    std::regex closed_comment (".*\\}");

    bool comment_open = false;

    int open_comment_line = 0;

    //-----------------------------------------------------

    std::vector<std::string>key_words = {"program", "var", "integer", "real", "boolean", "procedure",
    "begin", "end", "if", "then", "else", "while", "do", "not"};


    //contador de linhas
    int lines = 1;

    std::string str = "\0";

    struct lexical aux;

    //permanece em loop ate o fim do arquivo a ser lido
        //permanece em loop ate o fim do arquivo a ser lido
    while(program_template.peek() != EOF){

    
        std::smatch matches; //variavel para computar os "matches" de uma regra regex

        std::getline(program_template, str); //pega linha por linha do txt de entrada

        //procura um comentario que abre e fecha na mesma linha
        //caso exista ele apaga tudo que esta dentro do comentario
        if (std::regex_search(str, matches, comment_same_line)) {
            if(matches.empty()) continue;

            else
            {
                str = std::regex_replace(str, comment_same_line, "");
                
            }
        
        }

        //checagem de cada caracter da string no alfabeto
        if(!comment_open){
            for(char x : str){
                check_char(x, lines, 0);
            }
        }

        
        
        //caso tenha um comentario aberto em uma linha passada
        //ele checa se na leitura da linha atual possui um fechamento de comentario
        //caso exista, todos os caracteres antes do fechamento sao removidos
        //e a variavel "comment_open" se torna "false"

        //caso nao exista, ele soma uma linha ao contador de linhas e le a linha seguinte
        if(comment_open){
            if(str.find("}") != std::string::npos){
                str = std::regex_replace(str, closed_comment, "");
                comment_open = false;
            }
            else{
                ++lines;
                continue;
            }
        }

        //procura um comentario que foi aberto e nao fechado na mesma linha
        //caso ache, ele remove todos os caracteres da linha apos o comentario aberto
        //e tambem coloca a variavel "comment_open" para "true"
        if(std::regex_search(str, matches, comment_other_line)) {
            open_comment_line = lines; //salva a linha do comentario aberto

            if(matches.empty()){}

            else
            {   
                str = std::regex_replace(str, comment_other_line, "");
                comment_open = true;
                continue;
            }
        }
        //em cada linha, sao checadas todas as regras do vetor "rules" na ordem que foram postas na declaracao do vetor
        //e os "matches" sao colocados no arquivo "output_file.txt" no formato "linha" "indentificado" "tipo"

        std::string token;
        Type type;

        std::string::const_iterator start = str.begin(), end = str.end();
        int match_size, match_position; 

        while (true)
        {
            if(comment_open) break;
            match_size = 0;
            match_position = str.size();

            for (auto pattern : rules)
            {
                // procura por uma match por todos os padrões
                if (std::regex_search(start, end, matches, pattern.pattern))
                {
                    // Se a procura resultou em um match, duas condições são verificadas:

                    // 1: Caso a posição do match corrente seja menor do que a menor posição previamente escolhida, o match corrente torna-se o escolhido
                    // Em outras palavras, o match mais a esquerda é escolhido
                    if (matches.position() < match_position)
                    {
                        match_position = matches.position();
                        match_size = matches.length();
                        type = pattern.type;
                        token = matches[0];
                    }
                    // 2: Caso a posição do match corrente coincida com o match previamente escolhido, torna-se o escolhido aquele que tem mais caracteres
                    // Importante para tratar os casos de <= e <, em que ambos começam no mesmo lugar, porém o <= contém mais caracteres
                    else if (matches.position() == match_position)
                    {
                        if (matches.length() > match_size)
                        {
                            match_size = matches.length();
                            type = pattern.type;
                            token = matches[0];
                        }
                    }
                }
            }

            // Caso a busca não encontre um match, interrompe o laço e vai para a proxima linha (se possivel)

            if (match_size == 0)
                break;
            

            if(type == Words){
                if(in_array(token, key_words)){
                    type = Key_word;
                }else{
                    type = Identifier;
                }
            }

            aux.line = lines;
            aux.token = token;
            aux.type = type;

            result.push_back(aux);

            // Atualiza-se o inicio para logo após ao match encontrado
            start += match_position + match_size;
        }

        ++lines;
    }

    if(comment_open){
        std::cerr << "Erro: comment open in line: " << open_comment_line << "\n";
    }

    program_template.close();

    return result;
}

void print_lexical_analyser(std::vector<struct lexical> *analysed){
    if(analysed){
        for(auto element : (*analysed)){
            std::cout << element.line << ' ' << element.token << ' ' << element.type << std::endl;
        }
    }
}