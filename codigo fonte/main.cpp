//*********************************************************************
//  Computacao Gr?fica usando OpenGL
//  Autor: C.DANIEL && J.LUCAS
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <time.h>         //usada para gerar �ndices aleat�rios para a velocidade do barril
#include <windows.h>      //usada para reproduzir sons de fundo
#include <mmsystem.h>    //usada para reproduzir sons de fundo
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm.h"
#define QUANT_TEX 1
#define quantEscadas 4 //indica a quantidade de escadas do cen�rio
#define BLOCOS 45 //quantidade de blocos da fase(por onde o personagem vai andar)
#include <string>

//Variaveis Globais usadas para definicao de cores
float R, G, B;

bool cima = false, baixo = false, menu = true, gameover = false; //botoes para mover o personagem

float personX = 0.85, personY = 0.0, personZ = 0.83, raioPerson = 0.07; //coordenadas iniciais do personagem

GLMmodel * pmodel = NULL; //carrega um objeto
GLMmodel * pmode2 = NULL; //carrega um objeto
GLMmodel * pmode3 = NULL; //carrega um objeto
bool pulo = false; //vari�vel para controlar o pulo do personagem

int contPulo = 0; //Vari�vel para controlar a translacao do personagem ao pular

int contPrincesa = 0, anguloPrincesa = 180; //vari�veis para controlar a rotacao da princesa
//barril vari?veis

float raioBarril = 0.05; //coordenadas iniciais do barril
float velocidadesBarril[3] = {0.015 , 0.020, 0.025}; //Vetor que � usado para definir a velocidade do barril
typedef struct Retangulo;  //struct para auxiliar na textura das figuras dos menus

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void GerenciaMouse(int button, int state, int x, int y);
void display(void);
void criaBloco(void);
void criaCubo(float x);
void criaCenario(void);
void DefineIluminacao(void);
void criaEscada(float R, float G, float B);
void criaQuadrado(float x);
void criaPersonagens(void);
void menuPrincipal(void);
void menuGameOver(void);
void drawModel(char * fname);
void drawMode2(char * fname);
void drawMode3(char * fname);
void timer_callback(int value);
bool colisao(float x1, float y1, float z1, float raio1, float x2, float y2, float z2, float raio2); //colisao esfera esfera
bool colisaoEP(float x1, float y1 , float z1, float altura, float largura, float profundidade, float x2, float y2, float z2, float raio2); //colisao esfera e paralelepipedo
bool colisaoQP(Retangulo figura, float x_clique, float y_clique); //olha se um ponto(clique) est� dentro de um quadrilatero (colisao quadri�tero e ponto)


struct Escada {
	float x;
	float y;
	float z;
	float altura;
	float largura;
	float profundidade;

};

typedef struct BlocoTeste {
	float x;
	float y;
	float z;
	float raio;
	bool colide;
} tBloco;


typedef struct Barril {
	float x;
	float y;
	float z;
	float raio;
	float velX;
	float velZ;
	float rotacao;
	bool esquerda;
	bool direita;
	bool baixo;
} tBarril;

typedef struct Retangulo {
	float x;
	float y;
	float altura;
	float largura;
};

//Global
tBloco blocks[BLOCOS]; //vetor de blocos do cen�rio
Escada escadas[quantEscadas]; //vetor de escadas do cen�rio
tBloco buracos[3];  //vetor que indica os buracos do cen�rio
tBarril barris[20]; //vetor dos barris do cen�rio
int qte_barris = 0;  //vari�vel que indica a quantidade atual de barris
int intervalo = 0; //vari�vel usada para saber a quantidade de vezes que da fun��o display foi chamada

//variaveis usadas para rotacionar a tela durante o desenvolvimento do trabalho
double rotate_y = 0;
double rotate_x = 0;


