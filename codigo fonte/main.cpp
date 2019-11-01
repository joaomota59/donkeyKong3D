//*********************************************************************
//  Computacao Gráfica usando OpenGL
//  Autor: C.DANIEL && J.LUCAS
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define QUANT_TEX 1
#define BLOCOS 100 //quantidade de blocos da fase(por onde o personagem vai andar)


//Variaveis Globais usadas para definicao de cores
float R, G, B;
bool esquerda = false, direita = false, cima = false, baixo = false; //botoes para mover o personagem
float personX = 0.75, personY = -0.75; //coordenadas iniciais do personagem
float personComp = 50, personAlt = 30; //comprimento e altura do personagem 


//A-personagem
//B-algo que vai colidir com o personagem
bool colisao(float aX, float aY, float aComp, float aAlt, float bX, float bY, float bComp, float bAlt)
{
	if(aY + aAlt < bY)
		return false;
	else if(aY > bY + bAlt)
		return false;
	else if(aX + aComp < bX)
		return false;
	else if(aX > bX + bComp)
		return false;
	return true;
}

struct Bloco //Blocos do cenário
{
	float x;
	float y;
	float comp;
	float alt;
	bool colide;
};

//Global
Bloco blocos[BLOCOS];
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
	// Registra a função callback que será chamada a cada intervalo de tempo
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutDisplayFunc(display); //funcao callback de desenho
	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutSpecialFunc(keyboard_special);	//funcao callback do teclado especial
	glutMainLoop(); // executa o loop do OpenGL 
	return 0; // retorna 0 para o tipo inteiro da funcao main();
}


// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void)
{
	
		// Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
	     
	glEnable(GL_BLEND); 
		

	/* Activa o modelo de sombreagem de "Gouraud". */
	glShadeModel( GL_SMOOTH );

	/* Activa o z-buffering, de modo a remover as superfícies escondidas */
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
		if(personX < 1)
			personX += 0.035;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP://seta cima
		cima = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN://seta baixo
		baixo = true;
		glutPostRedisplay();
		break;
	default:
		break;

	}

}

GLfloat angulo = 0.0f;
// Função callback de redesenho da janela de visualização




// Funcao usada na funcao callback para desenhar na tela
void display(void)
{     
	// muda para o modo GL_MODELVIEW (nao pretendemos alterar a projecao
	// quando estivermos desenhando na tela)
	//glMatrixMode(GL_MODELVIEW);
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();//Limpa o Buffer de Cores;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpar a tela e o Z-buffer			 	 			

	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );

	gluLookAt(0.0f, .1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);//visao da camera    
	 		DefineIluminacao(); 
	criaCenario(); 
   
	  		 

	// glTranslatef( 0.1, 0.0, 0.0 );      // Não está incluído
	//glClear(GL_DEPTH_BUFFER_BIT); // Limpar a tela e o Z-buffer
	//glLoadIdentity();//Limpa o Buffer de Cores



	/*	if(esquerda && personX - 8 >= 0) //se a seta esquerda for pressionada e o eixo X nao sair da tela
		{
			personX -= 8; //subtrai 8 pixels
			for(int n = 0; n < BLOCOS; n++)
				if(blocos[n].colide)
				{
					if(colisao(personX, personY, personComp, personAlt, blocos[n].x, blocos[n].y, blocos[n].comp, blocos[n].alt))
					{
						personX += 8;
						break;
					}
				}
			esquerda = false;
		}
		else if(direita && personX + 8 <= 512 - personComp)
		{
			personX += 8;
			for(int n = 0; n < BLOCOS; n++)
				if(blocos[n].colide)
				{
					if(colisao(personX, personY, personComp, personAlt, blocos[n].x, blocos[n].y, blocos[n].comp, blocos[n].alt))
					{
						personX -= 8;
						break;
					}
				}
			cima = false;
			direita = false;
		}
		else if(cima && personY + 8 <= 512 - personAlt)
		{
			personY += 8;
			for(int n = 0; n < BLOCOS; n++)
				if(blocos[n].colide)
				{
					if(colisao(personX, personY, personComp, personAlt, blocos[n].x, blocos[n].y, blocos[n].comp, blocos[n].alt))
					{
						personY -= 8;
						break;
					}
				}
			cima = false;
		}
		else if(baixo && personY - 8 >= 0)
		{
			personY -= 8;
			for(int n = 0; n < BLOCOS; n++)
				if(blocos[n].colide)
				{
					if(colisao(personX, personY, personComp, personAlt, blocos[n].x, blocos[n].y, blocos[n].comp, blocos[n].alt))
					{
						personY += 8;
						break;
					}
				}
			baixo = false;
		}
		glBegin(GL_QUADS);//posicao do personagem
		glColor3f(1, 0, 0);
		glVertex2f(personX, personY); //Vértice superior esquerdo
		glVertex2f(personX + personComp, personY); //Vértice superior direito
		glVertex2f(personX + personComp, personY + personAlt); //Vértice inferior direito
		glVertex2f(personX, personY + personAlt); //Vértice inferior esquerdo
		glEnd();
		criaBloco();
		glutSolidCube(0.4);
	//	glScalef(x, x, 0);
	//	glTranslatef(200,200, 0);
	//	glRotatef(180, 0, 0, 1);
	*/
	
	glutSwapBuffers();

}


