#include "StdAfx.h"
#include "Pixel.h"
#include "cv.h"
#include "highgui.h"
using namespace cv;
using namespace std;

Pixel::Pixel(int lo,double sp,CvPoint p) :local(lo),speed(sp),point(p)
{
}

Pixel::~Pixel(void)
{
}
int Pixel:: getAngle()
{
	return local;
}
double Pixel::getSpeed()
{
	return speed;
}
