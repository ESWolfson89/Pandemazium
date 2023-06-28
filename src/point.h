// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef POINT_H_
#define POINT_H_

#include <cmath>

class point
{
	public:
		// default const.
 		point();
 		// copy const.
 		point(double, double);
 		// set...
		void set(double, double);
		// set y only
		void sety(double);
		// set x only
		void setx(double);
		// get y value of point.
		double y();
		// get x value of point.
		double x();
		// assignment overload
		point operator=(point);
	private:
		// member variables for x and y coord.
		double x_val,y_val;
};

bool inRange(point, point, point);

bool isAt(point, point);

int distanceLongestAxis(point, point);

point addPoints(point, point);

point multPoints(point, point);

point getMidpoint(point, point);

double distanceSquared(point,point);

#endif
