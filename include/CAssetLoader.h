typedef struct hnode1{
    char str[100];
    int cont;
    SDL_Surface *bitmap;
} HashNodeImagem;

typedef struct hnode2{
    char str[100];
    int cont;
    //Mix_Chunk *chunk;
} HashNodeAudio;

class CAssetLoader{

private:

    static int totalBitmaps;
    static PoolNumeros numBitmaps;
    static HashNodeImagem *slotsImagem[MAX_SLOTS_AL_IMAGEM];

    static int BuscaNomeImagem(char *str){
        for (int i=0;i<MAX_SLOTS_AL_IMAGEM;i++){
            if (slotsImagem[i]){
                if (strcmp(slotsImagem[i]->str,str)==0)
                    return i;
            }
        }
        return -1;
    }


public:

    static void Inicia(){
        numBitmaps = new CPoolNumeros(MAX_SLOTS_AL_IMAGEM);
        totalBitmaps=0;
        for (int i=0;i<MAX_SLOTS_AL_IMAGEM;i++)
            slotsImagem[i]=NULL;

    }

    static void Encerra(){
        for (int i=0;i<MAX_SLOTS_AL_IMAGEM;i++){
            if (slotsImagem[i]){
                SDL_FreeSurface(slotsImagem[i]->bitmap);
                free(slotsImagem[i]);
            }
        }
        delete numBitmaps;


    }

    static SDL_Surface *LoadImage(char *nomeArq){
        int indice = BuscaNomeImagem(nomeArq);
        if (indice==MAX_SLOTS_AL_IMAGEM){
            printf("Asset loader sobrecarregado\n");
        }else if (indice==-1){
            indice = numBitmaps->RetiraLivre();
            slotsImagem[indice] = (HashNodeImagem*)malloc(sizeof(HashNodeImagem));
            slotsImagem[indice]->bitmap = IMG_Load(nomeArq);
            slotsImagem[indice]->cont = 1;
            strcpy(slotsImagem[indice]->str,nomeArq);
            totalBitmaps++;
        }else{
            slotsImagem[indice]->cont++;
        }
        return slotsImagem[indice]->bitmap;
    }

    static void FreeImage(char *nomeArq){
        int indice = BuscaNomeImagem(nomeArq);
        if (indice==-1){
            printf("Nao existe asset carregado: %s\n",nomeArq);
        }else{
            slotsImagem[indice]->cont--;
            if (slotsImagem[indice]->cont==0){
                SDL_FreeSurface(slotsImagem[indice]->bitmap);
                free(slotsImagem[indice]);
                numBitmaps->DevolveUsado(indice);
            }
        }
    }


};

int CAssetLoader::totalBitmaps;
PoolNumeros CAssetLoader::numBitmaps;
HashNodeImagem* CAssetLoader::slotsImagem[MAX_SLOTS_AL_IMAGEM];

