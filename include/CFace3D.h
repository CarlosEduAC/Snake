class CFace{

private:

int indVerts[4];

public:
    int numFaces;
    float normal[3];

    CFace(int iv1,int iv2,int iv3,float normalx,float normaly,float normalz){
        indVerts[0] = iv1;
        indVerts[1] = iv2;
        indVerts[2] = iv3;
        normal[0] = normalx;
        normal[1] = normaly;
        normal[2] = normalz;
        numFaces = 3;
    }

    CFace(int iv1,int iv2,int iv3,int iv4, float normalx,float normaly,float normalz){
        indVerts[0] = iv1;
        indVerts[1] = iv2;
        indVerts[2] = iv3;
        indVerts[3] = iv4;
        normal[0] = normalx;
        normal[1] = normaly;
        normal[2] = normalz;
        numFaces = 4;
    }

    inline int* GetVerts(){
        return indVerts;
    }

};

typedef CFace* Face;
