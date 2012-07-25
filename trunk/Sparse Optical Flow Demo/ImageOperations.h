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

// 主要类
class ImageOperations
{
public:
	//IplImage *in_image, *out_image; char * out_win_name;
	// 图像的宽度和高度
	Frame  *cur,*pre;
	int width, height;
	
public:
	ImageOperations();
	~ImageOperations();
	//bool IsOutput();			// TRUE if  out_image <> NULL
	// 设置区域数量，为均值和方差（means and variances）分配内存。
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
	// 计算给定区域的均值和方差
	void CalculateMeanAndVariance(int region,int x, int y, int w, int h);
	// 计算全局能量，基于当前的标签数据
	double CalculateEnergy();
	// 计算site(i,j)的局部能量。假设事先标签已经赋予该site
	double LocalEnergy(int i, int j, int label);
    
	double computeRho(int &label,int i,int j,int x,int y);
	void Metropolis(bool mmd=false);  // executes Metropolis or MMD (if mmd=true)
	void ICM();			    // executes ICM
	void Gibbs();			    // executes Gibbs sampler

	virtual void OnIterationOver(void);
	// 初始化标签图像
	void InitOutImage();
	// 根据当前标签图像，绘制输出灰度图像
	void CreateOutput();
    double Tk(double x, double k=0.3);//Tk函数，（公式6），k的取值影响异常检测，k越小，检测出来的异常行为越少
	void GetBlock(Frame * f,int n);
protected:
	// 区域个数。也就是将图像像素分为多少类？
	int no_regions;
	// 2阶团的potential强度
	double beta;

	// 终止条件（阈值）。如果(deltaE < t)，终止。
	double t;
	// 初始温度（ICM不使用它）。用于
	double T0;
	// 温度变化系数T(n+1)=c*T(n).
	double c;
	double alpha;		            // alpha value for MMD

	// 每个区域的均值
	//double *mean;
	// 每个区域的方差
	//double *variance;
	//double *work_buf1;
	// 当前全局能量
	double E;
	// 上次迭代的全局能量
	double E_old;
	// 当前温度
	double T;
	// 当前迭代次数
	int K;
	Frame * Frame_Block;
    
	// 标签图像。
	int **classes;
	// 灰度图像
	//int **in_image_data;
	//local*** pre_frame_data_arr;
	local** pre_frame_data;
	local** cur_frame_data;
	vector<vector<local>> pre_frame_data_arrs;
	// 图像的单团似然度分布（evidence）
	double ** local_evidence;
	int use_local_evidence;

	// 计算site(i,j)被赋予标签k后的singleton potential
	double Singleton(int i, int j, int label);
	double _Singleton(int i, int j, int label);
	// 计算site(i,j)被赋予标签k后的doubleton potential
	double Doubleton(int i, int j, int label);
   private:
	 int block_size;
};