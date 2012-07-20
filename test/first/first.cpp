#include "stdafx.h"
/*
// first.cpp : 定义控制台应用程序的入口点。
//


#include<iostream>
#include<string>
#include<fstream>
#include <afx.h>
#include <vector>
#include <map>
using namespace std;
vector<string> v_filename;
vector<string> m_Classname;
vector<vector<string>> m_Filename;//可以用map<string,vector<string>>
map<string,vector<string>> m_ClassAndName;
int tmp_count=0;
void getFileName(const string s_path)
{
	CFileFind finder;
    CString path;
	path.Format("%s",s_path.c_str());
	CString tmp_path=path+"\\*.*";
	CString dir("\\");
	CString wildcard=tmp_path;
	CString end=".avi";
	tmp_count = 0;
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
                 getFileName(tmpdir.GetBuffer(tmpdir.GetLength()));
				 if (tmp_count!=0)
				 {
					 string class_name=found_filename.GetBuffer(found_filename.GetLength());
					 m_Classname.push_back(class_name);
					 m_Filename.push_back(v_filename);
					 m_ClassAndName.insert(make_pair(class_name,v_filename));
					 printf("The class %s\t\thas %d\n",found_filename,tmp_count);
					 
				 }
			}
			else
			{  

				if (found_filename.Find(end)>=0)
				{
					tmp_count++;
					
					
					CStringA tmp_filename=path+dir+found_filename;
					cout<<"name"<<tmp_filename<<endl;
					v_filename.push_back(tmp_filename.GetBuffer(tmp_filename.GetLength()));
				}
			}
		}
	}
}
int tmain2(int argc, _TCHAR* argv[])
{
	string s="E:\\bysj\\test\\example2\\Hu\\Train";
	getFileName(s);
	for (vector<vector<string>>::iterator iter=m_Filename.begin();iter!=m_Filename.end();iter++)
    {
		for(vector<string>::iterator siter=(*iter).begin();siter!=(*iter).end();siter++)
		{
			cout <<*siter<<endl;
		}
    }
	for (vector<string>::iterator citer=m_Classname.begin();citer!=m_Classname.end();citer++)
	{
		cout<<*citer<<endl;
	}
	for (map<string,vector<string>>::iterator m_iter=m_ClassAndName.begin();m_iter!=m_ClassAndName.end();m_iter++)
	{
		cout<<m_iter->first<<",";
		for (vector<string>::iterator iter=m_iter->second.begin();iter!=m_iter->second.end();iter++)
		{
			cout<<*iter<<" "<<m_iter->first<<endl;
		}
		//cout<<endl;
	}

	return 0;
}

*/
