/*
 * Copyright (c) 2018 - 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

 /**
  @addtogroup mpsl
  @{
  @defgroup mpsl_lib_interface MPSL library general interface
  @{

*/

#ifndef MPSL_H__
#define MPSL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_errno.h"
#include "mpsl_clock.h"

/** @brief High ISR priority
 *
 *  High ISR priority is used for realtime scheduling. It
 *  should not be used except when inside radio
 *  timeslot by the owner of the timeslot */
#define MPSL_HIGH_IRQ_PRIORITY 0

/** @brief    Function prototype for the assert handler.
 *
 * @note      If an internal assert occurs this function is called. It is supposed to log the assert and stop execution.
 *
 * @param[in] file   The filename where the assertion occurred.
 * @param[in] line   The line number where the assertion occurred.
 */
typedef void (*mpsl_assert_handler_t)(const char * const file, const uint32_t line);

/** @brief      MPSL initialization
 *
 * @note       This function can be called by multiple protocols
 *             The input parameters should be the same as the first call.
 *
 * @param[in]  p_clock_config    Clock configuration.
                                 If NULL the LF clock will be configured as an RC source with rc_ctiv =
                                 @ref MPSL_RECOMMENDED_RC_CTIV and .rc_temp_ctiv =
                                 @ref MPSL_RECOMMENDED_RC_TEMP_CTIV.
 * @param[in]  low_prio_irq      IRQ to pend when low priority processing should be executed. The application
 *                               shall call @ref mpsl_low_priority_process after this IRQ has occurred.
 * @param[in]  p_assert_handler  Pointer to MPSL assert handler.
 *
 *
 * @retval  0               MPSL is successfully initialized.
 * @retval  - ::NRF_EPERM   MPSL is already initialized with different parameters.
 */
int32_t mpsl_init(mpsl_clock_lf_cfg_t const * p_clock_config, IRQn_Type low_prio_irq, mpsl_assert_handler_t p_assert_handler);

/** @brief      Uninitialize MPSL. Stops clocks and scheduler. This will release all peripherals and
 *             reduce power usage.
 *
 * @note       This function assumes no protocol stacks are running, and no timeslots are requested.
 *             All initialized protocol stacks need to be stopped before calling this function.
 *             Failing to do so will lead to undefined behavior.
 */
void mpsl_uninit(void);

/** @brief      Returns true if MPSL is already initialized, false otherwise.
 */
bool mpsl_is_initialized(void);

/** @brief      RADIO interrupt handler
 *
 * @note       This handler should be placed in the interrupt vector table.
 *             The interrupt priority level should be priority 0.
 */
void MPSL_IRQ_RADIO_Handler(void);

/** @brief      RTC0 interrupt handler
 *
 * @note       This handler should be placed in the interrupt vector table.
 *             The interrupt priority level should be priority 0
 */
void MPSL_IRQ_RTC0_Handler(void);

/** @brief      TIMER0 interrupt handler.
 *
 * @note       This handler should be placed in the interrupt vector table.
 *             The interrupt priority level should be priority 0
 */
void MPSL_IRQ_TIMER0_Handler(void);

/** @brief      POWER_CLOCK interrupt handler
 *
 * @note       This handler should be placed in the interrupt vector table.
 *             The interrupt priority level should be lower than priority 0.
 */
void MPSL_IRQ_CLOCK_Handler(void);

/** @brief MPSL low priority processing handler.
 *
 * @note This handler should be called when MPSL signals low priority processing should be executed
 *       (via low_prio_irq provided to @ref mpsl_init) within reasonable time (a at least a few
 *       100 ms). The caller is responsible to ensure this function is not called concurrently with
 *       any other low priority MPSL API functions, for more information see thread safety in the
 *       MPSL documentation.
 */
void mpsl_low_priority_process(void);


#ifdef __cplusplus
}
#endif

#endif // MPSL_H__

/**
  @}
  @}
 */
