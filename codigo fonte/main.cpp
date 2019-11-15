//*********************************************************************
//  Computacao Gr�fica usando OpenGL
//  Autor: C.DANIEL && J.LUCAS
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm.h"
#define QUANT_TEX 1
#define BLOCOS 100 //quantidade de blocos da fase(por onde o personagem vai andar)
#include <string>
//Variaveis Globais usadas para definicao de cores
float R, G, B;
bool esquerda = false, direita = false, cima = false, baixo = false; //botoes para mover o personagem
float personX = 0.75, personY = -0.75; //coordenadas iniciais do personagem
float personComp = 50, personAlt = 30; //comprimento e altura do personagem
std::string MODEL_PATH;
GLuint elephant;
float elephantrot;
char ch = '1';
GLMmodel* pmodel = NULL;
GLMmodel* pmode2 = NULL;


//A-personagem
//B-algo que vai colidir com o personagem
// bool colisao(float aX, float aY, float aComp, float aAlt, float bX, float bY, float bComp, float bAlt)
// {
// 	if(aY + aAlt < bY)
// 		return false;
// 	else if(aY > bY + bAlt)
// 		return false;
// 	else if(aX + aComp < bX)
// 		return false;
// 	else if(aX > bX + bComp)
// 		return false;
// 	return true;
// }

bool colisao(float x, float y, float z, float raio){
	float d = sqrt((- x + personX)  * (- x +  personX) + ( - y + personY) * (- y + personY) + (-z + 1) * (-z + 1));
	printf("d = %.2f ", d);
	printf("s = %.2f\n", 0.25 + raio);
	
	if(d <= (0.3 + raio))
		return true;
	else 
		return false;
}

struct Bloco //Blocos do cen�rio
{
	float x;
	float y;
	float comp;
	float alt;
	bool colide;
};

typedef struct BlocoTeste
{
	float x;
	float y;
	float z;
	float raio;
	bool colide;
} tBloco;

struct Vertex
{
	float pos[3];
	float color[3];
	float texCoord[3];
};

//Global
Bloco blocos[BLOCOS];
tBloco blocks[BLOCOS];
double rotate_y = 0;
double rotate_x = 0;


// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void display(void);
void criaBloco(void);
void criaCubo(float x);
void criaCenario(void);
void DefineIluminacao(void);
void criaEscada(float x);
void criaPersonagens(void);
void drawModel(char *fname);
void loadObj(char *fname);

// Funcao Principal do C
int main(int argc, char** argv)
{
	glutInit(&argc, argv); // Passagens de parametros C para o glut
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Selecao do Modo do Display e do Sistema de cor utilizado
	glutInitWindowSize (1000, 700);  // Tamanho da janela do OpenGL
	glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
	glutCreateWindow ("Donkey Kong"); // Da nome para uma janela OpenGL

	init(); // Chama a funcao init();
	glEnable(GL_DEPTH_TEST);// Habilitando o teste de profundidade do Z-buffer
	// Registra a fun��o callback que ser� chamada a cada intervalo de tempo
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutDisplayFunc(display); //funcao callback de desenho
	glTranslatef(1, 1, 1);
	loadObj("../models/dk.obj");//replace porsche.obj with radar.obj or any other .obj to display it

	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutSpecialFunc(keyboard_special);	//funcao callback do teclado especial
	glutMainLoop(); // executa o loop do OpenGL
	return 0; // retorna 0 para o tipo inteiro da funcao main();
}


// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void)
{

	// Habilita a defini��o da cor do material a partir da cor corrente
	//

	glEnable(GL_FOG);
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de ilumina��o
	glEnable(GL_LIGHTING);
	// Habilita a luz de n�mero 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);


	/* Activa o modelo de sombreagem de "Gouraud". */
	glShadeModel( GL_SMOOTH );

	/* Activa o z-buffering, de modo a remover as superf�cies escondidas */
	glEnable(GL_DEPTH_TEST);

}

