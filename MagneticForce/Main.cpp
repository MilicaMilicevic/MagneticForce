#include<gl\glut.h>
#include<string.h>
#include<math.h>

int screenWidth,screenHeight;

const int speedMax=6;	//pomocna za regulisanje brizine okretanja elipsi!

float i1=1.0,i2=1.0;		//inicijalne jacine struja!
float ratio=1.0;		//odnos izmedju jacina struja!

float translateSpeed1=0.1,translateSpeed2=0.1;	//brzine kojima se krecu provodnici, svako na svoju stranu!
float translateSpeedAxe1=0.1,translateSpeedAxe2=0.1;

int matrixProperties[4][2];		//sve na podrazumijevano, ovo su samo pomocni brojaci, kontroleri, (i,speed)

int idEllipse[4]={0,1,2,3};    //oko svakog cilindra imamo 4 elipse!

bool sameDirection=false;	//naznacava da li su smijerov struja medjusobno razliciti!Kontrolna promjenljiva!

float xArrow,yArrow;
float const triangleSize=0.5;  //duzina stranice trougla, dijela strelice!
const float lengthArrow=1.5;   //duzina strelice za vektore!

const int lengthAxe=100;		

const int axeCenter1=0, axeCenter2=5;  //pomocne, prva i druga osa, druga pomjerena za 5 u odnosu na prvu!
const float cylindarRadius=0.5;			//pomocna, pri crtanju vektora!

const float textSpeedIncDec=0.00005;

int matrixAngle[6][2]={ //definise lukove koje treba zanemariti pri crtanju elipse!Vrijednosti su proizvoljno birane!
	{45,360},
	{-270,45},
	{-225,90},
	{-180,225},
	{-135,180},
	{-90,135}};

int matrixDistance[4][2]={ //vrijednosti elipsi!Birane tako da su silnice pri provodniku gusce (zavisno od rastojanja).
	{1.25,1.0},
	{2.5,2.0},
	{4.375,3.5},  
	{6.875,5.5}};

const float DEG2RAD = 3.14159/180.0;     

GLfloat textSpeed1=0,textSpeed2=0;

GLfloat xView=0.0, yView=5.0, zView=15.0;    //inicijalne vrijednosti za transf. 3D pogleda, njima se manipulise preko tastat.

GLUquadricObj *qobj=gluNewQuadric();		//pomocna za kreiranje cilindara/provodnika!

const GLfloat light[4][4]={                 //karakteristike izvora
	{10.0,10.0,10.0,1.0},	
	{1.0,1.0,1.0,1.0},		
	{1.0,1.0,1.0,1.0},		
	{0.1,0.1,0.1,0.1}};		

const GLfloat cylindar[3][4]={             //karakteristike cilindra!
	{1.0,1.0,0.0,1.0}, 
	{0.1,0.1,0.1,1.0}, 
	{0.1,0.1,0.1,1.0}}; 
const GLfloat shininess=10.0;

//----------------------CRTANJE ELIPSE (OBICNA)------------------------------------------------------------
void drawEllipse(float radiusX, float radiusY,int from, int to){
   glBegin(GL_LINE_STRIP); 
		for(int i=from;i<to;i++){
			float rad = i*DEG2RAD;
			glVertex2f(cos(rad)*radiusX,sin(rad)*radiusY);
			}
   glEnd();
}

void spinEllipse(GLfloat increment,int id){			//speedLimiter - da se ne vrti prebrzo, da se mogu uociti prazni lukovi!
	glPushMatrix();
	glTranslatef(increment,0,0);
	if(matrixProperties[id][0]<speedMax)
		matrixProperties[id][0]++;
	else{
		if(matrixProperties[id][1]<6)
			drawEllipse(matrixDistance[id][0],matrixDistance[id][1],matrixAngle[matrixProperties[id][1]][0],matrixAngle[matrixProperties[id][1]++][1]);
		else
			matrixProperties[id][1]=0;
		matrixProperties[id][0]=0;
		}
	glPopMatrix();
}

void init(void){
	glClearColor(1.0,1.0,1.0,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);	  //omogucavamo svijetlo!Model treba da bude jednostavan, nema potrebe za vise izvora!
	glLightfv(GL_LIGHT0,GL_POSITION,light[0]);	
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light[1]);	
	glLightfv(GL_LIGHT0,GL_SPECULAR,light[2]);	
	glLightfv(GL_LIGHT0,GL_AMBIENT,light[3]);	
}

void keyboard(unsigned char key, int x, int y){ //ESC-exit full screen!
	if(key==27){
			glutReshapeWindow(800,500);
			screenHeight=500;
			screenWidth=800;
			glutPositionWindow(100,100);
			}
			glFlush();
}

