#include "StdAfx.h"
#include "ImageOperations.h"
#include "frame.h"
#include "local.h"
#include <iostream>
#include "Vector.h"
#include <fstream>
using namespace std;

/* --------------------------------------------------------- */
ImageOperations::ImageOperations()
{
  //in_image = out_image = 0;
  cur=0;
  pre=0;
  //out_win_name = 0;
  width=height=0;
  Frame_Block=0;
  no_regions = -1; // -1 ==> num. of regions has not been specified yet!
  beta = 0.9;// 0.9;
  t = 0.001;//0.05
  T0 = 4; // 初始的温度值
  c = 0.98; // 每次迭代，温度降低2%
  K = 0;
  E = E_old = 0;
  T = 0;
 // mean = variance = 0;
 // work_buf1 = 0;
  alpha = 0.1;
  classes = 0;
  //in_image_data = 0;
  local_evidence = 0;
  pre_frame_data=0;
  cur_frame_data=0;
  block_size=0;
 // pre_frame_data_arrs.reserve(width*height*)
}
//设置类别数目，给mean variance work_buf1分配空间
void ImageOperations::SetNoRegions(int n) 
{ 
  no_regions = n;

}



//初始化分片
void ImageOperations::InitOutImage()
{
	
	if(cur==0)
		return;

	int r=0;//r,存放临时的类别，范围是0~no_region-1
	double e, e2;// store local energy

 
	//将preframe、curframe内部local数据取出来，存储在pre_frame_data[height][width]和cur_frame_data[height][width]中
	if (pre_frame_data==0)
	{
		pre_frame_data=new local*[height];
		for (int i=0;i<height;i++)
		{
			pre_frame_data[i]=new local[width];
		}
	}
	if (cur_frame_data==0)
	{
		cur_frame_data=new local*[height];
		for (int i=0;i<height;i++)
		{
			cur_frame_data[i]=new local[width];
		}
	}
	
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			pre_frame_data[i][j]=*(pre->getLocal(i,j));
			cur_frame_data[i][j]=*(cur->getLocal(i,j));
		}
	}
		
			

	//(2) 标签（类）数据为类标签分配width*height个空间，存储结果
	if(classes==0)
	{
		classes = new int* [height];// allocate memory for classes
		for (int i=0; i<height; ++i)
			classes[i] = new int[width];
	}

	//(3) 用单团能量，来初始化类标签(~ max. of singleton energy)
	/* initialize using Maximum Likelihood (~ max. of singleton energy)
   */
	
	for (int i=0; i<height; ++i)
		for (int j=0; j<width; ++j)
		{
			//int label=pre->label_result->data.fl[i*width+j];
			int label=1;
			e = (_Singleton(i, j,label ));
			
			classes[i][j] = label;
			//for (r=1; r<no_regions; ++r)
				//能力越小概率越大
				if ((e2=(_Singleton(i, j, r))) >e)
				{
					e = e2;
					classes[i][j] = r;
				}
			//	cout<<"E:"<<e<<endl;
		}

	//(4) 初始化单团观测证据
	if(local_evidence==0)
	{
		//ofstream local_evidences("local_evidence.txt");
		local_evidence = new double* [height];
		for (int i=0; i<height; ++i)
		{
			local_evidence[i] = new double[width*no_regions];
			for (int j=0; j<width; ++j)

			{
				//local_evidences<<"( ";
				for (r=0; r<no_regions; r++)
				{
					local_evidence[i][j*no_regions+r]=_Singleton(i, j, r);
					//local_evidences<<local_evidence[i][j*no_regions+r]<<",";
				}
				//local_evidences<<" )\t";
			}
			//local_evidences<<endl;
				
		}
		//local_evidences.close();
	}
	//CreateOutput();
	//cvShowImage(out_win_name, out_image);
	//cvWaitKey(1000);
}


void ImageOperations::CreateOutput()
{
 /* int i, j;
  unsigned char *out_data;

  if(out_image)
  {
	  out_data = (unsigned char *)out_image->imageData;
	
  // cretae output image
  for (i=0; i<height; ++i)
    for (j=0; j<width; ++j)
      {
		  out_data[(i*out_image->widthStep) + j] = (unsigned char)(classes[i][j]*255/no_regions);
      }
  }*/
	//ofstream o("label.txt",ios::app);
	//o<<"##################"<<endl;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
           cur->label_result->data.fl[i*width+j]=classes[i][j];
		  // o<<cur->label_result->data.fl[i*width+j]<<"\t";
		}
		//o<<endl;
	}
	//o<<endl;
	//o.clear();
	//o.close();
 

	
}


void ImageOperations::OnIterationOver(void)
{
	;
}

