#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;




class Tabela{
    private:
        ofstream fptr;
        unsigned int ano;
        turma;
        horario;
        materias;
        unsigned int n_alunos;
        professor_responsavel;


    public:
        const static unsigned int turma = 1;
        const static unsigned int ano = 2;
        const static unsigned int horario = 3;
        const static unsigned int materias = 4;
        const static unsigned int n_alunos = 5;
        const static unsigned int professor_responsavel = 6;

        Tabela(string fileName){
            this->fptr.open(fileName, ofstream::in|ofstream::out);
            if(!fptr.is_open()) {
                cout << "Erro no criar arquivo!";
                return;
            }
            
        }
        ~Tabela(){
            (this->fptr).close();
        }

        void end(){
            (this->fptr).close();
        }

        string pegarDados(string colunaNome){
            
        }

        void modificarDados(const unsigned int colunaId, string Dados){
            switch (colunaId)
            {
            case Tabela::turma:
                /* code */
                break;
            case Tabela::turma:
                break;
            case Tabela::turma:
                break;
            case Tabela::turma:
                break;            
            default:
                break;
            }
        }
};

int main(){
    return 0;
}