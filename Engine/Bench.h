#pragma once
#include "FrameTimer.h"
#include <string>
#include <fstream>

class Bench {
public:
	Bench() = default;
	void Start()
	{
		if(Running)
			ft.DeltaTime();
	}
	void UploadData()
	{
		if (Running && nMeasurements > 5)
		{	
			average /= nMeasurements;
			std::string aux = "minimum: " + std::to_string(minimum * 1000.0f) + " ms\n";
			OutputDebugStringA(aux.c_str());

			aux = "maximum: " + std::to_string(maximum * 1000.0f) + " ms\n";
			OutputDebugStringA(aux.c_str());

			aux = "average: " + std::to_string(average * 1000.0f) + " ms\n";
			OutputDebugStringA(aux.c_str());

			Running = false;
		}
	}
	void End()
	{
		if (Running)
		{
			BuildTime = ft.DeltaTime();

			average += BuildTime;

			if (nMeasurements == 0)
			{
				minimum = BuildTime;
				maximum = BuildTime;
			}

			minimum = std::min(BuildTime, minimum);
			maximum = std::max(BuildTime, maximum);

			nMeasurements++;
		}
	}
private:
	FrameTimer ft;
	std::string file_name = "dog.txt";

	bool Running = true;
	float BuildTime;
	float LastBuild;

	float minimum = 0;
	float maximum = 0;
	float average = 0;

	int nMeasurements = 0;
};