#include <GL/glew.h>
#include <GL/glu.h>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include "stdio.h"
#include "PIG.h"

#define ALT 12       //Quantidade de linhas do terreno
#define LARG 12      //Quantidade de colunas do terreno
#define LADO 1   //Medida de cada pedaço do terreno

//Matriz bimensional que representa o terreno do jogo. Onde possui 0 não existe um bloco, e 1 indica que existe.
int terreno[LARG][ALT] = {0},score = 0,n = 0;
float angHor = 0;
//Corresponde a uma parte da Cobra
typedef struct{
    int x,y,z;
    int ox,oy,oz;
}Bloco;
// Desenha o terreno do jogo
void DesenhaTerreno(){
    float baseY = (ALT/2)*LADO;
    float baseX = (LARG/2)*LADO;
    float baseZ = -5*LADO;
    glColor4f(160,85,0,1);
    glBegin(GL_LINES);
    for (int i=0;i<=ALT;i++){
        glVertex3f(-baseX,baseY-i*LADO,baseZ);
        glVertex3f(baseX,baseY-i*LADO,baseZ);
    }
    for (int i=0;i<=LARG;i++){
        glVertex3f(baseX-i*LADO,baseY,baseZ);
        glVertex3f(baseX-i*LADO,-baseY,baseZ);
    }
    glEnd();
}
//Desenha cobra na tela
void DesenhaObjeto(Bloco cobra,int cor,GLuint textObjeto){
    GLfloat ambiente[] = {0.8,0.8,0.8,1.0};
    GLfloat difusa[] = {0.8,0.8,0.8,1.0};
    GLfloat especular[] = {1.0,1.0,1.0,1.0};
    GLfloat emissor[] = {0.0,0.0,0.2,1.0};
    GLfloat brilho = 50;

    glBindTexture(GL_TEXTURE_2D,textObjeto);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0);

    glPushMatrix();
    glTranslatef(cobra.ox+cobra.x,cobra.oy+cobra.y,cobra.oz+cobra.z);
    switch(cor){
        case 1: glColor4f(0,1,0,0.1);break;
        case 2: glColor4f(1,0,0,0.1);break;
    }

    glMaterialfv(GL_FRONT,GL_AMBIENT,ambiente);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,difusa);
    glMaterialfv(GL_FRONT,GL_SPECULAR,especular);
    glMaterialfv(GL_FRONT,GL_EMISSION,emissor);
    glMaterialf(GL_FRONT,GL_SHININESS,brilho);

    glBegin(GL_QUADS);
        //cima
        glTexCoord2f(1,1);glVertex3f(0,0,LADO);
        glTexCoord2f(1,0);glVertex3f(0,LADO,LADO);
        glTexCoord2f(0,0);glVertex3f(LADO,LADO,LADO);
        glTexCoord2f(0,1);glVertex3f(LADO,0,LADO);
        //baixo
        glTexCoord2f(0,0);glVertex3f(0,0,0);
        glTexCoord2f(1,0);glVertex3f(LADO,0,0);
        glTexCoord2f(1,1);glVertex3f(LADO,LADO,0);
        glTexCoord2f(0,1);glVertex3f(0,LADO,0);
        //direita
        glTexCoord2f(1,0);glVertex3f(LADO,0,0);
        glTexCoord2f(1,1);glVertex3f(LADO,LADO,0);
        glTexCoord2f(0,1);glVertex3f(LADO,LADO,LADO);
        glTexCoord2f(0,0);glVertex3f(LADO,0,LADO);
        //esquerda
        glTexCoord2f(1,0);glVertex3f(0,0,0);
        glTexCoord2f(1,1);glVertex3f(0,LADO,0);
        glTexCoord2f(0,1);glVertex3f(0,LADO,LADO);
        glTexCoord2f(0,0);glVertex3f(0,0,LADO);
        //trás
        glTexCoord2f(0,0);glVertex3f(0,LADO,0);
        glTexCoord2f(1,0);glVertex3f(LADO,LADO,0);
        glTexCoord2f(1,1);glVertex3f(LADO,LADO,LADO);
        glTexCoord2f(0,1);glVertex3f(0,LADO,LADO);
        //frente
        glTexCoord2f(1,1);glVertex3f(0,0,0);
        glTexCoord2f(0,1);glVertex3f(LADO,0,0);
        glTexCoord2f(0,0);glVertex3f(LADO,0,LADO);
        glTexCoord2f(1,0);glVertex3f(0,0,LADO);
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glPopMatrix();
}
//Tenta fazer com o que o valor de X da cobra modifique
int TentaMoverX(Bloco *cobra,int dx){
    int novox = cobra[0].x+dx;
    if (novox<0||novox>=LARG)
        return false;
    for(int i=1; i<n; i++){
        if ((novox == cobra[i].x) && (cobra[0].y == cobra[i].y))
            return false;
    }
    for(int i=n-1; i>0; i--){
        terreno[cobra[i].x][cobra[i].y] = 0;
        cobra[i].x = cobra[i-1].x;
        cobra[i].y = cobra[i-1].y;
        terreno[cobra[i].x][cobra[i].y] = 1;
    }
    terreno[cobra[0].x][cobra[0].y] = 0;
    cobra[0].x += dx;
    terreno[cobra[0].x][cobra[0].y] = 1;
    return true;
}
//Tenta fazer com o que o valor de Y da cobra modifique
int TentaMoverY(Bloco *cobra,int dy){
    int novoy = cobra[0].y+dy;
    if (novoy<0||novoy>=ALT)
        return false;
    for(int i=1; i<n; i++){
        if ((cobra[0].x == cobra[i].x) && (novoy == cobra[i].y))
            return false;
    }
    for(int i=n-1; i>0; i--){
        terreno[cobra[i].x][cobra[i].y] = 0;
        cobra[i].x = cobra[i-1].x;
        cobra[i].y = cobra[i-1].y;
        terreno[cobra[i].x][cobra[i].y] = 1;
    }
    terreno[cobra[0].x][cobra[0].y] = 0;
    cobra[0].y += dy;
    terreno[cobra[0].x][cobra[0].y] = 1;
    return true;
}
//Faz a cobra se movimentar sozinha
int TrataMovimento(Bloco *cobra,int teclaAtual){
    if (teclaAtual==TECLA_ESQUERDA){
        if(!TentaMoverX(cobra,-1))
            return false;
    }else if(teclaAtual==TECLA_DIREITA){
        if(!TentaMoverX(cobra,+1))
            return false;

    }else if(teclaAtual==TECLA_CIMA){
        if(!TentaMoverY(cobra,+1))
            return false;
    }else if(teclaAtual==TECLA_BAIXO){
        if(!TentaMoverY(cobra,-1))
            return false;
    }
    return true;
}
//Coloca a Fruta em uma posição aleatória do terreno
void ModificaPosicaoFruta (int &fx,int &fy){
    int x,y;
    do{
        x = rand()%12;
        y = rand()%12;
    }while(terreno[x][y]>0);
    terreno[x][y]=2;
    fx = x;
    fy = y;
}
//Verifica se a cobra comeu a fruta
int PegouFruta (Bloco cobra,Bloco fruta){
	if((cobra.x == fruta.x) && (cobra.y == fruta.y)){
        score++;
        return true;
	}
	return false;
}
//Cria a cobra e a Fruta (op Controla qual objeto estou inicializando
Bloco CriaObjeto(int op){
    Bloco cobra;
    cobra.ox = (-LARG/2)*LADO;cobra.oy = (-ALT/2)*LADO;cobra.oz = -5*LADO;
    if(op == 0){
        cobra.x = 0;cobra.y = 0;cobra.z = 0;
    }else{
        do{
            cobra.x = rand()%12;
            cobra.y = rand()%12;
        }while(terreno[cobra.x][cobra.y]>0);
        cobra.z = 0;
        terreno[cobra.x][cobra.y]=2;
    }

    return cobra;
}
//Após comer uma fruta, aumenta o tamanho da cobra
void AdicionaBloco (Bloco *cobra, int teclaAtual){
    cobra[n] = CriaObjeto(0);
    if (teclaAtual==TECLA_ESQUERDA){
        cobra[n].x = cobra[n-1].x + 1;
        cobra[n].y = cobra[n-1].y;
    }else if(teclaAtual==TECLA_DIREITA){
        cobra[n].x = cobra[n-1].x - 1;
        cobra[n].y = cobra[n-1].y;
    }else if(teclaAtual==TECLA_CIMA){
        cobra[n].x = cobra[n-1].x;
        cobra[n].y = cobra[n-1].y - 1;
    }else if(teclaAtual==TECLA_BAIXO){
        cobra[n].x = cobra[n-1].x;
        cobra[n].y = cobra[n-1].y + 1;
    }
    terreno[cobra[n].x][cobra[n].y] = 1;
}
//Cria a textura da fruta e cobra
GLuint LoadPNGTextures(char *str ){
    GLuint idText;
    /* Create storage space for the texture */
    SDL_Surface *TextureImage = IMG_Load( str );
    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( TextureImage ){
	    /* Create The Texture */
	    glGenTextures( 1, &idText );
	    /* Typical Texture Generation Using Data From The Bitmap */
	    glBindTexture( GL_TEXTURE_2D, idText );
	    /* Generate The Texture */
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w,
			  TextureImage->h, 0, GL_RGBA,
			  GL_UNSIGNED_BYTE, TextureImage->pixels );
		glGetError();
	    /* Linear Filtering */
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        printf("erro\n");
        printf("IMG_Load: %s\n", IMG_GetError());
    }
    /* Free up any memory we may have used */
    if ( TextureImage )
	    SDL_FreeSurface( TextureImage );
    return idText;
}
//Mostra texto na tela
GLuint LoadTexto(char *str,PIG_Cor cor,GLuint antiga){
    glDeleteTextures(1,&antiga);
    SDL_Surface *TextureImage = jogo->GetSurface(str,cor);
    GLuint textureIdFrase;
    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( TextureImage ){
	    /* Create The Texture */
	    glGenTextures( 1, &textureIdFrase );
	    //printf("criei2 %d\n",textureIdFrase);
	    /* Typical Texture Generation Using Data From The Bitmap */
	    glBindTexture( GL_TEXTURE_2D, textureIdFrase );
	    /* Generate The Texture */
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w,
			  TextureImage->h, 0, GL_BGRA,
			  GL_UNSIGNED_BYTE, TextureImage->pixels );
        //printf("criei3\n");
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage->w, TextureImage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, TextureImage->pixels);
        glGetError();
	    /* Linear Filtering */
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
        //IMG_SavePNG(TextureImage, "texto.png");
    }else{
        printf("erro\n");
        printf("IMG_Load: %s\n", IMG_GetError());
    }
    /* Free up any memory we may have used */
    if ( TextureImage )
	    SDL_FreeSurface( TextureImage );
    return textureIdFrase;
}
//
void PreparaLuz(float amb, float dif){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambiente[] = {amb,amb,amb,1.0};
    GLfloat difusa[] = {dif,dif,dif,1.0};
    GLfloat especular[] = {1.0,0.0,0.0,1.0};

    glLightfv(GL_LIGHT0,GL_AMBIENT,ambiente);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,difusa);
    glLightfv(GL_LIGHT0,GL_SPECULAR,especular);
}
//Mostra qual placa de video do PC e versão do OpenGL
void ShowVersion(){
    const GLubyte* renderer = glGetString(GL_RENDERER); // Placa de Video
    const GLubyte* version = glGetString(GL_VERSION); // Versão do OpenGL
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
}
//Seta o modo de exibição para 2D
void Setup2D(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARG_TELA, ALT_TELA, 0, -1, 1);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//Seta o modo de exibição para 3D
void Setup3D(float angHor){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,1.0,0.1,500.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,5,-10);
    glRotatef(-45,1,0,0);
    glRotatef(angHor,0,0,1);
}
int main( int argc, char* args[] ){
    PIG_Evento evento;
    PIG_Teclado meuTeclado = GetTeclado();;

    CriaJogo("Snake");
    glewInit();
    //ShowVersion();
    srand(time(NULL));
    Bloco *cobra = (Bloco *) malloc((LARG*ALT)*sizeof(Bloco));
    Bloco fruta = CriaObjeto(1);
    int teclaAtual, timerMovimento = CriaTimer();
    float ambiente = 0.1, difusa = 0.3;
    enum numTelas{menu,jogo,perdeu,sair};

    terreno[0][0] = 1;
    cobra[0] = CriaObjeto(0);n++;
    glEnable(GL_TEXTURE_2D);
    GLuint idFrase=0;
    GLuint textFruta = LoadPNGTextures(".//imagens//fruta.png");
    GLuint textHead = LoadPNGTextures(".//imagens//head.png");
    GLuint textTail = LoadPNGTextures(".//imagens//tail.png");

    while (JogoRodando()!=0){
        evento = GetEvento();
        if(GetEstadoJogo()==menu){
            if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA){
                if (evento.teclado.tecla==TECLA_ENTER){
                    SetEstadoJogo(jogo);
                }
                if (evento.teclado.tecla==TECLA_ESC){
                    SetEstadoJogo(sair);
                }
            }
        }
        if(GetEstadoJogo()==jogo){
            if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA){
                teclaAtual = evento.teclado.tecla;
                if(teclaAtual==TECLA_BARRAESPACO){
                    SetEstadoJogo(menu);
                }else if(evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_KP_9){
                    ambiente +=0.1;
                    if (ambiente>1) ambiente = 1;
                }else if(evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_KP_3){
                    ambiente -= 0.1;
                    if (ambiente<0) ambiente = 0;
                }else if(evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_KP_7){
                    difusa +=0.1;
                    if (difusa>1) difusa = 1;
                }else if(evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_KP_1){
                    difusa -=0.1;
                    if (difusa<0) difusa = 0;
                }else if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_a){
                    angHor -= 90;
                }else if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA&&evento.teclado.tecla==TECLA_d){
                    angHor += 90;
                }else{
                    if(!TrataMovimento(cobra,teclaAtual)){
                        terreno[LARG][ALT] = {0};
                        score = 0;n=1;terreno[0][0] = 1;
                        cobra[0] = CriaObjeto(0);
                        angHor = 0;
                        SetEstadoJogo(perdeu);
                    }
                }
            }
        }
        if(GetEstadoJogo()==perdeu){
            if (evento.tipoEvento==EVENTO_TECLADO&&evento.teclado.acao==TECLA_PRESSIONADA){
                if (evento.teclado.tecla==TECLA_ENTER){
                    SetEstadoJogo(menu);
                }
                if (evento.teclado.tecla==TECLA_ESC){
                    SetEstadoJogo(sair);
                }
            }
        }

        IniciaDesenho();

        if(GetEstadoJogo()==menu){
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER,0);
            Setup2D();
            char str[20];
	        sprintf(str,"%s","Snake");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-40,50);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+60,50);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+60,140);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-40,140);
        	glEnd();
	        sprintf(str,"%s","Pressione Enter Para Jogar");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-80);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-80);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-60);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-60);
        	glEnd();
        	sprintf(str,"%s","Pressione Esc Para Sair");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-40);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-40);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-20);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-20);
        	glEnd();
        	glDisable(GL_ALPHA_TEST);
        }
        if(GetEstadoJogo()==jogo){
            //PreparaLuz(ambiente,difusa);
            Setup3D(angHor);
            if(PegouFruta(cobra[0],fruta)){
                ModificaPosicaoFruta(fruta.x,fruta.y);
                AdicionaBloco(cobra,teclaAtual);n++;
            }else{
                if(TempoDecorrido(timerMovimento)>0.8){
                    if(!TrataMovimento(cobra,teclaAtual)){
                        terreno[LARG][ALT] = {0};
                        score = 0;n=1;terreno[0][0] = 1;
                        cobra[0] = CriaObjeto(0);
                        angHor = 0;
                        SetEstadoJogo(perdeu);
                    }
                    ReiniciaTimer(timerMovimento);
                }
            }
            DesenhaTerreno();
            for(int i=0; i<n; i++){
                if(i==0)
                    DesenhaObjeto(cobra[i],1,textHead);
                else
                    DesenhaObjeto(cobra[i],1,textTail);
            }
            DesenhaObjeto(fruta,2,textFruta);
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER,0);

            Setup2D();
            char str[20];
	        sprintf(str,"Score %d",score);
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,30);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,30);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,120);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,120);
        	glEnd();
        	glDisable(GL_ALPHA_TEST);
        }
        if(GetEstadoJogo()==perdeu){
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER,0);
            Setup2D();
            char str[20];
	        sprintf(str,"%s","Você Perdeu !!!");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,50);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,50);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,140);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,140);
        	glEnd();
	        sprintf(str,"%s","Pressione Enter Para o Menu");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-80);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-80);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-60);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-60);
        	glEnd();
        	sprintf(str,"%s","Pressione Esc Para Sair");
	        idFrase = LoadTexto(str,BRANCO,idFrase);
	        glBindTexture(GL_TEXTURE_2D,idFrase);
	        glBegin(GL_QUADS);
	            glTexCoord2f(0.0,0.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-40);
	            glTexCoord2f(1.0,0.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-40);
	            glTexCoord2f(1.0,1.0);glVertex2i((LARG_TELA/2)+100,(ALT_TELA/2)-20);
	            glTexCoord2f(0.0,1.0);glVertex2i((LARG_TELA/2)-80,(ALT_TELA/2)-20);
        	glEnd();
        	glDisable(GL_ALPHA_TEST);
        }
        if(GetEstadoJogo()==sair){
            goto fim;
        }

        EncerraDesenho();
    }
    fim: system("cls");
    FinalizaJogo();
    return 0;
}