// Funcao Principal do C
int main(int argc, char * * argv) {

	glutInit( & argc, argv); // Passagens de parametros C para o glut
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Selecao do Modo do Display e do Sistema de cor utilizado
	glutInitWindowSize (1000, 700);  // Tamanho da janela do OpenGL
	glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL

	glutCreateWindow ("Donkey Kong"); // Da nome para uma janela OpenGL
	glutMouseFunc(GerenciaMouse);
	init(); // Chama a funcao init();
	glEnable(GL_DEPTH_TEST);// Habilitando o teste de profundidade do Z-buffer
	// Registra a fun??o callback que ser? chamada a cada intervalo de tempo
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutDisplayFunc(display); //funcao callback de desenho
	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutSpecialFunc(keyboard_special);	//funcao callback do teclado especial

	glutMainLoop(); // executa o loop do OpenGL
	glDisable(GL_TEXTURE_2D);
	return 0; // retorna 0 para o tipo inteiro da funcao main();
}




// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void) {
	PlaySound(TEXT("../audios/background.wav"), NULL, SND_ASYNC | SND_LOOP);  //Inicia o �udio de fundo do jogo
	srand(time(NULL));//fun��o para o random
	tBarril b; //criando o primeiro barril
	b.x = 0.2;
	b.y = 0.0;
	b.z = -0.65;
	b.raio = 0.05;
	b.velX = 0.015;
	b.velZ = 0.015;
	b.rotacao = velocidadesBarril[rand() % 2];  //seleciona a velocidade entre os valores do vetor ap�s o sorteio
	b.esquerda = true;
	b.direita = false;
	b.baixo = false;
	barris[qte_barris++] = b; //coloca o barril no vetor e incrementa a quantiade de barris
	//
	glEnable(GL_FOG);
	// Habilita a defini??o da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de ilumina??o
	glEnable(GL_LIGHTING);
	// Habilita a luz de n?mero 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);


	/* Activa o modelo de sombreagem de "Gouraud". */
	glShadeModel( GL_SMOOTH );

	/* Activa o z-buffering, de modo a remover as superf?cies escondidas */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glutTimerFunc(30, timer_callback, 30); //define a funcao a ser chamada a cada 30 milisegundos
}

void GerenciaMouse(int button, int state, int x, int y) {
	Retangulo r;
	if (button == GLUT_LEFT_BUTTON)
		if (state == GLUT_DOWN) {
			if(menu) {
				r.x = 500; //centroide (no centro em rela��o a funcao gerencia mouse)
				r.y = 350; //coordenada do centro  (0.0,0.0) � o msm (500,350) neste caso!
				r.largura = 400;
				r.altura = 200;

				if(colisaoQP(r, x, y)) {
					menu = false;
					PlaySound(TEXT("../audios/background.wav"), NULL, SND_ASYNC | SND_LOOP);
				}
			} else if(gameover) {
				r.x = 500; //centroide do tentar novamente
				r.y = 310;
				r.largura = 240;
				r.altura = 90;
				if(colisaoQP(r, x, y)) {
					gameover = false;
					PlaySound(TEXT("../audios/background.wav"), NULL, SND_ASYNC | SND_LOOP);
				} else {
					r.x = 500; //centroide do sair
					r.y = 360;
					r.largura = 80;
					r.altura = 30;
					if(colisaoQP(r, x, y)) {
						exit(0);
					}
				}
			}
		}
	glutPostRedisplay();
}

