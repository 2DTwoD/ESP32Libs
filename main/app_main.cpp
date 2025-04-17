#include "coil.h"
#include "simple_input.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern "C" void app_main(void) {
    Coil coil(2);
    SimpleInput simpleInput(13);
    while(1){
        coil.toggle();
        printf("current input 13 state: %d\n", simpleInput.isActive());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
