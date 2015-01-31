#ifndef GUI_H
#define GUI_H

#include "ball.h"
#include "eventHandling.h"

extern float _time;
extern float ratio;
extern int main_window;
extern GLuint bg_id; //The id of the texture

extern GLfloat light_ambient[];
extern GLfloat light_diffuse[];
extern GLfloat light_specular[];
extern GLfloat light_position[];

extern GLfloat mat_ambient[];
extern GLfloat mat_diffuse[];
extern GLfloat mat_specular[];
extern GLfloat high_shininess[];

extern GLfloat lights_rotation[];

GLuint loadTexture(Image* image);
void initRendering();
void handleResize(int w, int h);
void update(int value);
void drawScene();
void makeBalls();
void myGlutIdle( void );

#endif
