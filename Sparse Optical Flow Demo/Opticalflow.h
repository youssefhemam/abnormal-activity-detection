#pragma once
#include <iostream>
#include "utils.h"
#include "cv.h"
#include "highgui.h"
using namespace std;
using namespace cv;
class Opticalflow
{
public:
	Opticalflow(void);
	~Opticalflow(void);
	void calOpticalFlow(IplImage* frame1,IplImage* frame1_1C,IplImage* frame2_1C,int flag,int width_step);
	void calOpticalFlowLK(IplImage* image, IplImage* prev_grey,IplImage* grey,int flag,int width_step,vector<ComponentFeature> & components);
    void calOpticalFlowVar(IplImage* image, IplImage* prev_grey,IplImage* grey,int flag,int width_step,vector<ComponentFeature> & components);
private:
	

};
