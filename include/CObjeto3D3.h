#include "CGrupo3D.h"

class CObjeto3D{


private:

vector<float> vertexes;
vector<float> normals;
vector<int> vindices;
vector<int> vnormals;


float *vertices;
float *normais;
int *indVertices;
int qtdFaces,qtdVertices,qtdNormais;

GLuint _myVertexes,_myNormals,_myIndVerts;

float x,y,z;

public:
vector<Grupo3D> grupos;


    void desenha(){
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(x,y,z);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _myVertexes);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, _myNormals);
        glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
//printf("comecando desenho\n");
        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndVerts);

        for (int g=0;g<grupos.size();g++){
            if (grupos[g]->ativo!=0){

                int inicial = (grupos[g]->inicial);
                //printf("grupo %d  inicial %d  qtd %d\n",g,inicial,grupos[g]->qtdIndices);
                // Draw the triangles !
                glDrawElements(
                    GL_TRIANGLES,      // mode
                    grupos[g]->qtdIndices,    // count
                    GL_UNSIGNED_INT,   // type
                    (void*) (inicial*sizeof(int))           // element array buffer offset
                );
            }
        }
//printf("desenhado\n");
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(2);

        glPopMatrix();
    }

    void normalizar(){
        vertices = (float*) malloc(sizeof(float)*qtdVertices*3);
        normais = (float*) calloc(sizeof(float),qtdVertices*3);
        indVertices = (int*) malloc(sizeof(int)*vindices.size());
        int *cont = (int*) calloc(sizeof(int),qtdVertices);

        memcpy(vertices,&vertexes[0],sizeof(float)*qtdVertices*3);

        for (int i = 0; i < qtdFaces; i++) {
            int cur_n = vnormals[i];

            for (int j = 0; j < 3; j++) {
                GLushort cur_v = indVertices[3*i+j] = vindices[3*i+j];
                cont[cur_v]++;

                normais[3*cur_v] += normals[3*cur_n];       //x
                normais[3*cur_v+1] += normals[3*cur_n+1];   //y
                normais[3*cur_v+2] += normals[3*cur_n+2];   //z
            }
        }

        for (int i=0;i<qtdVertices;i++){
            if (cont[i]>=0) {
                normais[3*i] = normais[3*i]/cont[i];        //x
                normais[3*i+1] = normais[3*i+1]/cont[i];    //y
                normais[3*i+2] = normais[3*i+2]/cont[i];    //z
            }
        }

        free(cont);

        glGenBuffers(1,&_myVertexes);
        glBindBuffer(GL_ARRAY_BUFFER, _myVertexes);
        glBufferData(GL_ARRAY_BUFFER, qtdVertices*3*sizeof(float), vertices, GL_STATIC_DRAW);

        glGenBuffers(1,&_myNormals);
        glBindBuffer(GL_ARRAY_BUFFER, _myNormals);
        glBufferData(GL_ARRAY_BUFFER, qtdVertices*3*sizeof(float), normais, GL_STATIC_DRAW);

        glGenBuffers(1,&_myIndVerts);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _myIndVerts);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, qtdFaces*3*sizeof(int), indVertices, GL_STATIC_DRAW);
    }

    CObjeto3D(char* filename){
        qtdFaces = qtdVertices = qtdNormais = 0;
        loadFile(filename);
        normalizar();
        x=y=z=0;
    }

    void Move(float dx,float dy,float dz){
        x = dx;
        y = dy;
        z = dz;
    }

    void loadFile(const char* filename ) {
        char c1,c2;
        float v1,v2,v3;
        int iv,in,aux;
        char buffer[200];
        Grupo3D grupo=NULL;
        printf("comecando ler arquivo\n");
        FILE *arq = fopen(filename,"r");

        if (!arq) {printf("Problema ao carregar arquivo: %s\n",filename);return;}
        while (!feof(arq)){
            int line=1;
            fscanf(arq,"%c%c",&c1,&c2);
            if (c1=='v' && c2==' '){
                fscanf(arq,"%f%f%f",&v1,&v2,&v3);
                vertexes.push_back(v1);
                vertexes.push_back(v2);
                vertexes.push_back(v3);
                qtdVertices ++;
            }else if (c1=='v' && c2=='n'){
                fscanf(arq,"%f%f%f",&v1,&v2,&v3);
                normals.push_back(v1);
                normals.push_back(v2);
                normals.push_back(v3);
                qtdNormais ++;
            }else if (c1 =='f'){
                for (int i=0;i<3;i++){
                    fscanf(arq,"%d%c%d%c%d",&iv,&c1,&aux,&c2,&in);
                    iv--;
                    vindices.push_back(iv);
                }
                in--;
                vnormals.push_back(in);
                qtdFaces++;
            }else if(c1 == 'o'){
                fgets(buffer,sizeof(buffer),arq);
                printf("comecar face: %s",buffer);
                if (grupo != NULL){
                    grupo->DefineQtdFaces(vindices.size() - grupo->inicial);
                    grupos.push_back(grupo);
                }
                //printf("vamos pro new %d\n",vindices.size());
                grupo = new CGrupo3D(buffer,vindices.size());
                //printf("new ok\n");

                line = 0;
            }
            if (line) fgets(buffer,sizeof(buffer),arq);
        }
        if (grupo != NULL){
            grupo->DefineQtdFaces(vindices.size() - grupo->inicial);
            grupos.push_back(grupo);
            grupo = NULL;
        }
        fclose(arq);
        printf("grupos: %d\n",grupos.size());
        //system("pause");
        //printf("qtdVertices: %d\n",qtdVertices);
        //printf("qtdFaces: %d\n",qtdFaces);
        //printf("qtdNormais: %d\n",qtdNormais);
    }

    ~CObjeto3D(){
        free(normais);
        free(vertices);
        free(indVertices);
        vertexes.clear();
        normals.clear();
        vnormals.clear();
        vindices.clear();
        grupos.clear();
        glDeleteBuffers(1,&_myIndVerts);
        glDeleteBuffers(1,&_myVertexes);
        glDeleteBuffers(1,&_myNormals);
    }

};


