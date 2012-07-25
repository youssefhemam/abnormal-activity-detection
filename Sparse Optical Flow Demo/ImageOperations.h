#pragma once
#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"
#include "frame.h"
#include "local.h"

using namespace std;

// ��Ҫ��
class ImageOperations
{
public:
	//IplImage *in_image, *out_image; char * out_win_name;
	// ͼ��Ŀ�Ⱥ͸߶�
	Frame  *cur,*pre;
	int width, height;
	
public:
	ImageOperations();
	~ImageOperations();
	//bool IsOutput();			// TRUE if  out_image <> NULL
	// ��������������Ϊ��ֵ�ͷ��means and variances�������ڴ档
	// sets the number of regions,
	// allocates/frees memory for
	// mean vectors and covariance matrices
	void SetNoRegions(int n);
	int GetNoRegions() { return no_regions; }
	void SetBeta(double b) { beta = b; }
	void SetT(double x) { t = x; }
	void SetT0(double t) { T0 = t; }
	void SetC(double x) { c = x; }
	void SetAlpha(double x) { alpha = x; }
	int GetK() { return K; }
	double GetT() { return T; }
	double GetE() { return E; }
	//double GetTimer() { return (timer_valid? timer.GetElapsedTimeMs() : 0.0); }
	// �����������ľ�ֵ�ͷ���
	void CalculateMeanAndVariance(int region,int x, int y, int w, int h);
	// ����ȫ�����������ڵ�ǰ�ı�ǩ����
	double CalculateEnergy();
	// ����site(i,j)�ľֲ��������������ȱ�ǩ�Ѿ������site
	double LocalEnergy(int i, int j, int label);
    
	double computeRho(int &label,int i,int j,int x,int y);
	void Metropolis(bool mmd=false);  // executes Metropolis or MMD (if mmd=true)
	void ICM();			    // executes ICM
	void Gibbs();			    // executes Gibbs sampler

	virtual void OnIterationOver(void);
	// ��ʼ����ǩͼ��
	void InitOutImage();
	// ���ݵ�ǰ��ǩͼ�񣬻�������Ҷ�ͼ��
	void CreateOutput();
    double Tk(double x, double k=0.3);//Tk����������ʽ6����k��ȡֵӰ���쳣��⣬kԽС�����������쳣��ΪԽ��
	void GetBlock(Frame * f,int n);
protected:
	// ���������Ҳ���ǽ�ͼ�����ط�Ϊ�����ࣿ
	int no_regions;
	// 2���ŵ�potentialǿ��
	double beta;

	// ��ֹ��������ֵ�������(deltaE < t)����ֹ��
	double t;
	// ��ʼ�¶ȣ�ICM��ʹ������������
	double T0;
	// �¶ȱ仯ϵ��T(n+1)=c*T(n).
	double c;
	double alpha;		            // alpha value for MMD

	// ÿ������ľ�ֵ
	//double *mean;
	// ÿ������ķ���
	//double *variance;
	//double *work_buf1;
	// ��ǰȫ������
	double E;
	// �ϴε�����ȫ������
	double E_old;
	// ��ǰ�¶�
	double T;
	// ��ǰ��������
	int K;
	Frame * Frame_Block;
    
	// ��ǩͼ��
	int **classes;
	// �Ҷ�ͼ��
	//int **in_image_data;
	//local*** pre_frame_data_arr;
	local** pre_frame_data;
	local** cur_frame_data;
	vector<vector<local>> pre_frame_data_arrs;
	// ͼ��ĵ�����Ȼ�ȷֲ���evidence��
	double ** local_evidence;
	int use_local_evidence;

	// ����site(i,j)�������ǩk���singleton potential
	double Singleton(int i, int j, int label);
	double _Singleton(int i, int j, int label);
	// ����site(i,j)�������ǩk���doubleton potential
	double Doubleton(int i, int j, int label);
   private:
	 int block_size;
};