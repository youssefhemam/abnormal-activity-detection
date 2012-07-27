#include "StdAfx.h"
#include "Opticalflow.h"
#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "utils.h"
#include "VarFlow.h"
using namespace std;
using namespace cv;
Opticalflow::Opticalflow(void)
{
}

Opticalflow::~Opticalflow(void)
{
}
static const double pi = 3.14159265358979323846;
static const int MAX_FEATURE_NUMBERS=400;
//cvGoodFeaturesToTrack
// The first specifies the minimum quality of the features (based on the eigenvalues).
// The second specifies the minimum Euclidean distance between features.
static const double MINIMUM_QUALITY=0.01;
static const double MINIMUM_EUCLIDEAN_DISTANCE=0.01;
//This is the window size to use to avoid the aperture problem
static const int WINDOWS_SIZE=1;
//maximum number of pyramids
static const int MAXIMUM_NUMBER_OF_PYRAMIDS=5;
//光流法，使用金字塔LK算法，稀疏光流
void  Opticalflow:: calOpticalFlow(IplImage* frame1,IplImage* frame1_1C,IplImage* frame2_1C,int flag,int width_step)
{
   static IplImage   *eig_image = NULL, *temp_image = NULL, *pyramid1 = NULL, *pyramid2 = NULL;
   CvSize frame_size=cvSize(frame1_1C->width,frame1_1C->height);
   /* Shi and Tomasi Feature Tracking! */
   //一种角点检测算法

   /* Preparation: Allocate the necessary storage. */
   utils::allocateOnDemand( &eig_image, frame_size, IPL_DEPTH_32F, 1 );
   utils::allocateOnDemand( &temp_image, frame_size, IPL_DEPTH_32F, 1 );
   /* Preparation: This array will contain the features found in frame 1. */
		//存储frame1中的400个特征点
		CvPoint2D32f frame1_features[MAX_FEATURE_NUMBERS];

		/* Preparation: BEFORE the function call this variable is the array size
		 * (or the maximum number of features to find).  AFTER the function call
		 * this variable is the number of features actually found.
		 */
		int number_of_features;
		
		/* I'm hardcoding this at 400.  But you should make this a #define so that you can
		 * change the number of features you use for an accuracy/speed tradeoff analysis.
		 */
		number_of_features = MAX_FEATURE_NUMBERS;
		/* Actually run the Shi and Tomasi algorithm!!
		 * "frame1_1C" is the input image.
		 * "eig_image" and "temp_image" are just workspace for the algorithm.
		 * The first ".01" specifies the minimum quality of the features (based on the eigenvalues).
		 * The second ".01" specifies the minimum Euclidean distance between features.
		 * "NULL" means use the entire input image.  You could point to a part of the image.
		 * WHEN THE ALGORITHM RETURNS:
		 * "frame1_features" will contain the feature points.
		 * "number_of_features" will be set to a value <= 400 indicating the number of feature points found.
		 */
		//主要是获取frame1中的特征点，存储在frame1_features中，和特征点的个数，在number_of_features中
		cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image, frame1_features, &number_of_features, MINIMUM_QUALITY, MINIMUM_EUCLIDEAN_DISTANCE, NULL);

		/* Pyramidal Lucas Kanade Optical Flow! */

		/* This array will contain the locations of the points from frame 1 in frame 2. */
		//在frame2中存储frame1的点位置
		
		CvPoint2D32f frame2_features[MAX_FEATURE_NUMBERS];

		/* The i-th element of this array will be non-zero if and only if the i-th feature of
		 * frame 1 was found in frame 2.
		 */
         //光流特征点
		char optical_flow_found_feature[MAX_FEATURE_NUMBERS];

		/* The i-th element of this array is the error in the optical flow for the i-th feature
		 * of frame1 as found in frame 2.  If the i-th feature was not found (see the array above)
		 * I think the i-th entry in this array is undefined.
		 */
		//光流坏点
		float optical_flow_feature_error[MAX_FEATURE_NUMBERS];

		/* This is the window size to use to avoid the aperture problem (see slide "Optical Flow: Overview"). */
		//避免空洞用的窗口大小
		CvSize optical_flow_window = cvSize(WINDOWS_SIZE,WINDOWS_SIZE);
		
		/* This termination criteria tells the algorithm to stop when it has either done 20 iterations or when
		 * epsilon is better than .3.  You can play with these parameters for speed vs. accuracy but these values
		 * work pretty well in many situations.
		 */
		//终止条件
		CvTermCriteria optical_flow_termination_criteria
			= cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

		/* This is some workspace for the algorithm.
		 * (The algorithm actually carves the image into pyramids of different resolutions.)
		 */
		utils::allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );
		utils::allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );

		/* Actually run Pyramidal Lucas Kanade Optical Flow!!
		 * "frame1_1C" is the first frame with the known features.
		 * "frame2_1C" is the second frame where we want to find the first frame's features.
		 * "pyramid1" and "pyramid2" are workspace for the algorithm.
		 * "frame1_features" are the features from the first frame.
		 * "frame2_features" is the (outputted) locations of those features in the second frame.
		 * "number_of_features" is the number of features in the frame1_features array.
		 * "optical_flow_window" is the size of the window to use to avoid the aperture problem.
		 * "5" is the maximum number of pyramids to use.  0 would be just one level.
		 * "optical_flow_found_feature" is as described above (non-zero iff feature found by the flow).
		 * "optical_flow_feature_error" is as described above (error in the flow for this feature).
		 * "optical_flow_termination_criteria" is as described above (how long the algorithm should look).
		 * "0" means disable enhancements.  (For example, the second array isn't pre-initialized with guesses.)
		 */
		cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, frame1_features, frame2_features, number_of_features, optical_flow_window, MAXIMUM_NUMBER_OF_PYRAMIDS, 
			optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );
		/* For fun (and debugging :)), let's draw the flow field. */
		//开始画线咯  :-)
		for(int i = 0; i < number_of_features; i++)
		{
			/* If Pyramidal Lucas Kanade didn't really find the feature, skip it. */
			if ( optical_flow_found_feature[i] == 0 )	continue;

			int line_thickness;				line_thickness = 1;
			/* CV_RGB(red, green, blue) is the red, green, and blue components
			 * of the color you want, each out of 255.
			 */	
			CvScalar line_color;			line_color = CV_RGB(255,0,0);
	
			/* Let's make the flow field look nice with arrows. */

			/* The arrows will be a bit too short for a nice visualization because of the high framerate
			 * (ie: there's not much motion between the frames).  So let's lengthen them by a factor of 3.
			 */
			CvPoint p,q;
			p.x = (int) frame1_features[i].x;
			p.y = (int) frame1_features[i].y;
			q.x = (int) frame2_features[i].x;
			q.y = (int) frame2_features[i].y;
		//	cout<<"a:"<<p.x<<","<<p.y<<";"<<q.x<<","<<q.y<<endl;

			double angle;		angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
			double hypotenuse;	hypotenuse = sqrt( utils::square(p.y - q.y) + utils::square(p.x - q.x) );
			//angle_f<<"当前帧："<<current_frame<<"p：("<<p.x<<","<<p.y<<"),q：("<<q.x<<","<<q.y<<")角度："<<angle<<" "<<"长度："<<hypotenuse<<endl;
		
			/* Here we lengthen the arrow by a factor of three. */
			q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
			q.y = (int) (p.y - 3 * hypotenuse * sin(angle));
           // cout<<"b:"<<p.x<<","<<p.y<<";"<<q.x<<","<<q.y<<endl;
			/* Now we draw the main line of the arrow. */
			/* "frame1" is the frame to draw on.
			 * "p" is the point where the line begins.
			 * "q" is the point where the line stops.
			 * "CV_AA" means antialiased drawing.
			 * "0" means no fractional bits in the center cooridinate or radius.
			 */
			int x_factor=flag%width_step;
			int y_factor=flag/width_step;
			p.x=frame_size.width*x_factor+p.x;
			p.y=frame_size.height*y_factor+p.y;
			q.x=frame_size.width*x_factor+q.x;
			q.y=frame_size.height*y_factor+q.y;
			//cout<<"c:"<<p.x<<","<<p.y<<";"<<q.x<<","<<q.y<<endl;
		//	cvWaitKey(0);
			//cvSet2D(frame1,);
			//下面的画线部分
			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
			/* Now draw the tips of the arrow.  I do some scaling so that the
			 * tips look proportional to the main line of the arrow.
			 */		
			
			p.x = (int) (q.x + 9 * cos(angle + pi / 4));
			p.y = (int) (q.y + 9 * sin(angle + pi / 4));
			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );
			p.x = (int) (q.x + 9 * cos(angle - pi / 4));
			p.y = (int) (q.y + 9 * sin(angle - pi / 4));
			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );

		}
		/* Now display the image we drew on.  Recall that "Optical Flow" is the name of
		 * the window we created above.
		 */
		//cvShowImage("Optical Flow", frame1);
		//cvWaitKey(1);



}
void Opticalflow:: calOpticalFlowLK(IplImage* image, IplImage* prev_grey,IplImage* grey,int flag,int width_step,vector<ComponentFeature> & components)
{
 CvSize frame_size=cvSize(grey->width,grey->height);
 IplImage *velx = cvCreateImage( frame_size,IPL_DEPTH_32F, 1 );
 IplImage *vely = cvCreateImage( frame_size,IPL_DEPTH_32F, 1 );
 IplImage *abs_img=cvCreateImage(frame_size,IPL_DEPTH_8U,1);
 velx->origin =  vely->origin = grey->origin;
 CvSize winSize = cvSize(1	,1);//这个可以调整
 cvCalcOpticalFlowLK( prev_grey, grey, winSize, velx, vely );
 //cvShowImage("velx",velx);
 //cvShowImage("vely",vely);
 cvAbsDiff( grey,prev_grey, abs_img );
// cvShowImage("abs_img",abs_img);
// cvWaitKey(0);
 cvThreshold( abs_img, abs_img, 29, 255, CV_THRESH_BINARY); 
 //cvShowImage("abs_img_t",abs_img);
 //cvWaitKey(0);
 CvScalar xc,yc; 
 const int m=3,n=4;//划分成6*4个子区域

 ComponentFeature component[m][n]={};
int c=0;
 //vector<Pixel> pixels;
 //一行一行扫描velx的像素
 for(int y =0 ;y<velx->height; y++)//行
 {
  for(int x =0;x<velx->width;x++ )//列
  {
   xc = cvGetAt(velx,y,x);
   yc = cvGetAt(vely,y,x);

   
   float x_shift= (float)xc.val[0]; 
   float y_shift= (float)yc.val[0]; 
   const int winsize=1;  //计算光流的窗口大小


   if((x%(winsize*2)==0) && (y%(winsize*2)==0) ) 
   
   {

       
    
    if(x_shift!=0 || y_shift!=0)//为0代表速度和方向都没变化
    {
     
     if(x>winsize && y>winsize && x <(velx->width-winsize) && y<(velx->height-winsize) )//去掉图像最外面的winsize圈
     {
		  c++;
    
      cvSetImageROI( velx, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));

      CvScalar total_x = cvSum(velx);
      
      float xx = (float)total_x.val[0];
	 // cout<<"xx:"<<xx<<ends;
      cvResetImageROI(velx);
      //ROI区域为以像素（x,y）为中心的、边长为2*winsize的正方形
      cvSetImageROI( vely, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));
      CvScalar total_y = cvSum(vely);
      float yy = (float)total_y.val[0];
	//  cout<<"yy:"<<yy<<ends;
      cvResetImageROI(vely);
      
      cvSetImageROI( abs_img, cvRect( x-winsize, y-winsize, 2*winsize, 2*winsize));
      CvScalar total_speed = cvSum(abs_img);
      float ss = (float)total_speed.val[0]/(4*winsize*winsize)/255;
	 // cout<<"ss:"<<ss<<ends;
    //  cout<<"total_speed:"<<total_speed.val[0]<<endl;
      cvResetImageROI(abs_img);
   
      const double ZERO = 0.000001;
      //const double pi = 3.1415926;
      double alpha_angle;//弧度
	 
      
      if(xx<ZERO && xx>-ZERO)
       alpha_angle = pi/2;
      else
       alpha_angle = abs(atan(yy/xx));
      //调整角度
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
	  component[y/(frame_size.height/m)][x/(frame_size.width/n)].angle[utils::angleRegion(alpha_angle)]++;
      component[y/(frame_size.height/m)][x/(frame_size.width/n)].speed+=ss;
	//  cout<<"a:"<<pt1.x<<","<<pt1.y<<";"<<pt2.x<<","<<pt2.y<<endl;
	 // Pixel pixel(angleRegion(alpha_angle),ss*100,pt1);
	 // pixels.push_back(pixel);
	 
	  int x_factor=flag%width_step;
	  int y_factor=flag/width_step;
	  pt1.x=frame_size.width*x_factor+pt1.x;
	  pt1.y=frame_size.height*y_factor+pt1.y;
	  pt2.x=frame_size.width*x_factor+pt2.x;
	  pt2.y=frame_size.height*y_factor+pt2.y;
	//   cout<<"b:("<<pt1.x<<","<<pt1.y<<");("<<pt2.x<<","<<pt2.y<<")"<<endl;

    
      cvLine( image, pt1, pt2 , line_color, 1, CV_AA, 0 );
      CvPoint p;
      p.x = (int) (pt2.x + 6 * cos(alpha_angle - pi / 4*3));
      p.y = (int) (pt2.y + 6 * sin(alpha_angle - pi / 4*3));
      cvLine( image, p, pt2, line_color, 1, CV_AA, 0 );
      p.x = (int) (pt2.x + 6 * cos(alpha_angle + pi / 4*3));
      p.y = (int) (pt2.y + 6 * sin(alpha_angle + pi / 4*3));
      cvLine( image, p, pt2, line_color, 1, CV_AA, 0 );
       
	   //cvWaitKey(0);
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
 }
 //total_pixels.push_back(pixels);
 for (int i=0;i<m;i++)
 {
	 for (int j=0;j<n;j++)
	 {
         components.push_back(component[i][j]);
	 }
 }
 

 //iter[4][3];
 // cout<<c<<endl;
   //cvWaitKey(0);
//cvShowImage("Optical Flow", image);   

 //cvShowImage( "Contour_abs_img", abs_img);
 //cvShowImage( "Contour2_velx", velx);
 //cvShowImage( "Contour2_vely", vely);
/* for (int i=0;i<m;i++)
 {
	 delete[]component[i];
 }
 delete component;*/

 cvReleaseImage(&velx);
 cvReleaseImage(&vely);
 cvReleaseImage(&abs_img);
 //cvWaitKey(1);
 


}

void Opticalflow::calOpticalFlowVar( IplImage* image, IplImage* prev_grey,IplImage* grey,int flag,int width_step,vector<ComponentFeature> & components )
{
   CvSize frame_size=cvSize(grey->width,grey->height);
   IplImage* imgU = cvCreateImage(frame_size, IPL_DEPTH_32F, 1);
   IplImage* imgV = cvCreateImage(frame_size, IPL_DEPTH_32F, 1);
   int max_level = 4;
   int start_level = 0;
   int n1 = 2;
   int n2 = 2;
   float rho = 2.8;
   float alpha = 1400;
   float sigma = 1.5;

   const int m=3,n=4;//划分成6*4个子区域
   ComponentFeature component[m][n]={};
   
   VarFlow OpticalFlow(frame_size.width, frame_size.height, max_level, start_level, n1, n2, rho, alpha, sigma);
   cvZero(imgU);
   cvZero(imgV);
    OpticalFlow.CalcFlow(prev_grey, grey, imgU, imgV, 0);
	//int x,y;
	
	 int xSpace=1,  ySpace=1;
	 float cutoff=1; //边界值
	 int multiplier=1; 
	 CvScalar color=CV_RGB(255,0,0);
	 CvPoint p0=cvPoint(0,0);
	 CvPoint p1 = cvPoint(0,0);

	 float deltaX, deltaY, angle, hyp;
	 for(int y = ySpace; y < imgU->height; y+= ySpace ) {
		 for(int x = xSpace; x < imgU->width; x+= xSpace ){

			 p0.x = x;
			 p0.y = y;

			 deltaX = *((float*)(imgU->imageData + y*imgU->widthStep)+x);
			 deltaY = -(*((float*)(imgV->imageData + y*imgV->widthStep)+x));
			 const double ZERO = 0.000001;
			 


			 if(deltaX<ZERO && deltaX>-ZERO)
				 angle = pi/2;
			 else
				 angle = abs(atan(deltaY/deltaX));
			 //调整角度
			 if(deltaX<0 && deltaY>0) angle = pi - angle ;
			 if(deltaX<0 && deltaY<0) angle = pi + angle ;
			 if(deltaX>0 && deltaY<0) angle = 2*pi - angle ;
			// angle = atan2(deltaY, deltaX);
			 hyp = sqrt(deltaX*deltaX + deltaY*deltaY);
             

			 if(hyp > cutoff){

				 component[y/(frame_size.height/m)][x/frame_size.width/n].angle[utils::angleRegion(angle)]++;
				 component[y/(frame_size.height/m)][x/frame_size.width/n].speed+=hyp;
				 
				 
				 p1.x = p0.x + cvRound(multiplier*hyp*cos(angle));
				 p1.y = p0.y + cvRound(multiplier*hyp*sin(angle));
				 int x_factor=flag%width_step;
				 int y_factor=flag/width_step;
				 p0.x=frame_size.width*x_factor+p0.x;
				 p0.y=frame_size.height*y_factor+p0.y;
				 p1.x=frame_size.width*x_factor+p1.x;
				 p1.y=frame_size.height*y_factor+p1.y;

				 //cvLine( imgMotion, p0, p1, color,1, CV_AA, 0);
				 cvLine (image,p0,p1,color,1,CV_AA,0);

				 p0.x = p1.x + cvRound(3*cos(angle-pi + pi/4));
				 p0.y = p1.y + cvRound(3*sin(angle-pi + pi/4));
				 cvLine( image, p0, p1, color,1, CV_AA, 0);

				 p0.x = p1.x + cvRound(3*cos(angle-pi - pi/4));
				 p0.y = p1.y + cvRound(3*sin(angle-pi - pi/4));
				 cvLine( image, p0, p1, color,1, CV_AA, 0);
			 }

		 }
	 }
	 for (int i=0;i<m;i++)
	 {
		 for (int j=0;j<n;j++)
		 {
			 components.push_back(component[i][j]);
		 }
	 }
	 cvReleaseImage(&imgU);
	 cvReleaseImage(&imgV);
}

void drawMotionField(IplImage* imgU, IplImage* imgV,  IplImage * frame ,int xSpace, int ySpace, float cutoff, int multiplier, CvScalar color)
{
	int x, y;

	CvPoint p0 = cvPoint(0,0);
	CvPoint p1 = cvPoint(0,0);

	float deltaX, deltaY, angle, hyp;

	for(y = ySpace; y < imgU->height; y+= ySpace ) {
		for(x = xSpace; x < imgU->width; x+= xSpace ){

			p0.x = x;
			p0.y = y;

			deltaX = *((float*)(imgU->imageData + y*imgU->widthStep)+x);
			deltaY = -(*((float*)(imgV->imageData + y*imgV->widthStep)+x));

			angle = atan2(deltaY, deltaX);
			hyp = sqrt(deltaX*deltaX + deltaY*deltaY);

			if(hyp > cutoff){

				p1.x = p0.x + cvRound(multiplier*hyp*cos(angle));
				p1.y = p0.y + cvRound(multiplier*hyp*sin(angle));

				//  cvLine( imgMotion, p0, p1, color,1, CV_AA, 0);
				cvLine (frame,p0,p1,color,1,CV_AA,0);

				p0.x = p1.x + cvRound(3*cos(angle-pi + pi/4));
				p0.y = p1.y + cvRound(3*sin(angle-pi + pi/4));
				cvLine( frame, p0, p1, color,1, CV_AA, 0);

				p0.x = p1.x + cvRound(3*cos(angle-pi - pi/4));
				p0.y = p1.y + cvRound(3*sin(angle-pi - pi/4));
				cvLine( frame, p0, p1, color,1, CV_AA, 0);
			}

		}
	}

}