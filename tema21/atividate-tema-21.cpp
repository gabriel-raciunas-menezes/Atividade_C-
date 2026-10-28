#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#include <list>

#if defined(_WIN32) || defined(_WIN64)
    #define SLASH_CHR '\\'
    #define SLASH "\\"
#else
    #define SLASH "/"
    #define SLASH_CHR '/'
#endif


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
        string getkey(){return __key;}

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
        unsigned int loadFromString(const string &input, bool set = true){
            try{
                cout << "loading!" << input << endl;
                size_t pos;
                int res = stoi(input, &pos);

                if(pos != input.length() || res> UINT_MAX) {
                    if(set) __str_value = input;
                    currType = TYPE_STR;
                    return TYPE_STR;
                }
                if(res <0) {
                    currType = TYPE_STR;
                    return TYPE_INT;
                }

                if(set) unsigned_int_value = (unsigned int)res;
                currType = TYPE_STR;
                return TYPE_UNSIGNED_INT;
            }catch(const exception&){

                return UNDEFINED;
            }
        }

        string serialize(){
            switch(currType){
                    case keyValuePair::TYPE_INT: 
                    case keyValuePair::UNDEFINED : 
                        return "";
                    case keyValuePair::TYPE_UNSIGNED_INT: 
                        return strget();
                    case keyValuePair::TYPE_STR: 
                        return to_string(unsigned_intget());
                }
            throw runtime_error("Unknown type");
        }
};



class Tabela{
    private:
        fstream* __fptr;
        list<keyValuePair> datas;
        bool dataLoaded = false;
        char separator;
        string filePath;

        bool read(){
            if(dataLoaded) return true;
            if(__fptr->is_open()) __fptr->close();

            __fptr->clear();
            __fptr->open(filePath, ios::in);

            if(!__fptr->is_open()) return false;
            cout << "Reading file " << filePath << endl;

            list<keyValuePair> bufferList;
            string buffer;

            while(getline(*__fptr, buffer)){
                try{
                    bufferList.push_back(*parseLine(buffer));
                }catch(const exception& e){
                    cout << "Error: " << e.what() << endl;
                    __fptr->close();
                    return false;
                }
            }

            const bool ferr = __fptr->bad();
            __fptr->close();
            if(ferr) {
                cout << "ERROR: " << bufferList.empty() << " | " << ferr <<endl;
                return false;
            }

            dataLoaded = true;
            if(buffer.empty()) return true;
        
            datas = move(bufferList);
            
            return true;
        }

        keyValuePair* parseLine(string const &input){
            string key;
            string value;
            istringstream inputStream(input);

            if(!getline(inputStream, key, separator) || !getline(inputStream, value)) throw invalid_argument("line parsed don't have key or value");

            keyValuePair* pair = new keyValuePair(key);
            pair->loadFromString(value);

            return pair;
        }

        string getColumnName(const unsigned int &colunaId){
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
            return columnName;
        }

        string dataFormat(const unsigned int &colunaId, const string &data){
            string columnName = getColumnName(colunaId);
            return columnName + separator + data + '\n';
        }

        string dataFormat(const string &columnName, const string &data){
            return columnName + separator + data + '\n';
        }

    public:
        static constexpr unsigned int turma = 1;
        static constexpr unsigned int ano = 2;
        static constexpr unsigned int horario = 3;
        static constexpr unsigned int materias = 4;
        static constexpr unsigned int n_alunos = 5;
        static constexpr unsigned int professor_responsavel = 6;

        Tabela(const string &fileName, const string &currDir, const char separator) :
        separator(separator), filePath(currDir + SLASH_CHR + fileName), __fptr(new fstream())
        {
            __fptr->open(filePath, ios::app|ios::out|ios::in);
            
            if(!__fptr->is_open()) {
                string errorMsg = "failed to open or create file: ";
                delete __fptr;
                __fptr = nullptr;
                throw std::runtime_error(errorMsg + filePath);
            }
            __fptr->close();
        }
        ~Tabela(){
            if(__fptr != nullptr){
                if(__fptr->is_open())__fptr->close();
                delete __fptr;
                __fptr = nullptr;
            }

        }

