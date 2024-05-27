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
/*! \file sa1350.h */
#pragma once

using namespace std;

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SA1350_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// SA1350_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SA1350_EXPORTS
#define SA1350_API __declspec(dllexport) /*!< Add in-line comment */
#else
#define SA1350_API __declspec(dllimport) /*!< Add in-line comment */
#endif

//#define API_CALL   __stdcall /*!< Add in-line comment */
//#define API_CALL   __cdecl /*!< Add in-line comment */
#define API_CALL /*!< Add in-line comment */

#ifdef __cplusplus
extern "C"
{
#endif

#define FRAME_MAX_DATA_LENGTH	255 /*!< Add in-line comment */

/*!
 \brief Add brief

 \typedef struct sa1350UsbDevice SA1350UsbDevice
*/
/*!
 \brief Add brief

 \struct sa1350UsbDevice sa1350.h "sa1350.h"
*/
typedef struct sa1350UsbDevice
{
    char SerialNr[50];            /*!< Add in-line comment */
    char PortName[50];            /*!< Add in-line comment */
    char DevDesc[50];             /*!< Add in-line comment */
    char LocationInformation[50]; /*!< Add in-line comment */
    bool  Connected;              /*!< Add in-line comment */
}SA1350UsbDevice;

/*!
 \brief Add brief

 \typedef struct sa1350Frame SA1350Frame
*/
/*!
 \brief Add brief

 \struct sa1350Frame sa1350.h "sa1350.h"
*/
typedef struct sa1350Frame
{
    unsigned char  Cmd;            /*!< Add in-line comment */
    unsigned char  Length;         /*!< Add in-line comment */
    unsigned char  Data[FRAME_MAX_DATA_LENGTH]; /*!< Add in-line comment */
    unsigned short Crc;            /*!< Add in-line comment */
}SA1350Frame;

/*!
 \brief Add brief

 \typedef struct sa1350Status SA1350Status
*/
/*!
 \brief Add brief

 \struct sa1350Status sa1350.h "sa1350.h"
*/
typedef struct sa1350Status
{
    bool flagInit;                 /*!< Add in-line comment */
    bool flagOpen;                 /*!< Add in-line comment */
    bool flagUsbRemoval;           /*!< Add in-line comment */
    bool flagDrvError;             /*!< Add in-line comment */
    bool flagFrameReceived;        /*!< Add in-line comment */
    bool flagFrameError;           /*!< Add in-line comment */
    bool flagFrameErrorCrc;        /*!< Add in-line comment */
    bool flagFrameErrorTimeOut;    /*!< Add in-line comment */
}SA1350Status;

/*!
 \brief Returns SA1350 Interface Dll Version

 \return unsigned short
*/
SA1350_API unsigned short API_CALL sa1350GetDllVersion(void);

/*!
 \brief Returns number of detected and installed devices on the current PC

 \param NumDevices Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350FindDevices(unsigned short &NumDevices);

/*!
 \brief Returns SA1350 USB Interface Device Information

 \param DevNr Add param
 \param DevInfo Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350GetDeviceInfo(unsigned short DevNr, sa1350UsbDevice *DevInfo);

/*!
 \brief Init the SA1350 Interface Dll

 \return bool
*/
SA1350_API bool API_CALL sa1350Init(void);

/*!
 \brief Checks if all Objects are created and successfully initialized

 \return bool
*/
SA1350_API bool API_CALL sa1350IsInit(void);

/*!
 \brief Delete all generated objects

*/
SA1350_API void API_CALL sa1350DeInit(void);

/*!
 \brief Returns the current device status structure sa1350Status

 \param Status Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350GetStatus(sa1350Status &Status);

/*!
 \brief Connect dedicated device, given by the comport name

 \param strPort Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350Connect(const char *strPort);

/*!
 \brief Checks if the comport was successfully opened

 \return bool
*/
SA1350_API bool API_CALL sa1350IsConnected(void);

/*!
 \brief Disconnect active comport

 \return bool
*/
SA1350_API bool API_CALL sa1350Disconnect(void);

/*!
 \brief Checks if comport has received new frames from the SA1350 device in the meantime

 \return bool
*/
SA1350_API bool API_CALL sa1350IsFrameAvailable(void);

/*!
 \brief Return the actual number of received frames within the Frame Fifo Buffer

 \param Size Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350GetFrameFifoSize(unsigned short &Size);

/*!
 \brief Return the topmost frame from the Frame Fifo Buffer

 \param Frame Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350GetFrame(sa1350Frame *Frame);

/*!
 \brief Sends a Frame to the SA1350 Device

 \param Cmd Add param
 \param Data Add param
 \param Size Add param
 \return bool
*/
SA1350_API bool API_CALL sa1350SendCmd(unsigned char Cmd, unsigned char *Data, unsigned short Size);

#ifdef __cplusplus
}
#endif
