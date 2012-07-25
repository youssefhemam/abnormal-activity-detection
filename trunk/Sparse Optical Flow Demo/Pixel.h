#pragma once
#include "cv.h"
#include "highgui.h"
using namespace cv;
using namespace std;
class Pixel
{
public:
	Pixel(int lo,double sp,CvPoint p);
	~Pixel(void);
    int getAngle();
	double getSpeed();
	CvPoint getPoint();
private:
	int local;
	double speed;
	CvPoint point;
};
