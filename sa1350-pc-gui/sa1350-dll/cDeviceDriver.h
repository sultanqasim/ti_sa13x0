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
/*! \file cDeviceDriver.h */
#pragma once

using namespace std;

#include "cDriver.h"
#include "cMutex.h"
#include "cThread.h"
#include "sa1350TypeDef.h"

/*!
 \brief Frame Decoder States

 \enum DecoderState
*/
enum DecoderState
{
    DS_WAITSMARKER = 0, /*!< Add in-line comment */
    DS_WAITLENGTH,      /*!< Add in-line comment */
    DS_WAITCMD,         /*!< Add in-line comment */
    DS_WAITDATA,        /*!< Add in-line comment */
    DS_CRC_HIGH,        /*!< Add in-line comment */
    DS_CRC_LOW,         /*!< Add in-line comment */
};

/*!
 \brief Comport driver thread States

 \enum DriverState
*/
enum DriverState
{
    DRV_INIT=0, /*!< Add in-line comment */
    DRV_RUN,    /*!< Add in-line comment */
    DRV_STOP,   /*!< Add in-line comment */
    DRV_EXIT,   /*!< Add in-line comment */
};

/*!
 \brief TODO Add brief

 \typedef struct _sFrameDecoderState sFrameDecoderState
*/
/*!
 \brief Decoder Frame Format

 \struct _sFrameDecoderState cDeviceDriver.h "cDeviceDriver.h"
*/
typedef struct _sFrameDecoderState
{
    DecoderState  State; /*!< Add in-line comment */
    unsigned short DataIndex; /*!< Add in-line comment */
    unsigned short CrcIndex; /*!< Add in-line comment */
    unsigned short Crc; /*!< Add in-line comment */
    sFrame        Frame; /*!< Add in-line comment */
}sFrameDecoderState;

/*!
 \brief SA1350 Driver Class

 \class cDeviceDriver cDeviceDriver.h "cDeviceDriver.h"
*/
class cDeviceDriver
{
public:
    /*!
     \brief Constructor

    */
    cDeviceDriver();
    /*!
     \brief Destructor

    */
    ~cDeviceDriver();

    /*!
     \brief Check if SA1350 driver class is init

    */
    bool IsInit(void);
    /*!
     \brief Open SA1350 device

     \param strPort
    */
    bool Open(std::string strPort);
    /*!
     \brief Check if SA1350 is currently open

    */
    bool IsOpen(void);
    /*!
     \brief Check if SA1350 lowlevel driver is ok

    */
    bool IsDrvOk(void);
    /*!
     \brief Close actual opened SA1350 device

    */
    bool Close(void);
    /*!
     \brief Reset actual opened SA1350 driver

    */
    bool Reset(void);
    /*!
     \brief Send one frame to the SA1350 device

     \param Cmd Add param
     \param Data Add param
     \param size Add param
    */
    bool SendFrame(unsigned char Cmd, unsigned char *Data, unsigned short size);
    /*!
     \brief Checks if the receiver frame buffer is empty

    */
    bool IsFrameFifoEmpty(void);
    /*!
     \brief Return the actual number of received answer frames

     \param Size Add param
    */
    bool GetFrameFifoSize(unsigned short &Size);
    /*!
     \brief returns the first received frame from the Fifo and delete it from the Fifo

     \param Frame Add param
    */
    bool GetFrame(sFrame *Frame);
    /*!
     \brief Check if in the  meantime framed from the SA1350 device are received

    */
    bool HasFrameReceived(void);
    /*!
     \brief Checks the Crc value of the received SA1350 frame

    */
    bool HasFrameCrcError(void);
    /*!
     \brief Checks if the frame was received in-time

    */
    bool HasFrameTimeoutError(void);

private:
    // Device Driver Variables
    cEvent *eDrvOpen;       /*!< Driver Event */
    cEvent *eDecoderInit;   /*!< Decoder Event */
    cEvent *eDecoderStart;  /*!< Decoder Start Event */
    cEvent *eDecoderStop;   /*!< Decoder Stop Event */
    cEvent *eDecoderExit;   /*!< Decoder Exit Event */

