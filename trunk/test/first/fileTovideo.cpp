#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include<fstream>
#include <afx.h>
#include "cv.h"
#include "highgui.h"
using namespace std;
using namespace cv;
static int tmp_count=0;
static string video_name="";
static int count_1=0;
vector<string> v_filename;//存储完整路径
vector<string> v_FileName;//存储单一文件名

map<string,vector<string>> m_VideoNameAndImageName;
map<string,vector<string>> m_VideoNameAndImageFullName;
void transfer(vector<string>::iterator iter,int num_frame,string out_file)
{
  CvVideoWriter *writer=0;
  int count=0;
  IplImage * frame=0;
  
  string image_name;
 
  int fps=25;
  int isColor=1;
  int frameH=240;
  int frameW=360;
  
  writer=cvCreateVideoWriter(out_file.c_str(),CV_FOURCC('I','4','2','0'),fps,cvSize(frameW,frameH),isColor);
  for (int i=0;i<num_frame;i++,iter++)
  { 
    image_name=(*iter);
	string full_path=image_name;

	frame=cvLoadImage(full_path.c_str(),1);
	if(!frame)
	{
		exit(0);
	}
	//cvShowImage("main",frame);
	//cvWaitKey();
	cvWriteFrame(writer,frame);
	cvReleaseImage(&frame);
  }
  cvReleaseVideoWriter(&writer);

  

}
void getFileNameAndClass(const string s_path)
{
	CFileFind finder;
	CString path;
	path.Format("%s",s_path.c_str());
	CString tmp_path=path+"\\*.*";
	CString dir("\\");
	CString wildcard=tmp_path;
	CString end=".bmp";
	tmp_count = 0;
	count_1=0;
	video_name="";
	v_filename.clear();
	bool bWorking=finder.FindFile((LPCTSTR)wildcard); 

	while(bWorking)
	{
		bWorking=finder.FindNextFile();
		if (!finder.IsDots())
		{
			CString found_filename=finder.GetFileName();

			if (finder.IsDirectory())
			{

				CString tmpdir=path+dir+found_filename;
				getFileNameAndClass(tmpdir.GetBuffer(tmpdir.GetLength()));
				if (tmp_count!=0)
				{
					string class_name=found_filename.GetBuffer(found_filename.GetLength());
				   count_1++;
				   video_name+=class_name;
				   if (count_1>=1)
				   {
					  // m_VideoNameAndImageName.insert(make_pair(video_name,v_FileName));
					   m_VideoNameAndImageFullName.insert(make_pair(video_name,v_filename));
					   printf("The class %s\t\thas %d\n",found_filename,tmp_count);
					   break;
				   }
					

				}
			}
			else
			{  

				if (found_filename.Find(end)>=0)
				{
					tmp_count++;


					CStringA tmp_filename=path+dir+found_filename;
					cout<<"name"<<tmp_filename<<endl;
				//	v_FileName.push_back(found_filename.GetBuffer(found_filename.GetLength()));
					v_filename.push_back(tmp_filename.GetBuffer(tmp_filename.GetLength()));
				}
			}
		}
	}

}


/*int main()
{
   vector<string> image_name;
   string path;
   int num_frame;
   string output_file;
   string s[12]={"01","02","03","04","05","06","07","08","09","10","11","12"};
   for(int i=0 ;i<12;i++)
   {
	   getFileNameAndClass("F:\\实验室视频\\大\\"+s[i]);

	   for (map<string,vector<string>>::iterator m_iter=m_VideoNameAndImageFullName.begin();m_iter!=m_VideoNameAndImageFullName.end();m_iter++)
	   {
		   output_file= "E:\\avi\\"+s[i]+"\\"+ m_iter->first+".avi";
		   num_frame=m_iter->second.size();

		   transfer(m_iter->second.begin(),num_frame,output_file);
	   }
   }
   //


}*/

