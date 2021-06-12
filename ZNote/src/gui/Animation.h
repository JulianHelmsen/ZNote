#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace gui {


	class Animation {
	public:
		struct AnimationState {
			float currentTime;
			float progress;
			float maxTime;
		};

		using TimePoint = std::chrono::steady_clock::time_point;
		using UpdateFunc = std::function<void(const AnimationState&)>;

		

		static Animation* Create(float time);
		void Start();
		void TerminateAndJoin();
		void EnableAutoDeletion() { m_autoDelete = true; }


		void OnUpdate(UpdateFunc updateFunc) { m_updateFunc = updateFunc; }

	private:
		void Run();
		TimePoint m_start;

		AnimationState m_state;
		UpdateFunc m_updateFunc;
		std::thread m_updateThread;
		bool terminateRequested = false;
		bool m_autoDelete = false;

	};
}