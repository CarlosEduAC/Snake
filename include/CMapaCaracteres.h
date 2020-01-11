#include "COffscreenRenderer.h"

class CMapaCaracteres{

private:

    int *alturaLetra;
    int *larguraLetra;

    SDL_Renderer *render;
    SDL_Texture **glyphsT;
    SDL_Surface *surfaceTemp[ULTIMO_CAR-PRIMEIRO_CAR];
    TTF_Font *font;

    void DefineAlfa(SDL_Surface *resp,int nivelAlfa=0){
        Uint32 *pix32;
        Uint8 r,g,b,a;
        if (resp->format->BytesPerPixel==4){

            for (int h=0;h<resp->h;h++){
                for (int w=0;w<resp->w;w++){
                    pix32 = (Uint32*)resp->pixels+(h*resp->w+w);
                    SDL_GetRGBA((Uint32)*pix32,resp->format,&r,&g,&b,&a);
                    if (r==0&&b==0&&g==0){
                        a=0;//nivelAlfa;
                        //printf("%d %d\n",w,h);
                        //g=255*0;
                        //r=255;
                        //b=255;
                    }else a=255;
                    *pix32 = SDL_MapRGBA(resp->format,r,g,b,a);
                    //pix32++;
                }
            }
        }
        SDL_SetColorKey(resp,SDL_TRUE, SDL_MapRGBA(resp->format, 0, 0, 0,255));
    }

public:

    char nome[100];
    int tamFonte;

