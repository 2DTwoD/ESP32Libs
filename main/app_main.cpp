#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "simple_input.h"
#include "on_delay.h"
#include "pulse.h"
#include "off_delay.h"
#include "simple_input_delayed.h"


extern "C" void app_main(void) {
    Coil coil(2);
    SimpleInputDelayed simpleInput(13, 1000);
    OnDelay on(1000);
    OffDelay off(1000);
    Pulse pulse(2000);
    PulseInterrapt pulseI(2000);
    while(1){
        pulse = simpleInput.isActive();
        coil = pulse.get();
        printf("time: %lu\n", pulse.remain());
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
