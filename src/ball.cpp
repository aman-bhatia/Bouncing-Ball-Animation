#include "ball.h"

#include <stdlib.h>
#include <math.h>
#define PI 3.14156
using namespace std;

int max_balls = 10;
int num_balls = max_balls;
double min_radius = 0.2;
int threeD = 0;
float pers_angle = 30.0;
float z_plane = 6.0;
float wly = tan(pers_angle * PI / 360) * z_plane;
float wlx;
float globalR = 255;
float globalG = 0;
float globalB = 0;
float min_speed = 0.003;
float max_speed = 0.1;

const int num_colors = 7;
color clrArr[num_colors] = {{1,0,0},//red
				{0,1,0},//green
				{0,0,0.625},//dark blue
				{ 233.0/256, 171.0/256, 23.0/256},//yellow
				{0.5, 0,0},// brown
				{1,0.5, 0.25},//orange
				{75.0/256, 0, 130.0/256}//purple
				};

//---------------------------------------------------------Ball class-------------------------------------------------------------//

// it will return a value between -wly to +wly
float ran(){
	return (((-wly*10000) + rand()%((int)(2*wly*10000)))/10000);
}

ball::ball(int bid){
	id = bid;
	bclr = clrArr[rand()%num_colors];
}
ball::ball() {};
int ball::getId(){
	return id;
}
color ball::getColor(){
	return bclr;
}
void ball::setColor(color cl){
	bclr = cl;
}
void ball::setColor(){
	bclr = clrArr[rand()%num_colors];
}
void ball::setCentreX(float cenX){
	cx = cenX;
}
void ball::setCentreY(float cenY){
	cy = cenY;
}
void ball::setCentreZ(float cenZ){
	cz = cenZ;
}
float ball::getCentreX(){
	return cx;
}
float ball::getCentreY(){
	return cy;
}
float ball::getCentreZ(){
	return cz;
}
void ball::setVelocityX(float velX){
	vx = velX;
}
void ball::setVelocityY(float velY){
	vy = velY;
}
void ball::setVelocityZ(float velZ){
	vz = velZ;
}
float ball::getVelocityX(){
	return vx;
}
float ball::getVelocityY(){
	return vy;
}
float ball::getVelocityZ(){
	return vz;
}
void ball::setRadius(float rd){
	if (rd<min_radius) rad = min_radius;
	else rad = rd;
}
float ball::getRadius(){
	return rad;
}
void ball::moveX(float dx){
	cx += dx;
}
void ball::moveY(float dy){
	cy += dy;
}
void ball::moveZ(float dz){
	cz += dz;
}
// checks the collision of 'this' ball with 'bl' ball
bool ball::isBallCollision(ball &bl){
	float dx = cx - bl.getCentreX();
	float dy = cy - bl.getCentreY();
	if (threeD){
		float dz = cz - bl.getCentreZ();
		if (sqrtf(dx*dx + dy*dy + dz*dz) < rad + bl.getRadius()) return true;
		else return false;
	} else {
		if (sqrtf(dx*dx + dy*dy) < rad + bl.getRadius()) return true;
		else return false;
	} 
}
// checks and handle collision between ball and wall
void ball::handleBallWallCollision(){
	if (cx + rad > wlx){
		cx = wlx - rad;
		vx = -vx;
	} else if (cx - rad < -wlx){
		cx = -wlx + rad;
		vx = -vx;
	} else if (cy + rad > wly){
		cy = wly - rad;
		vy = -vy;
	}else if (cy - rad < -wly){
		cy = -wly + rad;
		vy = -vy;
	}
	if (threeD){
		 if (cz + rad > wly){
			cz = wly - rad;
			vz = -vz;
		}else if (cz - rad < -wly){
			cz = -wly + rad;
			vz = -vz;
		}
	}
}
// handle the collision between two balls
void ball:: handleBallBallCollision(ball &bl){
	if (threeD){
		float dx = cx - bl.getCentreX();
		float dy = cy - bl.getCentreY();
		float dz = cz - bl.getCentreZ();
		float dbc = sqrt(dx*dx + dy*dy + dz*dz);		// distance between centres
		float sor = rad + bl.getRadius();		// sum of radii
		
		// minimum translation distance to push balls apart after intersecting
		float mtdx = dx * ((sor - dbc)/dbc); 
		float mtdy = dy * ((sor - dbc)/dbc);
		float mtdz = dz * ((sor - dbc)/dbc);
		
		// inverse mass quantities
		float im1 = 1 / rad*rad*rad; 
		float im2 = 1 / bl.getRadius()*bl.getRadius()*bl.getRadius();
		
		// push-pull them apart based off their mass
		cx += (mtdx * (im1 / (im1 + im2)));
		cy += (mtdy * (im1 / (im1 + im2)));
		cz += (mtdz * (im1 / (im1 + im2)));
		
		bl.moveX(-(mtdx * (im2 / (im1 + im2))));
		bl.moveY(-(mtdy * (im2 / (im1 + im2))));
		bl.moveZ(-(mtdz * (im2 / (im1 + im2))));

		// impact speed
		float dvx = vx - bl.getVelocityX();
		float dvy = vy - bl.getVelocityY();
		float dvz = vz - bl.getVelocityZ();
		float vn = (dvx * (mtdx / sqrt(mtdx*mtdx + mtdy*mtdy + mtdz*mtdz))) + (dvy * (mtdy / sqrt(mtdx*mtdx + mtdy*mtdy + mtdz*mtdz))) + (dvz * (mtdz / sqrt(mtdx*mtdx + mtdy*mtdy + mtdz*mtdz)));
		
		 // sphere intersecting but moving away from each other already
		if (vn > 0) return;

		// impact speed
		float m1 = 1/im1;
		float m2 = 1/im2;
		
		float th = atan((cy - bl.getCentreY()) / (cx - bl.getCentreX()));
		float cth = cosf(th);
		float sth = sinf(th);
		
		// velocities in direction of centres
		float v1t = (cth*((m1-m2)*vx + 2*m2*bl.getVelocityX()) + sth*((m1-m2)*vy + 2*m2*bl.getVelocityY()))/(m1+m2);
		float v2t = v1t + (cth*(vx - bl.getVelocityX()) + sth*(vy - bl.getVelocityY()));
		float v1n = (vy*cth) - (sth*vx);
		float v2n = (cth*bl.getVelocityY() - sth*bl.getVelocityX());
		
		vx = (v1t*cth) - (v1n*sth);
		vy = (v1n*cth) + (v1t*sth);
		bl.setVelocityX((v2t*cth) - (v2n*sth));
		bl.setVelocityY((v2n*cth) + (v2t*sth));
	} else {
		float dx = cx - bl.getCentreX();
		float dy = cy - bl.getCentreY();
		float dbc = sqrt(dx*dx + dy*dy);		// distance between centres
		float sor = rad + bl.getRadius();		// sum of radii
		
		// minimum translation distance to push balls apart after intersecting
		float mtdx = dx * ((sor - dbc)/dbc); 
		float mtdy = dy * ((sor - dbc)/dbc);
		
		// inverse mass quantities
		float im1 = 1 / rad*rad; 
		float im2 = 1 / bl.getRadius()*bl.getRadius();
		
		// push-pull them apart based off their mass
		cx += (mtdx * (im1 / (im1 + im2)));
		cy += (mtdy * (im1 / (im1 + im2)));
		bl.moveX(-(mtdx * (im2 / (im1 + im2))));
		bl.moveY(-(mtdy * (im2 / (im1 + im2))));

		// impact speed
		float dvx = vx - bl.getVelocityX();
		float dvy = vy - bl.getVelocityY();
		float vn = (dvx * (mtdx / sqrt(mtdx*mtdx + mtdy*mtdy))) + (dvy * (mtdy / sqrt(mtdx*mtdx + mtdy*mtdy)));
		
		 // sphere intersecting but moving away from each other already
		if (vn > 0) return;
		
		// impact speed
		float m1 = 1/im1;
		float m2 = 1/im2;
		
		float th = atan((cy - bl.getCentreY()) / (cx - bl.getCentreX()));
		float cth = cosf(th);
		float sth = sinf(th);
		
		// velocities in direction of centres
		float v1t = (cth*((m1-m2)*vx + 2*m2*bl.getVelocityX()) + sth*((m1-m2)*vy + 2*m2*bl.getVelocityY()))/(m1+m2);
		float v2t = v1t + (cth*(vx - bl.getVelocityX()) + sth*(vy - bl.getVelocityY()));
		float v1n = (vy*cth) - (sth*vx);
		float v2n = (cth*bl.getVelocityY() - sth*bl.getVelocityX());
		
		vx = (v1t*cth) - (v1n*sth);
		vy = (v1n*cth) + (v1t*sth);
		bl.setVelocityX((v2t*cth) - (v2n*sth));
		bl.setVelocityY((v2n*cth) + (v2t*sth));
	}
}
