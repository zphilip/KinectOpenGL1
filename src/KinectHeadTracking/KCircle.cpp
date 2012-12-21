#include "KCircle.h"


KCircle::KCircle(void)
{
	// Initialize to zero-circle
	mCenter.mX = 0;
	mCenter.mY = 0;
	mCenter.mZ = 0;
	mCenter.mColor.r = 0;
	mCenter.mColor.b = 0;
	mCenter.mColor.g = 0;
	mRadius = 0;
}

KCircle::KCircle(KVertex Center, float radius){
	// Control the radius and set it
	if( radius < 0 )
		radius = -radius;
	
	mCenter = Center;
	mRadius = radius;
}


KCircle::~KCircle(void)
{
}


void KCircle::renderCircle(void)
{
	// Render the circle as a polygon, but only if there is a circle
	if(mRadius > 0) {
		glLineWidth(2);
		glBegin(GL_LINES);
			glColor3f(ITEM_LINE_COLOR);
			glVertex3f(mCenter.mX, mCenter.mY, mCenter.mZ);
			glVertex3f(mCenter.mX, mCenter.mY, -1);
		glEnd();
		glLineWidth(1);

		glBegin(GL_POLYGON);
			// Define the color
			glColor3f(	mCenter.mColor.r,
						mCenter.mColor.g,
						mCenter.mColor.b);

			// Paint the middle
			glVertex3f(	mCenter.mX,
						mCenter.mY,
						mCenter.mZ);

			// Paint the outer circle
			for( double theta = 0.0; theta < 2*K_PI+2*K_PI/CIRCLE_ACCURACY ; theta += 2*K_PI / CIRCLE_ACCURACY ) {
				glVertex3f(	mCenter.mX + cos( theta ) * mRadius, 
					mCenter.mY + sin( theta ) * mRadius,
					mCenter.mZ );
			}
		glEnd();
	}

}
