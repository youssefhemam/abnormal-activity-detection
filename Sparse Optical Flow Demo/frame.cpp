#include "StdAfx.h"
#include "frame.h"
#include <iostream>
#include <fstream>
#include "local.h"
#include "Vector.h"
#define DEBUG
using namespace std;

Frame::Frame(void)
{
	width=8;
	height=8;
	
	sa.reserve(width*height);
	sub=new int*[24];
	for (int i=0;i<24;i++)
	{
		sub[i]=new int[32];
	}
	label_result=cvCreateMat(height,width,CV_32FC1);
	

   
}

Frame::Frame( int s, int w,int h ):sequence(s),width(w),height(h)
{
	sa.reserve(width*height);
	
	sub=new int*[24];
	for (int i=0;i<24;i++)
	{
		sub[i]=new int[32];
	}
	label_result=cvCreateMat(height,width,CV_32FC1);
	
}

Frame::~Frame(void)
{
	
	for (int i=0;i<24;i++)
	{
           delete[] sub[i];
	}
	delete[] sub;

	vector<local*>().swap(sa);
	cvReleaseMat(&label_result);
	

}
/*
 *
 *参数为 Frame对象，起始点（相对于一帧划分后的24*32的（0,0）），localregion的行数，localregion的列数（3*4），行步长，列步长
 *返回值：存储在vector里面的64个local对象
 *
 **/

void  Frame:: frame2locals(CvPoint &startPoint,int row,int col,int width_step,int height_step)
{



	
	//vector<local*> *locals=new vector<local*>;
	//vector<local*> locals;
	//locals.reserve(64);
	for(int m=0;m<height_step;m++)
	{

		for (int n=0;n<width_step;n++)
		{
			
			startPoint.x=m*row;
			startPoint.y=n*col;
			local *l=new local;
			//local l;// 注意这个地方有问题。temp的实际地址空间在栈中， 函数结束后， temp就将被释放，这个时候vector中的内存指向就是非法。


			for (int i=0;i<row;i++)
			{
				for (int j=0;j<col;j++)
				{
					
					//cvmSet(l->m_subregion,i,j,static_cast<double>(f.sub[i+startPoint.x][j+startPoint.y]));
					l->m_subregion->data.fl[i*l->m_subregion->cols+j]=static_cast<float>(this->sub[i+startPoint.x][j+startPoint.y]);
					

				}
				
			}
			l->setLocal(cvPoint(m,n));
		    l->generateHistogram();
            
			sa.push_back(l);
			//delete l;
			//result[k++]=l;
		}
	}


	//return locals;

}
local* Frame::getLocal(int i, int j)
{
   return sa[i*width+j];
}

void Frame::setWH( int w ,int h)
{
   width=w;
   height=h;
}


