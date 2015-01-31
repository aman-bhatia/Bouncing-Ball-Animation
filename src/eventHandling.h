#ifndef EVENTHANDLING_H
#define EVENTHANDLING_H

#include <GL/glui.h>
#include "tinyfiledialogs.h"
#include "imageloader.h"
#include "ball.h"
extern ball* Balls;

extern int speed;
extern int prev_speed;
extern int ball_selected;
extern int same_size;
extern int same_color;
extern int winw;
extern int winh;
extern int last_x;
extern int last_y;
extern float rotationX;
extern float rotationY;
extern float scale;
extern bool playing;
extern float view_rotate[];
extern float obj_pos[];


#define ENABLE_ID 300
#define DISABLE_ID 301

enum theme {Default, BlackMetallic, HighContrast, Pool };
extern theme thm;
extern int thm_int;


/** Pointers to the windows and some of the controls we'll create **/
extern GLUI *glui, *glui2;
extern GLUI_RadioGroup *radio2D, *radio3D, *radio;
extern GLUI_Panel      *obj_panel;
extern GLUI_Scrollbar* sbRed, *sbGreen, *sbBlue;
extern GLUI_Button *gb, *gb_enable, *gb_disable;

struct Mouse 
{
	int x;		/*	the x coordinate of the mouse cursor	*/
	int y;		/*	the y coordinate of the mouse cursor	*/
	int lmb;	/*	is the left button pressed?		*/
	int mmb;	/*	is the middle button pressed?	*/
	int rmb;	/*	is the right button pressed?	*/
	int xpress; /*	stores the x-coord of when the first button press occurred	*/
	int ypress; /*	stores the y-coord of when the first button press occurred	*/
};

extern Mouse TheMouse;

bool ClickTest(ball* b,int x,int y);
void MouseButton(int button,int state,int x, int y);
void MouseMotion(int x, int y);
void MousePassiveMotion(int x, int y);
void handleKeypress(unsigned char key,int x, int y);
void handleSpecialKeypress(int key,int x, int y);

void cbForPP(int value);
void cbIncSpeed();
void cbIncBallSpeed(int bid);
void cbDecSpeed();
void cbDecBallSpeed(int bid);
void cbDefault();
void cbHighContrast();
void cbBlackMetallic();
void cbPool();
void cbTheme(int value);
void cbSameSize(int value);
void cbSameColor(int value);
void cbMode(int value);
void cbColorChooser(int value);
void cbSpeed(int value);
void control_cb(int control);

#endif
