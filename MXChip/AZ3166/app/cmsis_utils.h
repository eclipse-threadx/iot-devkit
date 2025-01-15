/* 
 * Copyright (c) Microsoft
 * Copyright (c) 2024 Eclipse Foundation
 * 
 *  This program and the accompanying materials are made available 
 *  under the terms of the MIT license which is available at
 *  https://opensource.org/license/mit.
 * 
 *  SPDX-License-Identifier: MIT
 * 
 *  Contributors: 
 *     Microsoft         - Initial version
 *     Frédéric Desbiens - 2024 version.
 */

#ifndef _CMSIS_UTILS_H
#define _CMSIS_UTILS_H

static __inline void systick_interval_set(uint32_t ticks)
{
    // 1. Disable the counter
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    
    // 2. Update the Systick timer period
    SysTick->LOAD = SystemCoreClock / ticks - 1;

    // 3. Clear the current value
    SysTick->VAL  = 0;

    // 4. Enable the counter
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

#endif
