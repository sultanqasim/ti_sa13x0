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
/*! \file appFwUpdater.h */
#pragma once
#include <QDialog>
#include <QProcess>
#include <QStringListModel>

namespace Ui {
class appFwUpdater;
}

/*!
 \brief Add brief

 \class appFwUpdater appFwUpdater.h "appFwUpdater.h"
*/
class appFwUpdater : public QDialog
{
    Q_OBJECT

public:
    /*!
     \brief Constructor

     \param parent
    */
    explicit appFwUpdater(QWidget *parent = 0);
    /*!
     \brief Destructor

    */
    ~appFwUpdater();

    /*!
     \brief FW Updater States

    */
    enum
    {
        Done           =  0,   /*!< Firmware update of device completed successfully */
        UPDPTFAIL      =  1,   /*!< Firmware update of device failed */
    };

    /*!
     \brief Add brief

     \param ComPort
     \param FilePath
     \param FileName
     \return bool
    */
    bool SetParameter(const char *ComPort, const char *FilePath, const char *FileName);

private slots:
    /*!
     \brief Add brief

    */
    void eventStartUpdate(void);
    /*!
     \brief Add brief

     \param Error
    */
    void eventCmdError(QProcess::ProcessError Error);
    /*!
     \brief Add brief

     \param exitCode
     \param exitStatus
    */
    void eventCmdFinished( int exitCode, QProcess::ExitStatus exitStatus);
    /*!
     \brief Add brief

    */
    void eventCmdReadyReadStandardError(void);
    /*!
     \brief Add brief

    */
    void eventCmdReadyReadStandardOutput(void);
    /*!
     \brief Add brief

    */
    void eventCmdStarted(void);
    /*!
     \brief Add brief

     \param newState
    */
    void eventCmdStateChanged(QProcess::ProcessState newState);
    /*!
     \brief Add brief

    */
    void eventTimerTick(void);

private:
    Ui::appFwUpdater *ui;          /*!< Add in-line comment */
    QProcess         *Cmd;         /*!< Add in-line comment */
    QString           cmdProgName; /*!< Add in-line comment */
    QStringList       argList;     /*!< Add in-line comment */
    QString           argComPort;  /*!< Add in-line comment */
    QString           argFilePath; /*!< Add in-line comment */
    QString           argFileName; /*!< Add in-line comment */
    QTimer           *Timer;       /*!< Add in-line comment */
    QStringList       slMonitor;   /*!< Add in-line comment */
    QStringListModel *mMonitor;    /*!< Add in-line comment */
};
