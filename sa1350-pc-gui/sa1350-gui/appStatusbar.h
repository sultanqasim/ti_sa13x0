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
/*! \file appStatusBar.h */
#pragma once
#include <QLabel>
#include <QStatusBar>

#include "../sa1350-dll/sa1350.h"

/*!
 \brief Add brief

 \class appStatusBar appStatusBar.h "appStatusBar.h"
*/
class appStatusBar: public QObject
{
    Q_OBJECT

public:
    /*!
     \brief Constructor

     \param statusBar
    */
    appStatusBar(QStatusBar *statusBar);
    /*!
     \brief Destructor

    */
    ~appStatusBar();

    /*!
     \brief Add brief

     \param VerDll
     \param VerGui
    */
    void SetDllGuiVersion(unsigned short VerDll,unsigned short VerGui);
    /*!
     \brief Add brief

     \param DevInfo
    */
    void SetDeviceConnected(sa1350UsbDevice *DevInfo);
    /*!
     \brief Add brief

    */
    void SetDeviceDisconnected(void);

private slots:
    /*!
     \brief Add brief

    */
    void eventTimerTick(void);

private:
    QStatusBar *sbCtrl;       /*!< Add in-line comment */

    QPixmap *imgConnected;    /*!< Add in-line comment */
    QPixmap *imgDisconnected; /*!< Add in-line comment */
    QLabel  txtStatusConnect; /*!< Add in-line comment */
    QLabel  txtStatusDllVer;  /*!< Add in-line comment */
    QLabel  txtTime;          /*!< Add in-line comment */
    QLabel  imgLogo;          /*!< Add in-line comment */

    QTimer  *timerCtrl;       /*!< Add in-line comment */

    /*!
     \brief Add brief

    */
    void Init(void);
    /*!
     \brief Add brief

    */
    void Update(void);
};
