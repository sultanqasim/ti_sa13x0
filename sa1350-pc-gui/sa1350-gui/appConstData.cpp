/* --COPYRIGHT--,BSD
 * Copyright (c) 2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#include "appTypedef.h"

const sFrqRefLevel FrqTableRefLevel[MAX_ITEMS_REFLEVEL]=
{
    {-35,128},
    {-40,144},
    {-45,145},
    {-50, 74},
    {-55, 12},
    {-60,179},
    {-65, 44},
    {-70, 61}
};

/* TODO Pull table from device on connect */
const sFrqSpan FrqTableSpan[MAX_ITEMS_SPAN]=
{
    {24, 83.328, 7}, /* 195.9 KHz */
    {18, 62.500, 6}, /* 155.4 KHz */
    {16, 55.542, 5}, /* 117.7 KHz */
    {12, 41.656, 3}, /*  77.7 KHz */
    { 8, 27.771, 2}, /*  58.9 KHz */
    { 6, 20.828, 1}, /*  49.0 KHz */
    { 4, 13.885, 0}, /*  38.9 KHz */
    { 2,  6.943, 0}, /*  38.9 KHz */
    { 1,  3.464, 0}  /*  38.9 KHz */
};

/* TODO Pull table from device on connect */
const sFrqRange FrqTableRange[MAX_ITEMS_RANGE]=
{
    {431, 527, 444},
    {861, 1054, 915},
    {2152, 2635, 2440}
};

const QVector<sBrdRfLimits> BrdTableRFLimits =
{
  // QString, bool,         bool
  // BoardID, Band900Tuned, Band440Tuned
  {"L200",    true,         false},   // CC1310 LP
  {"L201",    true,         false},   // CC1350-CC1190 LP
  {"L400",    true,         false},   // CC1350 LP
  {"L401",    false,        true },   // CC1350-433 LP
  {"L410",    true,         false}    // CC1352 LP
};
