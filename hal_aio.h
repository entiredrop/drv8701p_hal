#include <stdint.h>

#define HAL_IO_PWM_MAX 10000 // corresponds to 100.00% pwm

typedef enum
{
    HAL_AIO_nSLEEP,
    HAL_AIO_IN1,
    HAL_AIO_IN2,
} hal_aio_output_t;

void hal_aio_set_output( hal_aio_output_t output, uint16_t pwm);
