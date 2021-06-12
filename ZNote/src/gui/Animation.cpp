#include "Animation.h"
#include <core/Logger.h>


namespace gui {



	void Animation::Run() {
		// runs until the time is over or the animation is stopped
		TimePoint currentTime;
		float timeDiffSeconds;

		do {
			
			currentTime = std::chrono::high_resolution_clock::now();
			timeDiffSeconds = (float) std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_start).count() / 1000;

			if (timeDiffSeconds > m_state.maxTime)
				timeDiffSeconds = m_state.maxTime;

			m_state.currentTime = timeDiffSeconds;
			m_state.progress = m_state.currentTime / m_state.maxTime;

			ASSERT(m_updateFunc);
			m_updateFunc(m_state);

			// sleep for 1000 / 60 ms
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 100));


		} while (timeDiffSeconds != m_state.maxTime && !terminateRequested);

		// Do not set the
		m_state.progress = 1.0f;
		m_updateFunc(m_state);

		if (m_autoDelete) {
			m_updateThread.detach();
			delete this;
		}
	}

	Animation* Animation::Create(float time) {
		ASSERT(time > 0.0f);
		Animation* animation = new Animation;

		animation->m_state.maxTime = time;
		animation->m_state.currentTime = 0;
		animation->m_start = std::chrono::high_resolution_clock::now();

		return animation;
	}

	void Animation::Start() {
		m_updateThread = std::thread(&Animation::Run, this);
	}

	void Animation::TerminateAndJoin() {
		terminateRequested = true;
		m_updateThread.join();
	}
}