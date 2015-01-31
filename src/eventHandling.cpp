#include "eventHandling.h"
#include <GL/glui.h>
#include <random>
#include <string.h>
using namespace std;

int winw = 1200;			// intial width of the window
int winh = 680;				// intial height of the window
bool playing = true;
int speed = 5;
int prev_speed = 5;
int ball_selected = -1;
int same_size = 1;
int same_color = 0;
int last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;
float scale = 1.0;

theme thm = Default;
int thm_int = 0;


float view_rotate[16] = { 1,0,0,0,
							0,1,0,0,
							0,0,1,-5,
							 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

/** Pointers to the windows and some of the controls we'll create **/
GLUI *glui, *glui2;
GLUI_RadioGroup *radio2D, *radio3D, *radio;
GLUI_Panel      *obj_panel;
GLUI_Scrollbar* sbRed, *sbGreen, *sbBlue;
GLUI_Button *gb, *gb_enable, *gb_disable;

ball* Balls;
void makeBalls(void);

Mouse TheMouse = {0,0,0,0,0};

// this functions is triggered at every mouse click and checks whether click occurs on a ball or not
bool ClickTest(ball* b,int x,int y) {
	float w = winw-180;
	float h = winh - 80;
	float scx = (w*(b->getCentreX() + wlx))/(2*wlx);
	float scy =  h-(h*(b->getCentreY() + wly))/(2*wly);
	float srad = h * (b->getRadius() / (2*wly));
	
	if( x > (scx-srad) && 
		x < (scx+srad) &&
		y > (scy-srad) && 
		y < (scy+srad) ) {
			return true;
	}
	return false;
}

// changes attributes of TheMouse accordingly
void MouseButton(int button,int state,int x, int y){
	if (threeD){
		if ((button == 3) || (button == 4)){ // It's a wheel event
		   // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		   if (state == GLUT_UP) // Disregard redundant GLUT_UP events
		   (button == 3) ? (scale -= 0.25) : (scale += 0.25);
		}
		
		if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
			last_x = x;
			last_y = y;
		}
	}
	TheMouse.x = x;
	TheMouse.y = y;

	if (state == GLUT_DOWN) {
			TheMouse.xpress = x;
			TheMouse.ypress = y;
		switch(button) {
			case GLUT_LEFT_BUTTON:
				TheMouse.lmb = 1;
			case GLUT_MIDDLE_BUTTON:
				TheMouse.mmb = 1;
				break;
			case GLUT_RIGHT_BUTTON:
				TheMouse.rmb = 1;
				break;
		}
	} else {
		switch(button) {
			case GLUT_LEFT_BUTTON:
				TheMouse.lmb = 0;
				break;
			case GLUT_MIDDLE_BUTTON:
				TheMouse.mmb = 0;
				break;
			case GLUT_RIGHT_BUTTON:
				TheMouse.rmb = 0;
				break;
		}
	}
	// check ball selection
	ball_selected = -1;
	for (int i=0;i<num_balls;i++) {
		if( ClickTest(&Balls[i],TheMouse.xpress,TheMouse.ypress) && ClickTest(&Balls[i],x,y) )
		{
			ball_selected = i;			// set ball_selected equals to the id of the selected ball
		}
	}
	glutPostRedisplay();
}

 
void MouseMotion(int x, int y){
	TheMouse.x = x;
	TheMouse.y = y;
	
	if (threeD){
		rotationX += (float) (y - last_y);
		rotationY += (float) (x - last_x);

		last_x = x;
		last_y = y;
	}
	glutPostRedisplay();
}

void MousePassiveMotion(int x, int y){
	TheMouse.x = x;
	TheMouse.y = y;
}


//Called when a key is pressed
void handleKeypress(unsigned char key,int x, int y) {    //The current mouse coordinates
	switch (key){
		case (27):		//ESC
			exit(0);
			break;
		case (13):		//Enter
			exit(0);
			break;
		case(32):		//SpaceBar
			cbForPP(0);
			break;
	}
}