void reshape(int w, int h)
{
	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	glOrtho (0, 512, 0, 512, -1 , 1); // Define o volume de projecao ortografica;
	// Nesse caso o ponto (0,0,0) se encontra no
	// canto inferior esquerdo da tela e o viewport
	// tem 0 a 512 pixel no eixo x, 0 a 512 pixel no
	// eixo y. Como sera trabalhado imagens 2D definimos
	// o eixo z com tamanho -1 a 1;

//   glOrtho (-256, 256, -256, 256, -1 ,1);  // Neste caso, continuamos com 512 pixels, porem o
	// centro dos eixos passa a se localizar no centro
	// da tela.

//   glOrtho (-(w/2), (w/2),-(h/2),(h/2), -1 ,1); // Nesse caso, ficamos com a quantidade de pixels variavel,
	// dependente da largura e altura da janela, e o
	// centro dos eixos continua a se localizar no centro
	// da tela.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Funcao usada na funcao callback para controlar as teclas comuns (1 Byte) do teclado
void keyboard(unsigned char key, int x, int y)
{
	switch (key)   // key - variavel que possui valor ASCII da tecla precionada
	{
	case 27: // codigo ASCII da tecla ESC
		exit(0); // comando pra finalizacao do programa
		break;
	}
}

//Funcao para controlar as teclas especiais (2 Byte) do teclado
int flag = 0;
void keyboard_special(int key, int x, int y)
{
	switch(key)
	{
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
		esquerda = true;
		if(personX > -1)
			personX -= 0.035;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT://seta direita
		direita = true;
		//if(personX < 1)
			personX += 0.035;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP://seta cima
		cima = true;
		flag = 0;
		// if(personY < 1)
		// 	personY += 0.035;
		for(int i = 12; i < 22; i ++)
				if(personY < 1 && blocks[i].colide && colisao(blocks[i].x, blocks[i].y, blocks[i].z, blocks[i].raio) ){
					flag = 1;
					printf("figura %d ", i);
				}	
		if(flag == 0)
			personY += 0.035;
		else
		{
			printf("colidiu\n");
		}
		flag = 0;
		glutPostRedisplay();
		flag = 0;
		break;
	case GLUT_KEY_DOWN://seta baixo
		baixo = true;
		flag = 0;
		for(int i = 12; i < 22; i ++)
				if(personY < 1 && blocks[i].colide && colisao(blocks[i].x, blocks[i].y, blocks[i].z, blocks[i].raio) ){
					flag = 1;
					printf("figura %d ", i);
				}	
		if(flag == 0)
			personY -= 0.035;
		else
		{
			printf("colidiu\n");
		}
		flag = 0;
		glutPostRedisplay();
		flag = 0;
		break;
	default:
		break;

	}

}

GLfloat angulo = 0.0f;
// Fun��o callback de redesenho da janela de visualiza��o




// Funcao usada na funcao callback para desenhar na tela
void display(void)
{

	// muda para o modo GL_MODELVIEW (nao pretendemos alterar a projecao
	// quando estivermos desenhando na tela)
	//glMatrixMode(GL_MODELVIEW);
	// Especifica sistema de coordenadas de proje��o
	glMatrixMode(GL_PROJECTION);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpar a tela e o Z-buffer
	//glColor3ub(0, 0, 0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glLoadIdentity();//Limpa o Buffer de Cores;
	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );
	DefineIluminacao();
	gluLookAt(0.0f, 0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 1.0f, 0.f);//visao da camera

	criaCenario();
	criaPersonagens();



	glutSwapBuffers();
	glEnable(GL_LIGHTING);
}


