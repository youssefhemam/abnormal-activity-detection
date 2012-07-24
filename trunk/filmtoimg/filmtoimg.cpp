// filmtoimg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
using namespace std;
using namespace cv;


int _tmain(int argc, _TCHAR* argv[])
{
	const string filename="D:\\amit\\mall_1.AVI";
	CvCapture *capture=cvCaptureFromFile(filename.c_str());
	if (capture == NULL)
	{

		fprintf(stderr, "Error: Can't open video.\n");
		return -1;
	}
	static long code=1000000;
	
	static IplImage* pFrame=NULL;
	int k=0;
	cvQueryFrame( capture);
	long number_of_frames;
	number_of_frames=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
	cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, 0);
	while(true)
	{
      k++;
	  string end=".jpg";
	  char framename[7];
	  itoa(code+k,framename,10);
	  string fold="result\\";
	  string path=fold+framename+end;
	  pFrame = cvQueryFrame( capture );
	  if (pFrame == NULL)
	  {
		  /* Why did we get a NULL frame?  We shouldn't be at the end. */
		  fprintf(stderr, "Error: Hmm. The end came sooner than we thought.\n");
		  return -1;
	  }
	  cvSaveImage(path.c_str(),pFrame);

	}
	cvReleaseCapture(&capture);
	return 0;
}

