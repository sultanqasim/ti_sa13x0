/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 */

/*
 *
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@             @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@             @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@             @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@            @@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@            @@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@            @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@            @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@            @@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@           @@@@@@@@@@          @@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@           @@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@           @@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@           @@@@@@@@@@          @@@@@@@@@@@@@@@@@
 *                          @@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@@           @@@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@           @@@@@@@@@           @@@@@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     @@@@@@@@@     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                         @@@@@@@@@@@@@@@@@@@@@@@@@
 *             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@@@@
 *              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@@@@@@@@@@@@@@
 *              @@@@@@@@@@@@@@@      @@@@@@@@@@@                         @@@@@@@@@@@@@@@@@@@@
 *               @@@@@@@@@@@@         @@@@@@@@@@                         @@@@@@@@@@@@@@@@@
 *               @@@@@@@@@@@           @@@@@@@@@                        @@@@@@@@@@@@@@@@
 *                @@@@@@@@@@           @@@@@@@@@                        @@@@@@@@@@@@@@
 *                 @@@@@@@@             @@@@@@@@@                       @@@@@@@@@@@@
 *                   @@@@                @@@@@@@@                       @@@@@@@@@@@
 *                                       @@@@@@@@@                      @@@@@@@@@@
 *                                        @@@@@@@@@                     @@@@@@@@
 *                                        @@@@@@@@@@@@@                @@@@@@@@
 *                                         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                            @@@@@@@@@@@@@@@@@@@@@@@@@@@
 *                                             @@@@@@@@@@@@@@@@@@@@@@@@@
 *                                             @@@@@@@@@@@@@@@@@@@@@@@@@
 *                                              @@@@@@@@@@@@@@@@@@@@@@@@
 *                                              @@@@@@@@@@@@@@@@@@@@@@@
 *                                               @@@@@@@@@@@@@@@@@@@@@@
 *                                               @@@@@@@@@@@@@@@@@@@@@@
 *                                                @@@@@@@@@@@@@@@@@@@@@
 *                                                 @@@@@@@@@@@@@@@@@@@@
 *                                                 @@@@@@@@@@@@@@@@@@@@
 *                                                  @@@@@@@@@@@@@@@@@@@
 *                                                   @@@@@@@@@@@@@@@@@@
 *                                                    @@@@@@@@@@@@@@@@@@
 *                                                     @@@@@@@@@@@@@@@@@
 *                                                      @@@@@@@@@@@@@@@@
 *                                                        @@@@@@@@@@@@@@
 *                                                          @@@@@@@@@@@@@
 *                                                            @@@@@@@@@
 */
#include <ti/grlib/grlib.h>
#include <stdint.h>

uint32_t imgPalette[] = {0U, 0xFFFFFFU};