void criaCubo(float x, float* coord_x, float* coord_y, float* coord_z, float tx, float ty, float tz, float * raio)
{
	int contador = 0;
	// Desenhas as linhas das "bordas" do cubo
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(1.6f);
	glBegin(GL_LINE_LOOP);	// frontal
	glVertex3f(x, x, x);
	*raio = fabs(tx);
	*coord_x += x + tx;
	*coord_y += x + ty;
	*coord_z += x + tz;
	contador += 1;
	glVertex3f(-x, x, x);
	*coord_x +=  - x + tx;
	*coord_y += x + ty;
	*coord_z += x + tz; contador += 1;
	glVertex3f(-x, -x, x);
	*coord_x +=  - x + tx;
	*coord_y += - x + ty;
	*coord_z += x + tz; contador += 1;
	glVertex3f(x, -x, x);
	*coord_x +=  x + tx;
	*coord_y += - x + ty;
	*coord_z += x + tz; contador += 1;
	glEnd();
	glBegin(GL_LINE_LOOP);	//  posterior
	glVertex3f(x, x, -x);
	*coord_x +=  x + tx;
	*coord_y +=  x + ty;
	*coord_z += -x + tz;
	contador += 1;
	glVertex3f(x, -x, -x);
	*coord_x +=  x + tx;
	*coord_y += - x + ty;
	*coord_z += - x + tz;
	contador += 1;
	glVertex3f(-x, -x, -x);
	*coord_x +=  - x + tx;
	*coord_y += - x + ty;
	*coord_z +=  - x + tz;
	contador += 1;
	glVertex3f(-x, x, -x);
	*coord_x +=  - x + tx;
	*coord_y += x + ty;
	*coord_z += - x + tz;
	contador += 1;
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
	*coord_x /= contador;
	*coord_y /= contador;
	*coord_z /= contador;

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

	// Face inferior
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

void DefineIluminacao (void)
{

	GLfloat posLuz[4] = { -0.2, -0.2, -1.0, 1.0 };// Posi��o da fonte de luz
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

	// Define a reflet�ncia do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	// Define a concentra��o do brilho
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os par�metros da luz de n�mero 0
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



void criaCenario() //quantidade de blocos do cenario
{
	float c = 1;
	unsigned int t;
	glGenTextures(1, &t); //gera nomes identificadores de texturas
	int w, h;
	unsigned char *uc = stbi_load("../texturas/donkey_kong_arcade.jpg", &w, &h, NULL, 0);
	glBindTexture(GL_TEXTURE_2D, t); //Ativa a textura atual

	//Cria a textura lateral de cada bloco

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);
	int auxiliar = 0;
	for (int k = 0; k < 5; k++) //numero de andares 'k'
	{
		int BLOCKS = 11;
		if(k == 0) BLOCKS = 12;
		for(int i = 0; i < BLOCKS; i++, auxiliar++)//numero de blocos em cada andar 'i'
		{
			float tx, ty, tz;
			float x = 0, y = 0, z = 0;
			glPushMatrix();
			if(k<4 || (k==4 && i>3 && i<6))
			if(k % 2 == 0)
			{
				tx = c;
				ty = 0.0;
				tz = 1 - 0.5 * k;
			}
			else
			{
				tx = - 0.2 + c;
				ty = 0.0;
				tz = 1 - 0.5 * k;
			}
			glTranslatef(tx, ty, tz);
			float raio;
			criaCubo(0.09, &x, &y, &z, tx, ty, tz, &raio);
			tBloco b;
			b.x = x;
			b.y = y;
			b.z = z;
			b.raio = raio;
			if(i == BLOCKS - 1 || i == BLOCKS - 2|| i == BLOCKS - 3) //|| i == BLOCKS - 4 || i == BLOCKS - 5)
				b.colide = false;
			else
				b.colide = true;
			// printf("figura %d. x = %.2f, y = %.2f, z = %.2f\n", auxiliar, x, y, z);
			blocks[auxiliar] = b;
			c = c - 0.18;//distancia entre cada bloco
			glPopMatrix();
		}
		c = 1;
	}
	glDisable(GL_TEXTURE_2D);  //desativa a textura dos blocos
	stbi_image_free(uc);
	unsigned char *uc2 = stbi_load("../texturas/escada.png", &w, &h, NULL, 0);
	glGenTextures(1, &t); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, t); //Ativa a textura atual
	//Cria a textura de cada escada

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);//inicia a nova textura

	//fazendo as escadas de cada andar
	glPushMatrix();//escada 1
	glTranslatef( -0.9, -0.09, 0.75);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();//escada 2
	glTranslatef( 0.8 , -0.09, 0.25);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();//escada 3
	glTranslatef( -0.80, -0.09, -0.25);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();//escada 4
	glTranslatef( -0.80, -0.09, -0.25);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();//escada 5
	glTranslatef( 0.25, -0.09, -0.745);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);//desativa a textura da escada
	stbi_image_free(uc2);

	unsigned char *uc3 = stbi_load("../texturas/barril.jpg", &w, &h, NULL, 0);
	glGenTextures(1, &t); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, t); //Ativa a textura atual
	//Cria a textura de cada barril

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);//inicia a textura do barril


	//barril
	GLUquadric *quad = gluNewQuadric(); //cria um quadrado
	gluQuadricNormals(quad, GLU_FLAT); //FLAT significa uma normal para cada face do quadrado
	gluQuadricOrientation(quad, GLU_INSIDE); //vetor da normal apontando p dentro
	for(int barril = 0, fator = 0; barril < 4; barril++, fator++)
	{
		glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//cor do objeto verde
		glRotatef(90, 1.0f, 0.0f, 0.0f);//rota��o
		gluQuadricTexture(quad, GLU_TRUE);//textura do quadrado � ativada
		if(fator < 2)
			glTranslatef(0.75 + (fator - fator * 0.9), -0.64, 0.0); //posicao final do barril
		else
			glTranslatef(0.75 + ((fator - 2) - (fator - 2) * 0.9), -0.74, 0.0); //posicao final do barril

		gluSphere(quad, 0.05, 30, 30);//gera a esfera baseado no objeto do quadrado
		//gluCylinder(quad,0.1,0.1,0.4,30,30);//(quadrado,raio_1,raio2,altura,slices,stacks)
		//glutCylinder nao tem as faces de cima e de baixo.. se usar ele da erro na figura do canto
		glPopMatrix();

	}

	glDisable(GL_TEXTURE_2D);//desativa a textura do barril
	stbi_image_free(uc3);

	//personagem
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(personX, personY, 0.0);
	glutSolidSphere(0.1, 30, 30);
	glPopMatrix();


}

