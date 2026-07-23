#include "headfile.h"
#include "robot_mission.h"

int main(void)
{
    if (system_init() != ML_STATUS_OK) {
        while (1) {
        }
    }
    if (robot_mission_init() != ML_STATUS_OK) {
        while (1) {
            board_led_toggle();
            delay_ms(200U);
        }
    }

    while (1) {
        robot_mission_poll();
        __WFI();
    }
}
