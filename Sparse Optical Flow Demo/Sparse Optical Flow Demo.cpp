#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <fstream>
#include "ProfTimer.h"
#include "ComponentFeature.h"
#include "Pixel.h"
#include "utils.h"
#include "Opticalflow.h"
#include "Kmeans.h"
#include <map>
#include "frame.h"
#include <deque>
#include <list>
#include "local.h"
#include "mrf.h"
//#define DEBUG
using namespace std;
using namespace cv;

static const double pi = 3.14159265358979323846;
/*
 *����һ��CvScalar����,val[0]Ϊ��value����Node����������(�ڼ�֡)������
 *(val[1],val[2])Ϊ����value/(32*24)��Node�ڲ���ʵ������
 *����Ϊһ���㣨��ֵ��ʾ�ĵ㣩
 *���ܣ�����ֵ��ʾ�ĵ�ת��Ϊ�����������ʽ
 **/
CvScalar valueToScalar(int value)
{
	CvScalar scalar;
	scalar.val[0]=value/(32*24);
	int frame_location=value%(32*24);
	int local_region_num=frame_location/12;//ÿ��localregion��24��subregion
	int sub_region_num=frame_location%12;
	int local_line=local_region_num/8;//һ��8��localregion
	//scalar.val[0]=local_line;//node��������
	int subregion_line=local_line*3;//ÿһ��localregion��3��subregion,18
	int local_col=local_region_num%8;
	//scalar.val[1]=local_col;//node��������
	int subregion_col=local_col*4;
	int x=sub_region_num/4;
	int y=sub_region_num%4;
	scalar.val[1]=subregion_line+x;
	scalar.val[2]=subregion_col+y;

	return scalar;


}



//����ͼ��image�е�roi
IplImage * GetSubregion(IplImage* image,CvRect roi)
{
	static IplImage * result=NULL;
	cvSetImageROI(image,roi);
	result=cvCreateImage(cvSize(roi.width,roi.height),image->depth,image->nChannels);
	cvCopy(image,result);
	cvResetImageROI(image);
	return result;
}
//��ͼ��source�ָ�����ɴ�СΪlocalregion��С����ÿһ��Ϊwidth_step����width_stepΪ���ز���������һ����������
vector<IplImage*> GetSubregions(IplImage* source,CvSize localregion_size,int & width_step)
{
	
	int n_x=source->width/localregion_size.width;//8
	width_step=n_x;
	int n_y=source->height/localregion_size.height;//8
	 vector<IplImage*> subregions;
	 subregions.reserve(n_x*n_y);
	CvPoint point=cvPoint(0,0);
	for (int i=0;i<n_y;i++)
	{
		point.x=0;
		for (int j=0;j<n_x;j++)
		{
             IplImage * subregion;
             CvRect rect=cvRect(point.x,point.y,localregion_size.width,localregion_size.height);
             subregion=GetSubregion(source,rect);
             subregions.push_back(subregion);
			 point.x+=localregion_size.width;
			


		}
		point.y+=localregion_size.height;
	}
	return subregions;
	
}