    bool flagCreated;       /*!< Creat Flag */
    volatile bool flagOpen; /*!< Open Flag */

    /*!
     \brief Generate SA1350 frame based on Cmd, Date and payload length

     \param Frame Add param
     \param Cmd Add param
     \param Data Add param
     \param Length Add param
    */
    void MakeFrame(sFrame *Frame, unsigned char Cmd, unsigned char *Data, unsigned short Length);
    /*!
     \brief Converts byte stream to a SA1350 frame

     \param Raw Add param
     \param Frame Add param
    */
    void Raw2Frame(std::string &Raw, sFrame &Frame);
    /*!
     \brief Converts a SA1350 frame to a raw byte stream

     \param Frame Add param
     \param Raw Add param
    */
    void Frame2Raw(sFrame &Frame, std::string &Raw);
    // Crc16 Routines
    /*!
     \brief Calclulate a 16 Bit Crc out of a byte array

     \param Raw Add param
    */
    unsigned short calcCrc16(std::string &Raw);
    /*!
     \brief Add byte to the 16 bit Crc summ

     \param crc Add param
     \param u8 Add param
    */
    void crc16AddByte(unsigned short &crc, unsigned char u8);

    //Driver Variables and Functions
    cMutex  DrvAccess;  /*!< Driver Mutex Lock */
    cEvent  *eDrvError; /*!< Driver Error Event */

    cDriver *Drv;       /*!< Driver */
    sError  Error;      /*!< Error  */

    /*!
     \brief Open comport based on std string

     \param strPort Add param
    */
    bool drvOpen(std::string strPort);
    /*!
     \brief Close active comport

    */
    bool drvClose(void);
    /*!
     \brief Send given SA1350 Cmd Frame

     \param Frame Add param
    */
    bool drvSendFrame(sFrame *Frame);
    /*!
     \brief Read complete comport rx buffer and copy to std string buffer

     \param rxFifo Add param
    */
    bool drvGetData(std::string *rxFifo);
    /*!
     \brief Signals an error from the comport thread to upper class level

     \param Code
     \param Msg
    */
    void drvSignalError(unsigned int Code, std::string Msg);
    /*!
     \brief Signals Error Reset to received comport thread

    */
    void drvSignalErrorReset(void);

    // Thread Variables and Functions
    TThread<cDeviceDriver> *ThreadHandle; /*!< Add in-line comment */
    volatile bool flagExitThread; /*!< Add in-line comment */

    /*!
     \brief Comport receiver thread loop

    */
    void run(void);

    // FrameDecoder Variables and Functions
    cEvent  *eventFrameReceived; /*!< Add in-line comment */
    cEvent  *eventFrameErrorCrc; /*!< Add in-line comment */
    cEvent  *eventFrameErrorTimeOut; /*!< Add in-line comment */

    sFrameDecoderState DecoderState; /*!< Add in-line comment */

    /*!
     \brief Init SA1350 Frame Decoder

    */
    void FrameDecoderInit(void);

    // Frame Variables and Functions
    cMutex  FrameFifoAccess; /*!< Add in-line comment */
    list<sFrame> *FrameFifo; /*!< Add in-line comment */

    /*!
     \brief

    */
    int  FrameReceived(void);
    /*!
     \brief Push frame to the Fifo

     \param Frame Add param
    */
    bool FramePush(sFrame *Frame);
    /*!
     \brief Pop Frame from the Fifo

     \param Frame Add param
    */
    bool FramePop(sFrame *Frame);
    /*!
     \brief Decode input stream data

     \param rxFifo Add param
    */
    bool FrameDecoder(std::string *rxFifo);
    /*!
     \brief Reset frame content to initial values

     \param Frame Add param
    */
    void ResetFrame(sFrame *Frame);
};
