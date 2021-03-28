#include"Base.h"


namespace nautilus {

	void sleepcp(int milliseconds){

		clock_t time_end;
		time_end = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
		while (clock() < time_end)
		{
		}
	}


	namespace graphics {

		std::mt19937 Random::s_RandomEngine;
		std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

		float common_lerp(float x, float y, float t) {
			return x * (1.f - t) + y * t;
		}


	}


	FPSTimer::FPSTimer(){
	
		m_FrametimeQueue.resize(100);	
	}


	void FPSTimer::endFrame() {

		using namespace std;


		// Duration from frame start to frame end in milliseconds.
		double duration = getElapsedTime();

		m_FrametimeQueue.push_back(duration);

		// Limit frames storage to 100.
		// We erase the oldest value from queue.
		while (m_FrametimeQueue.size() > 100) m_FrametimeQueue.pop_front();


		// Govern with average duration of a frame for more consistent values...
		if (m_GovernFPS) _governFPS(getAverageMillisecondsPerFrame());
	}


	void FPSTimer::_governFPS(double dt){

		using namespace std;

		// E.g. "dt = 20.2" 
		// and "1000 / m_TargetFPS" = 16.6.
		if (dt > 1000 / m_TargetFPS) {

			// We need more milliseconds for one frame than we want to..
			// 
			// This means that our current FPS is LOWER than our ideal FPS,
			// and thus we want to ...
			// For now just do nothing.

		}

		// E.g. "dt" = 12.1 and
		// "1000 / m_TargetFPS" = 16.6
		else if (dt < 1000 / m_TargetFPS) {

			// We need less milliseconds for one frame than we want to..
			// 
			// This means, that our current FPS is HIGHER than our ideal FPS,
			// thus we want to sleep for the rest of the left frametime...

			double left_ms_to_ideal = (1000 / m_TargetFPS) - dt;

			sleepcp(left_ms_to_ideal + 0.5);

		}
		else { // Ideal framerate reached.
			return;
		}
	}


	double FPSTimer::getMillisecondsPerFrame() {

		return m_FrametimeQueue.back();
	}


	double FPSTimer::getAverageMillisecondsPerFrame() {

		double ret = 0.0;
		for (int i = 0; i < m_FrametimeQueue.size(); i++) {
			ret += m_FrametimeQueue[i];
		}

		return ((ret / m_FrametimeQueue.size()));
	}


	double FPSTimer::getFrametime() {

		// Frametime is defined as
		// x milliseconds divided by 1000.

		return m_FrametimeQueue.back() / 1000.0;
	}


	double FPSTimer::getAverageFrametime() {

		// Frametime is defined as
		// x milliseconds divided by 1000.

		double ret = 0.0;
		for (int i = 0; i < m_FrametimeQueue.size(); i++) {
			ret += m_FrametimeQueue[i];
		}

		return (	(ret / m_FrametimeQueue.size()) / 1000.0	);
	}


	double FPSTimer::getFPS() {

		// Frames per second are defined as
		// 1000 divided by x milliseconds.

		return (	1000.0 / m_FrametimeQueue.back()	);
	}

	double FPSTimer::getAverageFPS() {

		// Frames per second are defined as
		// 1000 divided by x milliseconds.


		double ret = 0.0;
		for (int i = 0; i < m_FrametimeQueue.size(); i++) {
			ret += m_FrametimeQueue[i];
		}

		return (	1000.0 / (ret / m_FrametimeQueue.size())	);
	}

}
