#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "rf_impulse.h"
#include "common_timer.h"
#include "updater.h"

class Sequence: private RFimpulse{
	private:
		bool strt;
		bool lck;
		bool fin;
		const uint8_t seq_step;
		uint8_t *cur_step_pointer;
		bool isMyStep();
	public:
		Sequence(uint8_t *const cur_step_pointer, uint8_t seq_step);
		void reset();
		void start(bool value);
		void lock(bool value);
		void finish(bool value);
		void slfSet(bool strt, bool lck, bool fin);
		bool active();
		bool started();
		bool locked();
		bool finished();
		bool finishedImpulse();
};

class SequenceDelayed: private CommonTimer, public Sequence, public IUpdated1ms{
	public:
		SequenceDelayed(uint8_t *const cur_step_pointer, uint8_t seq_step, uint32_t delay);
		void update1ms() override;
		using Sequence::reset;
		using Sequence::start;
		using Sequence::finish;
		using Sequence::active;
		using Sequence::started;
		using Sequence::locked;
		using Sequence::finished;
		using Sequence::finishedImpulse;
};
#endif //SEQUENCE_H