const uint8_t imgData[402] = {
    0x33U, 0x2CU, 0x91U, 0x2CU, 0x91U, 0x2CU, 0x91U, 0x2CU, 0x91U, 0x2CU, 0x91U, 0x2CU, 0x19U,
    0x10U, 0x65U, 0x2CU, 0x19U, 0x10U, 0x65U, 0x2CU, 0x19U, 0x10U, 0x65U, 0x2CU, 0x17U, 0x12U,
    0x65U, 0x2CU, 0x17U, 0x12U, 0x65U, 0x2CU, 0x17U, 0x10U, 0x67U, 0x2CU, 0x17U, 0x10U, 0x67U,
    0x2CU, 0x17U, 0x10U, 0x67U, 0x2CU, 0x15U, 0x12U, 0x67U, 0x2CU, 0x91U, 0x2CU, 0x91U, 0x2CU,
    0x91U, 0x2CU, 0x91U, 0x38U, 0x09U, 0x10U, 0x07U, 0x24U, 0x3BU, 0x38U, 0x07U, 0x12U, 0x07U,
    0x24U, 0x3BU, 0x38U, 0x07U, 0x10U, 0x07U, 0x26U, 0x3BU, 0x38U, 0x07U, 0x10U, 0x07U, 0x26U,
    0x3BU, 0x38U, 0x07U, 0x10U, 0x07U, 0x26U, 0x3BU, 0x36U, 0x09U, 0x10U, 0x07U, 0x26U, 0x3BU,
    0x36U, 0x07U, 0x12U, 0x07U, 0x26U, 0x3BU, 0x36U, 0x07U, 0x10U, 0x07U, 0x28U, 0x3BU, 0x36U,
    0x07U, 0x10U, 0x07U, 0x28U, 0x3BU, 0x34U, 0x09U, 0x10U, 0x07U, 0x28U, 0x3BU, 0x34U, 0x07U,
    0x12U, 0x07U, 0x28U, 0x3BU, 0x34U, 0x07U, 0x12U, 0x07U, 0x28U, 0x3BU, 0x26U, 0x15U, 0x12U,
    0x13U, 0x1CU, 0x3BU, 0x26U, 0x15U, 0x10U, 0x15U, 0x1CU, 0x3BU, 0x26U, 0x15U, 0x10U, 0x15U,
    0x1CU, 0x3BU, 0x26U, 0x13U, 0x12U, 0x15U, 0x1EU, 0x39U, 0x26U, 0x13U, 0x12U, 0x15U, 0x1EU,
    0x39U, 0x24U, 0x15U, 0x12U, 0x13U, 0x20U, 0x39U, 0x24U, 0x15U, 0x10U, 0x15U, 0x22U, 0x05U,
    0x56U, 0x15U, 0x10U, 0x15U, 0x22U, 0x05U, 0x56U, 0x15U, 0x10U, 0x15U, 0x24U, 0x03U, 0x54U,
    0x15U, 0x12U, 0x15U, 0x26U, 0x01U, 0x54U, 0x15U, 0x10U, 0x15U, 0x8EU, 0x07U, 0x10U, 0x07U,
    0x9AU, 0x09U, 0x10U, 0x07U, 0x9AU, 0x09U, 0x10U, 0x07U, 0x9AU, 0x07U, 0x12U, 0x07U, 0x38U,
    0x01U, 0x5EU, 0x07U, 0x10U, 0x07U, 0x3AU, 0x03U, 0x5CU, 0x07U, 0x10U, 0x07U, 0x3AU, 0x05U,
    0x58U, 0x09U, 0x10U, 0x07U, 0x3AU, 0x07U, 0x56U, 0x07U, 0x12U, 0x07U, 0x3AU, 0x0BU, 0x52U,
    0x07U, 0x12U, 0x07U, 0x3AU, 0x0DU, 0x50U, 0x07U, 0x12U, 0x07U, 0x3AU, 0x0DU, 0x50U, 0x07U,
    0x10U, 0x07U, 0x3CU, 0x0FU, 0x4EU, 0x07U, 0x10U, 0x07U, 0x3CU, 0x11U, 0x4AU, 0x09U, 0x10U,
    0x07U, 0x3CU, 0x13U, 0x48U, 0x07U, 0x12U, 0x07U, 0x3CU, 0x13U, 0x48U, 0x07U, 0x10U, 0x07U,
    0x3EU, 0x15U, 0x46U, 0x07U, 0x10U, 0x07U, 0x3EU, 0x15U, 0x46U, 0x07U, 0x10U, 0x07U, 0x3EU,
    0x15U, 0x44U, 0x09U, 0x10U, 0x09U, 0x3CU, 0x17U, 0x42U, 0x31U, 0x30U, 0x17U, 0x42U, 0x2FU,
    0x32U, 0x19U, 0x40U, 0x2FU, 0x2CU, 0x1FU, 0x1CU, 0x0BU, 0x14U, 0x31U, 0x26U, 0x27U, 0x16U,
    0x11U, 0x12U, 0x31U, 0x20U, 0x2DU, 0x14U, 0x15U, 0x10U, 0x2FU, 0x1EU, 0x33U, 0x12U, 0x15U,
    0x10U, 0x2FU, 0x1AU, 0x39U, 0x0EU, 0x19U, 0x10U, 0x2DU, 0x16U, 0x41U, 0x06U, 0x1FU, 0x0EU,
    0x2DU, 0x14U, 0x6BU, 0x10U, 0x2BU, 0x12U, 0x6FU, 0x10U, 0x29U, 0x0EU, 0x73U, 0x18U, 0x1FU,
    0x0EU, 0x77U, 0x44U, 0x79U, 0x42U, 0x7DU, 0x3EU, 0x7FU, 0x3CU, 0x83U, 0x3AU, 0x83U, 0x38U,
    0x85U, 0x36U, 0x89U, 0x34U, 0x8BU, 0x30U, 0x8DU, 0x30U, 0x8FU, 0x2EU, 0x8FU, 0x2CU, 0x93U,
    0x2AU, 0x93U, 0x2AU, 0x95U, 0x28U, 0x97U, 0x26U, 0x97U, 0x26U, 0x99U, 0x24U, 0x9BU, 0x22U,
    0x9DU, 0x22U, 0x9DU, 0x20U, 0x9FU, 0x1EU, 0xA3U, 0x1AU, 0xA7U, 0x18U, 0xA9U, 0x10U,
};

const tImage splashImage = {
    .bPP = IMAGE_FMT_1BPP_COMP_RLE7,
    .numColors = 2U,
    .xSize = 96U,
    .ySize = 96U,
    .pPalette = imgPalette,
    .pPixel = imgData
};