// 1阶团，返回-log(g(x)),g(x)为高斯分布，计算点(i,j)标签为label的概率，服从高斯分布
double ImageOperations::_Singleton(int i, int j, int label)
{
	// work_buf1保存的是查找表，用于性能优化。
	double sum=0.0f;
	//double k=0.3f;
	for (int index=0;index<cur_frame_data[i][j].activitydescriptor->GetLength();index++)
	{
		
		
           sum+=pre_frame_data[i][j].activitydescriptor->GetValue(index)*cur_frame_data[i][j].activitydescriptor->GetValue(index);
	}
	if (label==1)
	return Tk(sum);
	return 1-Tk(sum);
	//return  work_buf1[label] + pow((double)in_image_data[i][j]-mean[label],2)/(2.0*variance[label]);
}
//保存概率分布
double ImageOperations::Singleton(int i, int j, int label)
{
	// local_evidence保存的是概率分布，用于性能优化。（idea from "matlab MRF toy examples"）
	//  height 行，width*4列，一行中，4个为一组，分别保存点(i,j)处label为1~4的概率
	return local_evidence[i][j*no_regions+label];
}

// 2阶团
double ImageOperations::Doubleton(int i, int j, int label)
{
	double prob = 0.0;
    
	
	if (i!=height-1) // south
	{
	   prob+=computeRho(label,i,j,i+1,j);	
		
	}
	if (j!=width-1) // east
	{
		
		prob+=computeRho(label,i,j,i,j+1);
	}
	if (i!=0) // north
	{
		
		prob+=computeRho(label,i,j,i-1,j);
	}
	if (j!=0) // west
	{
	
		prob+=computeRho(label,i,j,i,j-1);
	}
	if (i!=0&&j!=0)//northwest
	{
       prob+=computeRho(label,i,j,i-1,j-1);
	}
	if (i!=0&&j!=width-1)//northeast
	{
		prob+=computeRho(label,i,j,i-1,j+1);
	}
	if (i!=height-1&&j!=0)//southwest
	{
		prob+=computeRho(label,i,j,i+1,j-1);
	}
	if (i!=height-1&&j!=width-1)//southeast
	{
		prob+=computeRho(label,i,j,i+1,j+1);
	}
	

	return (prob);
}

// 根据当前标签，计算全局期望值
double ImageOperations::CalculateEnergy()
{
	double singletons = 0.0;
	double doubletons = 0.0;
	int i, j, k;
	//ofstream doubleton("doubleton.txt");
	for (i=0; i<height; ++i)
		for (j=0; j<width; ++j)
		{
			k = classes[i][j];
			singletons += (Singleton(i,j,k));
			doubletons += (Doubleton(i,j,k));
			//doubleton<<"("<<i<<","<<j<<")"<<doubletons<<endl;
			// Note: here each doubleton is
			// counted twice ==> divide by
			// 2 at the end!
		}
		//doubleton.close();
		
	return  (singletons + doubletons/2); //将概率转换为能量
}

// 局部能量
double ImageOperations::LocalEnergy(int i, int j, int label)
{
  return (Singleton(i,j,label)) +(Doubleton(i,j,label));
}


void ImageOperations::Gibbs()
{
	int i, j;
	double *Ek;//保存局部能量的数组
	int s;
	double summa_deltaE;
	double sumE;
	double z;
	double r;

	CvRNG rg = cvRNG(time(0)); // make instance of random number generator

	Ek = new double[no_regions];

	K = 0;
	T = T0;
	E_old = CalculateEnergy();

	do
	{
		for (i=0; i<height; ++i)
			for (j=0; j<width; ++j)
			{
				sumE = 0.0;
				for (s=0; s<no_regions; ++s)
				{
					Ek[s] = exp(-LocalEnergy(i, j, s)/T);
					sumE += Ek[s];
				}
				r = cvRandReal(&rg);
				z = 0.0;
				for (s=0; s<no_regions; ++s)
				{
					z += Ek[s]/sumE; 
					if (z > r)
					{
						classes[i][j] = s;
						break;
					}
				}
			}

		E = CalculateEnergy();
		summa_deltaE = fabs(E_old-E);
		E_old = E;
		
		T *= c;
		++K;
		
	} while (summa_deltaE > t);
	OnIterationOver();

	delete Ek;
}

void ImageOperations::Metropolis(bool mmd)
{
	int i, j;
	double kszi = log(alpha);  // This is for MMD. When executing
	// Metropolis, kszi will be randomly generated.
	double summa_deltaE;

	CvRNG rg = cvRNG(time(0));  // create instance of random number generator

	K = 0;
	T = T0;
	E_old = CalculateEnergy();

	do
	{
		summa_deltaE = 0.0;
		for (i=0; i<height; ++i)
			for (j=0; j<width; ++j)
			{
				int new_lable;

				if (no_regions == 2)
					new_lable = 1 - classes[i][j];
				else
					new_lable = (classes[i][j] + (cvRandInt(&rg) % no_regions)+1 ) % no_regions;

				if (!mmd) kszi = log(cvRandReal(&rg));

				const double old_lable_E = LocalEnergy(i, j, classes[i][j]);
				const double new_label_E = LocalEnergy(i, j, new_lable);
				if (kszi <=  (old_lable_E - new_label_E) / T)
				{
					summa_deltaE += fabs(new_label_E - old_lable_E);
					E_old = E = E_old - old_lable_E + new_label_E;
					classes[i][j] = new_lable;
				}
			}
			T *= c;
			++K;
			
	} while (summa_deltaE > t); // stop when energy change is small
	OnIterationOver();
}


