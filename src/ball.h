#ifndef BALL_H
#define BALL_H

extern int max_balls;
extern int num_balls;
extern double min_radius;
extern int threeD;
extern float pers_angle;
extern float z_plane;
extern float wly;
extern float wlx;
extern float globalR;
extern float globalG;
extern float globalB;
extern float min_speed;
extern float max_speed;

struct color{
	float r,g,b;
};
extern const int num_colors;

float ran();

class ball{
	private:
		int id;
		color bclr;
		float cx,cy,cz,vx,vy,vz,rad;
	public:
		ball(int bid);
		ball();
		int getId();
		color getColor();
		void setColor(color cl);
		void setColor();
		void setCentreX(float cenX);
		void setCentreY(float cenY);
		void setCentreZ(float cenZ);
		float getCentreX();
		float getCentreY();
		float getCentreZ();
		void setVelocityX(float velX);
		void setVelocityY(float velY);
		void setVelocityZ(float velZ);
		float getVelocityX();
		float getVelocityY();
		float getVelocityZ();
		void setRadius(float rd);
		float getRadius();
		void moveX(float dx);
		void moveY(float dy);
		void moveZ(float dz);
		bool isBallCollision(ball &bl);
		void handleBallWallCollision();
		void handleBallBallCollision(ball &bl);
};

#endif
