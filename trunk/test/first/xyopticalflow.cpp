#include "stdafx.h"
#include <iostream>
#include "cv.h"
#include "highgui.h"
using namespace cv;
using namespace std;
inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	return;

	*img = cvCreateImage( size, depth, channels );
	if ( *img == NULL )
	{
		fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
		exit(-1);
	}
}

void ImgOpticalFlow(IplImage *prev_grey,IplImage *grey)

{
IplImage *velx = cvCreateImage( cvSize(grey->width ,grey->height),IPL_DEPTH_32F, 1 );
IplImage *vely = cvCreateImage( cvSize(grey->width ,grey->height),IPL_DEPTH_32F, 1 );
IplImage *abs_img = cvCreateImage( cvSize(grey->width ,grey->height),IPL_DEPTH_32F, 1 );
velx->origin =   vely->origin = grey->origin;
CvSize winSize = cvSize(5,5);
cvCalcOpticalFlowLK( prev_grey, grey, winSize, velx, vely );

cvAbsDiff( grey,prev_grey, abs_img );
cvThreshold( abs_img, abs_img, 29, 255, CV_THRESH_BINARY); 
CvScalar xc,yc; 
for(int y =0 ;y<velx->height; y++)
   for(int x =0;x<velx->width;x++ )
   {
xc = cvGetAt(velx,y,x);
yc = cvGetAt(vely,y,x);

float x_shift= (float)xc.val[0]; 
float y_shift= (float)yc.val[0]; 
const int winsize=5;   //计算光流的窗口大小

if((x%(winsize*2)==0) && (y%(winsize*2)==0) ) 
{
if(x_shift!=0 || y_shift!=0)
{
   
    if(x>winsize && y>winsize && x <(velx->width-winsize) && y<(velx->height-winsize) )
    {
   cvSetImageROI( velx, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));
   CvScalar total_x = cvSum(velx);
   float xx = (float)total_x.val[0];
   cvResetImageROI(velx);
   cvSetImageROI( vely, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));
   CvScalar total_y = cvSum(vely);
   float yy = (float)total_y.val[0];
   cvResetImageROI(vely);
   
   cvSetImageROI( abs_img, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));
   CvScalar total_speed = cvSum(abs_img);
   float ss = (float)total_speed.val[0]/(4*winsize*winsize)/255;
   cvResetImageROI(abs_img);
   const double ZERO = 0.000001;
   const double pi = 3.1415926;
   double alpha_angle;
   if(xx<ZERO && xx>-ZERO)
   alpha_angle = pi/2;
   else
   alpha_angle = abs(atan(yy/xx));
   
   if(xx<0 && yy>0) alpha_angle = pi - alpha_angle ;
   if(xx<0 && yy<0) alpha_angle = pi + alpha_angle ;
   if(xx>0 && yy<0) alpha_angle = 2*pi - alpha_angle ;

   
   CvScalar line_color;
   float scale_factor = ss*100;
   line_color = CV_RGB(255,0,0);
   CvPoint pt1,pt2;
   pt1.x = x; 
   pt1.y = y;
   pt2.x = static_cast<int>(x + scale_factor*cos(alpha_angle));
   pt2.y = static_cast<int>(y + scale_factor*sin(alpha_angle));
   cvLine( grey, pt1, pt2 , line_color, 1, CV_AA, 0 );
   CvPoint p;
   p.x = (int) (pt2.x + 6 * cos(alpha_angle - pi / 4*3));
   p.y = (int) (pt2.y + 6 * sin(alpha_angle - pi / 4*3));
   cvLine( grey, p, pt2, line_color, 1, CV_AA, 0 );
   p.x = (int) (pt2.x + 6 * cos(alpha_angle + pi / 4*3));
   p.y = (int) (pt2.y + 6 * sin(alpha_angle + pi / 4*3));
   cvLine( grey, p, pt2, line_color, 1, CV_AA, 0 );
   /*
   line_color = CV_RGB(255,255,0);
   pt1.x = x-winsize;
   pt1.y = y-winsize;
   pt2.x = x+winsize;
   pt2.y = y+winsize;
   cvRectangle(image, pt1,pt2,line_color,1,CV_AA,0);
   */
    }
}
}
   }


cvShowImage( "Contour", abs_img);
cvShowImage( "Contour2", vely);
cvReleaseImage(&velx);
cvReleaseImage(&vely);
cvWaitKey(0);
}

int mainc()
{

	const int winsize=5;  //计算光流的窗口大小
	CvCapture* capture = 0;
	capture = cvCaptureFromFile( "E:\\bysj\\test\\example2\\Hu\\wetherman\\00\\denis_jump.avi"); 
	IplImage *frame0=0;
	frame0=cvQueryFrame(capture);
	int total_frame_num=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
    int nFrmNum=0;
	IplImage * preFrame =NULL;
	while((preFrame=cvQueryFrame(capture))/*&&(nFrmNum!=(total_frame_num-1))*/)
	{
		nFrmNum++;
		IplImage * curFrame=NULL;
        int width=preFrame->width;
		int height=preFrame->height;
		curFrame=cvQueryFrame(capture);
		cvNamedWindow("a");
		cvShowImage("a",curFrame);
        cvReleaseImage(&curFrame);
        


	}
	cvReleaseImage(&preFrame);
	cvReleaseCapture(&capture);
	return 0;

    
	}