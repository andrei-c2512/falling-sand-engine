#pragma once

namespace e2d {
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
			if (Cnt < Time)
				Cnt += time;
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

	class Countdown {
	public:
		Countdown() = default;
		Countdown(float time)
			:Time(time)
		{
		}
		void Update(float delta) {
			if (Time > delta)
				Time -= delta;
			else
				Time = 0.0f;
		}
		bool IsReady() const
		{
			return Time == 0.0f;
		}
		float GetTime() const
		{
			return Time;
		}
		float GetTimeLimit() const {
			return Time;
		}
		void SetTime(float time) {
			Time = time;
		}
	private:
		float Time;
	};
}