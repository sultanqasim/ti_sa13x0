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
/*! \file mainwindow.h */
#pragma once
#include <QMainWindow>
#include <QCloseEvent>

#include "appFwUpdater.h"
#include "appPlot.h"
#include "appSettings.h"
#include "appStatusbar.h"
#include "appTypedef.h"
#include "drvSA1350.h"

namespace Ui {
    class MainWindow;
}

/*!
 \brief Add brief

 \typedef struct sStatusSpectrum sStatusSpectrum
*/
/*!
 \brief Add brief

 \struct sStatusSpectrum mainwindow.h "mainwindow.h"
*/
typedef struct sStatusSpectrum
{
    bool           flagModeContinuous;  /*!< Add in-line comment */
    bool           flagActiveFrqValues; /*!< Add in-line comment */
    bool           flagUndoFrqValues;   /*!< Add in-line comment */
    sFrqValues     activFrqValues;      /*!< Add in-line comment */
    sFrqValues     undoFrqValues;       /*!< Add in-line comment */
}sStatusSpectrum;

/*!
 \brief Add brief

 \typedef struct sStatus sStatus
*/
/*!
 \brief Add brief

 \struct sStatus mainwindow.h "mainwindow.h"
*/
typedef struct sStatus
{
    sStatusSpectrum Spectrum; /*!< Add in-line comment */
}sStatus;

/*!
 \brief Add brief

 \class MainWindow mainwindow.h "mainwindow.h"
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     \brief Constructor

     \param parent
    */
    explicit MainWindow(QWidget *parent = 0);
    /*!
     \brief Destructor

    */
    ~MainWindow();

