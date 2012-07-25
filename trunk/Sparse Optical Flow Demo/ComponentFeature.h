#pragma once

class ComponentFeature
{
public:
	ComponentFeature(void);
	~ComponentFeature(void);
	int angle[8];//存储像素点的8个方向特征
	double speed;//存储像素点的速度特征
};
