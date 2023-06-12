/**
 * @file drv8701p.c
 * @author Caio Cesar Cordeiro (caiocesarcordeiro@gmail.com)
 * @brief This is a simple abstraction layer for the DRV8701P.
 * @version 0.1
 * @date 2023-06-11
 * 
 * @copyright Copyright (c) 2023
 * 
 * Assumptions made:
 * 1 - The PWM signal is not inverted on the peripheral, meaning that
 *     a 30% duty cycle will yeald a signal high for 30% of the time
 *     and a low signal for 70% of the time.
 * 2 - To achieve the low-side
 *     slow decay at every off portion of the PWM cycle, it is needed
 *     to switch the low side, and maintain the high side always on.
 * 
 * Thigs to improve:
 * 1 - Use a structure that can store all the variables for the BDC,
 *     in a way that the driver is generic enough to handle any amount
 *     of BDC in the system.
 * 2 - In the case that the directio is suddently changed from forward
 *     to reverse, this driver will not brake the motor before changing
 *     direction, which can be a problem for the motor driver component
 *     itself. Thus a soft stop and change of directions is also needed.
 */

#include <drv8701p.h>

/**
 * @brief Indicates the target PWM for the driver.
 */
uint16_t drv8701p_target_pwm = DRV8701P_STOP_SPEED;

/**
 * @brief Indicates the current PWM for the driver.
 */
uint16_t drv8701p_current_pwm = DRV8701P_STOP_SPEED;

/**
 * @brief Indicates which PWM output is being currently switched.
 */
hal_aio_output_t drv8701p_switching_output = HAL_AIO_IN1;

/**
 * @brief Indicates the desired command to the BDC.
 */
drv8701p_direction_t drv8701_direction = DRV8701P_STOP;

/**
 * @brief Sets the desired direction and speed for the BDC motor.
 * 
 * @param command The desired direction for the BDC from drv8701p_direction_t.
 * @param speed The disired speed in thousands of percentage (100 = 10000).
 * @return uint8_t 0 if success, negative value otherwise.
 */
uint8_t drv8701p_set_command(drv8701p_direction_t command, uint16_t speed)
{

    // Return variable
    uint8_t ret = 0;

    /**
     * Disable interrupt or delete task for the 1ms callback.
     */

    // If the speed is 0, no matter the received command, we will stop
    if (DRV8701P_STOP_SPEED == speed)
    {
        command = DRV8701P_STOP;
    }
    
    switch (command)
    {
        case DRV8701P_FORWARD:
        {
            // Set IN1 to max PWM (high side)
            hal_aio_set_output(HAL_AIO_IN1, DRV8701P_PWM_MAX);

            // Switch IN2 for PWM (low side)
            drv8701p_switching_output = HAL_AIO_IN2;

            break;
        }
        case DRV8701P_REVERSE:
        {
            // Set IN2 to max PWM (high side)
            hal_aio_set_output(HAL_AIO_IN2, DRV8701P_PWM_MAX);

            // Switch IN1 for PWM (low side)
            drv8701p_switching_output = HAL_AIO_IN1;

            break;
        }
        case DRV8701P_STOP:
        {
            // Apply brakes
            hal_aio_set_output(HAL_AIO_IN1, DRV8701P_BRAKES_PWM);
            hal_aio_set_output(HAL_AIO_IN2, DRV8701P_BRAKES_PWM);

            // Set target and current speed to brakes.
            drv8701p_target_pwm = DRV8701P_BRAKES_PWM;
            drv8701p_current_pwm = DRV8701P_BRAKES_PWM;

            break;
        }
        default:
        {
            // Keep doing the same thing and return error
            ret = -1;
        }
    }

    /**
     * Enable interrupt or create task for 1ms callback again.
     */

    return ret;
}

/**
 * @brief Function to be called back from a 1ms timer/scheduler.
 *        This function will set the new PWM value if needed
 *        every 1ms.
 */
void drv8701p_elapse_1ms(void)
{

    // Variable for the inverted PWM signal
    uint16_t applied_pwm = 0;

    // First check if the current pwm is the same as target to reduce overhead
    if (drv8701p_target_pwm == drv8701p_current_pwm)
    {
        return;
    }

    /**
     * The operations below are performed with sum instead of subtraction
     * to avoid underflow on the variables. Thus it may seems confusing,
     * but if we did "drv8701p_current_pwm - DRV8701P_MAX_SLEW_RATE" an
     * underflow could happen.
     */

    // Check if needs to increase speed
    if (drv8701p_current_pwm + DRV8701P_MAX_SLEW_RATE < drv8701p_target_pwm)
    {
        drv8701p_current_pwm += DRV8701P_MAX_SLEW_RATE;
    }
    // Check if needs to decrease speed
    else if (drv8701p_current_pwm > drv8701p_target_pwm + DRV8701P_MAX_SLEW_RATE )
    {
        drv8701p_current_pwm -= DRV8701P_MAX_SLEW_RATE;
    }
    // If none of the conditions are met, set the current to the target
    else
    {
        drv8701p_current_pwm = drv8701p_target_pwm;
    }

    /**
     * Invert PWM value so assumption 1 can be met. Theoretically
     * this operation would not be needed had the previous part, where
     * we increse/decrease the PWM, been done according to the inverted
     * logic, but this way it is easier to understand the code and logig
     * of operation.
     */
    applied_pwm = DRV8701P_PWM_MAX - drv8701p_current_pwm;

    // Set new PWM value
    hal_aio_set_output(drv8701p_switching_output, applied_pwm);
}
