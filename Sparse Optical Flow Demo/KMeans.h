#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "ComponentFeature.h"
using namespace std;

const static int T=30;/* �������� */
const static double ED=0.0000001;/* �������� */
class Points
{
public:
	int  label;
	double  *p;
	double  *distance;
};

class Pop{
public:
	Points *clu_cent;	/* ��cluster_center �������� */
	int **cluster;	/* �������� */
	int *cluster_num;	/* ������һ�����ݵı�� */
	double fitness;		/* ������Ӧ��ֵ,�����жϽ������� */
	double old_fitness;	/* ǰһ�ε�������Ӧ��ֵ */
	double Je;		/* ����������ƽ������ */
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
	 map<int,int> result;//�����Ľ������ʽΪlabel,class
private:
	int Is_equal(int a[], int n, int b);
	double Euclid(int x, int y);
	void input_data();
	void Init_center();
	void calculate_distance();
	void Make_new_cluster();
	void Make_new_center();
	void output_info();

	Points *all_data;		/* ���ݴ�С */
	Pop pop;
	 int NA;/* ����ά�� */
	 int K;/* ������ */
	 int Psize;/* ��Ⱥ��С */
     vector<ComponentFeature> _tran;
	
};
