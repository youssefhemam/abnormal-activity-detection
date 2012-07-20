
#include "stdafx.h"
/*
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include "VFeature.h"
using namespace std;
using namespace cv;
int const MAX_CORNERS = 2000;
IplImage* ROI(IplImage* img, CvRect & rect)
{   
  
 static IplImage* roi_img=cvCreateImage(cvSize(rect.width,rect.height),8,1);
  cvSetImageROI(img,rect);
  cvCopy(img,roi_img,NULL);
  cvResetImageROI(img);
  return roi_img;
}
int mainq(int argc, char **argv)
{   
	 
  
	

	CvCapture* capture=NULL ;
	capture = cvCaptureFromFile( "E:\\bysj\\test\\example2\\Hu\\wetherman\\run\\daria_run.avi");  //get frame
    int total_frame=0;

	IplImage *src_img1;  //the previous frame (gray)
	IplImage *src_img2;  //the current frame(gray)
    static IplImage * frame1=NULL;
	int dx, dy;
	
	int rows, cols;
	int countn = MAX_CORNERS;

    frame1=cvQueryFrame(capture);
    / *frame1=cvQueryFrame(capture);
	cvReleaseImage(&frame1);* /
	CvSize img_sz = cvGetSize(frame1); 
	cols = img_sz.width; 
	rows = img_sz.height; 
	total_frame=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
   // cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0. );
	CvPoint2D32f * move_old_point = new CvPoint2D32f[ MAX_CORNERS];
	CvPoint2D32f * move_new_point = new CvPoint2D32f[ MAX_CORNERS];
	char *features_found = new char[MAX_CORNERS];
	float *features_error = new float[MAX_CORNERS];
	CvTermCriteria criteria; 
	criteria = cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 64, 0.01); 
	int current_frame=0;
    static int p=0;
	while(current_frame!=(total_frame-1))
	{
		int i,j;
		p = 0;
		
		IplImage *dst_img;   //the result
		static IplImage *cur_img=NULL;   
		static IplImage *pre_img=NULL;
	
        cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, current_frame);
		
        pre_img = cvQueryFrame(capture);

	
		
		src_img1 = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
		cvCvtColor(pre_img, src_img1, CV_RGB2GRAY);
		//cvSmooth(src_img1,src_img1,CV_MEDIAN);
        
		cur_img = cvQueryFrame(capture);
		src_img2 = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
		cvCvtColor(cur_img, src_img2, CV_RGB2GRAY);
		//cvSmooth(src_img2,src_img2,CV_MEDIAN);
		dst_img = (IplImage *)cvClone(cur_img);

		IplImage *move_img = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
		cvZero(move_img);
		//cvAbsDiff(src_img1, src_img2,move_img);
		
		for (i = 0; i <cols; i++)
		{
			for (j = 0; j<rows; j++)
			{ 
				double a = abs(cvGet2D(src_img1, j, i).val[0]-cvGet2D(src_img2, j, i).val[0]);

				CvScalar b = cvScalar(a, 0, 0,0);
				cvSet2D(move_img, j, i,b);//显示轮廓
				if (a>20)
				{
					if (p<MAX_CORNERS-1)
					{
						int d = ++p;
						move_old_point[d].x = i;
						move_old_point[d].y = j;
					}

				}
			}
		}

		//cvNamedWindow("moving object", 1);
		//cvShowImage("moving object", move_img);
        //cvWaitKey(0);
		CvSize Pyrsize = cvSize(src_img1->width +8, src_img1->height/3);
		IplImage * pyrA = cvCreateImage(Pyrsize, IPL_DEPTH_32F, 1); //pyrA是需要寻找的点，不是没有初始化的
		IplImage * pyrB = cvCreateImage(Pyrsize, IPL_DEPTH_32F, 1);

		cvCalcOpticalFlowPyrLK(src_img1,
			src_img2,
			pyrA, 
			pyrB,
			move_old_point,
			move_new_point,
			countn,
			cvSize(10, 10),
			3,
			features_found,
			features_error,
			criteria,
			0
			);
		IplImage* point_img=cvCreateImage(img_sz,8,3);
		IplImage* fx_img=cvCreateImage(img_sz,8,1);
		IplImage* fy_img=cvCreateImage(img_sz,8,1);

		cvSetZero(fx_img);
		cvSetZero(fy_img);
		int x1_min=255,y1_min=255,x1_max=0,y1_max=0;
		int x2_min=255,y2_min=255,x2_max=0,y2_max=0;
		for (i = 0; i < countn; i++)
		{   
			if(features_found[i]==0||features_error[i]<0)
			{
				continue;
			}
			int x1 = (int)move_new_point[i].x;
			int x2 = (int)move_old_point[i].x;
			int y1 = (int)move_new_point[i].y;
			int y2 = (int)move_old_point[i].y;
			if (x1<0||y1<0)
			{
				continue;
			}
            if (x1<x1_min)
            {
				x1_min=x1;
            }
			if (x2<x2_min)
			{
				x2_min=x2;
			}
			if (y1<y1_min)
			{
				y1_min=y1;
			}
			if (y2<y2_min)
			{
				y2_min=y2;
			}
			if (x1>x1_max)
			{
				x1_max=x1;
			}
			if (x2>x2_max)
			{
				x2_max=x2;
			}
			if (y1>y1_max)
			{
				y1_max=y1;
			}
			if (y2>y2_max)
			{
				y2_max=y2;
			}
		 
		   

			 dx = (int)(x1 - x2);
			 dy = (int)(y1 - y2);
			//if (dx >= 5&& dy >= 5)
			
				//cvLine (dst_img, cvPoint(x1, y1),cvPoint(x2, y2) , CV_RGB (255, 0, 0), 1, CV_AA, 0);
				cvCircle(dst_img,cvPoint(x2,y2),1,CV_RGB(255,0,0),1,8,0);
				
				
					cvSet2D(point_img,y1,x1,cvScalar(255,0,0,0));
				    
				    cvSet2D(fx_img,y2,x1,cvScalar(255));
					
					cvSet2D(fy_img,y1,x2,cvScalar(255));
                    
				
				
            
				
			
               
			
		}
		CvRect fx_roi=cvRect(x1_min,y2_min,(x1_max-x1_min),(y2_max-y2_min));
		CvRect fy_roi=cvRect(x2_min,y1_min,(x2_max-x2_min),(y1_max-y1_min));
		IplImage* fx_img_roi=ROI(fx_img,fx_roi);
		IplImage* fy_img_roi=ROI(fy_img,fy_roi);
		IplImage* fx_dst=cvCreateImage(cvSize(80,80),8,1);
		IplImage* fy_dst=cvCreateImage(cvSize(80,80),8,1);
		cvNamedWindow("fx_roi",1);
        
        cvNamedWindow("fy_roi",1);
		cvShowImage("fx_roi",fx_img_roi);
		cvShowImage("fy_roi",fy_img_roi);
		CFeature::ResizeImg_fill(fx_img_roi,fx_dst,80,80);
		CFeature::ResizeImg_fill(fy_img_roi,fy_dst,80,80);
		cvNamedWindow("fx_dst",1);
		cvShowImage("fx_dst",fx_dst);
		cvNamedWindow("fy_dst",1);
		cvShowImage("fy_dst",fy_dst);

		//cvSmooth(fx_img,fx_img,CV_MEDIAN);
		//cvSmooth(fy_img,fy_img,CV_MEDIAN);
		cvNamedWindow("FX",1);
		cvShowImage("FX",fx_img);
        cvNamedWindow("FY",1);
		cvShowImage("FY",fy_img);
        cvNamedWindow("point_img",1);
		cvShowImage("point_img",point_img);
        cvWaitKey(0);
		cvNamedWindow ("ImagePyrLK", 1);  
		cvShowImage ("ImagePyrLK", dst_img); 
		//cvWaitKey (1);
		current_frame++; 
		cvReleaseImage (&dst_img);
		cvReleaseImage(&pyrA);
		cvReleaseImage(&pyrB);
		cvReleaseImage(&move_img);
		cvReleaseImage(&point_img);
		cvReleaseImage(&fx_img);
		cvReleaseImage(&fy_img);
		cvReleaseImage(&fx_img_roi);
		cvReleaseImage(&fy_img_roi);
		cvReleaseImage(&fx_dst);
		cvReleaseImage(&fy_dst);
		//cvReleaseImage (&pre_img); 
	//	cvReleaseImage (&cur_img);
       // if (current_frame==total_frame-1)
       // {
		//	break;
      //  }

	}
 
	cvDestroyWindow("moving object");
	cvDestroyWindow ("ImagePyrLK");  
	cvDestroyWindow("point_img");
	cvDestroyWindow("FX");
	cvDestroyWindow("FY");
	cvDestroyWindow("fx_roi");
	cvDestroyWindow("fy_roi");
	cvDestroyWindow("fx_dst");
	cvDestroyWindow("fy_dst");
	cvReleaseImage (&src_img1); 
	cvReleaseImage (&src_img2);
	

	
	cvReleaseCapture(&capture);
	
	

	return 0;
}*/