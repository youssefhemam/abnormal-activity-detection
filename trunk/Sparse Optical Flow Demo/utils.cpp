#include "StdAfx.h"
#include <iostream>
#include "utils.h"
#include "ComponentFeature.h"
#include "cv.h"
#include "math.h"

using namespace std;
using namespace cv;
utils::utils(void)
{
}

utils::~utils(void)
{
}
static const double pi = 3.14159265358979323846;
//计算两组16*16区域中的特征之间的欧氏距离
double utils ::Eu_distance(ComponentFeature c1, ComponentFeature c2)
{
	double result=0;
	for (int i=0;i<8;i++)
	{
		result+=pow((double)c1.angle[i]-(double)c2.angle[i],2);
	}
	result+=pow(c1.speed-c2.speed,2);
	return result;
}
 double utils::square(int a)
{
	return a * a;
}
//按需要为图像分配内存
 void utils::allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	return;

	*img = cvCreateImage( size, depth, channels );
	if ( *img == NULL )
	{
		fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
		exit(-1);
	}
}
 int utils::angleRegion( double alpha_angle)
{
	double beta_angle=alpha_angle*180/pi;
	if(beta_angle>=0&&beta_angle<45) return 0;
	if(beta_angle>=45&&beta_angle<90) return 1;
	if(beta_angle>=90&&beta_angle<135) return 2;
	if(beta_angle>=135&&beta_angle<180) return 3;
	if(beta_angle>=180&&beta_angle<225) return 4;
	if(beta_angle>=225&&beta_angle<270) return 5;
	if(beta_angle>=270&&beta_angle<315) return 6;
	if(beta_angle>=315&&beta_angle<360) return 7;

}