void reshape(int w, int h) {

	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	glOrtho (0, 512, 0, 512, -1 , 1); // Define o volume de projecao ortografica;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Funcao usada na funcao callback para controlar as teclas comuns (1 Byte) do teclado
void keyboard(unsigned char key, int x, int y) {
	switch (key) { // key - variavel que possui valor ASCII da tecla precionada
	case 27: // codigo ASCII da tecla ESC
		exit(0); // comando pra finalizacao do programa
		break;
	case ' '://tecla espa?o do teclado
		pulo = true;
		break;
	}
}

void timer_callback(int value) {
	glutTimerFunc(value, timer_callback, value);
	//Verifica se o personagem colidiu com um dos barris
	for(int i = 0; i < qte_barris; i++) {
		if(colisao(barris[i].x, barris[i].y, barris[i].z, barris[i].raio, personX, personY, personZ, raioPerson)) {
			printf("Perdeu dd");
			personX = 0.85;//restaura todas coordenadas padr�es
			personY = 0.0;
			personZ = 0.83;
			contPulo = 0;
			cima = false;
			baixo = false;
			pulo = false;
			gameover = true;
			qte_barris = 0;
			PlaySound(TEXT("../audios/perdeu.wav"), NULL, SND_SYNC);
			break;
		}
		if(barris[i].x <= -1) {
			barris[i].esquerda = false;
			barris[i].direita = true;
			barris[i].baixo = false;
		}
		if(barris[i].x >= 1) {
			barris[i].esquerda = true;
			barris[i].direita = false;
			barris[i].baixo = false;
		}
		int flag = 0, number = -1;
		for(int j = 0; j < 45; j ++) { //verifica quando o barril vai descendo
			if(blocks[j].colide && colisao(blocks[j].x, blocks[j].y, blocks[j].z, blocks[j].raio, barris[i].x, barris[i].y, barris[i].z, barris[i].raio) ) {
				flag = 1;
				break;
			}
			if(j == 0 && !blocks[j].colide && colisao(blocks[j].x, blocks[j].y, blocks[j].z, blocks[j].raio, barris[i].x, barris[i].y, barris[i].z, barris[i].raio) ) {
				number = 0;
			}
		}
		if(flag == 0) { //entao ? pq o barril colidiu c o bloco proximo a escada
			barris[i].baixo = true;
			if(number == 0) { //restaura os valores de translacao inicial do barril
				barris[i].x = 0.2;
				barris[i].y = 0.0;
				barris[i].z = -0.65;
				barris[i].esquerda = true;
				barris[i].direita = false;
				barris[i].baixo = false;
			}
		}

	}
	if(!pulo) //se nao pulou, verifica se colidiu com um buraco
		for(int i = 0; i < 3; i++) { //buracos
			if(colisaoEP(buracos[i].x, buracos[i].y, buracos[i].z, buracos[i].raio * 2 + 0.25, 0.06, 0.08, personX, personY, personZ, raioPerson))
				personZ += 0.035;
		}
	if(colisao(0.23, 0.02, -1.21, 0.07, personX, personY, personZ, raioPerson)) { //verifica se colidiu com a princesa
		printf("Ganhou dd");
		PlaySound(TEXT("../audios/ganhou.wav"), NULL, SND_ASYNC);
		personX = 0.85;//restaura todas coordenadas padr�es
		personY = 0.0;
		personZ = 0.83;
		contPulo = 0;
		cima = false;
		baixo = false;
		pulo = false;
		menu = true;  //se ganhou, reinicia o menu
		qte_barris = 0;
		menuPrincipal();

	}
	glutPostRedisplay(); // Manda redesenhar o display em cada frame
}



//Funcao para controlar as teclas especiais (2 Byte) do teclado
int flag1 = 0;
void keyboard_special(int key, int x, int y) {

	switch(key) {
		//  Rotacao 5 graus esquerda
	case GLUT_KEY_F1:
		rotate_y += 5;
		glutPostRedisplay();
		break;

		//  Rotacao 5 graus direita
	case GLUT_KEY_F2:
		rotate_y -= 5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_F3: //Rotacao 5 graus cima
		rotate_x += 5;
		glutPostRedisplay();
		break;

	case GLUT_KEY_F4://Rotacao 5 graus baixo
		rotate_x -= 5;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT://seta esquerda
		if(cima || baixo) //evita que o personagem se movimente na horizontal se ele estiver subindo uma escada
			break;
		if(personX > -1)
			personX -= 0.035;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT://seta direita
		if(cima || baixo) //evita que o personagem se movimente na horizontal se ele estiver subindo uma escada
			break;
		if(personX < 1)
			personX += 0.035;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP://seta cima
		flag1 = 0;
		if(!pulo) {  // se subiu e nao pulou, verifica se colidiu com a escada
			for(int i = 0; i < quantEscadas; i++) { //verifica se h? colis?o com a escada e personagem
				if (colisaoEP(escadas[i].x, escadas[i].y, escadas[i].z, escadas[i].altura, escadas[i].largura, escadas[i].profundidade, personX, personY, personZ, raioPerson)) {
					personZ -= 0.035;
					cima = true;
					baixo = false;
					flag1 = 1;
					break;
				}
			}
			if(flag1 == 0) //quando n?o colidir mais com a escada
				cima = false;
		}
		glutPostRedisplay();
		break;


	case GLUT_KEY_DOWN://seta baixo
		for(int i = 1; i < 45; i++) {
			if(blocks[i].colide && colisao(blocks[i].x, blocks[i].y, blocks[i].z, blocks[i].raio, personX, personY, personZ, raioPerson)) {
				cima = false;
				baixo = false;
			}
			if(!blocks[i].colide && (i == 21 || i == 22) && colisaoEP(blocks[i].x, blocks[i].y, blocks[i].z, 0.18 * 2 + 0.09, 0.18, 0.18, personX, personY, personZ, raioPerson)) {

				personZ += 0.035;
				baixo = true;
				cima = false;
				break;
			} else if(!blocks[i].colide && (i == 24) && colisaoEP(blocks[i].x, blocks[i].y, blocks[i].z, 0.18 * 2 + 0.12, 0.05, 0.18, personX, personY, personZ, raioPerson)) {

				personZ += 0.035;
				baixo = true;
				cima = false;
				break;
			} else if(!blocks[i].colide && (i == 43) && colisaoEP(blocks[i].x, blocks[i].y, blocks[i].z, 0.18 * 2 + 0.13, 0.05, 0.18, personX, personY, personZ, raioPerson)) {

				personZ += 0.035;
				baixo = true;
				cima = false;
				break;
			}
		}

		glutPostRedisplay();
		break;
	default:
		glutPostRedisplay();
		break;

	}

}

GLfloat angulo = 0.0f;
// Fun??o callback de redesenho da janela de visualiza??o




// Funcao usada na funcao callback para desenhar na tela
void display(void) {


	/* Apaga o video e o depth buffer, e reinicia a matriz */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Especifica sistema de coordenadas de proje??o
	glMatrixMode(GL_PROJECTION);
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );
	DefineIluminacao();
	gluLookAt(0.0f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 1.0f, 0.f);//visao da camera

	if(menu)
		menuPrincipal();
	else {
		if(gameover) {
			menuGameOver();
		} else {
			criaCenario();
			criaPersonagens();
		}
	}
	intervalo += 1;
	glutSwapBuffers();
}