void handleSpecialKeypress(int key,int x, int y) {    //The current mouse coordinates
	switch (key){
		case(GLUT_KEY_UP):		//Up Arrow
			obj_pos[1]+= 0.05;
			break;
		case(GLUT_KEY_DOWN):		// Down Arrow
			obj_pos[1]-= 0.05;
			break;
		case(GLUT_KEY_LEFT):		// Down Arrow
			obj_pos[0]-= 0.05;
			break;
		case(GLUT_KEY_RIGHT):		// Down Arrow
			obj_pos[0]+= 0.05;
			break;
	}
}

// call back function for increasing speed of all balls
void cbIncSpeed(){	
	for (int i=0;i<num_balls;i++){
		if (Balls[i].getVelocityX() < 0) Balls[i].setVelocityX(Balls[i].getVelocityX() - 0.01);
		else Balls[i].setVelocityX(Balls[i].getVelocityX() + 0.01);
		if (Balls[i].getVelocityY() < 0) Balls[i].setVelocityY(Balls[i].getVelocityY() - 0.01);
		else Balls[i].setVelocityY(Balls[i].getVelocityY() + 0.01);
		if (threeD){
			if (Balls[i].getVelocityZ() < 0) Balls[i].setVelocityZ(Balls[i].getVelocityZ() - 0.01);
			else Balls[i].setVelocityZ(Balls[i].getVelocityZ() + 0.01);
		}
	}
}

// call back function for increasing speed of ball with id 'bid'
void cbIncBallSpeed(int bid){
	if (Balls[bid].getVelocityX() < 0) Balls[bid].setVelocityX(Balls[bid].getVelocityX() - 0.01);
	else Balls[bid].setVelocityX(Balls[bid].getVelocityX() + 0.01);
	if (Balls[bid].getVelocityY() < 0) Balls[bid].setVelocityY(Balls[bid].getVelocityY() - 0.01);
	else Balls[bid].setVelocityY(Balls[bid].getVelocityY() + 0.01);
	if (threeD){
		if (Balls[bid].getVelocityZ() < 0) Balls[bid].setVelocityZ(Balls[bid].getVelocityZ() - 0.01);
		else Balls[bid].setVelocityZ(Balls[bid].getVelocityZ() + 0.01);
	}
}

// call back function for decreasing speed of all balls
void cbDecSpeed(){
	for (int i=0;i<num_balls;i++){
		if (Balls[i].getVelocityX() < 0) Balls[i].setVelocityX(Balls[i].getVelocityX() + 0.01);
		else Balls[i].setVelocityX(Balls[i].getVelocityX() - 0.01);
		if (Balls[i].getVelocityY() < 0) Balls[i].setVelocityY(Balls[i].getVelocityY() + 0.01);
		else Balls[i].setVelocityY(Balls[i].getVelocityY() - 0.01);
		if (threeD){
			if (Balls[i].getVelocityZ() < 0) Balls[i].setVelocityZ(Balls[i].getVelocityZ() + 0.01);
			else Balls[i].setVelocityZ(Balls[i].getVelocityZ() - 0.01);
		}
	}
}

// call back function for decreasing speed of ball with id 'bid'
void cbDecBallSpeed(int bid){
	if (Balls[bid].getVelocityX() < 0) Balls[bid].setVelocityX(Balls[bid].getVelocityX() + 0.01);
	else Balls[bid].setVelocityX(Balls[bid].getVelocityX() - 0.01);
	if (Balls[bid].getVelocityY() < 0) Balls[bid].setVelocityY(Balls[bid].getVelocityY() + 0.01);
	else Balls[bid].setVelocityY(Balls[bid].getVelocityY() - 0.01);
	if (threeD){
		if (Balls[bid].getVelocityZ() < 0) Balls[bid].setVelocityZ(Balls[bid].getVelocityZ() + 0.01);
		else Balls[bid].setVelocityZ(Balls[bid].getVelocityZ() - 0.01);
	}
}

