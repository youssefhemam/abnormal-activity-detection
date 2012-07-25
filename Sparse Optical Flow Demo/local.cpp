#include "StdAfx.h"
#include "local.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <fstream>
//#define DEBUG
using namespace std;
using namespace cv;
static int start=0;
static int copys=0;
static int end=0;
static int locals=0;
local::local(const local& l)
{
  
}
local::local(void)
{
	

	activitydescriptor=new onlinesvr::Vector<double>;
  
	m_subregion=cvCreateMat(3,4,CV_32FC1);
	//cout<<"aaaa"<<start++<<endl;
}

local::~local(void)
{
	if (m_subregion!=NULL)
	{
		cvReleaseMat(&m_subregion);
	}
	
	if (activitydescriptor!=NULL)
	{
        delete activitydescriptor;
	}
        
		
		
		//cout<<"bbbb"<<end++<<endl;
	
	
}
local::local(int x,int y)
{
	point.x=x;
	point.y=y;
}
int local::isError()
{
    return 0;
}
void local::test()
{

}
void local::setLocal(CvPoint & s)
{
	point=s;
}

void local::generateHistogram()
{
	int HistogramBins = 11;

	int HistogramBinWidth; 

	float HistogramRange1[2]={1,11};

	float *HistogramRange[1]={&HistogramRange1[0]};
	
 
   CvHistogram* hist;

	hist = cvCreateHist(1,&HistogramBins,CV_HIST_ARRAY,HistogramRange);


	HistogramBinWidth=275/HistogramBins;
    #ifdef DEBUG
	IplImage *HistogramImage1;

	CvPoint Point1;

	CvPoint Point2;

	HistogramImage1 = cvCreateImage(cvSize(250,250),8,3);

	cvSetZero(HistogramImage1);

	HistogramImage1->origin=1;
	ofstream ma_out("matrix.txt",ios::app);
	ma_out<<"##################local"<<copys++<<"#############"<<endl;

	for (int i=0;i<3;i++)
	{
		for(int j=0;j<4;j++)
		{
		 ma_out<< m_subregion->data.fl[i*m_subregion->cols+j]<<"\t";
		}
		ma_out<<endl;
	}
	ma_out<<endl;
		ma_out.close();
		#endif

	
    
	cvCalcArrHist((CvArr**)(&m_subregion),hist);
	
    #ifdef DEBUG
	ofstream lo_out("histogram.txt",ios::app);
	lo_out<<"##################local"<<locals++<<"#############"<<endl;
   #endif
	for(int i=0;i<HistogramBins-1;i++)

	{

		
        #ifdef DEBUG
        lo_out<< ((CvMat *) hist->bins)->data.fl[i]<<"\t"; 
		

		Point1=cvPoint(i*HistogramBinWidth,0);

		Point2=cvPoint((i+1)*HistogramBinWidth,(int)cvQueryHistValue_1D(hist,i)*20);

		cvRectangle(HistogramImage1,Point1,Point2,CV_RGB(127*(i+1)%255,127*(i+2)%255,127*(i+3)%255));
        #endif
		activitydescriptor->Add(((CvMat *) hist->bins)->data.fl[i]);

	}
	activitydescriptor->DivideScalar(12.0f);//H(l)=p(l|t)
	if (hist!=NULL)
	{
		cvReleaseHist(&hist);
	}

   #ifdef DEBUG
	lo_out<<endl;
    lo_out.close();
	cvNamedWindow("Histogram",1);


	cvShowImage("Histogram",HistogramImage1);

	cvWaitKey(1);

	cvReleaseImage(&HistogramImage1);
   #endif
	
}

/*
const local & local::operator=( const local& copy )
{
	//cout<<"start"<<endl;
    if(this ==&copy)
	{
		//cout<<"end1"<<endl;
		return *this;
	}
	if (m_subregion!=NULL)
	{
		delete m_subregion;
		m_subregion=NULL;

	}
	if (activitydescriptor!=NULL)
	{
		delete activitydescriptor;
		activitydescriptor=NULL;
	}

	point=copy.point;
	m_subregion=cvCreateMat(3,4,CV_32FC1);
	for (int i=0;i<m_subregion->rows;i++)
	{
		for (int j=0;j<m_subregion->cols;j++)
		{
			m_subregion->data.fl[i*m_subregion->cols+j]=copy.m_subregion->data.fl[i*copy.m_subregion->cols+j];
		}
	}
	activitydescriptor=new onlinesvr::Vector<double>;
	memcpy(activitydescriptor,copy.activitydescriptor,sizeof(Vector<double>*));
	//cout<<"end"<<endl;
	return *this;

//return copy;

    
}*/
