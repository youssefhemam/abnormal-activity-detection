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
 *����Ϊ Frame������ʼ�㣨�����һ֡���ֺ��24*32�ģ�0,0������localregion��������localregion��������3*4�����в������в���
 *����ֵ���洢��vector�����64��local����
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
			//local l;// ע������ط������⡣temp��ʵ�ʵ�ַ�ռ���ջ�У� ���������� temp�ͽ����ͷţ����ʱ��vector�е��ڴ�ָ����ǷǷ���


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


