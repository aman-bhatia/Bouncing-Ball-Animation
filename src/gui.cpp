#include "ball.h"
#include "eventHandling.h"
#include "gui.h"
#include <cstdlib>
#include <math.h>
using namespace std;

float _time = 25;
float ratio;
int main_window;
GLuint bg_id; //The id of the texture

#define ENABLE_ID            300
#define DISABLE_ID           301

/********************************************************Lighting Attributes*******************************************/
GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.3f, 1.0f };
GLfloat light_diffuse[]  = { .6f, .6f, 0.6f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { .5f, .5f, 1.0f, 0.0f };

GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat high_shininess[] = { 100.0f };

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };


/*******************************************************Load BackGroung Image*********************************************/
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

//Initializes 3D rendering
void initRendering() {
    //Makes 3D drawing work when something is in front of something else
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); //Enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function
    Image* bg_image = loadBMP("bg.bmp");
	bg_id = loadTexture(bg_image);
	delete bg_image;
}

//Called when the window is resized
void handleResize(int w, int h) {
    //Tell OpenGL how to convert from coordinates to pixel values
    
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport( tx, ty, tw, th );

    winw = w;
	winh = h;
    ratio = (float)w / (float)h;
    wlx = wly * ratio;
}

// function for regularly updating the scene
void update(int value){
	if (playing){
		if (threeD){
			for (int i=0;i<num_balls;i++){
				Balls[i].moveX(Balls[i].getVelocityX());
				Balls[i].moveY(Balls[i].getVelocityY());
				Balls[i].moveZ(Balls[i].getVelocityZ());
				Balls[i].handleBallWallCollision();
			}
		} else {
			for (int i=0;i<num_balls;i++){
				Balls[i].moveX(Balls[i].getVelocityX());
				Balls[i].moveY(Balls[i].getVelocityY());
				Balls[i].handleBallWallCollision();
			}
		}
		for (int i=0;i<num_balls;i++){
				for (int j=i+1;j<num_balls;j++){
					if (Balls[i].isBallCollision(Balls[j])) Balls[i].handleBallBallCollision(Balls[j]);
				}
			}
	}
	
	glutPostRedisplay(); //Tell GLUT that the scene has changed    
    glutTimerFunc(_time, update, 0);
}

