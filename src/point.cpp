// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "point.h"

// default const.
point::point()
{
	x_val = y_val = 0;
}

point::point(double xx, double yy)
{
	x_val = xx;
	y_val = yy;
}

// set...
void point::set(double xx, double yy)
{
	x_val = xx;
	y_val = yy;
}

// set y only
void point::sety(double yy)
{
	y_val = yy;
}

// set x only
void point::setx(double xx)
{
	x_val = xx;
}

// get y value of point.
double point::y()
{
	return y_val;
}

// get x value of point.
double point::x()
{
	return x_val;
}

// assignment overload
point point::operator=(point p)
{
	x_val = p.x();
	y_val = p.y();
	return *this;
}

bool inRange(point p, point rmin, point rmax)
{
	return (p.x() >= rmin.x() && p.y() >= rmin.y() && p.x() <= rmax.x() && p.y() <= rmax.y());
}

bool isAt(point a, point b)
{
	return (a.x() == b.x() && a.y() == b.y());
}

int distanceLongestAxis(point a, point b)
{
	point dab = point(std::abs(a.y() - b.y()), std::abs(a.x()-b.x()));
	return (dab.y() > dab.x()) ? dab.y() : dab.x();
}

point addPoints(point a, point b)
{
	return point(a.x() + b.x(),a.y() + b.y());
}

point multPoints(point a, point b)
{
	return point(a.x() * b.x(),a.y() * b.y());
}

point getMidpoint(point a, point b)
{
    return point((a.x()+b.x())/2.0,(a.y()+b.y())/2.0);
}

double distanceSquared(point a, point b)
{
    point delta = point(b.x() - a.x(),b.y() - a.y());
    return (delta.x() * delta.x()) + (delta.y() * delta.y());
}
