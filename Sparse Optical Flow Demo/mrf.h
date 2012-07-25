#pragma once
#include "stdafx.h"
#include "ImageOperations.h"
#include "cv.h"
class mrf: public ImageOperations
{
public:
	void OnIterationOver(void)
	{
	
			CreateOutput();
			
			//cvWaitKey(3);
			cout<<"\rI="<<K<<"\tEnergy="<<T;
		
	}
};