void criaCubo(float x) {
	int contador = 0;
	// Desenhas as linhas das "bordas" do cubo
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(1.6f);
	glBegin(GL_LINE_LOOP);	// frontal
	glVertex3f(x, x, x);
	glVertex3f(-x, x, x);
	glVertex3f(-x, -x, x);
	glVertex3f(x, -x, x);
	glEnd();
	glBegin(GL_LINE_LOOP);	//  posterior
	glVertex3f(x, x, -x);
	glVertex3f(x, -x, -x);
	glVertex3f(-x, -x, -x);
	glVertex3f(-x, x, -x);
	glEnd();
	glBegin(GL_LINES);	//  laterais
	glVertex3f(-x, x, -x);
	glVertex3f(-x, x, x);
	glVertex3f(-x, -x, -x);
	glVertex3f(-x, -x, x);
	glVertex3f(x, x, -x);
	glVertex3f(x, x, x);
	glVertex3f(x, -x, -x);
	glVertex3f(x, -x, x);
	glEnd();

	// Desenha as faces do cubo preenchidas
	// Face frontal
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);	// Normal da face
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(x, x, x);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-x, x, x);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -x, x);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(x, -x, x);
	// Face posterior
	glNormal3f(0.0, 0.0, -1.0);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x, x, -x);

	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(x, -x, -x);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -x, -x);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-x, x, -x);

	// Face lateral esquerda
	glNormal3f(-1.0, 0.0, 0.0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-x, x, x);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-x, x, -x);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -x, -x);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -x, x);

	// Face lateral direita
	glNormal3f(1.0, 0.0, 0.0);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(x, x, x);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(x, -x, x);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(x, -x, -x);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x, x, -x);
	// Face superior
	glNormal3f(0.0, 1.0, 0.0);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-x, x, -x);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-x, x, x);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(x, x, x);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x, x, -x);
	
	glNormal3f(0.0, -1.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -x, -x);
	glTexCoord2f(1, 0); //atribui coordenada de textura ao objeto
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(x, -x, -x);
	glTexCoord2f(1, 1);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(x, -x, x);
	glTexCoord2f(0, 1);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -x, x);
	glTexCoord2f(0, 0);
	glEnd();
}

