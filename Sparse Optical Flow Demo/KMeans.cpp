#include "StdAfx.h"
#include "Kmeans.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include "ComponentFeature.h"
using namespace std;
//const static int NA=9;/* ����ά�� */
//const static int K=20;/* ������ */
//const static int Psize=7680;/* ��Ⱥ��С */
//const static int T=30;/* �������� */
//const static double ED=0.0000001;/* �������� */

/************************************************
 * ���ⲿ�ļ��������ݣ�����û�������ļ������� * 
 * �����ļ��ĸ�ʽ���� NA ����������NA = 4ʱ���� *
 * ������Ϊ��ά����test.data Ϊ��		*
 * 	1	2	3	4		*
 * 	1.0	1.2	1.3	1.4		*
 *		......				*
 * 		......				*
 ***********************************************/
void Kmeans::input_data()
{
	
	int i, j;
	double data;
	int label;

	vector<ComponentFeature>::iterator iter=_tran.begin();
	for(i = 0; i <Psize; i++)
	{
		
		label=i+1;
		
		for(j = 0; j < NA-1; j++){
			
			data=(*iter).angle[j];
			all_data[i].p[j] = data;
			
			//cout<<all_data[i].p[j]<<endl;
		//	printf("%d --%d  %lf\n", i, j, all_data[i].p[j]);
		}
		data=(*iter).speed;
		all_data[i].p[NA-1]=data;
		
		
		all_data[i].label=label;
		iter++;
	}
	
}

/***************************************************
 * �����ʼ���������ģ������������ͬʱ��������ִ��*
 **************************************************/
void Kmeans:: Init_center()
{
	int i, j;
	int num = 0;
	int rand_num;
	int* rand_num_tmp=new int[K];
	/* �����������0��Psize���� */
	while(num < K){
		rand_num = rand() % Psize;
		if(!Is_equal(rand_num_tmp, num, rand_num))
			rand_num_tmp[num++] = rand_num;
	}
	for(i = 0; i < K; i++){
	//	printf("��ʼ������%d:\n", i + 1);
		for(j = 0; j < NA; j++){
			pop.clu_cent[i].p[j] = all_data[rand_num_tmp[i]].p[j];
	//		printf("%lf	",pop.clu_cent[i].p[j]);	
		}
		//printf("\n");
	}
	delete[] rand_num_tmp;
}

/********************************** 
 * ��������Ƿ������,��ȷ���1 *
 *********************************/
int Kmeans:: Is_equal(int a[], int n, int b)
{
	int i;
	for(i = 0; i < n; i++)
		if(a[i] == b) return 1;
	return 0;
}

/********************************************
 * ����Psize�����ݵ�K�����ĵ�ŷ����¾���*
 *******************************************/
void  Kmeans:: calculate_distance()
{
	int i, j;
	for(i = 0; i < Psize; i++)
		for(j = 0; j < K; j++){
			all_data[i].distance[j] = Euclid(i, j);
//		printf("%d---%d--- %lf \n", i, j, all_data[i].distance[j]);
		}
}

/************************************************
 * �˺���Ϊŷ����¾��빫ʽ����,�˴����ڼ���*
 * һ�����ݵ���Ӧ�����ĵ�ŷ����¾��롣	*
 ***********************************************/
double Kmeans:: Euclid(int x, int y)
{
	int i;
	double distance = 0;
	for(i = 0; i < NA; i++){
		distance += pow((all_data[x].p[i] - pop.clu_cent[y].p[i]), 2);
	}
	distance = sqrt(distance);
	return distance;
}


/************************
 * �����ݽ��дؼ����� *
 ***********************/
void Kmeans:: Make_new_cluster()
{
	int i, j;

	double min;
	
	for(i = 0; i < K; i++)		/* ��ʼ����� */
		pop.cluster_num[i] = 0;
	for(i = 0; i < Psize; i++){	
		int index = 0;
		min = all_data[i].distance[0];
		for(j = 1; j < K; j++){		/* ɸѡ������ŷ�������С�� */
			if(all_data[i].distance[j] < min){
				min = all_data[i].distance[j];
				index = j;
			}
		}
		/* ���ִؼ� */
		pop.cluster[index][pop.cluster_num[index]++] = i;	
	}
	/* ��������������ƽ������ */
	pop.Je = 0;
	for(i = 0; i < K; i++)
		for(j = 0; j < pop.cluster_num[i]; j++){
		/* ���������ĵ�ֵ��Ϊ��ŷ����¾��� */
			pop.Je +=pow(all_data[pop.cluster[i][j]].distance[i],2);
		}
	pop.old_fitness = pop.fitness;	/* ǰһ�ε�����Ӧ��ֵ */
//	printf("old_fitness = %lf\n", pop.old_fitness);
	pop.fitness = pop.Je;	/* ��������ƽ�����ͼ�Ϊ��Ӧ��ֵ */
}

/*************************************************
 * ���´��ģ�������Ⱥ���ƽ������Ϊ�µĴ���  *
 ************************************************/
void Kmeans:: Make_new_center()
{
	int i, j, n;
	double tmp_sum;

	for(i = 0; i < K; i++)
		for(j = 0; j < NA; j++){
			tmp_sum = 0;
			for(n = 0; n < pop.cluster_num[i]; n++){
				/* ��i���صĵ�jά�����������ݺ� */
				tmp_sum += all_data[pop.cluster[i][n]].p[j];
			}
			/* ȡƽ�����õ��µĴ����� */
			pop.clu_cent[i].p[j] = tmp_sum / pop.cluster_num[i];
		}
}

/********************************
 *	�����Ϣ����		*				
 * ��ʾ��ʽΪ��		*
 * ����K:			*
 * NAά����������		*
 * ����K��			*
 * NAά���ڴ���K������		*
 *	......			*
 *	......			*
 *******************************/
void Kmeans:: output_info()
{
	int i, j;

   // ofstream outfile;
	//outfile.open("E:\\bysj\\test\\first\\Sparse Optical Flow Demo\\resule.txt");
	for(i = 0; i < K; i++){	

		for(j = 0; j < pop.cluster_num[i]; j++){	
			//outfile<<all_data[pop.cluster[i][j]].label<<":\t"<<i+1;
		     result.insert(make_pair(all_data[pop.cluster[i][j]].label,i+1));
			//outfile<<endl;
		}
	}
	//outfile.close();
}
void Kmeans:: start()
{
	int i;
	double differ = 1;	/* ��Ӧ�Ȳ�ֵ */
	

	input_data();		/* �������� */
	Init_center();		/* ��ʼ������ */

	for(i = 0; (i < T) && (differ > ED); i++){
		calculate_distance();		/* ����ŷ����¾��� */
		Make_new_cluster();		/* �����µľ��� */

		Make_new_center();		/* ���µľ�������µ����� */
		differ = pop.old_fitness - pop.fitness;	/* �ж����� */
		differ = fabs(differ);

		//	printf("differ = %lf\n", differ);
		//	printf("old_fitness = %lf\n", pop.old_fitness);
		//	printf("fitness = %lf\n", pop.fitness);
	}

	//printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	output_info();	/* �������ʾ��� */

}
