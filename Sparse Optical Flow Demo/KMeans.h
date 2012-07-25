#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "ComponentFeature.h"
using namespace std;

const static int T=30;/* 最大迭代数 */
const static double ED=0.0000001;/* 结束条件 */
class Points
{
public:
	int  label;
	double  *p;
	double  *distance;
};

class Pop{
public:
	Points *clu_cent;	/* 即cluster_center 簇类中心 */
	int **cluster;	/* 簇类数组 */
	int *cluster_num;	/* 簇类中一组数据的编号 */
	double fitness;		/* 样本适应度值,用于判断结束条件 */
	double old_fitness;	/* 前一次迭代的适应度值 */
	double Je;		/* 所有样本的平方误差和 */
};
class Kmeans
{
	
public:
	Kmeans(const vector< ComponentFeature >& tran,const int na,const int k)
	{
		
		copy(tran.begin(),tran.end(),back_inserter(_tran));
		NA=na;
		K=k;
		Psize=_tran.size();
		all_data=new Points[Psize];
		
		for(int i=0;i<Psize;i++)
		{
			all_data[i].p=new double[NA];
			all_data[i].distance=new double[K];
		}
		
		pop.clu_cent=new Points[K];
		for (int i=0;i<K;i++)
		{
			pop.clu_cent[i].p=new double[NA];
			pop.clu_cent[i].distance=new double[K];
		}
		pop.cluster=new int *[K];
		for (int i=0;i<K;i++)
		{
			pop.cluster[i]=new int[Psize];
		}
		pop.cluster_num=new int [K];
	}
	~Kmeans(void)
	{
		for (int i=0;i<Psize;i++)
		{
			delete []all_data[i].p;
			delete []all_data[i].distance;
		}
        
		delete []all_data;
		for (int i=0;i<K;i++)
		{
             delete[] pop.clu_cent[i].p;
			 delete[] pop.clu_cent[i].distance;
		}
		delete [] pop.clu_cent;
		delete [] pop.cluster_num;
		for (int i=0;i<K;i++)
		{
			delete[] pop.cluster[i];
			
		}
		delete [] pop.cluster;
		
	}
     void start();
	 map<int,int> result;//聚类后的结果，格式为label,class
private:
	int Is_equal(int a[], int n, int b);
	double Euclid(int x, int y);
	void input_data();
	void Init_center();
	void calculate_distance();
	void Make_new_cluster();
	void Make_new_center();
	void output_info();

	Points *all_data;		/* 数据大小 */
	Pop pop;
	 int NA;/* 数据维数 */
	 int K;/* 聚类数 */
	 int Psize;/* 种群大小 */
     vector<ComponentFeature> _tran;
	
};
