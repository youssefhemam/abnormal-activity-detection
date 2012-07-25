#pragma once
#include <iostream>
#include "local.h"
#include "cv.h"
#include "Vector.h"
using namespace std;
using namespace cv;
class Frame
{
public:
	Frame(void);
	//Frame & 
	//参数为队列中的顺序，帧结构的宽，帧结构的高
	Frame(int s, int w,int h);
	~Frame(void);
	void frame2locals(CvPoint &startPoint,int row=3,int col=4,int width_step=8,int height_step=8);
	local * getLocal(int i,int j);
	void setWH(int w,int h);
	
	int ** sub;//subregion坐标
	vector<local*> sa;
	CvMat * label_result;
	
	//int ** classes;
	int width;
	int height;
private:
	int x;
	int y;
	
	int sequence;
};
