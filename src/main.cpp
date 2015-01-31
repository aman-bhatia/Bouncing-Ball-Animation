#include "ball.h"
#include "eventHandling.h"
#include "gui.h"
#include "tinyfiledialogs.h"
#include "imageloader.h"
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <GL/glui.h>
using namespace std;

int main(int argc, char** argv) {
    srand(time(0));
    
    glutInit(&argc, argv);													//Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(winw,winh);											//Set the window size
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-winw)/2,(glutGet(GLUT_SCREEN_HEIGHT)-winh)/2);
    
	main_window = glutCreateWindow("Bouncing Balls Animation");				//Create the window
    initRendering();														//Initialize rendering
    makeBalls();															//initialize ball attributes
    
    //Set handler functions for drawing, keypresses, and window resizes
    glutDisplayFunc(drawScene);
    glutTimerFunc(_time, update, 0);

	GLUI_Master.set_glutKeyboardFunc(handleKeypress);
	GLUI_Master.set_glutSpecialFunc(handleSpecialKeypress);
	GLUI_Master.set_glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
	GLUI_Master.set_glutReshapeFunc(handleResize);
	
	///********************************************************** GLUI******************************************************///
	
	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT);
	
	obj_panel = new GLUI_Panel(glui, "Ball Properties" );
    
    new GLUI_StaticText(obj_panel,"");
	new GLUI_Button( obj_panel, "Play/Pause" , 0, cbForPP );
	new GLUI_Separator( obj_panel );
	
	(new GLUI_Spinner( obj_panel, "Number of Balls:", &num_balls))
		->set_int_limits( 1,max_balls );
    
    new GLUI_StaticText( obj_panel, "Change Speed of Balls : " );
	(new GLUI_Scrollbar( obj_panel, "Speed",GLUI_SCROLL_HORIZONTAL, &speed,1,cbSpeed))
		->set_int_limits( 1,10 );
	new GLUI_StaticText( obj_panel, "" );
		
		new GLUI_Checkbox( obj_panel, "Same Size of Balls", &same_size,1 ,cbSameSize);
		new GLUI_StaticText( obj_panel, "" );

	  new GLUI_Checkbox( obj_panel, "Change Color of Balls", &same_color, 1, cbSameColor );
	  new GLUI_StaticText( obj_panel, "" );
	  
	  char choose_color[] = "Choose Color";
	  gb = new GLUI_Button( obj_panel, choose_color, 0,(GLUI_Update_CB)cbColorChooser );
	  gb->disable();
	  
	  new GLUI_Separator( obj_panel );
	  new GLUI_StaticText( obj_panel, "Red, Green, Blue :" );
	  sbRed = new GLUI_Scrollbar( obj_panel, "Red", GLUI_SCROLL_HORIZONTAL, &globalR,1,cbSameColor);
	  sbRed->set_float_limits(1,256);
	  
	  sbGreen = new GLUI_Scrollbar( obj_panel, "Green", GLUI_SCROLL_HORIZONTAL, &globalG,1,cbSameColor);
	  sbGreen->set_float_limits(1,256);
	  
	  sbBlue = new GLUI_Scrollbar( obj_panel, "Blue", GLUI_SCROLL_HORIZONTAL, &globalB,1,cbSameColor);
	  sbBlue->set_float_limits(1,256);
	  
		sbRed->disable();
		sbGreen->disable();
		sbBlue->disable();
		
	new GLUI_Separator( glui );
		
	obj_panel = new GLUI_Rollout(glui, "Look And Feel", false );

		radio2D = new GLUI_RadioGroup( obj_panel,&thm_int,1, cbTheme);
		new GLUI_RadioButton( radio2D, "Default" );
		new GLUI_RadioButton( radio2D, "Black Metallic" );
		new GLUI_RadioButton( radio2D, "High Contrast" );
		new GLUI_RadioButton( radio2D, "Pool/Billiard" ); 
		new GLUI_StaticText( obj_panel, "" );

		radio3D = new GLUI_RadioGroup( obj_panel,&thm_int,1, cbTheme);
		new GLUI_RadioButton( radio3D, "Default" );
		new GLUI_RadioButton( radio3D, "Black Metallic" );
		new GLUI_RadioButton( radio3D, "High Contrast" );
		new GLUI_RadioButton( radio3D, "Pool/Billiard" ); 
		new GLUI_StaticText( obj_panel, "" );
		radio3D->disable();

	new GLUI_Separator( glui );
	
	obj_panel = new GLUI_Panel(glui, "Mode");
	radio = new GLUI_RadioGroup( obj_panel,&threeD,1, cbMode);
		new GLUI_RadioButton( radio, "2D" );
		new GLUI_RadioButton( radio, "3D" );
	
	new GLUI_Separator( glui );
	gb_enable = new GLUI_Button( glui, "Disable movement", DISABLE_ID, control_cb );
	gb_disable = new GLUI_Button( glui, "Enable movement", ENABLE_ID, control_cb );
	
	gb_enable->disable();
	gb_disable->disable();
		
  new GLUI_StaticText( glui, "" );
	 
	new GLUI_Separator( glui );
		new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );
	
	glui2 = GLUI_Master.create_glui_subwindow( main_window, 
                                             GLUI_SUBWINDOW_BOTTOM );
  glui2->set_main_gfx_window( main_window );
  
  GLUI_Translation *trans_xy = 
    new GLUI_Translation(glui2, "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
  trans_xy->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_x = 
    new GLUI_Translation(glui2, "Objects X", GLUI_TRANSLATION_X, obj_pos );
  trans_x->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_y = 
    new GLUI_Translation( glui2, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
  trans_y->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_z = 
    new GLUI_Translation( glui2, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  trans_z->set_speed( .005 );
	
	glui->set_main_gfx_window(main_window);
	
	
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	
	///******************************************************************************************************************///
	
    glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
    return 0; //This line is never reached
}
