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
/*! \file sa1350Cmd.h */
#pragma once

/*!
 \brief Add brief

 \enum SA1350ErrorCode
*/
enum SA1350ErrorCode
{
    ERR_NO_ERROR               =    0,             /*!< Add in-line comment */
    ERR_CMDBUFFEROVERFLOW      =  800,             /*!< Add in-line comment */
    ERR_WRONGCMDLENGTH         =  801,             /*!< Add in-line comment */
    ERR_CMDABORTED             =  802,             /*!< Add in-line comment */
    ERR_LOSTCMD                =  803,             /*!< Add in-line comment */
    ERR_CMDUNKNOWN             =  804,             /*!< Add in-line comment */
    ERR_TOOMUCHDATAREQUESTEDBYUSERFUNCTION  = 805, /*!< Add in-line comment */
    ERR_RESTOREPROGRAMCOUNTER  =  806,             /*!< Add in-line comment */
    ERR_BUFFERPOSOUTOFRANGE    =  807,             /*!< Add in-line comment */
    ERR_EEQBUFFEROVERFLOW      =  808,             /*!< Add in-line comment */
    ERR_WRONGCRCLOWBYTE        =  809,             /*!< Add in-line comment */
    ERR_WRONGCRCHIGHBYTE       =  810,             /*!< Add in-line comment */
    ERR_RESTOREFROMPACKETERROR =  812,             /*!< Add in-line comment */
    ERR_NOFRAMESTART           =  813,             /*!< Add in-line comment */
    ERR_WRONGPKTLENGTH         =  814,             /*!< Add in-line comment */
    ERR_PACKETINCOMPLETE       =  815,             /*!< Add in-line comment */
    ERR_PACKETERROR            =  816,             /*!< Add in-line comment */
    ERR_STUPIDPACKETHANDLER    =  817,             /*!< Add in-line comment */
    ERR_BUFFEROVERFLOW         =  850,             /*!< Add in-line comment */
    ERR_BUFFERUNDERRUN         =  851,             /*!< Add in-line comment */
    ERR_FLASHNOTERASED         = 1100,             /*!< Add in-line comment */
    ERR_FLASHMISMATCH          = 1101,             /*!< Add in-line comment */
    ERR_RSSIVALIDFLAGNOTSET    = 1200,             /*!< Add in-line comment */
    ERR_PLLNOTSETTLED          = 1201              /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum SA1350Cmd
*/
enum SA1350Cmd
{
    CMD_NONE           =  0,  /*!< Noop Command                                             */

    // General Commands
    CMD_CONNECT        =  1,  /*!< Connect to device's comport                              */
    CMD_DISCONNECT     =  2,  /*!< Disconnect from device's comport                         */
    CMD_GETDEVICEVER   =  3,  /*!< Get device's number                                      */
    CMD_GETFWVER       =  4,  /*!< Get device's firmware version                            */
    CMD_GETRFPARAMS    =  5,  /*!< Get RF RBW table size, RBW values, maximum spectrum size */
    CMD_GETLASTERROR   =  6,  /*!< Indicate full spectrum received                          */

    // Frequency Commands
    CMD_SETFRANGE      =  20, /*!< Set Frequency Range frange                               */
    CMD_SETFSTART      =  21, /*!< Set Start Frequency fstart                               */
    CMD_SETFSTOP       =  22, /*!< Set Stop  Frequency fstop                                */
    CMD_SETSPANINDEX   =  23, /*!< Set Frequency  Span index                                */
    CMD_SETFSTEP       =  24, /*!< Set Step  Frequency fstep                                */
    CMD_SETRBW         =  25, /*!< Set Rx Filter bandwidth                                  */
    CMD_SETSTEPCOUNT   =  26, /*!< Set number of fsteps per MHz                             */
    CMD_SETSPAN        =  27, /*!< Set Frequency  Span fspan                                */

    // Spectrum Measurement Comman
    CMD_INITPARAMETER  =  30, /*!< Setup the system for spectrum measurement                */
    CMD_GETSPECNOINIT  =  31, /*!< Measures the spectrum previously defined                 */
};
