#pragma once
#include "cv.h"
#include "Vector.h"
using namespace std;
using namespace cv;
class local
{
public:
	local(void);
	
	local(int x,int y);
    void test();
	~local(void);
	int isError();
    local(const local&);
	void setLocal(CvPoint & p);
	void generateHistogram();
	//int l[6][4];
	CvMat * m_subregion;
    //int **l;
	onlinesvr::Vector<double> * activitydescriptor;
	CvHistogram* hist;
private:
	CvPoint point;
	
	
};