    CMapaCaracteres(char *nomeFonte,SDL_Renderer* renderer,int tamanhoFonte,int estilo, PIG_Cor corFonte,int outline,PIG_Cor corOutline){
        strcpy(nome,nomeFonte);
        tamFonte = tamanhoFonte;
        render = renderer;

        alturaLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));
        larguraLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));

        glyphsT = (SDL_Texture**) malloc(sizeof(SDL_Texture*)*(ULTIMO_CAR-PRIMEIRO_CAR));

        font = TTF_OpenFont( nome, tamanhoFonte );
        if (font==NULL)
            printf("Erro: arquivo de fonte (%s) nao localizado\n");
        TTF_SetFontStyle(font,estilo);


        int altRend=0,largRend=0;

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            surfaceTemp[j-PRIMEIRO_CAR] = TTF_RenderGlyph_Solid(font,(Uint16)j,corFonte);
            alturaLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->h;
            if (surfaceTemp[j-PRIMEIRO_CAR]->h>altRend)
                altRend = surfaceTemp[j-PRIMEIRO_CAR]->h;
            larguraLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->w;
            largRend+=surfaceTemp[j-PRIMEIRO_CAR]->w;
        }

        SDL_Rect rect;
        rect.x=0;
        rect.y=0;
        rect.h=altRend;

        OffscreenRenderer off = new COffscreenRenderer(altRend,largRend);
        if (CORESIGUAIS(corFonte,BRANCO)){
            off->PintarFundo(PRETO);
        }else off->PintarFundo(BRANCO);
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(off->GetRenderer(),surfaceTemp[j-PRIMEIRO_CAR]);
            SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.w=larguraLetra[j-PRIMEIRO_CAR];
            SDL_RenderCopy(off->GetRenderer(),glyphsT[j-PRIMEIRO_CAR],NULL,&rect);
            SDL_DestroyTexture(glyphsT[j-PRIMEIRO_CAR]);
            rect.x += rect.w;
        }
        //off->EncerrarDesenho();
        off->SalvarImagem(NOME_ARQ_TEMP);

        rect.x=0;
        rect.y=0;
        rect.h=altRend;
        SDL_Rect dest;
        dest.x=dest.y=0;

        TTF_SetFontOutline(font,outline);

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            dest.h=alturaLetra[j-PRIMEIRO_CAR];
            rect.w=dest.w=larguraLetra[j-PRIMEIRO_CAR];
            surfaceTemp[j-PRIMEIRO_CAR] = SDL_CreateRGBSurface(0,larguraLetra[j-PRIMEIRO_CAR],alturaLetra[j-PRIMEIRO_CAR],24,0,0,0,0);
            SDL_BlitSurface(off->GetSurface(),&rect,surfaceTemp[j-PRIMEIRO_CAR],&dest);
            if (outline>0){
                SDL_Surface *out = TTF_RenderGlyph_Solid(font,(Uint16)j,corOutline);
                SDL_BlitSurface(out,NULL,surfaceTemp[j-PRIMEIRO_CAR],&dest);
                SDL_FreeSurface(out);
            }
            if (CORESIGUAIS(corFonte,BRANCO)){
                SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 0,0,0,255));//ROXO
            }else SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 255,255,255,255));//ROXO

            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(renderer,surfaceTemp[j-PRIMEIRO_CAR]);
            //SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.x+=larguraLetra[j-PRIMEIRO_CAR];
        }

        //TTF_CloseFont(font);
        delete off;
        remove(NOME_ARQ_TEMP);
    }

    CMapaCaracteres(char *nomeFonte,SDL_Renderer* renderer,int tamanhoFonte,int estilo, char *fundoFonte,int outline,PIG_Cor corOutline){
        strcpy(nome,nomeFonte);
        tamFonte = tamanhoFonte;
        render = renderer;

        alturaLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));
        larguraLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));

        glyphsT = (SDL_Texture**) malloc(sizeof(SDL_Texture*)*(ULTIMO_CAR-PRIMEIRO_CAR));

        font = TTF_OpenFont( nome, tamanhoFonte );
        TTF_SetFontStyle(font,estilo);

        SDL_Surface *surfaceTemp[ULTIMO_CAR-PRIMEIRO_CAR];
        int altOffScreen=0,largOffScreen=0;

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            surfaceTemp[j-PRIMEIRO_CAR] = TTF_RenderGlyph_Solid(font,(Uint16)j,PRETO);
            alturaLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->h;
            if (surfaceTemp[j-PRIMEIRO_CAR]->h>altOffScreen)
                altOffScreen = surfaceTemp[j-PRIMEIRO_CAR]->h;
            larguraLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->w;
            largOffScreen+=surfaceTemp[j-PRIMEIRO_CAR]->w;
        }

        SDL_Rect rect;
        rect.x=0;
        rect.y=0;
        rect.h=altOffScreen;

        OffscreenRenderer off = new COffscreenRenderer(altOffScreen,largOffScreen);
        off->PintarFundo(BRANCO);
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(off->GetRenderer(),surfaceTemp[j-PRIMEIRO_CAR]);
            SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.w=larguraLetra[j-PRIMEIRO_CAR];
            SDL_RenderCopy(off->GetRenderer(),glyphsT[j-PRIMEIRO_CAR],NULL,&rect);
            SDL_DestroyTexture(glyphsT[j-PRIMEIRO_CAR]);
            rect.x += rect.w;
        }

        rect.w=1;
        rect.h=altOffScreen;
        rect.x=0;
        rect.y=0;
        SDL_FillRect(off->GetSurface(),&rect,SDL_MapRGB(off->GetSurface()->format,0,0,0));

        //off->EncerrarDesenho();
        off->SalvarImagem(NOME_ARQ_TEMP);
        off->PintarFundo(BRANCO);

        SDL_Surface* stencil = IMG_Load(NOME_ARQ_TEMP);
        SDL_SetColorKey(stencil,SDL_TRUE,SDL_MapRGBA(stencil->format, 0,0,0,255));
        SDL_Texture *textStencil = SDL_CreateTextureFromSurface(off->GetRenderer(),stencil);

        SDL_Surface* fundo = IMG_Load(fundoFonte);
        SDL_Texture *textFundo = SDL_CreateTextureFromSurface(off->GetRenderer(),fundo);

        rect.w=fundo->w;
        rect.x=0;
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            rect.w=larguraLetra[j-PRIMEIRO_CAR];
            SDL_RenderCopy(off->GetRenderer(),textFundo,NULL,&rect);
            rect.x+=larguraLetra[j-PRIMEIRO_CAR];
        }
        SDL_RenderCopy(off->GetRenderer(),textStencil,NULL,NULL);
        //off->EncerrarDesenho();
        off->SalvarImagem(NOME_ARQ_TEMP);

        rect.x=0;
        rect.y=0;
        rect.h=altOffScreen;
        SDL_Rect dest;
        dest.x=dest.y=0;

        TTF_SetFontOutline(font,outline);

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            dest.h=alturaLetra[j-PRIMEIRO_CAR];
            rect.w=dest.w=larguraLetra[j-PRIMEIRO_CAR];
            surfaceTemp[j-PRIMEIRO_CAR] = SDL_CreateRGBSurface(0,larguraLetra[j-PRIMEIRO_CAR],alturaLetra[j-PRIMEIRO_CAR],24,0,0,0,0);
            SDL_BlitSurface(off->GetSurface(),&rect,surfaceTemp[j-PRIMEIRO_CAR],&dest);
            if (outline>0){
                SDL_Surface *out = TTF_RenderGlyph_Solid(font,(Uint16)j,corOutline);
                SDL_BlitSurface(out,NULL,surfaceTemp[j-PRIMEIRO_CAR],&dest);
                SDL_FreeSurface(out);
            }
            SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 255,255,255,255));//ROXO
            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(renderer,surfaceTemp[j-PRIMEIRO_CAR]);
            //SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.x+=larguraLetra[j-PRIMEIRO_CAR];
        }

        //TTF_CloseFont(font);
        delete off;

        remove(NOME_ARQ_TEMP);
    }

    ~CMapaCaracteres(){
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            SDL_DestroyTexture(glyphsT[j-PRIMEIRO_CAR]);
        }
        free(glyphsT);
        free(larguraLetra);
        free(alturaLetra);
        TTF_CloseFont(font);
    }

    int GetLarguraPixelsString(char *str){
        int resp=0;
        Uint16 aux;

        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;
            resp += larguraLetra[aux-PRIMEIRO_CAR];
        }

        return resp;
    }

    void EscreveStringCentralizado(char *str,int x,int y){
        int larguraPixels = GetLarguraPixelsString(str);
        EscreveStringEsquerda(str,x-larguraPixels/2,y);
    }

    void EscreveStringEsquerda(char *str,int x,int y){
        SDL_Rect rectDestino;
        rectDestino.x = x;
        rectDestino.y = ALT_TELA-y-tamFonte;

        Uint16 aux;
        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;//UTF16 string, retirando só o byte que interessa

            rectDestino.w = larguraLetra[aux-PRIMEIRO_CAR];
            rectDestino.h = alturaLetra[aux-PRIMEIRO_CAR];
            SDL_RenderCopy(render,glyphsT[aux-PRIMEIRO_CAR],NULL,&rectDestino);

            rectDestino.x += rectDestino.w;
        }
    }

    void EscreveStringDireita(char *str,int x,int y){
        int larguraPixels = GetLarguraPixelsString(str);
        EscreveStringEsquerda(str,x-larguraPixels,y);
    }

    SDL_Surface* GetSurface(char *str,PIG_Cor cor){
        //printf("vai %d\n",font);
        SDL_Surface *resp1 = TTF_RenderText_Blended(font, str, cor);
        //printf("ok");
        return resp1;

        /*Uint16 aux;
        int altura=0,largura=0;
        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;//UTF16 string, retirando só o byte que interessa

            largura += larguraLetra[aux-PRIMEIRO_CAR];
            if (alturaLetra[aux-PRIMEIRO_CAR]>altura)
                altura = alturaLetra[aux-PRIMEIRO_CAR];
        }


        SDL_Surface *resp = SDL_CreateRGBSurface(0,largura,altura,32,0,0,0,0);

        SDL_Rect rectDestino;
        rectDestino.x=0;
        rectDestino.y=0;

        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;//UTF16 string, retirando só o byte que interessa

            rectDestino.w = larguraLetra[aux-PRIMEIRO_CAR];
            rectDestino.h = alturaLetra[aux-PRIMEIRO_CAR];
            SDL_BlitSurface(surfaceTemp[aux-PRIMEIRO_CAR],NULL,resp,&rectDestino);

            rectDestino.x += rectDestino.w;
        }

        DefineAlfa(resp,255);

        IMG_SavePNG(resp,"bbb.png");
        SDL_FreeSurface(resp);
        resp = IMG_Load("bbb.png");
        //SDL_SetColorKey(resp,SDL_TRUE,SDL_MapRGBA(resp->format, 255,255,255,255));//ROXO
        //SDL_SetColorKey(resp, SDL_TRUE, SDL_MapRGBA(resp->format, 0, 0, 0,0));

        //SDL_Surface *resp2 = SDL_CreateRGBSurface(0,largura,altura,32,0,0,0,0);
        //SDL_BlitSurface(resp, NULL, resp2, NULL);
        //SDL_FreeSurface(resp);

        return resp;
        */
    }
};

typedef CMapaCaracteres* MapaCaracteres;
