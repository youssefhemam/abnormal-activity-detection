#pragma once
#include "ComponentFeature.h"
#include "cv.h"
using namespace std;
using namespace cv;
class utils
{
public:
	utils(void);
	~utils(void);
	static double Eu_distance(ComponentFeature c1, ComponentFeature c2);
	 static double square(int a);
	 static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels );
	 static int angleRegion( double alpha_angle);
private:
	
	
};
