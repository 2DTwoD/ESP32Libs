#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H
#include <cstdint>

class ITimer {
public:
    virtual void update() = 0;
    virtual void set(bool value) = 0;
    virtual void pause(bool value) = 0;
    virtual bool get() = 0;
    virtual void reset() = 0;
    virtual void again() = 0;
    ITimer& operator=(bool value) {
        set(value);
        return *this;
    }
    ITimer& operator+=(bool value) {
        pause(value);
        return *this;
    };
};

class CommonTimer {
	private:
		uint32_t period;
		uint32_t curTime{0};
		bool go{false};
		bool impulse{false};
	public:
		explicit CommonTimer(uint32_t period);
		void update();
		uint32_t getPeriod() const;
		void setPeriod(uint32_t value);
		uint32_t getCurrentTime() const;
		void setCurrentTime(uint32_t value);
		void start();
		void pause();
		void stop();
		void reset();
		void prepareAndStart();
		void finish();
		void setStart(bool value);
		void setPause(bool value);
		CommonTimer& operator=(bool value);
		CommonTimer& operator+=(bool value);
		virtual bool started();
	    bool notStarted() const;
		bool finished() const;
		bool notFinished() const;
		bool finishedImpulse();
		bool inWork();
		bool isFree() const;
};

#endif //COMMON_TIMER_H