void ImageOperations::ICM()
{
	int i, j;
	int r;
	double summa_deltaE;

	K = 0;
	E_old = CalculateEnergy();
	//cout<<"E_old:"<<E_old<<ends;
	do
	{
		for (i=0; i<height; ++i)
			for (j=0; j<width; ++j)
			{
				for (r=0; r<no_regions; ++r)
				{

					if (LocalEnergy(i, j, classes[i][j]) > LocalEnergy(i, j, r))
					{
						classes[i][j] = r;
					}
				}
			}
		E = CalculateEnergy();
		//cout<<"Enew:"<<E<<endl;
		summa_deltaE = fabs(E_old-E);
		E_old = E;

		++K;
		
	}while (summa_deltaE > t);
	OnIterationOver();
	
}

double ImageOperations::Tk( double x ,double k)
{
	
   if (0<=x&&x<k)
   {
	   return 0.5*x/k;
   }
   if (k<=x&&x<=1)
   {
	   return 1-0.5*log(x)/log(k);
   }
}
double ImageOperations::computeRho( int &label,int i,int j,int x,int y )
{
	double produceVectorResult;
	double modc=0,modn=0;
	double Rhos=0.0f;
	double Rhof=0.0f;
	if (label == classes[x][y]) 
	{
		onlinesvr::Vector<double> *cur_copy_ij=cur_frame_data[i][j].activitydescriptor->Clone();
		onlinesvr::Vector<double> *cur_copy_xy=cur_frame_data[x][y].activitydescriptor->Clone();
	   
		produceVectorResult=cur_frame_data[i][j].activitydescriptor->ProductVectorScalar(cur_frame_data[x][y].activitydescriptor);
		/*for (int m=0;m<cur_frame_data[i][j].activitydescriptor->GetLength();m++)
		{
			cout<<pow(cur_frame_data[i][j].activitydescriptor->GetValue(m),2)<<ends;
		}
		cout<<endl;*/
		//cur_frame_data[i][j].activitydescriptor->PowScalar(2);
		cur_copy_ij->PowScalar(2);
		/*for (int m=0;m<cur_frame_data[i][j].activitydescriptor->GetLength();m++)
		{
			cout<<cur_frame_data[i][j].activitydescriptor->GetValue(m)<<ends;
		}
		cout<<endl;*/
		//cur_frame_data[x][y].activitydescriptor->PowScalar(2);
		cur_copy_xy->PowScalar(2);
		
		modc=sqrt(cur_copy_ij->Sum());
		//cout<<modc<<endl;
		modn=sqrt(cur_copy_xy->Sum());
		//cout<<modn<<endl;
		Rhos=0.5*produceVectorResult/(modc*modn);
		//Rhos=-log(Rhos);
		//cout<<Rhos<<endl;
		delete cur_copy_ij;
		delete cur_copy_xy;

	}
	else
		Rhos=0;
	for(int index=0;index<cur_frame_data[i][j].activitydescriptor->GetLength();index++)
	{
		for (int n_index=0;n_index<cur_frame_data[x][y].activitydescriptor->GetLength();n_index++)
		{
			/*double tmp=0.0;
			for (int k=0;k<block_size;k++)
			{
                 tmp+=Frame_Block[k].getLocal(i,j)->activitydescriptor->GetValue(index)*Frame_Block[k].getLocal(x,y)->activitydescriptor->GetValue(n_index);
			}
			tmp/=block_size;*/
			Rhof+=cur_frame_data[i][j].activitydescriptor->GetValue(index)*cur_frame_data[x][y].activitydescriptor->GetValue(n_index)*
				(pre_frame_data[i][j].activitydescriptor->GetValue(index)*pre_frame_data[x][y].activitydescriptor->GetValue(n_index));
				//+cur_frame_data[i][j].activitydescriptor->GetValue(index)*cur_frame_data[x][y].activitydescriptor->GetValue(n_index));
			//Rhof+=cur_frame_data[i][j].activitydescriptor->GetValue(index)*cur_frame_data[x][y].activitydescriptor->GetValue(n_index)*tmp;
		}
	}
	Rhof=Tk(Rhof);
	if (label==1&&classes[x][y]==1)
	{

	} 
	else
	{
		Rhof=1-Rhof;
	}
	return (Rhof)+Rhos;
}
void ImageOperations::GetBlock( Frame * f,int n )
{
	if (Frame_Block==0)
	{
		Frame_Block=new Frame[n];
	}
	memcpy(Frame_Block,f,n*sizeof(Frame));
	cur=Frame_Block+n-1;
	block_size=n;//包括前9帧和第10帧


}
ImageOperations::~ImageOperations()
{
   /*for (int i=0;i<height;i++)
   {
	   delete[] pre_frame_data[i];
	   delete[] cur_frame_data[i];
	   delete[] classes[i];
	   delete[] local_evidence[i];
   }
   delete[] pre_frame_data;
   delete[] cur_frame_data;
   delete[] classes;
   delete[] local_evidence;*/
	

}






