#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#include <list>


struct RegistroEscolar{
    unsigned int ano;
    string turma;
    string horario;
    string materias;
    unsigned int n_alunos;
    string professor_responsavel;
};


class keyValuePair{
    private:
        string __key;
        string __str_value;
        unsigned int unsigned_int_value;
        unsigned int currType = UNDEFINED;
    public:
        const unsigned int static UNDEFINED = 0;
        const unsigned int static TYPE_UNSIGNED_INT = 1;
        const unsigned int static TYPE_STR = 2;
        const unsigned int static TYPE_INT = 3;

        keyValuePair(string key, string value) : __key(key), __str_value(value), currType(TYPE_STR){}
        keyValuePair(string key, unsigned int value) : __key(key), unsigned_int_value(value), currType(TYPE_UNSIGNED_INT){}
        keyValuePair(string key) : __key(key){}


        unsigned int unsigned_intget(){return unsigned_int_value;}
        string strget(){return __str_value;}

        bool unsigned_intset(unsigned int data){
            if(currType != TYPE_UNSIGNED_INT) return false;
            unsigned_int_value = data;
            return true;
        }

        bool strset(string data){
            if(currType != TYPE_STR) return false;
            __str_value = data;
            return true;
        }

        unsigned int typeofValue(){return currType;}
        unsigned int typeFromString(string &input, bool insert = true){
            try{
                size_t pos;
                int res = stoi(input, &pos);
                if(pos != input.length() || res> UINT_MAX) {
                    if(insert) __str_value = input;
                    return TYPE_STR;
                }
                if(res <0) return TYPE_INT;
                if(insert) unsigned_int_value = res;
                return TYPE_UNSIGNED_INT;
            }catch(const exception&){
                return UNDEFINED;
            }
        }
};

class Tabela{
    private:
        fstream* __fptr;
        list<keyValuePair> datas;
        char separator;
        string filePath;



        keyValuePair* parseLine(string const &input){
            bool keyParsed = false;
            bool valueParsed = false;
            string key;
            string value;
            istringstream inputStream(input);

            string buffer;
            if(!getline(inputStream, key, separator) || !getline(inputStream, value, separator)) throw invalid_argument("line parsed don't have key or value");

            keyValuePair* pair = new keyValuePair(key);
            pair->typeFromString(value);

            return pair;
        }

        string dataFormat(const unsigned int &colunaId, const string &data){
            string columnName;
            switch (colunaId)
                {
                    case turma:
                        columnName = "turma";
                        break;
                    case ano:
                        columnName = "ano";
                        break;
                    case horario:
                        columnName = "horario";
                        break;
                    case materias:
                        columnName = "materias";
                        break;
                    case n_alunos:
                        columnName = "n_alunos";
                        break;
                    case professor_responsavel:
                        columnName = "professor_responsavel"; 
                        break;       
                    default:
                        throw invalid_argument("parameter colunaId not found! must be one of constant defined in Tabela::const");
                }

            return columnName + separator + data;
        }

    public:
        const static unsigned int turma = 1;
        const static unsigned int ano = 2;
        const static unsigned int horario = 3;
        const static unsigned int materias = 4;
        const static unsigned int n_alunos = 5;
        const static unsigned int professor_responsavel = 6;

        Tabela(const string &fileName, const char separator, const string &currDir) : separator(separator){
            fstream* fptr = new fstream();
            fptr->open(currDir + fileName);
            
            if(!fptr->is_open()) {
                string errorMsg = "failed to open or create file: ";
                fptr->close();
                throw std::runtime_error(errorMsg + currDir + fileName);
            }
            fptr->close();

            __fptr = fptr;
        }
        ~Tabela(){
            __fptr->close();
            __fptr = NULL;
        }

        void end(){
            __fptr->close();
        }

        void insert(const unsigned int &colunaId, const string &data){
            __fptr->open(filePath, ios::in);
            if(!__fptr->is_open()) {
                string errorMsg = "failed to open or create file: ";
                __fptr->close();
                throw std::runtime_error(errorMsg + filePath);
            }

            *__fptr << dataFormat(colunaId,data);
            __fptr->close();
        }

        void modificarDados(const unsigned int &colunaId, string Dados){
            
        }
};

int main(){
    return 0;
}