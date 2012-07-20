#include "stdafx.h"
#include <iostream>
#include <string>
#include <iosfwd>
#include   "windows.h "   
#include <shellapi.h>
using namespace std;


int main()
{
	string save_filename="scale.range";//Àı∑≈πÊ‘Ú
	string restore_filename;

	::ShellExecute(NULL,"open","svm-scale123.exe","-l -1 -u 1 -s scale.range svmcombination_train.txt","C:\\Users\\dsm\\Desktop\\wetherman\\zo",SW_HIDE);
	::ShellExecute(NULL,"open","svm-scale123.exe","-l -1 -u 1 -r scale.range svmcombination_test.txt","C:\\Users\\dsm\\Desktop\\wetherman\\zo",SW_HIDE);
	return 0;
}