/*
void CompareHistogram(Frame &pre,Frame &cur,Frame &next)
{
	vector<local*>::iterator pre_iter=cur.sa.begin();
    for (int i=0;i<cur.sa.size();i++)
    {
		cur.sa[i]->hist;
    }
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<8;j++)
		{
			cur.sa[i*j+j]->hist;

		}
	}
	
}*/
int main(void)
{
	static long code=1000000;
	const static long start_frame=0;
	ofstream zhjg("zhjg.txt");
	ofstream logs("log.txt");
	list<vector<vector<ComponentFeature>>> video_Feature;
	CvCapture *input_video = cvCaptureFromFile(	
		"D:\\amit\\subway_exit_turnstiles.AVI"
		);
	if (input_video == NULL)
	{
		
		fprintf(stderr, "Error: Can't open video.\n");
		return -1;
	}
	CvSize frame_size;
	frame_size.height =(int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_HEIGHT );
	frame_size.width =(int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_WIDTH );

	/* Determine the number of frames in the AVI. */
	cvQueryFrame( input_video);
	long number_of_frames;
	number_of_frames=cvGetCaptureProperty(input_video,CV_CAP_PROP_FRAME_COUNT);
	cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES, 0);
    //cvNamedWindow("Optical Flow", CV_WINDOW_AUTOSIZE);
	long current_frame = start_frame;
	while(true)
	{
		static IplImage *frame = NULL, *frame1 = NULL, *frame1_1C = NULL, *frame2_1C = NULL,*frame1out=NULL;
		vector<IplImage*> frame1_1C_subregions;//����һ֡�ֳ�32���ֲ�����洢������
		vector<IplImage*> frame2_1C_subregions;//���ڶ�֡�ֳ�32���ֲ�����洢������
		int width_step=0;
		//��һ֡���ֳ�frame_width*frame_height��������
		const int frame_width=8;
		const int frame_height=8;
		char framename[7];
		itoa(code+current_frame,framename,10);
		//code++;
		/* Go to the frame we want.  Important if multiple frames are queried in
		 * the loop which they of course are for optical flow.  Note that the very
		 * first call to this is actually not needed. (Because the correct position
		 * is set outsite the for() loop.)
		 */
		cvSetCaptureProperty( input_video, CV_CAP_PROP_POS_FRAMES, current_frame );

	    //��õ�һ֡
		frame = cvQueryFrame( input_video );
		if (frame == NULL)
		{
			/* Why did we get a NULL frame?  We shouldn't be at the end. */
			fprintf(stderr, "Error: Hmm. The end came sooner than we thought.\n");
			return -1;
		}
		 //current_frame++;
		/* Allocate another image if not already allocated.
		 * Image has ONE channel of color (ie: monochrome) with 8-bit "color" depth.
		 * This is the image format OpenCV algorithms actually operate on (mostly).
		 */

        //frame1_1C �������еĵ�һ֡����ͨ�������Ϊ8
		utils::allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );
		/* Convert whatever the AVI image format is into OpenCV's preferred format.
		 * AND flip the image vertically.  Flip is a shameless hack.  OpenCV reads
		 * in AVIs upside-down by default.  (No comment :-))
		 */
		cvConvertImage(frame, frame1_1C,0);
	
		//frame1 ��ɫͼ�����������滭��
		utils::allocateOnDemand( &frame1, frame_size, IPL_DEPTH_8U, 3 );
		cvConvertImage(frame, frame1, 0);
		//cvShowImage("frame1",frame1);
		utils::allocateOnDemand( &frame1out, frame_size, IPL_DEPTH_8U, 3 );
		cvConvertImage(frame, frame1out, 0);
        
		/* ��õڶ�֡*/
		do 
		{
			frame = cvQueryFrame( input_video );
			if (frame == NULL)
			{
				fprintf(stderr, "Error: Hmm. The end came sooner than we thought.\n");
				return -1;
			}
			current_frame++;

		} while (current_frame%25!=0);
		
		//frame = cvQueryFrame( input_video );
	
		//frame2_1C �������еĵڶ�֡����ͨ�������Ϊ8
		utils::allocateOnDemand( &frame2_1C, frame_size, IPL_DEPTH_8U, 1 );
		cvConvertImage(frame, frame2_1C, 0);
	//	cvShowImage("frame2_1C",frame2_1C);

		ProfTimer t;
		t.Start();
	    
		vector<vector<ComponentFeature>> frame_components;//����洢32���ֲ�����ֱ���й���������֮��õ���9ά��������
		frame_components.reserve(frame_width*frame_height);//Ϊframe_components���ڴ��з���frame_width*frame_height���Ż�vector�ٶ�
       frame1_1C_subregions.reserve(frame_width*frame_height);
       frame2_1C_subregions.reserve((frame_width*frame_height));
       //�зֵ�һ֡���洢��������
       frame1_1C_subregions= GetSubregions(frame1_1C,cvSize(frame_size.width/frame_width,frame_size.height/frame_height),width_step);
       //�зֵڶ�֡���洢��������
	   frame2_1C_subregions= GetSubregions(frame2_1C,cvSize(frame_size.width/frame_width,frame_size.height/frame_height),width_step);
	   //�ֱ�������������ָ��������Ƭ����
	   vector<IplImage*> ::iterator iter_frame1_1C=frame1_1C_subregions.begin();
	   vector<IplImage*> ::iterator iter_frame2_1C=frame2_1C_subregions.begin();
	   //��ÿ����������й�����
	   Opticalflow of;
	   int flag=0;
	   for (;iter_frame1_1C!=frame1_1C_subregions.end();iter_frame1_1C++,iter_frame2_1C++)
	   {
           vector<ComponentFeature> components;	//local_regionһ����֡��1/64�������12��16*16��subregion
		   components.reserve(12);
		   //of.calOpticalFlowLK(frame1,*iter_frame1_1C,*iter_frame2_1C,flag,width_step,components);
		   of.calOpticalFlowVar(frame1,*iter_frame1_1C,*iter_frame2_1C,flag,width_step,components);
		   cvReleaseImage(&(*iter_frame1_1C));
		   cvReleaseImage(&(*iter_frame2_1C));
		   frame_components.push_back(components);//local_region���뵽֡
		   vector<ComponentFeature>().swap(components);//���components
		   flag++;
	   }
	 //  cvShowImage("Optic",frame1);
	    
		video_Feature.push_back(frame_components);//��һ֡������Ƶ��
		vector<vector<ComponentFeature>>().swap(frame_components);
	   
		t.Stop();
		cout<<"time=: "<<t.GetDurationInSecs ()*1000<<"(ms)"<<endl;
		
	//	current_frame++;
      //  cvWaitKey(1);
		
        //���濪ʼѭ��
		if (video_Feature.size()==10)
		{    
			vector<ComponentFeature> tran;
			tran.reserve(video_Feature.size());
			//����֡
			for (list<vector<vector<ComponentFeature>>>::iterator first=video_Feature.begin();first!=video_Feature.end();first++)
           {   //�ֲ�����
			   for (vector<vector<ComponentFeature>>::iterator second=(*first).begin();second!=(*first).end();second++)
			   {
				   for (vector<ComponentFeature>::iterator third=(*second).begin();third!=(*second).end();third++)
				   {
					   
					   tran.push_back(*third);
					   
					  
				   }
			   }
           }
		
			Kmeans k(tran,9,10);
			k.start();
			Frame * f=new Frame[10];
		
			map<int,int> ::iterator result_iter=k.result.begin();
		
			for (;result_iter!=k.result.end();result_iter++)
			{
                  
		      CvScalar three=valueToScalar(result_iter->first-1);
       
              f[static_cast<int>(three.val[0])].sub[static_cast<int>(three.val[1])][static_cast<int>(three.val[2])]=result_iter->second;
			 

			}
			#ifdef DEBUG
			ofstream out("frame.txt");
           for (int i=0;i<10;i++)
			{
				out<<"################################frame"<<i<<endl;
				for (int j=0;j<24;j++)
				{
					for (int k=0;k<32;k++)
					{
						out<<f[i].sub[j][k]<<"\t";
					}
					out<<endl;
				}
			}
		   out.close();
            #endif

            for (int i=0;i<10;i++)
			{
				//f[i].height=frame_height;
				//f[i].width=frame_width;
				f[i].frame2locals(cvPoint(0,0));
			}
			//cout<<   f[9].label_result->data.fl[0];
			int label_result[frame_height][frame_width]={0};
			for (int k=0;k<9;k++)
			{
				mrf mrf1;

				//mrf1.GetBlock(f,10);
				mrf1.pre=&f[k];
				mrf1.cur=&f[k+1];

				mrf1.width=frame_width;
				mrf1.height=frame_height;
				mrf1.SetNoRegions(2);
				mrf1.InitOutImage();
				//mrf1.Gibbs();
				//mrf1.Metropolis();
				mrf1.ICM();
				for (int i=0;i<frame_height;i++)
				{
					for (int j=0;j<frame_width;j++)
					{
						//int s=f[9].label_result->data.fl[i*frame_width+j];
						label_result[i][j]+=
							f[k+1].label_result->data.fl[i*frame_width+j];
					}
				}

			}
			
			

			zhjg<<"##############################"<<current_frame<<endl;
		   
			for (int i=0;i<frame_height;i++)
			{   
				for (int j=0;j<frame_width;j++)
				{
                  zhjg<<label_result[i][j]<<"\t";
				  if ((label_result[i][j]<=3)&&i!=0&&i!=frame_height-2)
				  {
					  logs<<current_frame<<endl;
					   int cvheight=frame_size.height/frame_height;
					   int cvwidth=frame_size.width/frame_width;
                       CvPoint startPoint=cvPoint(j*cvwidth,i*cvheight);
					   CvPoint endPoint =cvPoint((j+1)*cvwidth,(i+1)*cvheight);
					   cvRectangle(frame1out,startPoint,endPoint,CV_RGB(255,0,0),1,8,0);

				  }
				}
				zhjg <<endl;
			}
			zhjg<<endl;
          
			//����������local����
		    for (int i=0;i<10;i++)
			{
			   
			   vector<local*> ::iterator v_l_iter=f[i].sa.begin();
			   
			   for (;v_l_iter!=f[i].sa.end();v_l_iter++)
			   {
				 
				   delete (*v_l_iter);
			   }

			}
			
     	 
		 
			//break;
			//ȥ�����ϵ�һ֡
			video_Feature.pop_front();
		}
		//���������ͼ��
        string end=".jpg";
		
		string fold="result\\";
		string path=fold+framename+end;
        cvSaveImage(path.c_str(),frame1out);
		/*int key_pressed;
		key_pressed = cvWaitKey(1);
		if (key_pressed=='Q'||key_pressed=='q')
		{
			break;
		}*/
		if (current_frame < start_frame)						
			current_frame = start_frame;
		//if (current_frame >= number_of_frames - 1)	//current_frame = number_of_frames - 2;
		if (current_frame>=number_of_frames-1)
		break;
   }
   zhjg.close();
   logs.close();
	
}
