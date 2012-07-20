// filefind.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;
void swap(int &a,int &b)
{
  a=a^b;
  b=a^b;
  a=a^b;
}
void listDir(int *a,int n)
{
	
    for (int i=0;i<n-1;i++)
    {
		for (int j=i+1;j>0&&a[j]<a[j-1];j--)
		{
			swap(a[j],a[j-1]);
		}
    }
	//return a;
}

int main(int argc,char *argv[])
{
	int  a[]={5,3,6,7,1,2,8,4};
	listDir(a,8);
	for (int i=0;i<8;i++)
	{
		cout<<a[i]<<ends;
	}
	
}