void mouse(int button, int state, int x,int y){ 
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
		sameDirection=!sameDirection;
}

void print(int x, int y, const char *string, int mode){       //x, y koordinate teksta, string -niz, NAPOMENA : x i y su koordinate prozora!!!!
	GLint viewport [4];
	glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix ();
			glLoadIdentity();
			if(mode==1){
				glTranslatef(textSpeed1,0,0);
				if(!sameDirection){
					if(textSpeed1>-0.5)
						textSpeed1=textSpeed1-textSpeedIncDec;
							}
				else{
					if(textSpeed1<0.05)
						textSpeed1=textSpeed1+textSpeedIncDec;
					}
				}
				else if(mode==2){
				glTranslatef(textSpeed2,0,0);
				if(!sameDirection){
					if(textSpeed2<0.8)
						textSpeed2=textSpeed2+textSpeedIncDec;
							}
				else if(textSpeed2>0.25)
					textSpeed2=textSpeed2-textSpeedIncDec;
					}
			glGetIntegerv (GL_VIEWPORT,viewport);
			gluOrtho2D (0,viewport[2],viewport[3],0);
			glDepthFunc(GL_ALWAYS);
			glRasterPos2f(x, y);
			for(int i=0;i<strlen(string);++i)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,string[i]);
			glDepthFunc(GL_LESS);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix ();
}

//---------------------------------------------------------CRTANJE VEKTORA-------------------------------------------------
void drawArrowCurrent(float xArrow,float yArrow,bool sameDirection,float increment,float incrementAxe){ //mode=0 -> struja, mode=1-> sila
	glPushMatrix();
		glTranslatef(increment,0,0);
		glTranslatef(0,incrementAxe,0);
		glBegin(GL_LINE_STRIP);
			glVertex2f(xArrow,yArrow);
			glVertex2f(xArrow,yArrow+lengthArrow);
		glEnd();
	glPopMatrix();
	if(!sameDirection)
		yArrow=yArrow+lengthArrow;
	glPushMatrix();
		glTranslatef(increment,0,0);
		glTranslatef(0,incrementAxe,0);
		if(sameDirection){
			glBegin(GL_TRIANGLES);
				glVertex2f(xArrow,yArrow-triangleSize); //Cudno se ponasa, uvodjenjem tmp
				glVertex2f(xArrow-triangleSize,yArrow);
				glVertex2f(xArrow+triangleSize,yArrow);
			glEnd();
		}
		else{
			glBegin(GL_TRIANGLES);
				glVertex2f(xArrow,yArrow+triangleSize); 
				glVertex2f(xArrow-triangleSize,yArrow);
				glVertex2f(xArrow+triangleSize,yArrow);
			glEnd();
			}
	glPopMatrix();
}

void drawAxe(float x,float increment){//----------------------CRTANJE OSE------------------------------
	glPushMatrix(); 
		glTranslatef(increment,0,0);
		glBegin(GL_LINES);
			glVertex2f(x,lengthAxe);
			glVertex2f(x,-lengthAxe);
		glEnd();
	glPopMatrix();
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xView,yView,zView,0.0,0.0,0.0,0.0,1.0,0.0);

	glMaterialfv(GL_FRONT,GL_DIFFUSE,cylindar[0]);
	glMaterialfv(GL_FRONT,GL_SPECULAR,cylindar[1]);
	glMaterialfv(GL_FRONT,GL_AMBIENT,cylindar[2]);
	glMaterialf(GL_FRONT,GL_SHININESS,shininess);
	
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glColor3f(0,0,0);
	drawAxe(axeCenter2,translateSpeed2);
	drawAxe(axeCenter1,translateSpeed1);
	print(10,20,"Press F1 to increase I1",0);
	glLineWidth(2.7);
	glColor3f(0,0.8,0);
	print(screenWidth/2,screenHeight*0.2,"I2",2);
	print(screenWidth/2,screenHeight*0.2,"I1",1);
	drawArrowCurrent(axeCenter1-cylindarRadius,6,sameDirection,translateSpeed1,translateSpeedAxe1);			//CRTANJE 1. STRUJE!
	drawArrowCurrent(axeCenter2+cylindarRadius,6,true,translateSpeed2,translateSpeedAxe2);			//necemo mijenjati smijer druge strelice!	crtanje 2. struje!
	if(sameDirection){
		if(translateSpeedAxe1>-yView)
			translateSpeedAxe1=translateSpeedAxe1-0.0005;
		else 
			translateSpeedAxe1=0;
		if(translateSpeedAxe2>-yView)
			translateSpeedAxe2=translateSpeedAxe2-0.0005;
		else
			translateSpeedAxe2=0;
		}
	else{
		if(translateSpeedAxe1<0) 
			translateSpeedAxe1=translateSpeedAxe1+0.0005;
		else
			translateSpeedAxe1=-yView;
		if(translateSpeedAxe2>-yView)
			translateSpeedAxe2=translateSpeedAxe2-0.0005;
		else
			translateSpeedAxe2=0;
	}
		
