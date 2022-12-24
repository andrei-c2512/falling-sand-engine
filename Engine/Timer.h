#pragma once

class Timer {
public:
	Timer() = default;
	Timer(float time)
		:Time(time)
	{
		Cnt = 0.0f;
		Ready = false;
	}
	void Update(float time) {
		if (float(Cnt) < float(Time))
		{
			Cnt += time;
		}
		else
			Ready = true;
	}
	bool IsReady() const
	{
		return Ready;
	}
	void ResetTimer()
	{
		Cnt = 0.0f;
		Ready = false;
	}
	bool operator==(Timer& timer)
	{
		return Time == timer.Time;
	}
	bool operator!=(Timer& timer)
	{
		return Time != timer.Time;
	}
	float GetTime() const
	{
		return Cnt;
	}
	float GetTimeLimit() const {
		return Time;
	}
private:
	float Time;
	float Cnt; // counter
	bool Ready;
};