void criaCubo(float x)
{
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
	
	GLfloat posLuz[4] = { -0.2, -0.2, -1.0, 1.0 };// Posição da fonte de luz
	GLfloat luzAmbiente[4] = {1.0, 1.0, 1.0, 1.0};//intensidade /pos da luz que reflete na esfera
	GLfloat luzDifusa[4] = {0.7, 0.7, 0.7, 1.0};	 // "cor"
	GLfloat luzEspecular[4] = {1.0, 1.0, 1.0, 1.0}; // "brilho"

	// Capacidade de brilho do material
	GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
	GLint especMaterial = 60;

	// Define a refletância do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	// Define a concentração do brilho
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posLuz );
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
	for (int k = 0; k < 4; k++) //numero de andares 'k'
	{
		int BLOCKS = 11;
		if(k == 0) BLOCKS = 12;
		for(int i = 0; i < BLOCKS; i++)//numero de blocos em cada andar 'i'
		{
			glPushMatrix();
			if(k % 2 == 0)
				glTranslatef( c, 0.0, 1 - 0.5 * k);
			else
				glTranslatef( -0.2 + c, 0.0, 1 - 0.5 * k);
			criaCubo(0.09);
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
	glPushMatrix();
	glTranslatef( -0.9, -0.09, 0.75);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( 0.8 , -0.09, 0.25);
	glScalef(0.8, 0.0, 1.75);
	criaEscada(0.09);
	glPopMatrix();
	glPushMatrix();
	glTranslatef( -0.80, -0.09, -0.25);
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
    gluQuadricNormals(quad,GLU_FLAT);//FLAT significa uma normal para cada face do quadrado
    gluQuadricOrientation(quad,GLU_INSIDE);//vetor da normal apontando p dentro
    for(int barril=0,fator=0;barril<4;barril++,fator++){
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);//cor do objeto verde
	glRotatef(90, 1.0f, 0.0f, 0.0f);//rotação
	gluQuadricTexture(quad, GLU_TRUE);//textura do quadrado é ativada
	if(fator<2)
		glTranslatef(0.75+(fator-fator*0.9), -0.64, 0.0);//posicao final do barril	
	else
		glTranslatef(0.75+((fator-2)-(fator-2)*0.9), -0.74, 0.0);//posicao final do barril

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
	
	
	
	/*unsigned char *uc4 = stbi_load("../sprites/donkey_kong_arcade.jpg.jpg", &w, &h, NULL, 0);
	glGenTextures(1, &t); //gera nomes identificadores de texturas
	glBindTexture(GL_TEXTURE_2D, t); //Ativa a textura atual
	//Cria a textura de cada escada

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
				 0, GL_RGB, GL_UNSIGNED_BYTE, uc4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glEnable(GL_TEXTURE_2D);//inicia a nova textura
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);//inicia a textura do macaco
	
	glPushMatrix(); //cria macaco
	//glRotatef(45, 1.0f, 0.0f, 0.0f);//rotação
	glTranslatef( 0.59, -0.02, -0.7);
	glColor3f(1.0f, 0.0f, 0.0f);	
	criaEscada(0.09);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);//desativa a textura do macaco
    stbi_image_free(uc4); */
} 

void criaEscada(float x) //Cria um quadrilátero serve p escada e para alguns personagens
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
		glVertex2f(blocos[n].x, blocos[n].y); //Vértice inferior esquerdo
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y); //Vértice inferior direito
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y + blocos[n].alt); //Vértice superior direito
		glVertex2f(blocos[n].x, blocos[n].y + blocos[n].alt); //Vértice superior esquerdo
		glEnd();
		glLineWidth(4);//espessura das linhas
		glBegin(GL_LINES);//linhas cruzadas
		glVertex2f(blocos[n].x, blocos[n].y); //Vértice inferior esquerdo
		glVertex2f((int)((blocos[n].x + blocos[n].comp + blocos[n].x) / 2), (int)((blocos[n].y + blocos[n].alt + blocos[n].y + blocos[n].alt) / 2)); //Vértice superior (fica no meio)
		glVertex2f((int)((blocos[n].x + blocos[n].comp + blocos[n].x) / 2), (int)((blocos[n].y + blocos[n].alt + blocos[n].y + blocos[n].alt) / 2)); //Vértice superior (fica no meio)
		glVertex2f(blocos[n].x + blocos[n].comp, blocos[n].y); //Vértice inferior direito
		glEnd();
	}

} 
