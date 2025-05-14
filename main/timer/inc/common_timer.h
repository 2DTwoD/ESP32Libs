#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H
#include <cstdint>

class CommonTimer {
	private:
		uint32_t period;
		uint32_t curTime{0};
		bool go{false};
		bool impulse{false};
	public:
		explicit CommonTimer(uint32_t period);
        virtual ~CommonTimer();
        virtual void update();
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
        virtual CommonTimer& operator=(bool value);
        virtual CommonTimer& operator+=(bool value);
		virtual bool started();
	    bool notStarted() const;
		bool finished() const;
		bool notFinished() const;
		bool finishedImpulse();
		bool inWork();
		bool isFree() const;
		bool inPause() const;
};


class ITimer: protected CommonTimer {
public:
    explicit ITimer(uint32_t period) : CommonTimer(period) {}
    virtual void set(bool value) = 0;
    virtual bool get() = 0;
    virtual void wait(bool value) = 0;
    virtual void again() = 0;
    virtual uint32_t time(){
        return CommonTimer::getCurrentTime();
    }
    virtual uint32_t remain(){
        return CommonTimer::getPeriod() - time();
    }
    ITimer& operator=(bool value) override{
        set(value);
        return *this;
    }
    ITimer& operator+=(bool value) override {
        wait(value);
        return *this;
    };
};
#endif //COMMON_TIMER_H