private slots:
    // App Events
    /*!
     \brief Add brief

     \param *event
    */
    void closeEvent(QCloseEvent *event);

    // Driver Events
    /*!
     \brief Add brief

    */
    void eventSA1350DeviceConnected(void);
    /*!
     \brief Add brief

    */
    void eventSA1350DeviceDisconnected(void);
    /*!
     \brief Add brief

    */
    void eventSA1350BoardRFLimits(void);
    /*!
     \brief Add brief

     \param ComPort
    */
    void eventSA1350FirmwareUpdateRequired(QString ComPort);
    /*!
     \brief Add brief

     \param Done
     \param SpecId
    */
    void eventSA1350NewParameterSet(bool Done, int SpecId);
    /*!
     \brief Add brief

    */
    void eventSA1350SpectrumReceived(void);
    /*!
     \brief Add brief

     \param Msg
    */
    void eventSA1350ErrorMsg(QString Msg);

    // Help Events
    /*!
     \brief Open User's Guide from \c actionHelpMenu trigger

     \warning Requires specified document to be present within executable directory
     \snippet sa1350-gui/mainwindow.cpp User's Guide path
    */
    void eventHelpOpenUsersGuide(void);
    /*!
     \brief Open user support webpage from \c actionSupportLink trigger

     \warning Requires active internet connection
    */
    void eventHelpOpenSupportLink(void);
    /*!
     \brief Open Readme from \c actionHelpMenu trigger

     \warning Requires specified document to be present within executable directory
     \snippet sa1350-gui/mainwindow.cpp Readme path
    */
    void eventHelpOpenReadme(void);

    // FirmwareUpdate Events
    /*!
     \brief Add brief

    */
    void eventFirmwareUpdate(void);

    // Frequency Settings Events
    /*!
     \brief Add brief

    */
    void eventFrqStartChanged(void);
    /*!
     \brief Add brief

    */
    void eventFrqStopChanged(void);
    /*!
     \brief Add brief

    */
    void eventFrqCenterChanged(void);
    /*!
     \brief Add brief

    */
    void eventRfModeEasyRfChange(void);
    /*!
     \brief Add brief

    */
    void eventFrqSpanChanged(void);
    /*!
     \brief Add brief

    */
    void eventExpertFrqSpanChanged(void);

    // Date & Time Events
    /*!
     \brief Add brief

    */
    void eventTimeDateUpdateTick(void);

    // Hardware Events
    /*!
     \brief Add brief

    */
    void eventUpdateDeviceList(void);
    /*!
     \brief Add brief

     \param index
    */
    void eventDeviceListIndexChanged(int index);
    /*!
     \brief Add brief

     \param On
    */
    void eventDeviceStartStop(bool On);
    /*!
     \brief Add brief

    */
    void eventDeviceConnect(void);
    /*!
     \brief Add brief

    */
    void eventDeviceDisconnect(void);

    // Grid Events
    /*!
     \brief Add brief

    */
    void eventGridMode_Off(void);
    /*!
     \brief Add brief

    */
    void eventGridMode_Lines(void);
    /*!
     \brief Add brief

    */
    void eventGridMode_Dotted(void);

    /*!
     \brief Add brief

    */
    void eventFrqModeCenterRangeChange(void);
    /*!
     \brief Add brief

    */
    void eventFrqStepWidthChanged(void);

    // Marker Events
    void eventMarkerPeakOnOff(bool flagOn);
    /*!
     \brief Add brief

     \param index
    */
    void eventMarker0SetTrace(int index);
    /*!
     \brief Add brief

     \param index
    */
    void eventMarker1SetTrace(int index);
    /*!
     \brief Add brief

     \param index
    */
    void eventMarker2SetTrace(int index);
    /*!
     \brief Add brief

    */
    void eventMarkerAllOff(void);
    /*!
     \brief Add brief

     \param index
    */
    void eventMarkerMove(int index);
    /*!
     \brief Add brief

    */
    void eventSelectMarker0(void);
    /*!
     \brief Add brief

    */
    void eventSelectMarker1(void);
    /*!
     \brief Add brief

    */
    void eventSelectMarker2(void);
    // Trace Events
    /*!
     \brief Add brief

     \param mode
    */
    void eventTrace0ModeChanged(int mode);
    /*!
     \brief Add brief

     \param mode
    */
    void eventTrace1ModeChanged(int mode);
    /*!
     \brief Add brief

     \param mode
    */
    void eventTrace2ModeChanged(int mode);
    /*!
     \brief Add brief

     \param mode
    */
    void eventTrace3ModeChanged(int mode);
    /*!
     \brief Add brief

    */
    void eventTrace0ColorChange(void);
    /*!
     \brief Add brief

    */
    void eventTrace1ColorChange(void);
    /*!
     \brief Add brief

    */
    void eventTrace2ColorChange(void);
    /*!
     \brief Add brief

    */
    void eventTrace3ColorChange(void);
    /*!
     \brief Add brief

    */
    void eventTrace0Export(void);
    /*!
     \brief Add brief

    */
    void eventTrace1Export(void);
    /*!
     \brief Add brief

    */
    void eventTrace2Export(void);
    /*!
     \brief Add brief

    */
    void eventTrace3Export(void);
    /*!
     \brief Add brief

     \param On
    */
    void eventTrace0HoldTrigger(bool On);
    /*!
     \brief Add brief

     \param On
    */
    void eventTrace1HoldTrigger(bool On);
    /*!
     \brief Add brief

     \param On
    */
    void eventTrace2HoldTrigger(bool On);
    /*!
     \brief Add brief

     \param On
    */
    void eventTrace3HoldTrigger(bool On);
    /*!
     \brief Add brief

    */
    void eventTrace0Clear(void);
    /*!
     \brief Add brief

    */
    void eventTrace1Clear(void);
    /*!
     \brief Add brief

    */
    void eventTrace2Clear(void);
    /*!
     \brief Add brief

    */
    void eventTrace3Clear(void);
    /*!
     \brief TODO Add brief

    */
    void eventTraceAllClear(void);
    /*!
     \brief TODO Add brief

    */
    void eventTraceAllOff(void);

    // Frqency Settings Events
    /*!
     \brief Add brief

    */
    void eventFrqRangeChanged(void);

    /*!
     \brief Add brief

    */
    void eventFrqSet(void);
    /*!
     \brief Add brief

    */
    void eventFrqSpectrumStop(void);
    /*!
     \brief Add brief

    */
    void eventFrqSave(void);
    /*!
     \brief Add brief

    */
    void eventFrqLoad(void);
    /*!
     \brief Add brief

    */
    void eventFrqUndo(void);

    // Div. Tool Events
    /*!
     \brief Add brief

     \param Text
    */
    void eventScreenCommentTextChanged(QString Text);
    /*!
     \brief Add brief

     \param graphOnlyChecked
    */
    void eventSaveGraphOnlyChanged(bool graphOnlyChecked);
    /*!
     \brief TODO Add brief

    */
    void eventExport(void);
    /*!
     \brief Add brief

    */
    void eventPrint(void);
    /*!
     \brief Add brief

    */
    void eventSave(void);
    /*!
     \brief TODO Add brief

    */
    void eventCapture(void);
    /*!
     \brief Add brief

    */
    void eventRBWchanged(int index);

