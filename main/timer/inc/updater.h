#ifndef UPDATER_H
#define UPDATER_H

#include "driver/gptimer.h"

#include "array_list.h"

class IUpdated1ms {
public:
    virtual void update1ms() = 0;
};

class IUpdatedSomewhere {
public:
    virtual void updateSomewhere() = 0;
};

class Updater{
private:
    inline static ArrayList<IUpdated1ms*>* updateList{nullptr};
    inline static gptimer_handle_t timer{nullptr};
public:
    static void init();
    static bool timerCallBack(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *userData);
    static void addObj(IUpdated1ms* obj);
    static void start();
    static void stop();
    static void reset();
};

#endif //UPDATER_H