void DefineIluminacao (void) {

	GLfloat posLuz[4] = { -0.2, -0.2, -1.0, 1.0 };// Posi??o da fonte de luz
	GLfloat luzAmbiente[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat luzDifusa[4] = {0.7, 0.7, 0.7, 1.0};
	GLfloat luzEspecular[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat Ka[4] = {0.0, 1.0, 0.2, 0.01},
					Kd[4] = {0.0, 1.0, 0.8, 0.01},
							Ks[4] = {0.0, 1.0, 1.0, 0.01},
									Ke[4] = {0.0, 1.0, 0.0, 0.01},
											local_viewer[4] = {1.0, 1.0, 1.0, 1.0},
													two_side[4] = {0.0, 1.0, 0.0, 1.0};
	// Capacidade de brilho do material
	GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
	GLint especMaterial = 60;

	// Define a reflet?ncia do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	// Define a concentra??o do brilho
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os par?metros da luz de n?mero 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posLuz );

//	glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);
	//  glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
	//glMaterialfv(GL_FRONT, GL_EMISSION, Ke);

	//glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_viewer);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, two_side);
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ka);
}



void criaCenario() { //quantidade de blocos do cenario
	float c = 1;
	GLuint texture1, texture2;

	int w, h;
	unsigned char * uc = stbi_load("../texturas/donkey_kong_arcade.jpg", & w, & h, NULL, 0);
	glGenTextures(1, & texture1); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture1); //Ativa a textura atual

	//Cria a textura lateral de cada bloco

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


	int auxiliar = 0;
	tBloco b;
	for (int k = 0; k < 5; k++) { //numero de andares 'k'
		int BLOCKS = 12;
		for(int i = 0; i < BLOCKS; i++) { //numero de blocos em cada andar 'i'
			float tx, ty, tz;
			float x = 0, y = 0, z = 0;
			glPushMatrix();
			if(k == 0 || (k < 4 && i < 11) || (k == 4 && i > 3 && i < 6)) {
				if(k % 2 == 0) {
					tx = c;
					ty = 0.0;
					tz = 1 - 0.5 * k;
				} else {
					tx = - 0.2 + c;
					ty = 0.0;
					tz = 1 - 0.5 * k;
				}

				float raio;

				b.x = tx;
				b.y = ty;
				b.z = tz;
				b.raio = 0.12;
				if(k == 0) {
					if(i == 0)
						b.colide = false;
					else
						b.colide = true;
				} else if(k == 1) {
					if(i == BLOCKS - 2 || i == BLOCKS - 3)
						b.colide = false;
					else
						b.colide = true;
				} else if(k == 2) {
					if(i < 2)
						b.colide = false;
					else
						b.colide = true;
				}

				else if(k == 3) {
					if(i == BLOCKS - 2 || i == BLOCKS - 3)
						b.colide = false;
					else
						b.colide = true;
				}
				blocks[auxiliar] = b;
				auxiliar++;
			} else {
				if(k == 1) {
					b.x = 1.0;
					b.y = 0.0;
					b.z = 0.5;
					b.raio = 0.12;
					b.colide = false;
					buracos[0] = b;
				} else if(k == 2) {
					b.x = -0.98;
					b.y = 0.0;
					b.z = 0.0;
					b.raio = 0.12;
					b.colide = false;
					buracos[1] = b;
				} else if(k == 3) {
					b.x = 1;
					b.y = 0.0;
					b.z = -0.5;
					b.raio = 0.12;
					b.colide = false;
					buracos[2] = b;
				}
			}
			glTranslatef(tx, ty, tz);
			criaCubo(0.09);
			c = c - 0.18;//distancia entre cada bloco
			glPopMatrix();
		}
		c = 1;
	}
	//glDisable(GL_TEXTURE_2D);  //desativa a textura dos blocos
	glDeleteTextures(1, & texture1);
	stbi_image_free(uc);

	criaEscada(0.59, 0.29, 0.0);

	unsigned char * uc3 = stbi_load("../texturas/barril.jpg", & w, & h, NULL, 0);
	glGenTextures(1, & texture2); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture2); //Ativa a textura atual
	//Cria a textura de cada barril

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glEnable(GL_COLOR_MATERIAL);


	//barril
	GLUquadric * quad = gluNewQuadric(); //cria um quadrado
	gluQuadricNormals(quad, GLU_FLAT); //FLAT significa uma normal para cada face do quadrado
	gluQuadricOrientation(quad, GLU_INSIDE); //vetor da normal apontando p dentro
	for(int barril = 0, fator = 0; barril < 4; barril++, fator++) { //barril proximo ao macaco
		glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//cor do objeto verde
		glRotatef(90, 1.0f, 0.0f, 0.0f);//rota??o
		gluQuadricTexture(quad, GLU_TRUE);//textura do quadrado ? ativada
		if(fator < 2)
			glTranslatef(0.75 + (fator - fator * 0.9), -0.64, 0.0); //posicao final do barril
		else
			glTranslatef(0.75 + ((fator - 2) - (fator - 2) * 0.9), -0.74, 0.0); //posicao final do barril

		gluSphere(quad, raioBarril, 30, 30); //gera a esfera baseado no objeto do quadrado
		//gluCylinder(quad,0.1,0.1,0.4,30,30);//(quadrado,raio_1,raio2,altura,slices,stacks)
		//glutCylinder nao tem as faces de cima e de baixo.. se usar ele da erro na figura do canto
		glPopMatrix();

	}
	if(intervalo % 200 == 0 && qte_barris < 6) {
		srand(time(NULL));
		tBarril b;
		b.x = 0.2;
		b.y = 0.0;
		b.z = -0.65;
		b.raio = 0.05;
		b.velX = velocidadesBarril[rand() % 3];
		b.velZ = 0.015;
		b.rotacao = -0.02;
		b.esquerda = true;
		b.direita = false;
		b.baixo = false;
		barris[qte_barris++] = b;
	}

	for(int i = 0; i < qte_barris; i ++) {

		glPushMatrix();//barril em movimento
		glColor3f(0.0f, 0.0f, 0.0f);//cor do objeto verde
		gluQuadricTexture(quad, GLU_TRUE);//textura do quadrado ? ativada
		glTranslatef(barris[i].x, barris[i].y, barris[i].z); //posicao final do barril
		glRotatef(90, barris[i].rotacao, 0.0f, 0.0f);//rotacao
		gluSphere(quad, barris[i].raio, 30, 30);//gera a esfera baseado no objeto do quadrado
		glPopMatrix();
		if(barris[i].esquerda)
			barris[i].x -= barris[i].velX;
		if(barris[i].direita)
			barris[i].x += barris[i].velX;
		if(barris[i].baixo) {
			barris[i].z += barris[i].velZ;
			barris[i].baixo = false;
		}

		barris[i].rotacao = - barris[i].rotacao;  //constante em qualquer situa??o

	}
	glDeleteTextures(1, & texture2);
	stbi_image_free(uc3);
	//glDisable(GL_TEXTURE_2D);//desativa a textura do barril

	//personagem
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(personX, personY, personZ);
	glutSolidSphere(raioPerson, 30, 30);
	glPopMatrix();
	if (pulo) {
		contPulo += 1;
		if (contPulo <= 15) //numero de frames
			personZ -= 0.010;
		else if(contPulo > 15 && contPulo <= 30)
			personZ += 0.010;
		else {
			contPulo = 0;
			pulo = false;
		}
	}

}

