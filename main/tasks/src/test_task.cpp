#include "task_common.h"

#include "shtc3.h"
#include "common.h"

void testTask(void *pvParameters){
    SHTC3Sensor sensor(I2C_NUM_0, 22, 21);
    while (1){
        sensor.updateSomewhere();
        printf("Temperature: %.2f °C, Humidity: %.2f %%\n", sensor.getTemperature(), sensor.getHumidity());
        OsDelay(1);
    }
}