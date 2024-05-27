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
/*! \file cRegAccess.h */
#pragma once
#include <windows.h>

using namespace std;

#include "sa1350TypeDef.h"

#define MAX_KEY_LENGTH 255   /*!< Add in-line comment */
#define MAX_VALUE_NAME 16383 /*!< Add in-line comment */

/*!
 \brief Add brief

 \class cRegAccess cRegAccess.h "cRegAccess.h"
*/
class cRegAccess
{
public:
    /*!
     \brief Constructor

    */
    cRegAccess(void);
    /*!
     \brief Destructor

    */
    virtual ~cRegAccess(void);

    /*!
     \brief Load the active comport list from the Windows OS Registry

     \param comPortList
     \return bool
    */
    bool LoadComPortList(lComPortList *comPortList);
    /*!
     \brief Return all installed SA1350 Device informations

     \param usbDeviceList
     \return bool
    */
    bool LoadRegDeviceList(vUsbDeviceList *usbDeviceList);
    /*!
     \brief Compare the USB Device information list with given comport name

     \param port
     \param UsbSerial
     \return bool
    */
    bool GetPortSerial(std::string port, std::string & UsbSerial);
    /*!
     \brief Returns the dedicated SA1350 USB device information

     \param item
     \return bool
    */
    bool LoadRegDeviceDetails(vUsbDeviceList::iterator item);

private:
    /*!
     \brief

     \class _sRegEntry cRegAccess.h "cRegAccess.h"
    */
    typedef struct sRegEntry
    {
        std::string strKey;    /*!< Add in-line comment */
        std::string strValue;  /*!< Add in-line comment */
    }sRegEntry;

    /*!
     \brief Returns the specific Windows Registry Key value(s)

     \param hKey
     \param regEntryList
     \return bool
    */
    bool RegGetKeyValues(HKEY hKey, list<sRegEntry> *regEntryList);
};