        void end(){
            __fptr->close();
        }

        void insert(const unsigned int &colunaId, const string &data){
            string columnName = getColumnName(colunaId);

            if(!dataLoaded && !read()) {
                cout << "failed to modify data!" << endl;
                return;
            }

            keyValuePair* pair = new keyValuePair(columnName);
            pair->loadFromString(data);

            datas.push_back(move(*pair));
        }

        void upsert(const unsigned int &colunaId, const string &data){
            if(!this->isset(colunaId)) this->insert(colunaId, data);
            else this->modificarDados(colunaId, data);
            
        }

        bool modificarDados(const unsigned int &colunaId, const string dados){
            if(!dataLoaded && !read()) {
                cout << "failed to modify data!" << endl;
                return false;
            }

            try{
                keyValuePair &targetData = (*this)[colunaId];
                targetData.loadFromString(dados);
            }catch(const exception &e){
                cout << "ERROR while modifying data: "<< e.what();
                return false;
            }
            return true;
        }

        bool isset(const string &columnName){
            try{
                (*this)[columnName];
                return true;
            }catch(const out_of_range &e){
                return false;
            }
        }

        bool isset(const unsigned int &colunaId){
            try{
                (*this)[colunaId];
                return true;
            }catch(const out_of_range &e){
                return false;
            }
        }


        keyValuePair &operator[](const string &columnName){
            if(!dataLoaded && !read()) {
                cout << "Failed to read data" << endl;
                throw out_of_range("No data to load!");
            }

            for(auto &pair: datas){
                string currKey = pair.getkey();
                if(currKey==columnName) return pair;
            }

            throw out_of_range("Column not found!");
        }

        keyValuePair &operator[](const unsigned int &colunaId){
            return (*this)[getColumnName(colunaId)];
        }

        void commit(){
            if(!dataLoaded || datas.empty()) {
                cout << "iNFO: No data to commit..." << endl;
                return;
            }

            if(__fptr->is_open()) __fptr->close();
            __fptr->open(filePath,ios::out | ios::trunc);

            if(!__fptr->is_open()){
                cout << "ERROR: cannot open file -[" << filePath << "]" << endl;
                return;
            }

            for(auto it : datas){
                string key = it.getkey();
                string value = it.serialize();
                
                (*__fptr) << dataFormat(key, value);
                cout << "INFO: Inserted new data! [key:\"" << key << "\" | value:"<< value << "]"<< endl;
            }
            __fptr->close();
        }
};

int main(){
    try{
        Tabela *registro = new Tabela("registroEscolar.txt","C:\\Users\\PC-Jian\\Downloads\\atividade-c++\\Atividade_C-\\tema21",':');

        unsigned int ano;
        unsigned int n_aluno;
        string turma;
        string horario;
        string materia;
        string prof_responsavel;

        cout << "insere seu ano: " <<endl;
        cin >> ano;

        cout << "insere seu quantidade de alunos: " <<endl;
        cin >> n_aluno;

        cout << "insere sua turma: " <<endl;
        cin >> turma;

        cout << "insere horario: " <<endl;
        cin >> horario;

        cout << "insere materia: " <<endl;
        cin >> materia;

        cout << "insere professor(a) responsavel: " <<endl;
        cin >> prof_responsavel;

        registro->upsert(Tabela::ano,to_string(ano));
        registro->upsert(Tabela::n_alunos,to_string(n_aluno));
        registro->upsert(Tabela::turma,turma);
        registro->upsert(Tabela::horario,horario);
        registro->upsert(Tabela::materias,materia);
        registro->upsert(Tabela::professor_responsavel,prof_responsavel);
        registro->commit();
        registro->end();

        cout << "Dados registrados!" <<endl;
    }catch(const exception& e){
        cout<< "ERROR: " << e.what() << endl;
    }


    return 0;
}