// this funtion draws the scene everytime accordingly
void drawScene() {
	if (threeD){
		
		glDisable(GL_CULL_FACE);
	 
		glDisable(GL_DEPTH_TEST);
		
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING); 
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(pers_angle,(winh==0)?(1):((float)winw/winh),1,200);
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, bg_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		switch (thm){
			case (Default) :
				glColor4f(1,1,0,1);
				break;
			case(BlackMetallic) :
				glColor4f(1,1,0,1);
				break;
			case(HighContrast) :
				glColor4f(0.1,0.1,0.1,1);
				break;
			case(Pool) :
				glColor4f(0,1,0,1);
				break;
			}
	  
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f( -50,-50,-1);
			glTexCoord2f(1.0f, 0.0f);    
			glVertex3f(-50,50,-1);
			glTexCoord2f(1.0f, 1.0f);    
			glVertex3f(50,50,-1);
			glTexCoord2f(0.0f, 1.0f);   
			glVertex3f( 50,-50,-1);
		glEnd(); 
		glDisable(GL_TEXTURE_2D);
		

		//Set perspective viewing transformation
		
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(0.0f,0.0f,-wly-2);
		glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] );

		//Draw the 3D elements in the scene
		
		glPushMatrix();
			glTranslatef(0,0,-z_plane-scale); 
			glRotatef( rotationY, 0.0, 1.0, 0.0 );
			glRotatef( rotationX, 1.0, 0.0, 0.0 );
			
			glColor4f(1,1,1,0.05);
			glBegin(GL_QUADS);
				glVertex3f( wlx, wly, -wly);    
				glVertex3f(-wlx, wly, -wly);    
				glVertex3f(-wlx,-wly, -wly);   
				glVertex3f( wlx,-wly, -wly);    
				
				glVertex3f( wlx, wly, wly);
				glVertex3f(-wlx, wly, wly);     
				glVertex3f(-wlx, wly, -wly);   
				glVertex3f( wlx, wly, -wly);    
								
				glVertex3f( wlx, -wly, -wly);
				glVertex3f(-wlx, -wly, -wly);
				glVertex3f(-wlx, -wly, wly); 
				glVertex3f( wlx, -wly, wly);
				
				glVertex3f( wlx, wly, -wly); 
				glVertex3f( wlx, -wly, -wly);
				glVertex3f( wlx, -wly, wly); 
				glVertex3f( wlx, wly, wly);
				
				glVertex3f( -wlx, wly, wly);
				glVertex3f( -wlx, -wly, wly); 
				glVertex3f( -wlx, -wly, -wly);
				glVertex3f( -wlx, wly, -wly);  
				
				glVertex3f( wlx, wly, wly);    
				glVertex3f(-wlx, wly, wly);    
				glVertex3f(-wlx,-wly, wly);   
				glVertex3f( wlx,-wly, wly);    
				
			glEnd();
			for (int z=0;z<num_balls;z++){
				switch (thm){
					case (BlackMetallic) :
						glColor3ub(10,10,10);
						break;
					default :
						glColor3f(Balls[z].getColor().r,Balls[z].getColor().g,Balls[z].getColor().b);
						break;
					}
				glPushMatrix();
					glTranslatef(Balls[z].getCentreX(),Balls[z].getCentreY(),Balls[z].getCentreZ());
					glutSolidSphere(Balls[z].getRadius(),50,50);
				glPopMatrix();
			}
		glPopMatrix();

		glutSwapBuffers(); //Send the 3D scene to the screen
	} else {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	 
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	 
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING); 
	 
		glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	 
		glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); 
		
		//Clear the background
		switch (thm){
			case (Default):
				glClearColor(0.9,0.9,0.9,0);
				break;
			case (BlackMetallic) :
				glClearColor(1,1,1,0);
				break;
			case (Pool) :
				glClearColor(37.0/256,143.0/256,46.0/256,0);
				break;
			case (HighContrast) :
				glClearColor(0,0,0,0);
				break;
		}
	
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//Set perspective viewing transformation
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(pers_angle,(winh==0)?(1):((float)winw/winh),1,200);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		//glMultMatrixf( lights_rotation );

		//Draw the 3D elements in the scene
		glPushMatrix();
			glTranslatef(0,0,-z_plane);
			for (int z=0;z<num_balls;z++){
				switch (thm){
					case (BlackMetallic) :
						glColor3ub(10,10,10);
						break;
					default :
						glColor3f(Balls[z].getColor().r,Balls[z].getColor().g,Balls[z].getColor().b);
						break;
					}
				glPushMatrix();
					glTranslatef(Balls[z].getCentreX(),Balls[z].getCentreY(),0);
					glutSolidSphere(Balls[z].getRadius(),50,50);
					if (thm == BlackMetallic || thm == HighContrast) glColor3f(1,1,1);
					else glColor3f(0,0,0);
					if (ball_selected == Balls[z].getId())  glutWireSphere(Balls[z].getRadius()+0.03,20,20);
					
				glPopMatrix();
		}
		glPopMatrix();
	
		glutSwapBuffers(); //Send the 3D scene to the screen
	}
}

// initialize bal attributes
void makeBalls(){
	num_balls = 10;
	Balls = new ball[num_balls];
	for (int i=0;i<num_balls;i++){
		Balls[i] = ball(i);
		Balls[i].setCentreX(ran());
		Balls[i].setCentreY(ran());
		if (threeD) Balls[i].setCentreZ(ran());
		Balls[i].setVelocityX(min_speed+(ran()/30));
		Balls[i].setVelocityY(min_speed+(ran()/30));
		if (threeD) Balls[i].setVelocityZ(min_speed+(ran()/30));
		if (same_size) Balls[i].setRadius(min_radius);
		else Balls[i].setRadius(max((double)(abs((long)ran())/4),min_radius));
	}
}

void myGlutIdle( void ){
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  

    GLUI_Master.sync_live_all();

	glutPostRedisplay();
}
