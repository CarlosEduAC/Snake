class CGrupo3D{

private:

int id;
char nome[100];


public:

int ativo,qtdIndices,inicial;

    CGrupo3D(char *nomeFace,int inicial){
        strcpy(nome,nomeFace);
        ativo = 1;
        this->inicial = inicial;
    }

    void DefineQtdFaces(int valor){
        qtdIndices = valor;
    }

};

typedef CGrupo3D *Grupo3D;
