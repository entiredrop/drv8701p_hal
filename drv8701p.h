/**
 * @file drv8701p.h
 * @author Caio Cesar Cordeiro (caiocesarcordeiro@gmail.com)
 * @brief This is the header for the simple abstraction layer
 *        for the DRV8701P BDC motor driver.
 * @version 0.1
 * @date 2023-06-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdint.h>
#include "hal_aio.h"

/**
 * @brief Set maximum slew rate for 1% per callback.
 */
#define DRV8701P_MAX_SLEW_RATE HAL_IO_PWM_MAX/100

/**
 * @brief Indicates the PWM value for stop state.
 */
#define DRV8701P_STOP_SPEED 0

/**
 * @brief Indicates the maximum PWM of the driver.
 *        Redefinition for better portability of this module in the future.
 *        If we ever want to change the HAL_IO for a new microcontroller,
 *        we don't need to change the driver code, just these macros.
 */
#define DRV8701P_PWM_MAX HAL_IO_PWM_MAX

/**
 * @brief Indicates the PWM value that represents the brake
 */
#define DRV8701P_BRAKES_PWM HAL_IO_PWM_MAX

/**
 * @brief Structure with the possible directions that the BDC
 *        can be set to.
 */
typedef enum
{
    DRV8701P_STOP = 0,
    DRV8701P_FORWARD,
    DRV8701P_REVERSE,
} drv8701p_direction_t;

/**
 * @brief Function to be called back from a 1ms timer/scheduler.
 *        This function will set the new PWM value if needed
 *        every 1ms.
 */
void drv8701p_elapse_1ms(void);

/**
 * @brief Sets the desired direction and speed for the BDC motor.
 * 
 * @param command The desired direction for the BDC from drv8701p_direction_t.
 * @param speed The disired speed in thousands of percentage (100 = 10000).
 * @return uint8_t 0 if success, negative value otherwise.
 */
uint8_t drv8701p_set_command(drv8701p_direction_t command, uint16_t speed);