void menuPrincipal() {

	GLuint texture1, texture2;
	int w, h;
	unsigned char * uc = stbi_load("../texturas/start.png", & w, & h, NULL, 0);
	glGenTextures(1, & texture1); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture1); //Ativa a textura atual

	//Cria a textura do menu

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glTranslatef( 0, 0, 0);//origem
	glRotatef(180, 0, 0, 0); //rotacao de 180 graus
	glScalef(4.5, 0, 4.5);
	criaQuadrado(0.09);
	glPopMatrix();
	glDeleteTextures(1, & texture1);
	stbi_image_free(uc);


	unsigned char * uc2 = stbi_load("../texturas/dklogo.png", & w, & h, NULL, 0);
	glGenTextures(1, & texture2); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture2); //vincula a textura atual

	//Cria a textura com o nome donkey kong

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glTranslatef(0, 0, -0.9);
	glRotatef(180, 0, 0, 0); //rotacao de 180 graus
	glScalef(4.5, 0, 4.5);
	criaQuadrado(0.09);
	glPopMatrix();
	glDeleteTextures(1, & texture2);
	stbi_image_free(uc2);

}

void menuGameOver() {
	GLuint texture1, texture2;
	int w, h;
	unsigned char * uc = stbi_load("../texturas/gameover.png", & w, & h, NULL, 0);
	glGenTextures(1, & texture1); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture1); //Ativa a textura atual

	//Cria a textura com o nome game over

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glTranslatef( 0, 0, -1);
	glRotatef(180, 0, 0, 0); //rotacao de 180 graus
	glScalef(5.5, 0, 5.5);
	criaQuadrado(0.09);
	glPopMatrix();
	glDeleteTextures(1, & texture1);
	stbi_image_free(uc);

	unsigned char * uc2 = stbi_load("../texturas/telagamerover.png", & w, & h, NULL, 0);
	glGenTextures(1, & texture2); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, texture2); //Ativa a textura atual

	//Cria a textura do menu Game Over

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glTranslatef( 0, 0, 0);//origem
	glRotatef(180, 0, 0, 0); //rotacao de 180 graus
	glScalef(4.5, 0, 4.5);
	criaQuadrado(0.09);
	glPopMatrix();
	glDeleteTextures(1, & texture2);
	stbi_image_free(uc2);

	gameover = true;


}