///------------------------------------------CRTANJE SILE---------------------------------------------------------
	glColor3f(1,0,0);
	if(!sameDirection){  //OVAJ DIO JE TESKO GENERALIZOVATI!
	glPushMatrix();
		glTranslatef(translateSpeed1,0,0);
		glBegin(GL_LINES);
			glVertex2f(axeCenter1-cylindarRadius,-5);
			glVertex2f(axeCenter1-cylindarRadius-lengthArrow,-5);
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed1,0,0);
		glBegin(GL_TRIANGLES);
			glVertex2f(axeCenter1-cylindarRadius-lengthArrow-triangleSize,-5);
			glVertex2f(axeCenter1-cylindarRadius-lengthArrow,-4.5);
			glVertex2f(axeCenter1-cylindarRadius-lengthArrow,-5.5);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(translateSpeed2,0,0);
		glBegin(GL_LINES);
			glVertex2f(axeCenter2+cylindarRadius,-5);
			glVertex2f(axeCenter2+cylindarRadius+lengthArrow,-5);
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed2,0,0);
		glBegin(GL_TRIANGLES);
		glVertex2f(axeCenter2+cylindarRadius+lengthArrow+triangleSize,-5);
			glVertex2f(axeCenter2+cylindarRadius+lengthArrow,-4.5);
			glVertex2f(axeCenter2+cylindarRadius+lengthArrow,-5.5);
		glEnd();
	glPopMatrix();
	}
	else {
		glPushMatrix();
		glTranslatef(translateSpeed1,0,0);
		glBegin(GL_LINES);
			glVertex2f(axeCenter1+cylindarRadius,-5);
			glVertex2f(axeCenter1+cylindarRadius+lengthArrow,-5);
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed1,0,0);
		glBegin(GL_TRIANGLES);
			glVertex2f(axeCenter1+cylindarRadius+lengthArrow+triangleSize,-5);
			glVertex2f(axeCenter1+cylindarRadius+lengthArrow,-4.5);
			glVertex2f(axeCenter1+cylindarRadius+lengthArrow,-5.5);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(translateSpeed2,0,0);
		glBegin(GL_LINES);
				glVertex2f(axeCenter2-cylindarRadius,-5);
				glVertex2f(axeCenter2-cylindarRadius-lengthArrow,-5);
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed2,0,0);
		glBegin(GL_TRIANGLES);
		glVertex2f(axeCenter2-cylindarRadius-lengthArrow-triangleSize,-5);
			glVertex2f(axeCenter2-cylindarRadius-lengthArrow,-4.5);
			glVertex2f(axeCenter2-cylindarRadius-lengthArrow,-5.5);
		glEnd();
	glPopMatrix();
	}
	print(screenWidth/2,screenHeight*0.84,"F1",2);
	print(screenWidth/2,screenHeight*0.84,"F2",1);
//-----------------------------CRTANJE VEKTORA MAGNETNE INDUKCIJE------------------------------------------------------------
	glColor3f(0,0,1);
	glPushMatrix();
		glTranslatef(translateSpeed1,0,0);
		glBegin(GL_LINES);
			glVertex3f(0,-5,-cylindarRadius);
			glVertex3f(0,-5,-(cylindarRadius+lengthArrow));
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed1,0,0);
		glBegin(GL_TRIANGLES);
			glVertex3f(0,-5,-(cylindarRadius+lengthArrow+triangleSize));
			glVertex3f(0,-4.5,-(cylindarRadius+lengthArrow));
			glVertex3f(0,-5.5,-(cylindarRadius+lengthArrow));
		glEnd();
	glPopMatrix();

	if(sameDirection){ 
		glPushMatrix();
		glTranslatef(translateSpeed2,0,0);
		glBegin(GL_LINES);
			glVertex3f(5,-5,+cylindarRadius);
			glVertex3f(5,-5,cylindarRadius+lengthArrow);
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed2,0,0);
		glBegin(GL_TRIANGLES);
			glVertex3f(5,-5,cylindarRadius+lengthArrow+triangleSize);
			glVertex3f(5,-4.5,cylindarRadius+lengthArrow);
			glVertex3f(5,-5.5,cylindarRadius+lengthArrow);
		glEnd();
	glPopMatrix();
	}
