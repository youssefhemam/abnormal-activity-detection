#include "StdAfx.h"
#include "ComponentFeature.h"
#include <iostream>
using namespace std;

ComponentFeature::ComponentFeature(void)
{

   	
	speed=0;
	memset(angle,0,8*sizeof(int));
	
	//cout<<"#######ComponentFeatureS:"<<start++<<endl;
}

ComponentFeature::~ComponentFeature(void)
{
	//cout<<"#######ComponentFeatureE:"<<end++<<endl;
}