void criaEscada(float R, float G , float B) { //Cria as escadas do cen?rio
	Escada e;

	//cria 4 escadas
	glPushMatrix();
	glColor3f(R, G, B);
	glTranslatef( -0.9, -0.09, 0.66);
	e.x = -0.9;
	e.y = -0.09;
	e.z = 0.66;
	e.altura = 0.5;
	e.largura = 0.06;
	e.profundidade = 0.08;
	escadas[0] = e;
	glScalef(0.1, 0.1, 0.25);
	glRotatef(180, 0.0f, 1.0f, -1.2f);//rotacao
	drawMode3("../models/Tritix1.obj");
	glPopMatrix();

	glPushMatrix();
	glColor3f(R, G, B);
	glTranslatef( 0.80 , -0.09, 0.16);
	e.x = 0.80;
	e.y = -0.09;
	e.z = 0.16;
	e.altura = 0.48;
	e.largura = 0.06;
	e.profundidade = 0.08;
	escadas[1] = e;
	glScalef(0.1, 0.1, 0.25);
	glRotatef(180, 0.0f, 1.0f, -1.2f);//rotacao
	drawMode3("../models/Tritix1.obj");
	glPopMatrix();

	glPushMatrix();
	glColor3f(R, G, B);
	glTranslatef( -0.80, -0.1, -0.33);
	e.x = -0.80;
	e.y = -0.1;
	e.z = -0.33;
	e.altura = 0.5101;
	e.largura = 0.06;
	e.profundidade = 0.08;
	escadas[2] = e;
	glScalef(0.1, 0.1, 0.26);
	glRotatef(180, 0.0f, 1.0f, -1.2f);//rotacao
	drawMode3("../models/Tritix1.obj");
	glPopMatrix();

	glPushMatrix();
	glColor3f(R, G, B);
	glTranslatef( 0.25, -0.09, -0.855);
	e.x = 0.25;
	e.y = -0.09;
	e.z = -0.855;
	e.altura = 0.5;
	e.largura = 0.06;
	e.profundidade = 0.08;
	escadas[3] = e;
	glScalef(0.1, 0.1, 0.25);
	glRotatef(180, 0.0f, 1.0f, -1.2f);//rotacao
	drawMode3("../models/Tritix1.obj");
	glPopMatrix();

}

void drawModel(char * fname) {

	if (!pmodel) {
		pmodel = glmReadOBJ(fname);
		if (!pmodel)
			exit(0);
		glmUnitize(pmodel);
		glmFacetNormals(pmodel);
		glmVertexNormals(pmodel, 90.0);
		glmDraw(pmodel, GLM_SMOOTH | GLM_FLAT);
	}
	glmDraw(pmodel, GLM_SMOOTH | GLM_FLAT);


}