else {	
	glPushMatrix();
		glTranslatef(translateSpeed2,0,0);
		glBegin(GL_LINES);
			glVertex3f(5,-5,-cylindarRadius);
			glVertex3f(5,-5,-(cylindarRadius+lengthArrow));
		glEnd();
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(translateSpeed2,0,0);
		glBegin(GL_TRIANGLES);
			glVertex3f(5,-5,-(cylindarRadius+lengthArrow+triangleSize));
			glVertex3f(5,-4.5,-(cylindarRadius+lengthArrow));
			glVertex3f(5,-5.5,-(cylindarRadius+lengthArrow));
		glEnd();
	glPopMatrix();
	}

	glEnable(GL_LIGHTING);
	glRotatef(90,1,0,0);			//po defaultu cilindri orijentisani duz z ose!

	glPushMatrix();      //--------------------------------------------CRTANJE 1.CILINDRA------------------
		glTranslatef(translateSpeed1,0,0);
		gluCylinder(qobj, 0.5, 0.5, 10, 16, 20);
	glPopMatrix();
	if(sameDirection){	
		if(translateSpeed1<0.01)
			translateSpeed1=translateSpeed1+0.001;
		}
	else{
		if(translateSpeed1>-5.0)
			translateSpeed1=translateSpeed1-0.001; 
		}
	glDisable(GL_LIGHTING);				//-------------------------CRTANJE ELIPSI---------------
	glPushMatrix();
		glTranslatef(5,0,3);
		glColor3f(0,0,1);
		glLineWidth(1.0);
		for(int i=0;i<4;i++)
			spinEllipse(translateSpeed2,idEllipse[i]);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glPushMatrix();				//---------------------------------------------------------------------------CRTANJE 2. CILINDRA-
	glTranslatef(5, 0.0, 0.0);   
	glTranslatef(translateSpeed2,0,0);
    gluCylinder(qobj,0.5,0.5, 10, 16, 20);   //drugi cilindar pomijeren u odnosu na prethodni!
	if(sameDirection){
		if(translateSpeed2>-0.01)
			translateSpeed2=(translateSpeed2-0.001)*ratio;	//TREBA PROMIJENITI USLOV ZA RATIO!
	}
	else{
		if(translateSpeed2<5.0*ratio)
			translateSpeed2=(translateSpeed2+0.001)*ratio;  //povecava se, ide u desno!
	}
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glTranslatef(0,0,3);
	glColor3f(0,0,1);
	for(int i=0;i<4;i++)
			spinEllipse(translateSpeed1,idEllipse[i]);
	glEnable(GL_LIGHTING);

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void specialInput(int key,int x,int y){
	switch(key){
		case GLUT_KEY_UP:
			yView=yView+2.0;
			break;
		case GLUT_KEY_DOWN:
			yView=yView-2.0;
			break;
		case GLUT_KEY_LEFT:
			xView=xView-2.0;
			break;
		case GLUT_KEY_RIGHT:
			xView=xView+2.0;
			break;
		case GLUT_KEY_F1:
			i1=i1+0.01;
	}
	ratio=i1/i2; 
	glFlush();
}

void reshape(int w, int h){
	glViewport(0,0,w,h);
	screenHeight=h;  //radi teksta!
	screenWidth=w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,20.0);
}

void menu(int id){
	if(id==2)
		exit(0);
	else if(id==3)
		zView=zView-2.0;
	else if(id=4)
		zView=zView+2.0;
}

void subMenu(int id){
	switch(id){
		case 1: 
			specialInput(GLUT_KEY_LEFT,0,0);
			break;
		case 2:
			specialInput(GLUT_KEY_RIGHT,0,0);
			break;
		case 3:
			specialInput(GLUT_KEY_UP,0,0);
			break;
		case 4:
			specialInput(GLUT_KEY_DOWN,0,0);
	}
}

void createMenu(){
	int subMenu_id=glutCreateMenu(subMenu);
	glutAddMenuEntry("Left",1);
	glutAddMenuEntry("Right",2);
	glutAddMenuEntry("Up",3);
	glutAddMenuEntry("Down",4);
	glutCreateMenu(menu);
	if(screenWidth>800 && screenHeight>500)  //nema smisla prikazivati ako vec nemamo FULL SCREEN!
		glutAddMenuEntry("Exit Full Screen",2);
	glutAddMenuEntry("Zoom in",3);
	glutAddMenuEntry("Zoom out",4);
	glutAddSubMenu("Move to",subMenu_id);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutCreateWindow("MagneticForce");
	glutFullScreen();
    init();
	createMenu();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInput);
	glutMouseFunc(mouse);
    glutMainLoop();
}