// call back function for setting Default Theme
void cbDefault(){
	thm = Default;
}

// call back function for setting High Contrast Theme
void cbHighContrast(){
	thm = HighContrast;
}

// call back function for setting Black Metallic Theme
void cbBlackMetallic(){
	thm = BlackMetallic;
}

// call back function for setting Pool Theme
void cbPool(){
	thm = Pool;
}

// call back function for play and pause
void cbForPP(int value){
	playing = !(playing);
}

void cbTheme(int value){
	if (threeD){
		radio2D->disable();
		radio3D->enable();
	} else {
		radio2D->enable();
		radio3D->disable();
	}
	switch(thm_int){
		case (0):
			cbDefault();
			break;
		case (1):
			cbBlackMetallic();
			break;
		case (2):
			cbHighContrast();
			break;
		case (3):
			cbPool();
			break;
	}
}


// call back function when size changes
void cbSameSize(int value){
	if (same_size){
		for (int i=0;i<max_balls;i++) Balls[i].setRadius(min_radius);
	} else {
		for (int i=0;i<max_balls;i++) Balls[i].setRadius(max((double)(abs(ran())/4),min_radius));
	}
	
}
// call back function when color changes
void cbSameColor(int value){
	if (ball_selected == -1){
		if (same_color){
			sbRed->enable();
			sbGreen->enable();
			sbBlue->enable();
			gb->enable();
			
			color clr = {globalR/256,globalG/256,globalB/256};
			for (int i=0;i<num_balls;i++){
				Balls[i].setColor(clr);
			}
		} else {
			sbRed->disable();
			sbGreen->disable();
			sbBlue->disable();
			gb->disable();
			
			for (int i=0;i<num_balls;i++){
				Balls[i].setColor();
			}
		}
	} else {
		if (same_color){
			sbRed->enable();
			sbGreen->enable();
			sbBlue->enable();
			gb->enable();
			
			color clr = {(float)globalR/256,(float)globalG/256,(float)globalB/256};
			Balls[ball_selected].setColor(clr);
		} else {
			sbRed->disable();
			sbGreen->disable();
			sbBlue->disable();
			gb->disable();
			
			Balls[ball_selected].setColor();
		}
	}
}

// call back function when mode (2D/3D) changes
void cbMode(int value){
	if(threeD){
		gb_enable->enable();
		gb_disable->enable();
	} else {			
		gb_enable->disable();
		gb_disable->disable();
	}
	makeBalls();
	cbSameSize(1);
	cbSameColor(1);
	cbTheme(1);
	glutPostRedisplay();
}

// call back function for chosing color
void cbColorChooser(int value){
	unsigned char ret[3] = {(unsigned char)globalR,(unsigned char)globalG,(unsigned char)globalB};
	tinyfd_colorChooser("Choose Color", NULL,ret,ret);
	globalR = ret[0];
	globalG = ret[1];
	globalB = ret[2];
	cbSameColor(1);
}

// call back function for changing speed
void cbSpeed(int value){
	if (ball_selected == -1){
		if (speed > prev_speed){
			int count = speed - prev_speed;
			while (count--) cbIncSpeed();
		} else if (speed < prev_speed){
			int count = prev_speed - speed;
			while (count--) cbDecSpeed();
		}
		prev_speed = speed;
	} else {
		if (speed > prev_speed){
			int count = speed - prev_speed;
			while (count--) cbIncBallSpeed(ball_selected);
		} else if (speed < prev_speed){
			int count = prev_speed - speed;
			while (count--) cbDecBallSpeed(ball_selected);
		}
		prev_speed = speed;
	}
	
}

void control_cb( int control ){
  if ( control == ENABLE_ID ){
    glui2->enable();
  } else if ( control == DISABLE_ID ){
    glui2->disable();
  }
}
