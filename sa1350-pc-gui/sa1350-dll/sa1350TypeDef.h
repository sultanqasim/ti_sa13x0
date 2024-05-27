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
/*! \file sa1350TypeDef.h */
#pragma once
#include <list>
#include <string>
#include <vector>

using namespace std;

/*!
 \brief Add brief

 \typedef struct sComPortDescr sComPortDescr
*/
/*!
 \brief Add brief

 \struct sComPortDescr sa1350TypeDef.h "sa1350TypeDef.h"
*/
typedef struct sComPortDescr
{
 std::string Name; /*!< Add in-line comment */
 std::string Nr; /*!< Add in-line comment */
}sComPortDescr;

/*!
 \brief Add brief

 \typedef lComPortList
*/
typedef list<sComPortDescr>  lComPortList; /*!< Add in-line comment */

/*!
 \brief Add brief

 \typedef struct sUsbDeviceInfo sUsbDeviceInfo
*/
/*!
\brief Add brief

\struct sUsbDeviceInfo sa1350TypeDef.h "sa1350TypeDef.h"
*/
typedef struct sUsbDeviceInfo
 {
  std::string SerialNr;         /*!< Add in-line comment */
  std::string PortName;         /*!< Add in-line comment */
  std::string DevDesc;          /*!< Add in-line comment */
  std::string LocationInformation; /*!< Add in-line comment */
  std::string ParentIdPrefix;   /*!< Add in-line comment */
  bool   Connected;             /*!< Add in-line comment */
 }sUsbDeviceInfo;

/*!
 \brief Add brief

 \typedef struct sFrame sFrame
*/
/*!
 \brief Add brief

 \struct sFrame sa1350TypeDef.h "sa1350TypeDef.h"
*/
typedef struct sFrame
{
 unsigned char  Cmd;            /*!< Add in-line comment */
 unsigned char  Length;         /*!< Add in-line comment */
 std::string    Data;           /*!< Add in-line comment */
 unsigned short Crc;            /*!< Add in-line comment */
}sFrame;

/*!
 \brief Add brief

 \typedef vUsbDeviceList
*/
typedef vector<sUsbDeviceInfo> vUsbDeviceList; /*!< Add in-line comment */
