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
	void UploadTime()
	{
		BuildTime = ft.DeltaTime();

		std::string aux = "Time: " + std::to_string(BuildTime * 1000.0f) + " ms\n";
		OutputDebugStringA(aux.c_str());
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
	void DrawFrameCounter(Graphics& gfx )
	{
		FrameTimer.Update(BuildTime);

		if (FrameTimer.IsReady())
		{
			Frames = int(1.0f / BuildTime);
			FrameTimer.ResetTimer();
		}

		const std::string frame_str = std::to_string(Frames);

		int space = 10;
		auto dim = font.GetLetterDim();
		Vec2I pos = Vec2I(Graphics::ScreenWidth - space - frame_str.length() * dim.width,
			space + dim.height);

		font.DrawWords(frame_str, gfx, pos);
	}
private:
	FrameTimer ft;
	Font font = { "Fixedsys16x28.bmp" };


	bool  Running = true;
	float BuildTime = 0;
	float LastBuild = 0;

	float minimum = 0;
	float maximum = 0;
	float average = 0;

	int   Frames = 0;
	Timer FrameTimer = { 0.5f};
	int   nMeasurements = 0;
};