void criaEscada(float x) //Cria um quadril�tero serve p escada e para alguns personagens
{

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0.0, -1.0, 0.0);
//	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -x, -x);
	glTexCoord2f(1, 0); //atribui coordenada de textura ao objeto
//	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(x, -x, -x);
	glTexCoord2f(1, 1);
//	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(x, -x, x);
	glTexCoord2f(0, 1);
//	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -x, x);
	glTexCoord2f(0, 0);
	glEnd();

}



void criaBloco()//bloco do projeto 2D
{

	int x = 15, y = 10; //coordenadas iniciais dos blocos
	for(int n = 0; n < BLOCOS; n++)
	{
		if(x > 512 - 30) //quando chegar no canto direito
		{
			x = 15;
			y += 63;
		}
		blocos[n].x = x;
		blocos[n].y = y;
		blocos[n].comp = 20;
		blocos[n].alt = 10;
		blocos[n].colide = true;
		x += blocos[n].comp;
	}
	for(int n = 0; n < BLOCOS; n++)
	{
		glLineWidth(5);//espessura das linhas
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex2f(blocos[n].x, blocos[n].y); //V�rtice inferior esquerdo
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y); //V�rtice inferior direito
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y + blocos[n].alt); //V�rtice superior direito
		glVertex2f(blocos[n].x, blocos[n].y + blocos[n].alt); //V�rtice superior esquerdo
		glEnd();
		glLineWidth(4);//espessura das linhas
		glBegin(GL_LINES);//linhas cruzadas
		glVertex2f(blocos[n].x, blocos[n].y); //V�rtice inferior esquerdo
		glVertex2f((int)((blocos[n].x + blocos[n].comp + blocos[n].x) / 2), (int)((blocos[n].y + blocos[n].alt + blocos[n].y + blocos[n].alt) / 2)); //V�rtice superior (fica no meio)
		glVertex2f((int)((blocos[n].x + blocos[n].comp + blocos[n].x) / 2), (int)((blocos[n].y + blocos[n].alt + blocos[n].y + blocos[n].alt) / 2)); //V�rtice superior (fica no meio)
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y); //V�rtice inferior direito
		glEnd();
	}

}





void loadObj(char *fname)
{

	FILE *fp;
	int read;
	GLfloat x, y, z;
	char ch;
	elephant = glGenLists(1);
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file %s\n", fname);
		exit(1);
	}
	glPointSize(2.0);
	glNewList(elephant, GL_COMPILE);
	{
		glPushMatrix();
//	   	glTranslatef(0.0, -18, 0.0);
		glBegin(GL_POINTS);
		while(!(feof(fp)))
		{
			read = fscanf(fp, "%c %f %f %f", &ch, &x, &y, &z);
			if(read == 4 && ch == 'v')
			{
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();
	fclose(fp);
}
void drawModel(char *fname)
{

	if (!pmodel)
	{
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

void drawMode2(char *fname)
{
	if (!pmode2)
	{
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

void criaPersonagens()
{

	//cria macaco
	glPushMatrix();
	glColor3f(0.2f, 0.0f, 0.0f);
	glTranslatef( 0.55, 0.0, -0.68);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(180, 0.0f, 1.0f, -0.5f);//rota��o
	drawModel("../models/dk.obj");
	glPopMatrix();

	//cria princesa
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.0f);
	glTranslatef( 0.23, 0.0, -1.21);
	glScalef(0.1, 0.1, 0.1);
	glRotatef(180, 0.0f, 1.0f, -0.5f);//rota��o
	drawMode2("../models/leia.obj");
	glPopMatrix();

}