void drawMode2(char * fname) {
	if (!pmode2) {
		pmode2 = glmReadOBJ(fname);
		if (!pmode2)
			exit(0);
		glmUnitize(pmode2);
		glmFacetNormals(pmode2);
		glmVertexNormals(pmode2, 90.0);
		glmDraw(pmode2, GLM_SMOOTH | GLM_FLAT);
	}
	glmDraw(pmode2, GLM_SMOOTH | GLM_FLAT);
}

void drawMode3(char * fname) {
	if (!pmode3) {
		pmode3 = glmReadOBJ(fname);
		if (!pmode3)
			exit(0);
		glmUnitize(pmode3);
		glmFacetNormals(pmode3);
		glmVertexNormals(pmode3, 90.0);
		glmDraw(pmode3, GLM_SMOOTH | GLM_FLAT);
	}
	glmDraw(pmode3, GLM_SMOOTH | GLM_FLAT);
}

void criaPersonagens() { //personagens(macaco e princesa) e as escadas!

	//cria macaco
	glPushMatrix();
	glColor3f(0.2f, 0.0f, 0.0f);
	glTranslatef( 0.55, 0.0, -0.68);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(180, 0.0f, 1.0f, -0.5f);//rotacao
	drawModel("../models/dk.obj");
	glPopMatrix();

	//cria princesa

	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.0f);
	glTranslatef( 0.23, 0.02, -1.21);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(180, 0.0f, 1.0f, -0.5f);//rotacao
	if(contPrincesa <= 27) {
		glRotatef(anguloPrincesa - 5, 0.0f, 1.0f, 0.0f); //rotacao
	} else if (contPrincesa > 27 && contPrincesa <= 56) {
		glRotatef(anguloPrincesa + 5, 0.0f, 1.0f, 0.0f); //rotacao
	} else
		contPrincesa = 0;
	contPrincesa++;
	drawMode2("../models/leia.obj");
	glPopMatrix();


}

void criaQuadrado(float x) {

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);//cor verde do objeto
	glTexCoord2f(1, 1); //atribui coordenada de textura ao objeto
	glVertex3f(-x, -x, -x);
	glTexCoord2f(0, 1);
	glVertex3f(x, -x, -x);
	glTexCoord2f(0, 0);
	glVertex3f(x, -x, x);
	glTexCoord2f(1, 0);
	glVertex3f(-x, -x, x);
	glEnd();
}
//x1,y1,z1,raio 1 coordenadas do 1 elemento e x2,y2,z2,raio 2 coordenadas do 2 elemento
bool colisao(float x1, float y1, float z1, float raio1, float x2, float y2, float z2, float raio2) {
	float d = sqrt((- x1 + x2)  * (- x1 +  x2) + (-y1 + y2) * (-y1 + y2) +  ( -z1 + z2) * (-z1 + z2));
	if(d <= (raio1 + raio2))
		return true;
	else
		return false;
}

bool colisaoEP(float x1, float y1 , float z1, float altura, float largura, float profundidade, float x2, float y2, float z2, float raio2) {
	double sphereXDistance = fabs(x2 - x1);
	double sphereYDistance = fabs(y2 - y1);//profundidade no caso do jogo
	double sphereZDistance = fabs(z2 - z1);

	if (sphereXDistance > (largura / 2 + raio2)) {
		return false;
	}
	if (sphereYDistance > (profundidade / 2 + raio2)) {
		return false;
	}
	if (sphereZDistance > (altura / 2 + raio2)) {
		return false;
	}

	if (sphereXDistance <= (largura / 2)) {
		return true;
	}
	if (sphereYDistance <= (profundidade / 2)) {
		return true;
	}
	if (sphereZDistance <= (altura / 2)) {
		return true;
	}

	double cornerDistance_sq = ((sphereXDistance - largura / 2) * (sphereXDistance - largura / 2)) +
							   ((sphereYDistance - profundidade / 2) * (sphereYDistance - profundidade / 2) +
								((sphereZDistance - altura / 2) * (sphereZDistance - altura / 2)));

	return (cornerDistance_sq <= (raio2 * raio2));
}
bool colisaoQP(Retangulo figura, float x_clique, float y_clique) { //colisao quadrado e um ponto qualquer(clique)
	return (abs(figura.x - x_clique) * 2 < (figura.largura )) &&
		   (abs(figura.y - y_clique) * 2 < (figura.altura));
}