private:
    Ui::MainWindow  *ui;               /*!< Add in-line comment */

    appFwUpdater    *sa1350FwUpdater;  /*!< Add in-line comment */

    QTimer          *timedateCtrl;     /*!< Add in-line comment */
    drvSA1350        *deviceCtrl;      /*!< Add in-line comment */
    appPlot         *plotCtrl;         /*!< Add in-line comment */
    appStatusBar    *statusbarCtrl;    /*!< Add in-line comment */
    appSettings     *Settings;         /*!< Add in-line comment */
    sStatus         Status;            /*!< Add in-line comment */
    QList<sa1350UsbDevice> DeviceList; /*!< Add in-line comment */

    QStringList      strHwUsbInfoList; /*!< Add in-line comment */
    QStringListModel *mHwUsbInfo;      /*!< Add in-line comment */

    QStringList      strHwDevInfoList; /*!< Add in-line comment */
    QStringListModel *mHwDevInfo;      /*!< Add in-line comment */

    sFrqValues ActiveSpecParameter;    /*!< Add in-line comment */
    int        ActiveSpecId;           /*!< Add in-line comment */

    QAction *actionConnect;            /*!< Add in-line comment */
    QAction *actionDisconnect;         /*!< Add in-line comment */
    QAction *actionSpectrumStart;      /*!< Add in-line comment */
    QAction *actionSpectrumStop;       /*!< Add in-line comment */
    QAction *actionSave;               /*!< Add in-line comment */
    QAction *actionPrint;              /*!< Add in-line comment */
    QAction *actionOpenUsersGuide;     /*!< Add in-line comment */
    QAction *actionOpenSupportLink;    /*!< Add in-line comment */

    /*!
     \brief Add brief

    */
    void initGui(void);
    /*!
     \brief Add brief

    */
    void initGuiEvents(void);
    /*!
     \brief Add brief

    */
    void initPlotCtrl(void);
    /*!
     \brief Add brief

    */
    void guiUpdateDeviceList(void);
    /*!
     \brief Add brief

    */
    void guiUpdateRBWList(sDeviceInfo *devInfo);
    /*!
     \brief Add brief

     \param FrqSetting
    */
    void guiUpdateFrq(sFrqValues *FrqSetting);
    /*!
     \brief Add brief

     \param mode
    */
    void guiSetGridMode(eGridMode mode);
    /*!
     \brief Add brief

     \param trace
     \param mode
    */
    void guiSetTraceMode(eTrace trace, eTraceMode mode);
    /*!
     \brief Add brief

     \param tmpFrqSet
     \return bool
    */
    bool guiSaveFrqProfile(sFrqSetting &tmpFrqSet);
    /*!
     \brief Add brief

     \param newFrqSetting
     \return bool
    */
    bool guiLoadFrqProfile(sFrqSetting *newFrqSetting);
    /*!
     \brief Add brief

     \param usbInfo
     \return bool
    */
    bool guiDisplayHwUsbInfo(sa1350UsbDevice *usbInfo);
    /*!
     \brief Add brief

     \param devInfo
     \return bool
    */
    bool guiDisplayHwDevInfo(sDeviceInfo *devInfo);
    /*!
     \brief Add brief

     \param calData
     \return bool
    */
    bool guiDisplayHwCalData(sCalibrationData *calData);
    /*!
     \brief Add brief

     \param FrqSettings
     \return bool
    */
    bool guiDisplayFrqSettings(sFrqValues *FrqSettings);
    /*!
     \brief Add brief

     \param mInfo
     \return bool
    */
    bool guiDisplayMarkerInfo(sMarkerInfo *mInfo);

    /*!
     \brief Add brief

     \param Marker
    */
    void guiMarkerSelect(eMarker Marker);
    /*!
     \brief Add brief

     \param Marker
     \param TraceNr
     \return bool
    */
    bool guiMarkerSetTrace(eMarker Marker, eTrace TraceNr);
    /*!
     \brief Add brief

     \param TraceNr
    */
    void guiMarkerSetTraceUpdate(eTrace TraceNr);
    /*!
     \brief Add brief

     \param TraceNr
    */
    void guiTraceColorSelect(eTrace TraceNr);
    /*!
     \brief Add brief

     \param TraceNr
     \param newColor
    */
    void guiTraceColorSet(eTrace TraceNr, QColor newColor);
    /*!
     \brief Add brief

     \param TraceNr
     \param trcMode
    */
    void guiTraceModeChange(eTrace TraceNr, eTraceMode trcMode);
    /*!
     \brief Add brief

     \param TraceNr
     \param TraceMode
     \return bool
    */
    bool guiTraceExport(eTrace TraceNr,eTraceMode TraceMode);
    /*!
     \brief Add brief

     \param TraceNr
     \return bool
    */
    bool guiTraceHoldTrigger(eTrace TraceNr);
    /*!
     \brief Add brief

     \param TraceNr
     \return bool
    */
    bool guiTraceHoldReset(eTrace TraceNr);
    /*!
     \brief Add brief

     \param TraceNr
     \return bool
    */
    bool guiTraceClear(eTrace TraceNr);

    /*!
     \brief Add brief

     \param actualFrqValues
     \return bool
    */
    bool guiFrqGetActualSettings(sFrqValues *actualFrqValues);
    /*!
     \brief Add brief

    */
    void guiFrqUpdateExpertRFSpanLimit(unsigned short fspan);
    /*!
     \brief Add brief

    */
    void guiFrqStartStop2Center(void);
    /*!
     \brief Add brief

    */
    void guiFrqCenter2StartStop(void);
    /*!
     \brief Add brief

    */
    void guiFrqUpdateLimits(void);
    /*!
     \brief Add brief

    */
    void guiFrqBand2StartStop(void);
    /*!
     \brief Add brief

    */
    void guiFrqSpan2StartStop(void);
    /*!
     \brief Add brief

    */
    void guiFrqExpertRFSpan2StepWidth(void);

    /*!
     \brief Add brief

     \param ComPort
     \return bool
    */
    bool guiFwUpdate(QString ComPort);
    /*!
     \brief Add brief

     \param ComPort
     \return bool
    */
    bool guiDrvrInstall(QString ComPort);
};
