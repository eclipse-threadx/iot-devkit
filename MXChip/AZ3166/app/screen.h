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

#ifndef _SCREEN_H
#define _SCREEN_H

/* Enumration for line on the screen */
typedef enum
{
    L0 = 0,
    L1 = 18,
    L2 = 36,
    L3 = 54
} LINE_NUM;

void screen_print(char* str, LINE_NUM line);
void screen_printn(const char* str, unsigned int str_length, LINE_NUM line);

#endif // _SCREEN_H