typedef CObjeto3D* Objeto3D;


/*
void normalizar(){
        int *indv;
        int *indn;
        int *nb_seen = (int*) calloc(sizeof(int),vertexes.size());
        normais = (float*) calloc(sizeof(float),vertexes.size()*3);

        printf("Comecar\n");
        for (int i = 0; i < faces.size(); i++) {
            Face face = faces[i];
            indv = face->GetVerts();

            for (int j = 0; j < 3; j++) {
                GLushort cur_v = indv[j];
                nb_seen[cur_v]++;

                normais[3*cur_v] += face->normal[0];
                normais[3*cur_v+1] += face->normal[1];
                normais[3*cur_v+2] += face->normal[2];
            }
        }

        for (int i=0;i<vertexes.size();i++){
            normais[3*i] = normais[3*i]/nb_seen[i];
            normais[3*i+1] = normais[3*i+1]/nb_seen[i];
            normais[3*i+2] = normais[3*i+2]/nb_seen[i];
        }

        free(nb_seen);
    }


void desenha(){
        int *indv;
        int *indn;
        int aux;
        Face face;
        //x+=0.001f;
        //x+=0.0001f;
        //x+=0.0001f;

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(x,y,z);

        for (int i=0;i<faces.size();i++){
            face = faces[i];
            indv = face->GetVerts();
            //if (face->numFaces==3){
                glBegin(GL_TRIANGLES);
                for (int k=0;k<3;k++){
                    glNormal3fv(&normais[indv[k]*3]);
                    glVertex3fv(&vertices[indv[k]*3]);
                }
            /*}else{
                glBegin(GL_QUADS);
                for (int k=0;k<4;k++){
                    //glNormal3fv(&soma[indv[k]*3]);
                    glVertex3fv(&vertices[indv[k]*3]);
                }
            }
            glEnd();
        }
        glPopMatrix();
    }

    void load_obj(const char* filename ) {
        char c1,c2;
        float v1,v2,v3;
        int iv[3],in;
        char buffer[200];
        FILE *arq = fopen(filename,"r");
        if (!arq) {printf("Problema ao carregar arquivo: %s\n",filename);return;}
        while (!feof(arq)){
            int line=1;
            fscanf(arq,"%c%c",&c1,&c2);
            if (c1=='v' && c2==' '){
                fscanf(arq,"%f%f%f",&v1,&v2,&v3);
                //printf("Lido vertice: %f %f %f\n",v1,v2,v3);
                vertexes.push_back(v1);
                vertexes.push_back(v2);
                vertexes.push_back(v3);
            }else if (c1=='v' && c2=='n'){
                fscanf(arq,"%f%f%f",&v1,&v2,&v3);
                normals.push_back(v1);
                normals.push_back(v2);
                normals.push_back(v3);
                //printf("Lida normal: %f %f %f\n",v1,v2,v3);
            }else if (c1 =='f'){
                printf("vou ler face\n");
                for (int i=0;i<3;i++){
                    fscanf(arq,"%d%c%c%d",&iv[i],&c1,&c2,&in);
                    //printf("Lida face%d: %d %d\n",i,i1,i2);
                }
                fscanf(arq,"%c",&c1);
                in--;
                Face newFace;
                if (c1==' '){
                    fscanf(arq,"%d%c%c%d",&iv[3],&c1,&c2,&in);
                    newFace = new CFace(iv[0]-1,iv[1]-1,iv[2]-1,iv[3]-1,normals[3*in],normals[3*in+1],normals[3*in+2]);
                }else{
                    newFace = new CFace(iv[0]-1,iv[1]-1,iv[2]-1,normals[3*in],normals[3*in+1],normals[3*in+2]);
                    line=0;
                }

                //printf("Lida face: %d %d %d\n",iv[0],iv[1],iv[2]);
                faces.push_back(newFace);
            }
            if (line) fgets(buffer,sizeof(buffer),arq);
            //system("pause");
        }

        fclose(arq);
    }
*/
