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
//const static int NA=9;/* 数据维数 */
//const static int K=20;/* 聚类数 */
//const static int Psize=7680;/* 种群大小 */
//const static int T=30;/* 最大迭代数 */
//const static double ED=0.0000001;/* 结束条件 */

/************************************************
 * 从外部文件导入数据，对于没有数据文件将报错， * 
 * 数据文件的格式根据 NA 决定，例如NA = 4时，测 *
 * 试数据为四维，则test.data 为：		*
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
 * 随机初始化聚类质心，当随机数有相同时跳过继续执行*
 **************************************************/
void Kmeans:: Init_center()
{
	int i, j;
	int num = 0;
	int rand_num;
	int* rand_num_tmp=new int[K];
	/* 随机产生三个0～Psize的数 */
	while(num < K){
		rand_num = rand() % Psize;
		if(!Is_equal(rand_num_tmp, num, rand_num))
			rand_num_tmp[num++] = rand_num;
	}
	for(i = 0; i < K; i++){
	//	printf("初始化质心%d:\n", i + 1);
		for(j = 0; j < NA; j++){
			pop.clu_cent[i].p[j] = all_data[rand_num_tmp[i]].p[j];
	//		printf("%lf	",pop.clu_cent[i].p[j]);	
		}
		//printf("\n");
	}
	delete[] rand_num_tmp;
}

/********************************** 
 * 检查数据是否有相等,相等返回1 *
 *********************************/
int Kmeans:: Is_equal(int a[], int n, int b)
{
	int i;
	for(i = 0; i < n; i++)
		if(a[i] == b) return 1;
	return 0;
}

/********************************************
 * 计算Psize组数据到K个质心的欧几里德距离*
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
 * 此函数为欧几里德距离公式函数,此处用于计算*
 * 一组数据到对应簇中心的欧几里德距离。	*
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
 * 将数据进行簇集归类 *
 ***********************/
void Kmeans:: Make_new_cluster()
{
	int i, j;

	double min;
	
	for(i = 0; i < K; i++)		/* 初始化编号 */
		pop.cluster_num[i] = 0;
	for(i = 0; i < Psize; i++){	
		int index = 0;
		min = all_data[i].distance[0];
		for(j = 1; j < K; j++){		/* 筛选到簇心欧几里德最小的 */
			if(all_data[i].distance[j] < min){
				min = all_data[i].distance[j];
				index = j;
			}
		}
		/* 划分簇集 */
		pop.cluster[index][pop.cluster_num[index]++] = i;	
	}
	/* 计算所有样本的平方误差和 */
	pop.Je = 0;
	for(i = 0; i < K; i++)
		for(j = 0; j < pop.cluster_num[i]; j++){
		/* 样本到簇心的值即为其欧几里德距离 */
			pop.Je +=pow(all_data[pop.cluster[i][j]].distance[i],2);
		}
	pop.old_fitness = pop.fitness;	/* 前一次迭代适应度值 */
//	printf("old_fitness = %lf\n", pop.old_fitness);
	pop.fitness = pop.Je;	/* 所有样本平方误差和即为适应度值 */
}

/*************************************************
 * 更新簇心，即求其群类的平均距离为新的簇心  *
 ************************************************/
void Kmeans:: Make_new_center()
{
	int i, j, n;
	double tmp_sum;

	for(i = 0; i < K; i++)
		for(j = 0; j < NA; j++){
			tmp_sum = 0;
			for(n = 0; n < pop.cluster_num[i]; n++){
				/* 第i个簇的第j维数的所有数据和 */
				tmp_sum += all_data[pop.cluster[i][n]].p[j];
			}
			/* 取平均数得到新的簇中心 */
			pop.clu_cent[i].p[j] = tmp_sum / pop.cluster_num[i];
		}
}

/********************************
 *	输出信息函数		*				
 * 显示格式为：		*
 * 质心K:			*
 * NA维的质心数据		*
 * 簇类K：			*
 * NA维属于簇类K的数据		*
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
	double differ = 1;	/* 适应度差值 */
	

	input_data();		/* 导入数据 */
	Init_center();		/* 初始化质心 */

	for(i = 0; (i < T) && (differ > ED); i++){
		calculate_distance();		/* 计算欧几里德距离 */
		Make_new_cluster();		/* 产生新的聚类 */

		Make_new_center();		/* 对新的聚类产生新的质心 */
		differ = pop.old_fitness - pop.fitness;	/* 判断条件 */
		differ = fabs(differ);

		//	printf("differ = %lf\n", differ);
		//	printf("old_fitness = %lf\n", pop.old_fitness);
		//	printf("fitness = %lf\n", pop.fitness);
	}

	//printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	output_info();	/* 聚类后显示结果 */

}
