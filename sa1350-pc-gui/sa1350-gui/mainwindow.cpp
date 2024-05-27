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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "appReportCsv.h"

#define GUI_VERSION		((unsigned short)(0x0103)) /*!<  GUI version number in High_byte.Low_byte format  */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    deviceCtrl     = new drvSA1350();
    plotCtrl       = new appPlot(ui->plotwindow);
    statusbarCtrl  = new appStatusBar(ui->statusBar);
    Settings       = new appSettings;
    mHwUsbInfo     = new QStringListModel(this);
    mHwDevInfo     = new QStringListModel(this);
    timedateCtrl   = new QTimer(this);

    sa1350FwUpdater = new appFwUpdater(this);

    Status.Spectrum.flagModeContinuous = false;
    Status.Spectrum.flagActiveFrqValues= false;
    Status.Spectrum.flagUndoFrqValues  = false;

    initGui();
    initGuiEvents();
    initPlotCtrl();

    statusbarCtrl->SetDllGuiVersion(deviceCtrl->GetDllVer(),GUI_VERSION);

    // Load Connected Device List
    guiUpdateDeviceList();
}

MainWindow::~MainWindow()
{
    delete deviceCtrl;
    delete plotCtrl;
    delete statusbarCtrl;
    delete Settings;
    delete mHwUsbInfo;
    delete mHwDevInfo;
    delete ui;
}

// Private Slot Defintion
// App Events
void MainWindow::closeEvent(QCloseEvent *event)
{
    // If connected to target, send disconnect command
    deviceCtrl->deviceDisconnect();
    event->accept();
}

// Driver Events
void MainWindow::eventSA1350DeviceConnected(void)
{
    bool done = false;

    sa1350UsbDevice  usbInfo;
    sDeviceInfo      devInfo;

    if(deviceCtrl->deviceGetUsbInfo(&usbInfo))
    {
        if(deviceCtrl->deviceGetDevInfo(&devInfo))
        {
            done = true;
        };
    };

    if(done)
    {
        statusbarCtrl->SetDeviceConnected(&usbInfo);
        guiDisplayHwUsbInfo(&usbInfo);
        guiDisplayHwDevInfo(&devInfo);
        ui->tabMarker->setEnabled(true);
        ui->tabTraces->setEnabled(true);
        ui->tabRfSettings->setEnabled(true);
        actionSpectrumStart->setEnabled(true);
        actionSpectrumStop->setEnabled(true);

        // Update device specific items on connect
        // Check board RF limits
        eventSA1350BoardRFLimits();

        if(ui->rbRfRange_2152_2635->isChecked())
        {
            devInfo.ActiveRBWTable = &devInfo.RBWTableBand1;
        }
        else
        {
            devInfo.ActiveRBWTable = &devInfo.RBWTableBand0;
        }

        // Load RBW list
        guiUpdateRBWList(&devInfo);
    };
}

void MainWindow::eventSA1350DeviceDisconnected(void)
{
    statusbarCtrl->SetDeviceDisconnected();
    ui->bttnHwConnect->setEnabled(true);
    ui->bttnHwConnect->setChecked(false);
    ui->bttnHwDeviceListScan->setEnabled(true);
    guiUpdateDeviceList();
    guiDisplayHwDevInfo(NULL);

    ui->tabMarker->setEnabled(false);
    ui->tabTraces->setEnabled(false);
    ui->tabRfSettings->setEnabled(false);
    ui->tabToolBox->setCurrentIndex(0);
    actionSpectrumStart->setEnabled(false);
    actionSpectrumStop->setEnabled(false);

}

void MainWindow::eventSA1350BoardRFLimits(void)
{
    bool done = false;
    sa1350UsbDevice  usbInfo;
    sDeviceInfo      devInfo;

    if(deviceCtrl->deviceGetUsbInfo(&usbInfo))
    {
        if(deviceCtrl->deviceGetDevInfo(&devInfo))
        {
            done = true;
        };
    };

    if(done)
    {
        // First check device level RF limitations
        if (devInfo.DeviceVersion == "1310")
        {
            if(ui->rbRfRange_2152_2635->isChecked())
            {
                ui->rbRfRange_861_1054->setChecked(true);
                ui->rbRfRange_2152_2635->repaint();
                eventFrqRangeChanged();
            }
            ui->rbRfRange_2152_2635->setCheckable(false);
            ui->rbRfRange_2152_2635->setDisabled(true);
        }
        else
        {
            ui->rbRfRange_2152_2635->setDisabled(false);
            ui->rbRfRange_2152_2635->setCheckable(true);
        };

        // Next check LP board RF limitations
        if(ui->cbLPBoardRFLimits->isChecked())
        {
            sBrdRfLimits thisBoard;
            bool boardfound = false;

            // Lookup board-specific RF characteristics
            for (int index = 0; index < BrdTableRFLimits.size(); index++)
            {
                if (QString(usbInfo.SerialNr).startsWith(BrdTableRFLimits.at(index).BoardID))
                {
                    thisBoard = BrdTableRFLimits.at(index);
                    boardfound = true;
                };
            };

            // Apply GUI restrictions based on board characteristics
            // Check 440
            if (boardfound && !thisBoard.Band440Tuned)
            {
                // If board is not tuned for this band, make sure its not checked
                if(ui->rbRfRange_431_527->isChecked())
                {
                    if(thisBoard.Band900Tuned)
                    {
                        ui->rbRfRange_861_1054->setChecked(true);
                    }
                    else
                    {
                        ui->rbRfRange_2152_2635->setChecked(true);
                    };
                    ui->rbRfRange_431_527->repaint();
                    eventFrqRangeChanged();
                };
                ui->rbRfRange_431_527->setCheckable(false);
                ui->rbRfRange_431_527->setDisabled(true);
            }
            else // Board not found or board is tuned for 440MHz
            {
                ui->rbRfRange_431_527->setCheckable(true);
                ui->rbRfRange_431_527->setDisabled(false);
            };

            // Check 900
            if (boardfound && !thisBoard.Band900Tuned)
            {
                // If board is not tuned for this band, make sure its not checked
                if(ui->rbRfRange_861_1054->isChecked())
                {
                    if(thisBoard.Band440Tuned)
                    {
                        ui->rbRfRange_431_527->setChecked(true);
                    }
                    else
                    {
                        ui->rbRfRange_2152_2635->setChecked(true);
                    };
                    ui->rbRfRange_861_1054->repaint();
                    eventFrqRangeChanged();
                };
                ui->rbRfRange_861_1054->setCheckable(false);
                ui->rbRfRange_861_1054->setDisabled(true);
            }
            else // Board not found or board is tuned for 900MHz
            {
                ui->rbRfRange_861_1054->setCheckable(true);
                ui->rbRfRange_861_1054->setDisabled(false);
            };
        }
        else // Disable GUI restrictions for board limitations
        {
            ui->rbRfRange_431_527->setCheckable(true);
            ui->rbRfRange_431_527->setDisabled(false);
            ui->rbRfRange_861_1054->setCheckable(true);
            ui->rbRfRange_861_1054->setDisabled(false);
        };
    };
}

void MainWindow::eventSA1350FirmwareUpdateRequired(QString ComPort)
{
    if(!ComPort.isEmpty())
    {
        guiFwUpdate(ComPort);
    };
}

void MainWindow::eventSA1350NewParameterSet(bool Done, int SpecId)
{
    int Steps;
    sFrqValues        FrqCorrected;
    sCalibrationData  CalibrationData;
    if(Done)
    {
        ActiveSpecId = SpecId;
        deviceCtrl->spectrumGetParameter(&ActiveSpecParameter,&FrqCorrected);
        deviceCtrl->deviceGetCalData(&CalibrationData);
        plotCtrl->SetNewSpectrumParameter(SpecId,&FrqCorrected,&CalibrationData);
        Steps = (unsigned long)((FrqCorrected.FrqSpan*(float)1000.0)/FrqCorrected.FrqStepWidth) + 1;
        ui->MarkerDial->setMinimum(0);
        ui->MarkerDial->setMaximum(Steps);
        guiDisplayFrqSettings(&FrqCorrected);
    }
    else
    {
        guiDisplayFrqSettings(NULL);
        plotCtrl->SetNewSpectrumParameter(SpecId,NULL,NULL);
    };
}

void MainWindow::eventSA1350SpectrumReceived(void)
{
    sSpectrum spectrum;
    sMarkerInfo minfo;
    int steps;
    sFrqValues FrqCorrected;

    if(deviceCtrl->spectrumGetData(&spectrum))
    {
        deviceCtrl->spectrumGetParameter(&ActiveSpecParameter,&FrqCorrected);
        steps = (unsigned long)((FrqCorrected.FrqSpan*(float)1000.0)/FrqCorrected.FrqStepWidth) + 1;
        if(steps == spectrum.Data.size())
        {
            plotCtrl->SetSpectrumData(&spectrum);
            plotCtrl->MarkerUpdatePos();
            plotCtrl->MarkerGetInfo(&minfo);
            guiDisplayMarkerInfo(&minfo);
        };
        if(Status.Spectrum.flagModeContinuous)
            deviceCtrl->spectrumTriggerOn();
        else
            deviceCtrl->spectrumTriggerOff();
    };
}

void MainWindow::eventSA1350ErrorMsg(QString Msg)
{
    QMessageBox::warning(this, tr("SA1350 Device Driver"),Msg,QMessageBox::Ok,QMessageBox::NoButton);
}

// Help Events
void MainWindow::eventHelpOpenUsersGuide(void)
{
    QUrl link;

    if(QFile::exists("USERGUIDE.html"))
    {
        // [User's Guide path]
        link = QUrl::fromLocalFile("USERGUIDE.html");
        // [User's Guide path]
        if(link.isValid())
        {
            QDesktopServices::openUrl(link);
        }
        else
        {// Link not valid
            // Add Handler
        };
    }
    else
    {// File not Found
        // Add Handler
    };

}

void MainWindow::eventHelpOpenSupportLink(void)
{
    if(!QDesktopServices::openUrl(QUrl("http://e2e.ti.com/", QUrl::StrictMode)))
    {// Failed to Open
        // Add Handler
    };
}

void MainWindow::eventHelpOpenReadme(void)
{
    QUrl link;

    if(QFile::exists("README.html"))
    {
        // [Readme path]
        link = QUrl::fromLocalFile("README.html");
        // [Readme path]
        if(link.isValid())
        {
            QDesktopServices::openUrl(link);
        }
        else
        {// Link not valid
            // Add Handler
        };
    }
    else
    {// File not Found
        // Add Handler
    };

}

// FirmwareUpdate Events
void MainWindow::eventFirmwareUpdate(void)
{
    if(DeviceList.isEmpty())
    {
        QMessageBox::information(this,"SA1350 Device Driver","No Device Found/Detected... Scan again for connected Devices!",QMessageBox::Ok);
    }
    else
    {
        sa1350UsbDevice devinfo = DeviceList.at(ui->cbDeviceList->currentIndex());
        guiFwUpdate(QString(devinfo.PortName));
    };
}

// Frequency Settings Events
void MainWindow::eventFrqStartChanged(void)
{
    double fspan;

    // Calculate fspan
    if(!ui->bttnRfModeEasyRf->isEnabled())
    {
        fspan = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].SpanMHz;
    }
    else // ExpertRF mode, changing start freq changes span, not stop freq
    {
        if (ui->spFrqStartValue->value() < ui->spFrqStopValue->value())
        {
            unsigned short fspanNew = (unsigned short)lround(ui->spFrqStopValue->value() - ui->spFrqStartValue->value());

            guiFrqUpdateExpertRFSpanLimit(fspanNew);
        }; // Move stop freq ahead by span if start freq was set beyond stop freq

        fspan = ui->sbFrqSpanInput->value();
    };

    // Update Gui Controls
    const QSignalBlocker fstopBlocker(ui->spFrqStopValue);
    ui->spFrqStopValue->setValue(ui->spFrqStartValue->value() + fspan);
    const QSignalBlocker fstartBlocker(ui->spFrqStartValue);
    ui->spFrqStartValue->setValue(ui->spFrqStopValue->value() - fspan);

    guiFrqStartStop2Center();
}

void MainWindow::eventFrqStopChanged(void)
{
    double fspan;

    // Calculate fspan
    if(!ui->bttnRfModeEasyRf->isEnabled())
    {
        fspan = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].SpanMHz;
    }
    else // ExpertRF mode, changing stop freq changes span, not start freq
    {
        if (ui->spFrqStartValue->value() < ui->spFrqStopValue->value())
        {
            unsigned short fspanNew = (unsigned short)lround(ui->spFrqStopValue->value() - ui->spFrqStartValue->value());

            guiFrqUpdateExpertRFSpanLimit(fspanNew);
        }; // Move start freq back by span if stop freq was set before start freq

        fspan = ui->sbFrqSpanInput->value();
    };

    // Update Gui Controls
    const QSignalBlocker fstartBlocker(ui->spFrqStartValue);
    ui->spFrqStartValue->setValue(ui->spFrqStopValue->value() - fspan);
    const QSignalBlocker fstopBlocker(ui->spFrqStopValue);
    ui->spFrqStopValue->setValue(ui->spFrqStartValue->value() + fspan);

    guiFrqStartStop2Center();
}

void MainWindow::eventFrqCenterChanged(void)
{
    // Calculate fspan
    if(ui->bttnRfModeEasyRf->isEnabled())
    { // ExpertRF mode, changing center freq changes span
        unsigned short fspanNew = ui->sbFrqSpanInput->value();

        if(ui->spFrqCenterValue->value() + (fspanNew/(double)2.0) > ui->spFrqStopValue->maximum())
        { // Decrease span to allow fstop to be within current band
            fspanNew = (unsigned short)((ui->spFrqStopValue->maximum() - ui->spFrqCenterValue->value())*(double)2.0);
        }
        else if(ui->spFrqCenterValue->value() - (fspanNew/(double)2.0) < ui->spFrqStartValue->minimum())
        { // Decrease span to allow fstart to be within current band
            fspanNew = (unsigned short)((ui->spFrqCenterValue->value() - ui->spFrqStartValue->minimum())*(double)2.0);
        };

        guiFrqUpdateExpertRFSpanLimit(fspanNew);
    };

    guiFrqCenter2StartStop();
}

void MainWindow::eventFrqSpanChanged(void)
{
    guiFrqSpan2StartStop();
}

void MainWindow::eventExpertFrqSpanChanged(void)
{
    guiFrqUpdateExpertRFSpanLimit(ui->sbFrqSpanInput->value());
    guiFrqSpan2StartStop();
}

void MainWindow::eventFrqStepWidthChanged(void)
{
    // Initial value of sbFrqStepwidthInput
    static double fstepPrev = (double)83.328;
    double fstep            = ui->sbFrqStepwidthInput->value();
    double fstepHwDividendPrev;
    double fstepHwDividend;

    // Adjust fstep according to input
    // Calculate old/new fstep divisors
    fstepHwDividendPrev = round((fstepPrev/(double)1000.0)*(double)65536.0);
    fstepHwDividend     = round((fstep/(double)1000.0)*(double)65536.0);

    if(round(abs(fstep - fstepPrev)*(double)100.0) ==
            round(ui->sbFrqStepwidthInput->singleStep()*(double)100.0))
    {  // Requested change differs by up/down arrow value
        unsigned short fstepCount = (unsigned short)round((double)65536.0/fstepHwDividendPrev);

        if (fstep > fstepPrev)
        { // Move down to next step count per MHz
            do
            {
                // Calculate next even step count
                fstepCount = fstepCount - 2;
                fstepCount += fstepCount%2;
                // Calculate next even step dividend
                fstepHwDividend = floor((double)65536.0/(double)(fstepCount));
                fstepHwDividend -= (unsigned short)fstepHwDividend%2;
            } while((unsigned short)((double)ui->sbFrqSpanInput->value()/(fstepHwDividend/(double)65536.0)) % 2);
            // Continue to search until sample count will be odd
        }
        else
        { // Move up to next step count per MHz
            do
            {
                // Calculate next even step count
                fstepCount = fstepCount + 2;
                fstepCount -= fstepCount%2;
                // Calculate next even step dividend
                fstepHwDividend = floor((double)65536.0/(double)(fstepCount));
                fstepHwDividend -= (unsigned short)fstepHwDividend%2;
            } while((unsigned short)((double)ui->sbFrqSpanInput->value()/(fstepHwDividend/(double)65536.0)) % 2);
            // Continue to search until sample count will be odd
        };
    }
    else if(fstepHwDividend == fstepHwDividendPrev)
    { // Requested change too small to change number of steps
        if (fstep > fstepPrev)
        { // Move up to next valid FSW
            fstepHwDividend = fstepHwDividendPrev + 2;
        }
        else
        { // Move down to next valid FSW
            fstepHwDividend = fstepHwDividendPrev - 2;
        };
    };

    fstep = (fstepHwDividend/(double)65536.0)*(double)1000.0;

    // Update Gui Controls
    const QSignalBlocker fstepBlocker(ui->sbFrqStepwidthInput);
    ui->sbFrqStepwidthInput->setValue(fstep);

    guiFrqExpertRFSpan2StepWidth();

    fstepPrev = ui->sbFrqStepwidthInput->value();
}

// Date & Time Events
void MainWindow::eventTimeDateUpdateTick(void)
{
    ui->txtGraphDate->setText(QDateTime::currentDateTime().toString());
}

// Hardware Events
void MainWindow::eventUpdateDeviceList(void)
{
    guiUpdateDeviceList();
}

void MainWindow::eventDeviceListIndexChanged(int index)
{
    sa1350UsbDevice Info;
    if(index>=0)
    {
        Info = DeviceList.at(index);
        if(Info.Connected)
        {
            ui->bttnHwConnect->setEnabled(true);
        }
        else
        {
            ui->bttnHwConnect->setEnabled(false);
        };
        guiDisplayHwUsbInfo(&Info);
    }
    else
    {
        ui->bttnHwConnect->setEnabled(false);
    };
}

void MainWindow::eventDeviceStartStop(bool On)
{
    if(On)
    {// Event: Connect
        if(DeviceList.isEmpty())
        {
            QMessageBox::information(this,"SA1350 Device Driver","No Device Found/Detected... Scan again for connected Devices!",QMessageBox::Ok);
        }
        else
        {
            sa1350UsbDevice usbinfo = DeviceList.at(ui->cbDeviceList->currentIndex());
            if(deviceCtrl->deviceConnect(&usbinfo))
            {
                ui->bttnHwDeviceListScan->setEnabled(false);
                actionConnect->setEnabled(false);
                actionDisconnect->setEnabled(true);
                ui->cbDeviceList->setEnabled(false);
            };
        };
    }
    else
    {// Event: Disconnect
        deviceCtrl->deviceDisconnect();
        ui->bttnHwDeviceListScan->setEnabled(true);
        ui->bttnHwConnect->setChecked(false);
        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);
        ui->cbDeviceList->setEnabled(true);
    };
}

void MainWindow::eventDeviceConnect(void)
{
    ui->bttnHwConnect->setChecked(true);
}

void MainWindow::eventDeviceDisconnect(void)
{
    ui->bttnHwConnect->setChecked(false);
}

// Grid Events
void MainWindow::eventGridMode_Off(void)
{
    guiSetGridMode(GRID_OFF);
}

void MainWindow::eventGridMode_Lines(void)
{
    guiSetGridMode(GRID_LINES);
}

void MainWindow::eventGridMode_Dotted(void)
{
    guiSetGridMode(GRID_DOTTED);
}

void MainWindow::eventFrqModeCenterRangeChange(void)
{
    if(ui->swFrqModeCenterRange->currentIndex()==0)
    {//Mode FSpan/2<-FCenter->FSpan/2
        ui->swFrqModeCenterRange->setCurrentIndex(1);
        ui->bttnFrqRangeMode->setEnabled(true);
        ui->bttnFrqCenterMode->setEnabled(false);
        guiFrqStartStop2Center();
    }
    else
    {// Mode FStart<->FStop
        ui->swFrqModeCenterRange->setCurrentIndex(0);
        ui->bttnFrqRangeMode->setEnabled(false);
        ui->bttnFrqCenterMode->setEnabled(true);
        guiFrqCenter2StartStop();
    };
}

void MainWindow::eventRfModeEasyRfChange(void)
{
    if (ui->swFrqSpan->currentIndex()==0)
    {// Mode Expert RF
        ui->swFrqSpan->setCurrentIndex(1);
        ui->bttnRfModeEasyRf->setEnabled(true);
        ui->bttnRfModeExpertRf->setEnabled(false);
        ui->grpRfBandWidth->setHidden(false);
        guiFrqSpan2StartStop();
    }
    else
    {// Mode Easy RF
        ui->swFrqSpan->setCurrentIndex(0);
        ui->bttnRfModeEasyRf->setEnabled(false);
        ui->bttnRfModeExpertRf->setEnabled(true);
        ui->grpRfBandWidth->setHidden(true);
        guiFrqSpan2StartStop();
    };
}

// Marker Events
void MainWindow::eventMarkerPeakOnOff(bool flagOn)
{
    Q_UNUSED(flagOn)
    //guiMarkerOnOff(M_PEAK,flagOn);
}

void MainWindow::eventMarker0SetTrace(int index)
{
    guiMarkerSetTrace(M_0,(eTrace)index);
}

void MainWindow::eventMarker1SetTrace(int index)
{
    guiMarkerSetTrace(M_1,(eTrace)index);
}

void MainWindow::eventMarker2SetTrace(int index)
{
    guiMarkerSetTrace(M_2,(eTrace)index);
}

void MainWindow::eventMarkerAllOff(void)
{
    guiMarkerSetTrace(M_0,TRACE_OFF);
    guiMarkerSetTrace(M_1,TRACE_OFF);
    guiMarkerSetTrace(M_2,TRACE_OFF);
}

void MainWindow::eventMarkerMove(int index)
{
    sMarkerInfo minfo;
    if(plotCtrl->MarkerSetActivePos(index) == index)
    {
        plotCtrl->MarkerGetInfo(&minfo);
        guiDisplayMarkerInfo(&minfo);
    };
}

void MainWindow::eventSelectMarker0(void)
{
    guiMarkerSelect(M_0);
}

void MainWindow::eventSelectMarker1(void)
{
    guiMarkerSelect(M_1);
}

void MainWindow::eventSelectMarker2(void)
{
    guiMarkerSelect(M_2);
}

// Trace Event Function Defintion
void MainWindow::eventTrace0ModeChanged(int mode)
{
    if(ui->bttnTraceHold0->isChecked())
    {
        ui->bttnTraceHold0->setChecked(false);
    };
    guiTraceModeChange(TRACE_0,(eTraceMode)mode);
}

void MainWindow::eventTrace1ModeChanged(int mode)
{
    if(ui->bttnTraceHold1->isChecked())
    {
        ui->bttnTraceHold1->setChecked(false);
    };
    guiTraceModeChange(TRACE_1,(eTraceMode)mode);
}

void MainWindow::eventTrace2ModeChanged(int mode)
{
    if(ui->bttnTraceHold2->isChecked())
    {
        ui->bttnTraceHold2->setChecked(false);
    };
    guiTraceModeChange(TRACE_2,(eTraceMode)mode);
}

void MainWindow::eventTrace3ModeChanged(int mode)
{
    if(ui->bttnTraceHold3->isChecked())
    {
        ui->bttnTraceHold3->setChecked(false);
    };
    guiTraceModeChange(TRACE_3,(eTraceMode) mode);
}

void MainWindow::eventTrace0ColorChange(void)
{
    guiTraceColorSelect(TRACE_0);
}

void MainWindow::eventTrace1ColorChange(void)
{
    guiTraceColorSelect(TRACE_1);
}

void MainWindow::eventTrace2ColorChange(void)
{
    guiTraceColorSelect(TRACE_2);
}

void MainWindow::eventTrace3ColorChange(void)
{
    guiTraceColorSelect(TRACE_3);
}

void MainWindow::eventTrace0Export(void)
{
    guiTraceExport(TRACE_0,(eTraceMode) ui->cbTraceMode0->currentIndex());
}

void MainWindow::eventTrace1Export(void)
{
    guiTraceExport(TRACE_1,(eTraceMode) ui->cbTraceMode1->currentIndex());
}

void MainWindow::eventTrace2Export(void)
{
    guiTraceExport(TRACE_2,(eTraceMode) ui->cbTraceMode2->currentIndex());
}

void MainWindow::eventTrace3Export(void)
{
    guiTraceExport(TRACE_3,(eTraceMode) ui->cbTraceMode3->currentIndex());
}

void MainWindow::eventTrace0HoldTrigger(bool On)
{
    if(On)
    {
        guiTraceHoldTrigger(TRACE_0);
    }
    else
    {
        guiTraceHoldReset(TRACE_0);
    };
}

void MainWindow::eventTrace1HoldTrigger(bool On)
{
    if(On)
    {
        guiTraceHoldTrigger(TRACE_1);
    }
    else
    {
        guiTraceHoldReset(TRACE_1);
    };
}

void MainWindow::eventTrace2HoldTrigger(bool On)
{
    if(On)
    {
        guiTraceHoldTrigger(TRACE_2);
    }
    else
    {
        guiTraceHoldReset(TRACE_2);
    };
}

void MainWindow::eventTrace3HoldTrigger(bool On)
{
    if(On)
    {
        guiTraceHoldTrigger(TRACE_3);
    }
    else
    {
        guiTraceHoldReset(TRACE_3);
    };
}

void MainWindow::eventTrace0Clear(void)
{
    guiTraceClear(TRACE_0);
}

void MainWindow::eventTrace1Clear(void)
{
    guiTraceClear(TRACE_1);
}

void MainWindow::eventTrace2Clear(void)
{
    guiTraceClear(TRACE_2);
}

void MainWindow::eventTrace3Clear(void)
{
    guiTraceClear(TRACE_3);
}

void MainWindow::eventTraceAllClear(void)
{
    guiTraceClear(TRACE_0);
    guiTraceClear(TRACE_1);
    guiTraceClear(TRACE_2);
    guiTraceClear(TRACE_3);
}

void MainWindow::eventTraceAllOff(void)
{
    ui->cbTraceMode0->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode1->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode2->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode3->setCurrentIndex(T_MODE_OFF);
}

// Frequency Settings Events
void MainWindow::eventFrqRangeChanged(void)
{
    guiFrqBand2StartStop();
}

void MainWindow::eventFrqSet(void)
{
    sFrqValues newFrqValues;

    if(guiFrqGetActualSettings(&newFrqValues))
    {
        if(deviceCtrl->spectrumSetParameter(&newFrqValues))
        {
            // Switch off Trace Hold
            if(ui->bttnTraceHold0->isChecked())
                ui->bttnTraceHold0->setChecked(false);
            if(ui->bttnTraceHold1->isChecked())
                ui->bttnTraceHold1->setChecked(false);
            if(ui->bttnTraceHold2->isChecked())
                ui->bttnTraceHold2->setChecked(false);
            if(ui->bttnTraceHold3->isChecked())
                ui->bttnTraceHold3->setChecked(false);
            // If no Traces are active switch On Trc0->ClrWrite and Trc1->Average
            if(
                    ui->cbTraceMode0->currentIndex()==T_MODE_OFF
                    && ui->cbTraceMode1->currentIndex()==T_MODE_OFF
                    && ui->cbTraceMode2->currentIndex()==T_MODE_OFF
                    )
            {
                ui->cbTraceMode0->setCurrentIndex(T_MODE_CLEARWRITE);
                ui->cbTraceMode1->setCurrentIndex(T_MODE_AVERAGE);
            };
            // Update Status Info
            // Status Continuous Mode Flag
            Status.Spectrum.flagModeContinuous = newFrqValues.flagModeContinuous;
            // Statuc Save Active Frq Settings
            if(Status.Spectrum.flagActiveFrqValues == true)
            {// Save Undo Frq Settings
                Status.Spectrum.undoFrqValues     = Status.Spectrum.activFrqValues;
                Status.Spectrum.flagUndoFrqValues = true;
            };
            // Save New and Active Frq Settings
            Status.Spectrum.flagActiveFrqValues = true;
            Status.Spectrum.activFrqValues      = newFrqValues;
            // FrqSet Done
        };
    };
}

void MainWindow::eventFrqSpectrumStop(void)
{
    Status.Spectrum.flagModeContinuous = false;
}

void MainWindow::eventFrqSave(void)
{
    QString     strFileName;
    sFrqSetting FrqSettings;

    strFileName = QFileDialog::getSaveFileName(this,tr("Save Frq Profile"),QDir::currentPath(), tr("Frq Profile Files (*.xml)"));

    if(strFileName.isEmpty())
        return;

    FrqSettings.Name = strFileName;

    if(guiFrqGetActualSettings(&FrqSettings.Values))
    {
        if(Settings->SaveFrqSettings(&FrqSettings))
        {
            // FrqSave Done
        };
    };
}

void MainWindow::eventFrqLoad(void)
{
    QString FileName;
    sFrqSetting newFrqSettings;
    FileName = QFileDialog::getOpenFileName(this,tr("Load Frq Profile"),QDir::currentPath(), tr("Frq Profile Files (*.xml)"));


    if(!FileName.isEmpty())
    {
        if(Settings->LoadFrqSettings(FileName,&newFrqSettings))
        {
            guiLoadFrqProfile(&newFrqSettings);
        };
    };
}

void MainWindow::eventFrqUndo(void)
{
    sFrqSetting undoFrqSettings;

    if(Status.Spectrum.flagUndoFrqValues)
    {
        undoFrqSettings.Name.clear();
        undoFrqSettings.Values = Status.Spectrum.undoFrqValues;
        guiLoadFrqProfile(&undoFrqSettings);
    };
}

// Div. Tool Events
void MainWindow::eventScreenCommentTextChanged(QString Text)
{
    plotCtrl->SetTitle(&Text);
}

void MainWindow::eventSaveGraphOnlyChanged(bool graphOnlyChecked)
{
    if(graphOnlyChecked)
    {
        ui->bttnScreenSaveToFile->setIcon(QIcon(":/img/32Plot"));
        actionSave->setIcon(QIcon(":/img/32Plot"));
    }
    else
    {
        ui->bttnScreenSaveToFile->setIcon(QIcon(":/img/32Capture"));
        actionSave->setIcon(QIcon(":/img/32Capture"));
    }
}

void MainWindow::eventExport(void)
{
    plotCtrl->Export(ui->cbScreenSaveToFileInvertColor->isChecked());
}

void MainWindow::eventPrint(void)
{
    plotCtrl->Print(ui->cbScreenPrintInvertColor->isChecked());
}

void MainWindow::eventSave(void)
{
    if(ui->cbScreenSaveToFileGraphOnly->isChecked())
    {
        plotCtrl->Export(ui->cbScreenSaveToFileInvertColor->isChecked());
    }
    else
    {
        QString format = "png";
        QString initialPath = QDir::currentPath() + "/untitled." + format;
        QImage capture = this->grab().toImage();
        if(ui->cbScreenSaveToFileInvertColor->isChecked())
        {
            capture.invertPixels();
        };

        QString fileName = QFileDialog::getSaveFileName(this, "Save As", initialPath,
                                                        "PNG Files (*.png);;All Files (*)", NULL, QFileDialog::DontConfirmOverwrite);
        if(!fileName.isEmpty())
            if(!capture.save(fileName,format.toLatin1()))
            {
                fileName=fileName;
            };
    };
}

void MainWindow::eventCapture(void)
{
    QString format = "png";
    QString initialPath = QDir::currentPath() + "/untitled." + format;
    QImage capture = this->grab().toImage();
    if(ui->cbScreenSaveToFileInvertColor->isChecked())
    {
        capture.invertPixels();
    };

    QString fileName = QFileDialog::getSaveFileName(this, "Save As", initialPath,
                                                    "PNG Files (*.png);;All Files (*)", NULL, QFileDialog::DontConfirmOverwrite);
    if(!fileName.isEmpty())
        if(!capture.save(fileName,format.toLatin1()))
        {
            fileName=fileName;
        };
}

// Private Function Defintion
void MainWindow::initGui(void)
{
    // Select hardware tab as start tab
    ui->tabToolBox->setCurrentWidget(ui->tabHardware);

    // Marker Trace Items
    ui->cbMarker0TraceSelected->setCurrentIndex(M_OFF);
    ui->cbMarker1TraceSelected->setCurrentIndex(M_OFF);
    ui->cbMarker2TraceSelected->setCurrentIndex(M_OFF);
    ui->MarkerDial->setMaximum(48);
    ui->MarkerDial->setMinimum(0);
    // Traces
    QPixmap pm(32,14);
    pm.fill(Qt::red);
    ui->bttnTraceColor0->setIcon(QIcon(pm));
    guiTraceColorSet(TRACE_0,QColor(Qt::red));
    pm.fill(Qt::green);
    ui->bttnTraceColor1->setIcon(QIcon(pm));
    guiTraceColorSet(TRACE_1,QColor(Qt::green));
    pm.fill(Qt::yellow);
    ui->bttnTraceColor2->setIcon(QIcon(pm));
    guiTraceColorSet(TRACE_2,QColor(Qt::yellow));
    pm.fill(Qt::blue);
    ui->bttnTraceColor3->setIcon(QIcon(pm));
    guiTraceColorSet(TRACE_3,QColor(Qt::blue));

    ui->cbTraceMode0->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode1->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode2->setCurrentIndex(T_MODE_OFF);
    ui->cbTraceMode3->setCurrentIndex(T_MODE_OFF);

    ui->bttnTraceClear0->setEnabled(false);
    ui->bttnTraceClear1->setEnabled(false);
    ui->bttnTraceClear2->setEnabled(false);
    ui->bttnTraceClear3->setEnabled(false);

    ui->bttnTraceHold0->setEnabled(false);
    ui->bttnTraceHold1->setEnabled(false);
    ui->bttnTraceHold2->setEnabled(false);
    ui->bttnTraceHold3->setEnabled(false);

    // Hardware
    ui->bttnHwConnect->setEnabled(false);

    strHwUsbInfoList.clear();
    mHwUsbInfo->setStringList(strHwUsbInfoList);
    ui->lvHwUsbInfo->setModel(mHwUsbInfo);
    ui->grpHwUsbInfo->setVisible(true);

    strHwDevInfoList.clear();
    mHwDevInfo->setStringList(strHwDevInfoList);
    ui->lvHwDevInfo->setModel(mHwDevInfo);
    ui->grpHwDevInfo->setVisible(false);
    // Freuqency Settings
    ui->swFrqModeCenterRange->setCurrentIndex(1);
    ui->bttnFrqRangeMode->setEnabled(true);
    ui->bttnFrqCenterMode->setEnabled(false);
    // RF Mode
    ui->swFrqSpan->setCurrentIndex(0);
    ui->grpRfBandWidth->setHidden(true);
    ui->bttnRfModeEasyRf->setEnabled(false);
    ui->bttnRfModeExpertRf->setEnabled(true);
    // Range
    ui->rbRfRange_861_1054->setChecked(true);
    ui->cbLPBoardRFLimits->setChecked(true);
    guiFrqBand2StartStop();
    // Reference Level
    ui->cbRefDcLevelValue->setCurrentIndex(ui->cbRefDcLevelValue->count()-1);
    // Resolution Bandwidth
    ui->cbResolutionBandWidthInput->setCurrentIndex(0);

    // ToolBox TabControl
    ui->tabMarker->setEnabled(false);
    ui->tabScreen->setEnabled(true);
    ui->tabTraces->setEnabled(false);
    ui->tabRfSettings->setEnabled(false);
    // Main Tool Bar
    // Create Action
    actionConnect      = new QAction(QIcon(":/img/32Connect"),tr("&Connect"), this);
    actionDisconnect   = new QAction(QIcon(":/img/32Disconnect"),tr("&Disconnect"), this);
    actionSpectrumStart= new QAction(QIcon(":/img/32Play"),tr("&Start"), this);
    actionSpectrumStop = new QAction(QIcon(":/img/32Stop"),tr("&Stop"), this);
    actionSave         = new QAction(QIcon(":/img/32Capture"),tr("&Save"), this);
    actionPrint        = new QAction(QIcon(":/img/32Print"),tr("&Print"), this);
    // Tool Tip Assignment
    actionConnect->setStatusTip(tr("Connect selected Device"));
    actionDisconnect->setStatusTip(tr("Close selected Device"));
    actionSpectrumStart->setStatusTip(tr("Start Spectrum"));
    actionSpectrumStop->setStatusTip(tr("Stop Spectrum"));
    actionSave->setStatusTip(tr("Save"));
    actionPrint->setStatusTip(tr("Print"));
    // Graph

    // Register Action to ToolBar
    ui->mainToolBar->addAction(actionConnect);
    ui->mainToolBar->addAction(actionDisconnect);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionSpectrumStart);
    ui->mainToolBar->addAction(actionSpectrumStop);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionSave);
    ui->mainToolBar->addAction(actionPrint);
    // Init State
    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionSpectrumStart->setEnabled(false);
    actionSpectrumStop->setEnabled(false);
    // Init Trace
    ui->txtTrace0_Label->hide();
    ui->txtTrace0_Mode->hide();
    ui->txtTrace1_Label->hide();
    ui->txtTrace1_Mode->hide();
    ui->txtTrace2_Label->hide();
    ui->txtTrace2_Mode->hide();
    ui->txtTrace3_Label->hide();
    ui->txtTrace3_Mode->hide();
    // Others
    eventTimeDateUpdateTick();
    timedateCtrl->start(1000);

    ui->grpMarkerPeakDetection->hide();
}

void MainWindow::initGuiEvents(void)
{
    // Application Specific Events

    // Menu Entry Events
    // Menu Entry Help
    connect(ui->actionHelpManual,SIGNAL(triggered()),this,SLOT(eventHelpOpenUsersGuide()));
    connect(ui->actionSupportLink,SIGNAL(triggered()),this,SLOT(eventHelpOpenSupportLink()));

    connect(ui->actionHelpAbout,SIGNAL(triggered()),this,SLOT(eventHelpOpenReadme()));

    // Menu Entry Spectrum
    connect(ui->actionTracesAllClear,SIGNAL(triggered()),this,SLOT(eventTraceAllClear()));
    connect(ui->actionTracesAllOff,SIGNAL(triggered()),this,SLOT(eventTraceAllOff()));

    connect(ui->actionMarkerAllOff,SIGNAL(triggered()),this,SLOT(eventMarkerAllOff()));

    connect(ui->actionGridOff,SIGNAL(triggered()),this,SLOT(eventGridMode_Off()));
    connect(ui->actionGridLines,SIGNAL(triggered()),this,SLOT(eventGridMode_Lines()));
    connect(ui->actionGridDotted,SIGNAL(triggered()),this,SLOT(eventGridMode_Dotted()));

    // Menu Entry Settings
    connect(ui->actionSettingsUndo,SIGNAL(triggered()),this,SLOT(eventFrqUndo()));
    connect(ui->actionSettingsSave,SIGNAL(triggered()),this,SLOT(eventFrqSave()));
    connect(ui->actionSettingsLoad,SIGNAL(triggered()),this,SLOT(eventFrqLoad()));

    // Menu Entry Hardware
    connect(ui->actionDeviceConnect,SIGNAL(triggered()),this,SLOT(eventDeviceConnect()));
    connect(ui->actionDeviceDisconnect,SIGNAL(triggered()),this,SLOT(eventDeviceDisconnect()));

    connect(ui->actionDeviceFwUpdate,SIGNAL(triggered()),this,SLOT(eventFirmwareUpdate()));

    // Menu Entry File
    connect(ui->actionPrint,SIGNAL(triggered()),this,SLOT(eventPrint()));
    connect(ui->actionCapturePng,SIGNAL(triggered()),this,SLOT(eventCapture()));
    connect(ui->actionExport,SIGNAL(triggered()),this,SLOT(eventExport()));

    connect(ui->actionAppExit,SIGNAL(triggered()),this,SLOT(close()));

    // Tab ToolBox Events
    // Tab ToolBox Screen
    connect(ui->rbGridModeOff,SIGNAL(clicked()),this,SLOT(eventGridMode_Off()));
    connect(ui->rbGridModeLine,SIGNAL(clicked()),this,SLOT(eventGridMode_Lines()));
    connect(ui->rbGridModeDotted,SIGNAL(clicked()),this,SLOT(eventGridMode_Dotted()));

    connect(ui->cbScreenDisplayLogo,SIGNAL(toggled(bool)),ui->imgLogo,SLOT(setVisible(bool)));
    connect(ui->cbScreenDisplayDateTime,SIGNAL(toggled(bool)),ui->txtGraphDate,SLOT(setVisible(bool)));
    connect(ui->txtScreenDisplayLabel,SIGNAL(textChanged(QString)),this,SLOT(eventScreenCommentTextChanged(QString)));
    connect(ui->txtScreenDisplayComment,SIGNAL(textChanged(QString)),ui->txtGraphComment,SLOT(setText(QString)));

    connect(ui->bttnScreenPrint,SIGNAL(clicked(bool)),this,SLOT(eventPrint()));
    connect(ui->cbScreenSaveToFileGraphOnly,SIGNAL(toggled(bool)),this,SLOT(eventSaveGraphOnlyChanged(bool)));
    connect(ui->bttnScreenSaveToFile,SIGNAL(clicked()),this,SLOT(eventSave()));

    // Tab ToolBox Markers
    connect(ui->cbMarker0TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker0SetTrace(int)));
    connect(ui->cbMarker1TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker1SetTrace(int)));
    connect(ui->cbMarker2TraceSelected,SIGNAL(currentIndexChanged(int)),this,SLOT(eventMarker2SetTrace(int)));

    connect(ui->bttnMarkerMoveM0,SIGNAL(clicked()),this,SLOT(eventSelectMarker0()));
    connect(ui->bttnMarkerMoveM1,SIGNAL(clicked()),this,SLOT(eventSelectMarker1()));
    connect(ui->bttnMarkerMoveM2,SIGNAL(clicked()),this,SLOT(eventSelectMarker2()));

    connect(ui->MarkerDial,SIGNAL(valueChanged(int)),this,SLOT(eventMarkerMove(int)));

    // Tab ToolBox Traces
    connect(ui->cbTraceMode0,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace0ModeChanged(int)));
    connect(ui->cbTraceMode1,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace1ModeChanged(int)));
    connect(ui->cbTraceMode2,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace2ModeChanged(int)));
    connect(ui->cbTraceMode3,SIGNAL(currentIndexChanged(int)),this,SLOT(eventTrace3ModeChanged(int)));

    connect(ui->bttnTraceExport0,SIGNAL(clicked()),this,SLOT(eventTrace0Export()));
    connect(ui->bttnTraceExport1,SIGNAL(clicked()),this,SLOT(eventTrace1Export()));
    connect(ui->bttnTraceExport2,SIGNAL(clicked()),this,SLOT(eventTrace2Export()));
    connect(ui->bttnTraceExport3,SIGNAL(clicked()),this,SLOT(eventTrace3Export()));

    connect(ui->txtTraceLabel0,SIGNAL(textChanged(QString)),ui->txtTrace0_Label,SLOT(setText(QString)));
    connect(ui->txtTraceLabel1,SIGNAL(textChanged(QString)),ui->txtTrace1_Label,SLOT(setText(QString)));
    connect(ui->txtTraceLabel2,SIGNAL(textChanged(QString)),ui->txtTrace2_Label,SLOT(setText(QString)));
    connect(ui->txtTraceLabel3,SIGNAL(textChanged(QString)),ui->txtTrace3_Label,SLOT(setText(QString)));

    connect(ui->bttnTraceColor0,SIGNAL(clicked()),this,SLOT(eventTrace0ColorChange()));
    connect(ui->bttnTraceColor1,SIGNAL(clicked()),this,SLOT(eventTrace1ColorChange()));
    connect(ui->bttnTraceColor2,SIGNAL(clicked()),this,SLOT(eventTrace2ColorChange()));
    connect(ui->bttnTraceColor3,SIGNAL(clicked()),this,SLOT(eventTrace3ColorChange()));

    connect(ui->bttnTraceHold0,SIGNAL(toggled(bool)),this,SLOT(eventTrace0HoldTrigger(bool)));
    connect(ui->bttnTraceHold1,SIGNAL(toggled(bool)),this,SLOT(eventTrace1HoldTrigger(bool)));
    connect(ui->bttnTraceHold2,SIGNAL(toggled(bool)),this,SLOT(eventTrace2HoldTrigger(bool)));
    connect(ui->bttnTraceHold3,SIGNAL(toggled(bool)),this,SLOT(eventTrace3HoldTrigger(bool)));

    connect(ui->bttnTraceClear0,SIGNAL(clicked()),this,SLOT(eventTrace0Clear()));
    connect(ui->bttnTraceClear1,SIGNAL(clicked()),this,SLOT(eventTrace1Clear()));
    connect(ui->bttnTraceClear2,SIGNAL(clicked()),this,SLOT(eventTrace2Clear()));
    connect(ui->bttnTraceClear3,SIGNAL(clicked()),this,SLOT(eventTrace3Clear()));

    // Tab ToolBox Frequency Settings
    connect(ui->bttnRfModeEasyRf,SIGNAL(clicked()),this,SLOT(eventRfModeEasyRfChange()));
    connect(ui->bttnRfModeExpertRf,SIGNAL(clicked()),this,SLOT(eventRfModeEasyRfChange()));

    connect(ui->cbLPBoardRFLimits, SIGNAL(clicked()),this,SLOT(eventSA1350BoardRFLimits()));
    connect(ui->rbRfRange_431_527,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));
    connect(ui->rbRfRange_861_1054,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));
    connect(ui->rbRfRange_2152_2635,SIGNAL(clicked()),this,SLOT(eventFrqRangeChanged()));

    connect(ui->bttnFrqCenterMode,SIGNAL(clicked()),this,SLOT(eventFrqModeCenterRangeChange()));
    connect(ui->bttnFrqRangeMode,SIGNAL(clicked()),this,SLOT(eventFrqModeCenterRangeChange()));
    connect(ui->spFrqCenterValue,SIGNAL(editingFinished()),this,SLOT(eventFrqCenterChanged()));
    connect(ui->cbFrqSpanValue,SIGNAL(currentIndexChanged(int)),this,SLOT(eventFrqSpanChanged()));
    connect(ui->sbFrqSpanInput,SIGNAL(valueChanged(int)),this,SLOT(eventExpertFrqSpanChanged()));
    connect(ui->spFrqStartValue,SIGNAL(editingFinished()),this,SLOT(eventFrqStartChanged()));
    connect(ui->spFrqStopValue,SIGNAL(editingFinished()),this,SLOT(eventFrqStopChanged()));

    connect(ui->cbResolutionBandWidthInput,SIGNAL(currentIndexChanged(int)),this,SLOT(eventRBWchanged(int)));
    connect(ui->sbFrqStepwidthInput,SIGNAL(valueChanged(double)),this,SLOT(eventFrqStepWidthChanged(void)));

    connect(ui->bttnFrqSet,SIGNAL(clicked()),this,SLOT(eventFrqSet()));
    connect(ui->bttnFrqStop,SIGNAL(clicked()),this,SLOT(eventFrqSpectrumStop()));

    connect(ui->bttnFrqSettingUndo,SIGNAL(clicked()),this,SLOT(eventFrqUndo()));
    connect(ui->bttnFrqSettingSave,SIGNAL(clicked()),this,SLOT(eventFrqSave()));
    connect(ui->bttnFrqSettingLoad,SIGNAL(clicked()),this,SLOT(eventFrqLoad()));

    // Tab ToolBox Hardware
    connect(ui->bttnHwDeviceListScan,SIGNAL(clicked()),this,SLOT(eventUpdateDeviceList()));
    connect(ui->cbDeviceList,SIGNAL(currentIndexChanged(int)),this,SLOT(eventDeviceListIndexChanged(int)));
    connect(ui->bttnHwConnect,SIGNAL(toggled(bool)),this,SLOT(eventDeviceStartStop(bool)));

    // ToolBar
    connect(actionConnect,SIGNAL(triggered()),this,SLOT(eventDeviceConnect()));
    connect(actionDisconnect,SIGNAL(triggered()),this,SLOT(eventDeviceDisconnect()));
    connect(actionSave,SIGNAL(triggered()),this,SLOT(eventSave()));
    connect(actionPrint,SIGNAL(triggered()),this,SLOT(eventPrint()));
    connect(actionSpectrumStart,SIGNAL(triggered()),this,SLOT(eventFrqSet()));
    connect(actionSpectrumStop ,SIGNAL(triggered()),this,SLOT(eventFrqSpectrumStop()));

    // Others
    connect(timedateCtrl,SIGNAL(timeout()),this,SLOT(eventTimeDateUpdateTick()));

    //SA1350Driver Events
    connect(deviceCtrl,SIGNAL(signalConnected()),this,SLOT(eventSA1350DeviceConnected()));
    connect(deviceCtrl,SIGNAL(signalDisconnected()),this,SLOT(eventSA1350DeviceDisconnected()));
    connect(deviceCtrl,SIGNAL(signalErrorMsg(QString)),this,SLOT(eventSA1350ErrorMsg(QString)));
    connect(deviceCtrl,SIGNAL(signalSpectrumReceived()),this,SLOT(eventSA1350SpectrumReceived()));
    connect(deviceCtrl,SIGNAL(signalNewParameterSet(bool,int)),this,SLOT(eventSA1350NewParameterSet(bool,int)));
    connect(deviceCtrl,SIGNAL(signalDeviceUpdateRequired(QString)),this,SLOT(eventSA1350FirmwareUpdateRequired(QString)));
}

void MainWindow::initPlotCtrl(void)
{
    // Grid
    guiSetGridMode(GRID_DOTTED);
    // Markers
    plotCtrl->MarkerOff(M_0);
    plotCtrl->MarkerOff(M_1);
    plotCtrl->MarkerOff(M_2);
    // Traces
    plotCtrl->TraceSetMode(TRACE_0,T_MODE_OFF);
    plotCtrl->TraceSetMode(TRACE_1,T_MODE_OFF);
    plotCtrl->TraceSetMode(TRACE_2,T_MODE_OFF);
    plotCtrl->TraceSetMode(TRACE_3,T_MODE_OFF);
}

void MainWindow::guiUpdateDeviceList(void)
{
    QString prevPort = ui->cbDeviceList->itemText(ui->cbDeviceList->currentIndex());
    unsigned short prevPortIndex = 0;
    sa1350UsbDevice deviceItem;
    unsigned short NrOfDevices;

    prevPort.truncate(prevPort.indexOf(' '));

    // Search Number Of Connected Devices
    ui->cbDeviceList->clear();
    DeviceList.clear();
    if(deviceCtrl->deviceSearch(NrOfDevices))
    {
        for(unsigned short index=0;index<NrOfDevices;index++)
        {// Get for each device the Device Details
            if(deviceCtrl->deviceGetInfo(index,&deviceItem))
            {
                if(deviceItem.Connected)
                {// Add the information to the device list
                    DeviceList.append(deviceItem);
                };
            };
        };
        // Update Hardware device List field
        foreach(sa1350UsbDevice item,DeviceList)
        {
            if(item.Connected)
            {
                ui->cbDeviceList->addItem(QString("%0 - Connected").arg(item.PortName));
                if(QString(item.PortName).startsWith(prevPort))
                {
                    prevPortIndex = ui->cbDeviceList->count()-1;
                };
            };
        };
    };
    if(DeviceList.length() > 0)
    {// Restore to previously connected or first device index
        ui->cbDeviceList->setCurrentIndex(prevPortIndex);
        ui->cbDeviceList->setEnabled(true);
    }
    else
    {// No devices currently available
        ui->cbDeviceList->setEnabled(false);
    };
}

void MainWindow::guiUpdateRBWList(sDeviceInfo *devInfo)
{
    int savedRBWindex;

    // Save current RBW index incase we're reconnecting.
    savedRBWindex = ui->cbResolutionBandWidthInput->currentIndex();

    // Load RBW information from target
    ui->cbResolutionBandWidthInput->blockSignals(true);
    ui->cbResolutionBandWidthInput->clear();
    for(int i=0;i<devInfo->ActiveRBWTable->size();i++)
    {
        ui->cbResolutionBandWidthInput->addItem(QLocale(QLocale::English, QLocale::UnitedStates).toString(devInfo->ActiveRBWTable->at(i).FrqKHz, 'f', 1));
    };

    // Restore RBW index incase we're reconnecting.
    if ((savedRBWindex > -1) && (savedRBWindex < devInfo->ActiveRBWTable->size()))
    {
        ui->cbResolutionBandWidthInput->setCurrentIndex(savedRBWindex);
    }
    // Set to lowest RBW greater than 2x FSW on initial connect.
    else
    {
        for(savedRBWindex=0;savedRBWindex<devInfo->ActiveRBWTable->size();savedRBWindex++)
        {
            if(ui->cbResolutionBandWidthInput->itemText(savedRBWindex).toDouble() >=
                    ui->sbFrqStepwidthInput->value()*2)
                break;
        };
        ui->cbResolutionBandWidthInput->setCurrentIndex(savedRBWindex);
    };
    ui->cbResolutionBandWidthInput->blockSignals(false);
}

void MainWindow::guiUpdateFrq(sFrqValues *FrqSetting)
{
    guiDisplayFrqSettings(FrqSetting);
}

void MainWindow::guiSetGridMode(eGridMode mode)
{
    switch(mode)
    {
    case GRID_OFF:
        if(!ui->rbGridModeOff->isChecked())
        {
            ui->rbGridModeOff->setChecked(true);
            ui->rbGridModeLine->setChecked(false);
            ui->rbGridModeDotted->setChecked(false);
        };
        ui->actionGridOff->setChecked(true);
        ui->actionGridDotted->setChecked(false);
        ui->actionGridLines->setChecked(false);
        plotCtrl->GridOff();
        break;
    case GRID_LINES:
        if(!ui->rbGridModeLine->isChecked())
        {
            ui->rbGridModeOff->setChecked(false);
            ui->rbGridModeLine->setChecked(true);
            ui->rbGridModeDotted->setChecked(false);
        };
        ui->actionGridOff->setChecked(false);
        ui->actionGridDotted->setChecked(false);
        ui->actionGridLines->setChecked(true);
        plotCtrl->GridLines();
        break;
    case GRID_DOTTED:
        if(!ui->rbGridModeDotted->isChecked())
        {
            ui->rbGridModeOff->setChecked(false);
            ui->rbGridModeLine->setChecked(false);
            ui->rbGridModeDotted->setChecked(true);
        };
        ui->actionGridOff->setChecked(false);
        ui->actionGridDotted->setChecked(true);
        ui->actionGridLines->setChecked(false);
        plotCtrl->GridDotted();
        break;
    default:
        break;
    }
}

void MainWindow::guiSetTraceMode(eTrace trace, eTraceMode mode)
{
    trace = trace;
    mode = mode;
}

bool MainWindow::guiSaveFrqProfile(sFrqSetting &tmpFrqSet)
{
    Q_UNUSED(tmpFrqSet)

    return(true);
}

bool MainWindow::guiLoadFrqProfile(sFrqSetting *newFrqSetting)
{
    bool done = false;

    if(newFrqSetting)
    {
        // Frq Expert Mode
        ui->bttnRfModeEasyRf->setEnabled(newFrqSetting->Values.flagModeEasyRf);
        eventRfModeEasyRfChange();

        // Frq CenterSpan/StartStop Mode
        if(newFrqSetting->Values.flagModeStartStop)
        {// Mode StartStop
            ui->bttnFrqCenterMode->setEnabled(true);
            ui->bttnFrqRangeMode->setEnabled(false);
            // Frq ModeCenterSpan / StartStop Tab
            ui->swFrqModeCenterRange->setCurrentIndex(0);
        }
        else
        {// Mode CenterSpan
            ui->bttnFrqCenterMode->setEnabled(false);
            ui->bttnFrqRangeMode->setEnabled(true);
            // Frq ModeCenterSpan / StartStop Tab
            ui->swFrqModeCenterRange->setCurrentIndex(1);
        };
        // Frq FrqRange
        switch(newFrqSetting->Values.FrqRange)
        {
        case FRQRANGE_431_527:
            ui->rbRfRange_431_527->setChecked(true);
            break;
        case FRQRANGE_861_1054:
            ui->rbRfRange_861_1054->setChecked(true);
            break;
        case FRQRANGE_2152_2635:
            ui->rbRfRange_2152_2635->setChecked(true);
            break;
        case FRQRANGE_UNDEFINED:
            break;
        default:
            break;
        };
        eventFrqRangeChanged();
        // Frq Values Start<->Stop
        ui->spFrqStartValue->setValue(newFrqSetting->Values.FrqStart);
        ui->spFrqStopValue->setValue(newFrqSetting->Values.FrqStop);
        // Frq Values Span<-Center->Span
        ui->spFrqCenterValue->setValue(newFrqSetting->Values.FrqCenter);
        ui->cbFrqSpanValue->setCurrentIndex(newFrqSetting->Values.FrqSpanIndex);
        ui->sbFrqSpanInput->setValue(newFrqSetting->Values.FrqSpan);
        // Frq Single Continuous Mode
        ui->rbSweepModeContinuous->setChecked(newFrqSetting->Values.flagModeContinuous);
        // Frq StepWidth
        ui->sbFrqStepwidthInput->setValue(newFrqSetting->Values.FrqStepWidth);
        // Frq DcLevel Index
        ui->cbRefDcLevelValue->setCurrentIndex(newFrqSetting->Values.RefDcLevelIndex);
        // Frq RBW Index
        ui->cbResolutionBandWidthInput->setCurrentIndex(newFrqSetting->Values.RBWIndex);
        // Frq Sweep
        if(newFrqSetting->Values.flagModeContinuous)
        {// Continuous
            ui->rbSweepModeContinuous->setChecked(true);
        }
        else
        {// Single
            ui->rbSweepModeSingle->setChecked(true);
        };
        done = true;
    };

    return(done);
}

bool MainWindow::guiDisplayHwUsbInfo(sa1350UsbDevice *usbInfo)
{
    bool done = false;
    strHwUsbInfoList.clear();
    if(usbInfo)
    {
        strHwUsbInfoList.append(QString("Connected:"));
        if(usbInfo->Connected)
            strHwUsbInfoList.append(QString("   Yes"));
        else
            strHwUsbInfoList.append(QString("   No "));
        strHwUsbInfoList.append(QString("USBSerNr:"));
        strHwUsbInfoList.append(QString("   %0").arg(usbInfo->SerialNr));
        strHwUsbInfoList.append(QString("USBDescr:"));
        strHwUsbInfoList.append(QString("   %0").arg(usbInfo->DevDesc));
        strHwUsbInfoList.append(QString("ComPort :"));
        strHwUsbInfoList.append(QString("   %0").arg(usbInfo->PortName));
        done = true;
    };

    mHwUsbInfo->setStringList(strHwUsbInfoList);

    return(done);
}

bool MainWindow::guiDisplayHwDevInfo(sDeviceInfo *devInfo)
{
    strHwDevInfoList.clear();

    if(devInfo)
    {
        QString strDevVersion = devInfo->DeviceVersion;

        strHwDevInfoList.append(QString("Product :"));
        strHwDevInfoList.append(QString("  Device Version"));
        if(strDevVersion.size()>24)
        {
            strDevVersion.resize(21);
            strDevVersion.append("...");
        };
        strHwDevInfoList.append(QString("     %0").arg(strDevVersion));
        strHwDevInfoList.append(QString("  FW Version"));
        strHwDevInfoList.append(QString("     %0.%1").arg((unsigned char) (devInfo->FWVersion>>8),0,16).arg((unsigned char)(devInfo->FWVersion&0xff)));
        strHwDevInfoList.append(QString("  RxBW Options"));
        strHwDevInfoList.append(QString("     %0").arg((unsigned int) (devInfo->ActiveRBWTable->size())));
        strHwDevInfoList.append(QString("  Max Spectrum Size"));
        strHwDevInfoList.append(QString("     %0").arg((unsigned short) (devInfo->MaxSpecLength)));
        ui->grpHwDevInfo->setVisible(true);
    }
    else
    {
        ui->grpHwDevInfo->setVisible(false);
    };

    mHwDevInfo->setStringList(strHwDevInfoList);

    return(true);
}

bool MainWindow::guiDisplayHwCalData(sCalibrationData *calData)
{

    strHwDevInfoList.append(QString("Calibration :"));

    if(calData)
    {
        // Status
        strHwDevInfoList.append(QString("  Status"));
        strHwDevInfoList.append(QString("     Loaded"));
        // Date
        strHwDevInfoList.append(QString("  Date"));
        calData->CalDate[15] = 0;
        strHwDevInfoList.append(QString("     %0").arg(QString(&calData->CalDate[0])));
        // Format Version
        strHwDevInfoList.append(QString("  Format Version"));
        strHwDevInfoList.append(QString("     V%0.%1").arg((unsigned short)calData->CalFormatVer>>8,0,16).arg((unsigned short)calData->CalFormatVer & 0xff ,0,16));

        ui->grpHwDevInfo->setVisible(true);
    }
    else
    {
        ui->grpHwDevInfo->setVisible(false);
    };

    mHwDevInfo->setStringList(strHwDevInfoList);

    return(true);
}

bool MainWindow::guiDisplayFrqSettings(sFrqValues *FrqSettings)
{
    bool done = false;
    if(FrqSettings)
    {
        long steps = (unsigned long)((FrqSettings->FrqSpan*(double)1000.0)/FrqSettings->FrqStepWidth) + 1;

        if(!ui->bttnFrqRangeMode->isEnabled())
        {//FStartStop
            // Label
            ui->txtFrqStartCenter_Label->setText(QString("Start"));
            ui->txtFrqStopSpan_Label->setText(QString("Stop"));
            // Value
            ui->txtFrqStartCenter_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(FrqSettings->FrqStart*1000000)));
            ui->txtFrqStopSpan_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(FrqSettings->FrqStop*1000000)));
        }
        else
        {//FCenterSpan
            // Label
            ui->txtFrqStartCenter_Label->setText(QString("Center"));
            ui->txtFrqStopSpan_Label->setText(QString("Span"));
            // Value
            ui->txtFrqStartCenter_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(FrqSettings->FrqCenter*1000000)));
            ui->txtFrqStopSpan_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(FrqSettings->FrqSpan*1000000)));
        };
        ui->txtFrqPoints_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)steps));
        ui->txtFrqFSW_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(FrqSettings->FrqStepWidth,'f',3));
        ui->txtFrqRBW_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(FrqSettings->RBW,'f',3));
        ui->txtFrqRefLvl_Value->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)FrqSettings->RefDcLevel));
        done = true;
    }
    else
    {
        if(ui->bttnFrqRangeMode)
        {//FStartStop
            // Label
            ui->txtFrqStartCenter_Label->setText(QString("Start"));
            ui->txtFrqStopSpan_Label->setText(QString("Stop"));
            // Value
            ui->txtFrqStartCenter_Value->setText(" --- ");
            ui->txtFrqStopSpan_Value->setText(" --- ");
        }
        else
        {//FCenterSpan
            // Label
            ui->txtFrqStartCenter_Label->setText("Center");
            ui->txtFrqStopSpan_Label->setText("Span");
            // Value
            ui->txtFrqStartCenter_Value->setText(" --- ");
            ui->txtFrqStopSpan_Value->setText(" --- ");
        };

        ui->txtFrqStartCenter_Value->setText(" --- ");
        ui->txtFrqStopSpan_Value->setText(" --- ");
        ui->txtFrqRBW_Value->setText(" --- ");
        ui->txtFrqFSW_Value->setText(" --- ");
        ui->txtFrqRefLvl_Value->setText(" --- ");
    };

    return(done);
}

bool MainWindow::guiDisplayMarkerInfo(sMarkerInfo *mInfo)
{
    bool done = false;
    bool flagMarker0Delta = false;
    bool flagMarker2Delta = false;
    double deltaMHz = 0;
    double deltadBm = 0;

    QLabel *ptrMarkerdBm[3]={ui->txtGraphMarker0_Value_dBm,ui->txtGraphMarker1_Value_dBm,ui->txtGraphMarker2_Value_dBm};
    QLabel *ptrMarkerMHz[3]={ui->txtGraphMarker0_Value_MHz,ui->txtGraphMarker1_Value_MHz,ui->txtGraphMarker2_Value_MHz};
    QLabel *ptrMarkerLabel[3]={ui->txtGraphMarker0_Label,ui->txtGraphMarker1_Label,ui->txtGraphMarker2_Label};
    QLabel *ptrMarkerUnitdBm[3]={ui->txtGraphMarker0_Unit_dBm,ui->txtGraphMarker1_Unit_dBm,ui->txtGraphMarker2_Unit_dBm};

    if(mInfo)
    {// Init Display Information
        // Check Delta Mode
        if(mInfo->On[0] && mInfo->On[1])
            if(ui->rbMarker0MakeDeltaM1->isEnabled())
                if(ui->rbMarker0MakeDeltaM1->isChecked())
                    flagMarker0Delta = true;

        if(mInfo->On[2] && mInfo->On[1])
            if(ui->rbMarker2MakeDeltaM1->isEnabled())
                if(ui->rbMarker2MakeDeltaM1->isChecked())
                    flagMarker2Delta = true;
        //Update Display
        // Marker 0
        if(mInfo->On[0])
        {
            if(flagMarker0Delta)
            {
                if(mInfo->dBmValue[0] > mInfo->dBmValue[1])
                {
                    deltadBm = mInfo->dBmValue[0] - mInfo->dBmValue[1];
                }
                else
                {
                    deltadBm =(mInfo->dBmValue[1] - mInfo->dBmValue[0]);
                    deltadBm = -deltadBm;
                };
                if(mInfo->FrqValue[0] > mInfo->FrqValue[1])
                {
                    deltaMHz = mInfo->FrqValue[0] - mInfo->FrqValue[1];
                }
                else
                {
                    deltaMHz = (mInfo->FrqValue[1] - mInfo->FrqValue[0]);
                    deltaMHz = -deltaMHz;
                };
                ptrMarkerLabel[0]->setText("M0*");
                ptrMarkerUnitdBm[0]->setText("dB ");
                ptrMarkerdBm[0]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(deltadBm,'f',1));
                ptrMarkerMHz[0]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(deltaMHz*1000000)));
            }
            else
            {
                ptrMarkerLabel[0]->setText("M0  ");
                ptrMarkerUnitdBm[0]->setText("dBm");
                ptrMarkerdBm[0]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(mInfo->dBmValue[0],'f',1));
                ptrMarkerMHz[0]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(mInfo->FrqValue[0]*1000000)));
            };
        }
        else
        {
            ptrMarkerLabel[0]->setText("M0  ");
            ptrMarkerUnitdBm[0]->setText("dBm");
            ptrMarkerdBm[0]->setText(QString(" --- "));
            ptrMarkerMHz[0]->setText(QString(" --- "));
        };
        // Marker 1
        if(mInfo->On[1])
        {
            ptrMarkerdBm[1]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(mInfo->dBmValue[1],'f',1));
            ptrMarkerMHz[1]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(mInfo->FrqValue[1]*1000000)));
        }
        else
        {
            ptrMarkerdBm[1]->setText(QString(" --- "));
            ptrMarkerMHz[1]->setText(QString(" --- "));
        };
        // Marker 2
        if(mInfo->On[2])
        {
            if(flagMarker2Delta)
            {
                if(mInfo->dBmValue[2] > mInfo->dBmValue[1])
                {
                    deltadBm = mInfo->dBmValue[2] - mInfo->dBmValue[1];
                }
                else
                {
                    deltadBm = (mInfo->dBmValue[1] - mInfo->dBmValue[2]);
                    deltadBm = -deltadBm;
                };
                if(mInfo->FrqValue[2] > mInfo->FrqValue[1])
                {
                    deltaMHz = mInfo->FrqValue[2] - mInfo->FrqValue[1];
                }
                else
                {
                    deltaMHz = (mInfo->FrqValue[1] - mInfo->FrqValue[2]);
                    deltaMHz = -deltaMHz;
                };
                ptrMarkerLabel[2]->setText("M2*");
                ptrMarkerUnitdBm[2]->setText("dB ");
                ptrMarkerdBm[2]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(deltadBm,'f',1));
                ptrMarkerMHz[2]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)((deltaMHz)*1000000)));
            }
            else
            {
                ptrMarkerLabel[2]->setText("M2  ");
                ptrMarkerUnitdBm[2]->setText("dBm");
                ptrMarkerdBm[2]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString(mInfo->dBmValue[2],'f',1));
                ptrMarkerMHz[2]->setText(QLocale(QLocale::English, QLocale::UnitedStates).toString((qlonglong)(mInfo->FrqValue[2]*1000000)));
            };
        }
        else
        {
            ptrMarkerLabel[2]->setText("M2  ");
            ptrMarkerUnitdBm[2]->setText("dBm");
            ptrMarkerdBm[2]->setText(QString(" --- "));
            ptrMarkerMHz[2]->setText(QString(" --- "));
        };
    };

    return(done);
}

void MainWindow::guiMarkerSelect(eMarker Marker)
{
    plotCtrl->MarkerSetActive(Marker);
    ui->MarkerDial->setValue(plotCtrl->MarkerGetPos(Marker));
    switch(Marker)
    {
    case M_0:
        ui->bttnMarkerMoveM0->setChecked(true);
        ui->bttnMarkerMoveM1->setChecked(false);
        ui->bttnMarkerMoveM2->setChecked(false);
        break;
    case M_1:
        ui->bttnMarkerMoveM0->setChecked(false);
        ui->bttnMarkerMoveM1->setChecked(true);
        ui->bttnMarkerMoveM2->setChecked(false);
        break;
    case M_2:
        ui->bttnMarkerMoveM0->setChecked(false);
        ui->bttnMarkerMoveM1->setChecked(false);
        ui->bttnMarkerMoveM2->setChecked(true);
        break;
    case M_OFF:
    case M_PEAK:
    case M_ALL:
        break;
    default:
        break;
    };
}

bool MainWindow::guiMarkerSetTrace(eMarker Marker, eTrace TraceNr)
{
    bool done = false;

    if(plotCtrl->MarkerSetTrace(Marker,TraceNr))
    {
        switch(Marker)
        {
        case M_0:
            ui->bttnMarkerMoveM0->setEnabled(true);
            if(ui->cbMarker1TraceSelected->currentIndex()!=M_OFF)
            {
                ui->rbMarker0MakeDeltaM1->setChecked(false);
                ui->rbMarker0MakeDeltaM1->setEnabled(true);
            };
            guiMarkerSelect(Marker);
            break;
        case M_1:
            ui->bttnMarkerMoveM1->setEnabled(true);
            if(ui->cbMarker0TraceSelected->currentIndex()!=M_OFF)
            {
                ui->rbMarker0MakeDeltaM1->setChecked(false);
                ui->rbMarker0MakeDeltaM1->setEnabled(true);
            };
            if(ui->cbMarker2TraceSelected->currentIndex()!=M_OFF)
            {
                ui->rbMarker2MakeDeltaM1->setChecked(false);
                ui->rbMarker2MakeDeltaM1->setEnabled(true);
            };
            guiMarkerSelect(Marker);
            break;
        case M_2:
            ui->bttnMarkerMoveM2->setEnabled(true);
            if(ui->cbMarker1TraceSelected->currentIndex()!=M_OFF)
            {
                ui->rbMarker2MakeDeltaM1->setChecked(false);
                ui->rbMarker2MakeDeltaM1->setEnabled(true);
            };
            guiMarkerSelect(Marker);
            break;
        case M_OFF:
        case M_PEAK:
        case M_ALL:
            break;
        default:
            break;
        };
        done = true;
    }
    else
    {
        switch(Marker)
        {
        case M_0:
            ui->cbMarker0TraceSelected->blockSignals(true);
            ui->cbMarker0TraceSelected->setCurrentIndex(M_OFF);
            ui->bttnMarkerMoveM0->setChecked(false);
            ui->bttnMarkerMoveM0->setEnabled(false);
            ui->rbMarker0MakeDeltaM1->setChecked(false);
            ui->rbMarker0MakeDeltaM1->setEnabled(false);
            ui->cbMarker0TraceSelected->blockSignals(false);
            break;
        case M_1:
            ui->cbMarker1TraceSelected->blockSignals(true);
            ui->cbMarker1TraceSelected->setCurrentIndex(M_OFF);
            ui->bttnMarkerMoveM1->setChecked(false);
            ui->bttnMarkerMoveM1->setEnabled(false);
            ui->rbMarker0MakeDeltaM1->setChecked(false);
            ui->rbMarker0MakeDeltaM1->setEnabled(false);
            ui->rbMarker2MakeDeltaM1->setChecked(false);
            ui->rbMarker2MakeDeltaM1->setEnabled(false);
            ui->cbMarker1TraceSelected->blockSignals(false);
            break;
        case M_2:
            ui->cbMarker2TraceSelected->blockSignals(true);
            ui->cbMarker2TraceSelected->setCurrentIndex(M_OFF);
            ui->bttnMarkerMoveM2->setChecked(false);
            ui->bttnMarkerMoveM2->setEnabled(false);
            ui->rbMarker2MakeDeltaM1->setChecked(false);
            ui->rbMarker2MakeDeltaM1->setEnabled(false);
            ui->cbMarker2TraceSelected->blockSignals(false);
            break;
        case M_OFF:
        case M_PEAK:
        case M_ALL:
            break;
        default:
            break;
        };
        done = false;
    };

    return(done);
}

void MainWindow::guiMarkerSetTraceUpdate(eTrace TraceNr)
{
    //Marker 0
    if(ui->cbMarker0TraceSelected->currentIndex() == TraceNr)
    {
        guiMarkerSetTrace(M_0,TraceNr);
    };
    //Marker 1
    if(ui->cbMarker1TraceSelected->currentIndex() == TraceNr)
    {
        guiMarkerSetTrace(M_1,TraceNr);
    };
    //Marker 1
    if(ui->cbMarker2TraceSelected->currentIndex() == TraceNr)
    {
        guiMarkerSetTrace(M_2,TraceNr);
    };
}

void MainWindow::guiTraceColorSelect(eTrace TraceNr)
{
    QColor color;

    color = QColorDialog::getColor(plotCtrl->TraceGetColor(TraceNr), this,"Select Trace Color");
    if(color.isValid())
    {
        guiTraceColorSet(TraceNr,color);
    };
}

void MainWindow::guiTraceColorSet(eTrace TraceNr, QColor newColor)
{
    QPixmap pm(32,14);
    QString txtStyleSheet;

    plotCtrl->TraceSetColor(TraceNr,newColor);
    pm.fill(newColor);
    txtStyleSheet = QString("color: rgb(%0 ,%1 ,%2);")
            .arg(newColor.toRgb().red())
            .arg(newColor.toRgb().green())
            .arg(newColor.toRgb().blue());
    switch(TraceNr)
    {
    case TRACE_0:
        ui->bttnTraceColor0->setIcon(QIcon(pm));
        ui->txtTrace0_Label->setStyleSheet(txtStyleSheet);
        ui->txtTrace0_Mode->setStyleSheet(txtStyleSheet);
        break;
    case TRACE_1:
        ui->bttnTraceColor1->setIcon(QIcon(pm));
        ui->txtTrace1_Label->setStyleSheet(txtStyleSheet);
        ui->txtTrace1_Mode->setStyleSheet(txtStyleSheet);
        break;
    case TRACE_2:
        ui->bttnTraceColor2->setIcon(QIcon(pm));
        ui->txtTrace2_Label->setStyleSheet(txtStyleSheet);
        ui->txtTrace2_Mode->setStyleSheet(txtStyleSheet);
        break;
    case TRACE_3:
        ui->bttnTraceColor3->setIcon(QIcon(pm));
        ui->txtTrace3_Label->setStyleSheet(txtStyleSheet);
        ui->txtTrace3_Mode->setStyleSheet(txtStyleSheet);
        break;
    case TRACE_UNDEFINED:
        break;

    default:
        break;
    };
}

void MainWindow::guiTraceModeChange(eTrace TraceNr, eTraceMode trcMode)
{
    bool flagClrWrite = false;
    QString txtMode;
    switch(trcMode)
    {
    case T_MODE_UNDEFINED:
    case T_MODE_OFF:
        // Switch Trace enabled Marker
        if(ui->cbMarker0TraceSelected->currentIndex() == TraceNr)
            guiMarkerSetTrace(M_0,TRACE_OFF);
        if(ui->cbMarker1TraceSelected->currentIndex() == TraceNr)
            guiMarkerSetTrace(M_1,TRACE_OFF);
        if(ui->cbMarker2TraceSelected->currentIndex() == TraceNr)
            guiMarkerSetTrace(M_2,TRACE_OFF);
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        // Switch Off continuous Spec Trigger if not required
        if(
                ui->cbTraceMode0->currentIndex() == T_MODE_OFF
                && ui->cbTraceMode1->currentIndex() == T_MODE_OFF
                && ui->cbTraceMode2->currentIndex() == T_MODE_OFF
                && ui->cbTraceMode3->currentIndex() == T_MODE_OFF
                )
        {
            Status.Spectrum.flagModeContinuous = false;
        };
        txtMode.append("OFF");
        break;
    case T_MODE_CLEARWRITE:
        flagClrWrite = true;
        txtMode.append("ACT");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
        break;
    case T_MODE_MAXHOLD:
        txtMode.append("MAX");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
        break;
    case T_MODE_AVERAGE:
        txtMode.append("AVG");
        plotCtrl->TraceSetMode(TraceNr,trcMode);
        guiMarkerSetTraceUpdate(TraceNr);
        break;

    default:
        break;
    };

    if(trcMode==T_MODE_OFF)
    {
        switch(TraceNr)
        {
        case TRACE_0:
            ui->bttnTraceClear0->setDisabled(true);
            ui->bttnTraceColor0->setDisabled(true);
            ui->bttnTraceExport0->setDisabled(true);
            ui->txtTraceLabel0->setDisabled(true);
            ui->txtTrace0_Label->hide();
            ui->txtTrace0_Mode->hide();
            ui->bttnTraceHold0->setEnabled(false);
            break;
        case TRACE_1:
            ui->bttnTraceClear1->setDisabled(true);
            ui->bttnTraceColor1->setDisabled(true);
            ui->bttnTraceExport1->setDisabled(true);
            ui->txtTraceLabel1->setDisabled(true);
            ui->txtTrace1_Label->hide();
            ui->txtTrace1_Mode->hide();
            ui->bttnTraceHold1->setEnabled(false);
            break;
        case TRACE_2:
            ui->bttnTraceClear2->setDisabled(true);
            ui->bttnTraceColor2->setDisabled(true);
            ui->bttnTraceExport2->setDisabled(true);
            ui->txtTraceLabel2->setDisabled(true);
            ui->txtTrace2_Label->hide();
            ui->txtTrace2_Mode->hide();
            ui->bttnTraceHold2->setEnabled(false);
            break;
        case TRACE_3:
            ui->bttnTraceClear3->setDisabled(true);
            ui->bttnTraceColor3->setDisabled(true);
            ui->bttnTraceExport3->setDisabled(true);
            ui->txtTraceLabel3->setDisabled(true);
            ui->txtTrace3_Label->hide();
            ui->txtTrace3_Mode->hide();
            ui->bttnTraceHold3->setEnabled(false);
            break;

        default:
            break;
        };
    }
    else
    {
        switch(TraceNr)
        {
        case TRACE_0:
            if(flagClrWrite)
                ui->bttnTraceClear0->setDisabled(true);
            else
                ui->bttnTraceClear0->setDisabled(false);
            ui->bttnTraceColor0->setDisabled(false);
            ui->bttnTraceExport0->setDisabled(false);
            ui->txtTraceLabel0->setDisabled(false);
            ui->txtTrace0_Mode->setText(txtMode);
            ui->txtTrace0_Label->show();
            ui->txtTrace0_Mode->show();
            ui->bttnTraceHold0->setEnabled(true);
            break;
        case TRACE_1:
            if(flagClrWrite)
                ui->bttnTraceClear1->setDisabled(true);
            else
                ui->bttnTraceClear1->setDisabled(false);
            ui->bttnTraceClear1->setDisabled(false);
            ui->bttnTraceColor1->setDisabled(false);
            ui->bttnTraceExport1->setDisabled(false);
            ui->txtTraceLabel1->setDisabled(false);
            ui->txtTrace1_Mode->setText(txtMode);
            ui->txtTrace1_Label->show();
            ui->txtTrace1_Mode->show();
            ui->bttnTraceHold1->setEnabled(true);
            break;
        case TRACE_2:
            if(flagClrWrite)
                ui->bttnTraceClear2->setDisabled(true);
            else
                ui->bttnTraceClear2->setDisabled(false);
            ui->bttnTraceClear2->setDisabled(false);
            ui->bttnTraceColor2->setDisabled(false);
            ui->bttnTraceExport2->setDisabled(false);
            ui->txtTraceLabel2->setDisabled(false);
            ui->txtTrace2_Mode->setText(txtMode);
            ui->txtTrace2_Label->show();
            ui->txtTrace2_Mode->show();
            ui->bttnTraceHold2->setEnabled(true);
            break;
        case TRACE_3:
            if(flagClrWrite)
                ui->bttnTraceClear3->setDisabled(true);
            else
                ui->bttnTraceClear3->setDisabled(false);
            ui->bttnTraceClear3->setDisabled(false);
            ui->bttnTraceColor3->setDisabled(false);
            ui->bttnTraceExport3->setDisabled(false);
            ui->txtTraceLabel3->setDisabled(false);
            ui->txtTrace3_Mode->setText(txtMode);
            ui->txtTrace3_Label->show();
            ui->txtTrace3_Mode->show();
            ui->bttnTraceHold3->setEnabled(true);
            break;

        default:
            break;
        };
    };
}

bool MainWindow::guiTraceExport(eTrace TraceNr,eTraceMode TraceMode)
{
    bool         done = false;
    QString      strFileName;
    QVector<double> DataFrq;
    QVector<double> DatadBm;
    appReportCsv csvWriter;

    if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
        if(Status.Spectrum.flagActiveFrqValues)
            if(TraceMode != T_MODE_OFF)
            {
                strFileName = QFileDialog::getSaveFileName(this,tr("Save Trace Curve Data"),QDir::currentPath(), tr("Trace Curve File(*.csv)"));
                if(!strFileName.isEmpty())
                    if(plotCtrl->TraceGetData(TraceNr,&DataFrq,&DatadBm))
                        if(csvWriter.WriteCurveCsv(strFileName,&Status.Spectrum.activFrqValues,&DataFrq,&DatadBm))
                        {
                            done = true;
                        };
            };

    return(done);
}

bool MainWindow::guiTraceHoldTrigger(eTrace TraceNr)
{
    bool done = false;

    if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
    {
        plotCtrl->TraceHoldTrigger(TraceNr);
        done = true;
    };

    return(done);
}

bool MainWindow::guiTraceHoldReset(eTrace TraceNr)
{
    bool done = false;

    if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
    {
        plotCtrl->TraceHoldReset(TraceNr);
        done = true;
    };

    return(done);
}

bool MainWindow::guiTraceClear(eTrace TraceNr)
{
    bool done = false;

    if(TraceNr>= TRACE_0 && TraceNr <=TRACE_3)
    {
        plotCtrl->TraceClear(TraceNr);
        done = true;
    };

    return(done);
}

bool MainWindow::guiFrqGetActualSettings(sFrqValues *actualFrqValues)
{
    bool done = false;

    if(actualFrqValues)
    {
        // Check Frequency Range
        if(ui->rbRfRange_431_527->isChecked())
            actualFrqValues->FrqRange = FRQRANGE_431_527;
        else if(ui->rbRfRange_861_1054->isChecked())
            actualFrqValues->FrqRange = FRQRANGE_861_1054;
        else if(ui->rbRfRange_2152_2635->isChecked())
            actualFrqValues->FrqRange = FRQRANGE_2152_2635;
        // Check Spectrum Parameters
        if(!ui->bttnFrqRangeMode->isEnabled())
        {// FStart <-> FStop
            actualFrqValues->flagModeStartStop = true;
            guiFrqStartStop2Center();
        }
        else
        {// FSpan<-FCenter->FSpan
            actualFrqValues->flagModeStartStop = false;
            guiFrqCenter2StartStop();
        };
        actualFrqValues->FrqStart  = ui->spFrqStartValue->value();
        actualFrqValues->FrqStop   = ui->spFrqStopValue->value();
        actualFrqValues->FrqCenter = ui->spFrqCenterValue->value();

        // Reference Level
        actualFrqValues->RefDcLevelIndex = ui->cbRefDcLevelValue->currentIndex();
        actualFrqValues->RefDcLevel      = FrqTableRefLevel[ui->cbRefDcLevelValue->currentIndex()].Value;

        // Easy Rf and Expert Mode
        if(!ui->bttnRfModeEasyRf->isEnabled())
        {// EasyRF
            actualFrqValues->flagModeEasyRf  = true;

            // Frequency Span Index
            actualFrqValues->FrqSpanIndex    = ui->cbFrqSpanValue->currentIndex();

            // Frequency Span
            actualFrqValues->FrqSpan         = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].SpanMHz;

            // Resolution Band Width Index
            actualFrqValues->RBWIndex        = ui->cbFrqSpanValue->currentIndex();

            // Resolution Band Width
            actualFrqValues->RBW             = ui->cbResolutionBandWidthInput->itemText(FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].RBWindex).toDouble();

            // Frequency Step Width
            actualFrqValues->FrqStepWidth    = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].FSWkHz;
        }
        else
        {// ExpertRF
            bool ok;

            actualFrqValues->flagModeEasyRf  = false;

            // Frequency Span Index
            actualFrqValues->FrqSpanIndex    = EXPERT_RF_MODE;

            // Frequency Span
            actualFrqValues->FrqSpan         = ui->sbFrqSpanInput->value();

            // Resolution Band Width Index
            actualFrqValues->RBWIndex        = ui->cbResolutionBandWidthInput->currentIndex();

            // Resolution Band Width
            actualFrqValues->RBW             = QLocale(QLocale::English, QLocale::UnitedStates).toDouble(ui->cbResolutionBandWidthInput->currentText(), &ok);

            // Frequency Step Width
            actualFrqValues->FrqStepWidth    = ui->sbFrqStepwidthInput->value();

        };
        // Continuous and Single Mode
        if(ui->rbSweepModeContinuous->isChecked())
        {
            actualFrqValues->flagModeContinuous   = true;
        }
        else
        {
            actualFrqValues->flagModeContinuous   = false;
        };

        done = true;
    };

    return(done);
}

void MainWindow::guiFrqUpdateExpertRFSpanLimit(unsigned short fspan)
{
    unsigned short fsteps = round((double)1000.0/ui->sbFrqStepwidthInput->value());
    unsigned short fspanMax = USHRT_MAX;
    sDeviceInfo devInfo;

    // Calculate fspan
    if(deviceCtrl->deviceGetDevInfo(&devInfo))
    {
        fspanMax = (unsigned short)(devInfo.MaxSpecLength/fsteps);
    };

    // If user selection is greater than max alowable value, then set to max
    if (fspan > fspanMax)
    {
        fspan = fspanMax;
    };
    // Otherwise just set active value to user selection

    // Update Gui Controls
    const QSignalBlocker spanBlocker(ui->sbFrqSpanInput);
    ui->sbFrqSpanInput->setValue(fspan);
}

void MainWindow::guiFrqStartStop2Center(void)
{
    volatile double fstart = ui->spFrqStartValue->value();
    volatile double fstop  = ui->spFrqStopValue->value();
    double fcenter;

    // Calculate fcenter
    fcenter = (fstart + fstop)/(double)2.0;

    // Update Gui Controls
    const QSignalBlocker fcenterBlocker(ui->spFrqCenterValue);
    ui->spFrqCenterValue->setValue(fcenter);
}

void MainWindow::guiFrqCenter2StartStop(void)
{
    volatile double fcenter = ui->spFrqCenterValue->value();
    volatile double fspan;
    double fstart;
    double fstop;

    // Calculate fspan, fstart, and fstop
    if(!ui->bttnRfModeEasyRf->isEnabled())
    {
        fspan = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].SpanMHz;
    }
    else // Expert RF Mode
    {
        fspan = ui->sbFrqSpanInput->value();
    };
    fstart = fcenter - (fspan/(double)2.0);
    fstop  = fcenter + (fspan/(double)2.0);

    // Update Gui Controls
    const QSignalBlocker fstartBlocker(ui->spFrqStartValue);
    ui->spFrqStartValue->setValue(fstart);
    const QSignalBlocker fstopBlocker(ui->spFrqStopValue);
    ui->spFrqStopValue->setValue(fstop);
}

void MainWindow::guiFrqUpdateLimits(void)
{
    double fmax;
    double fmin;

    // Determine fmax, fmin
    if(ui->rbRfRange_431_527->isChecked())
    {
        fmax = FrqTableRange[FRQRANGE_431_527].FrqMax;
        fmin = FrqTableRange[FRQRANGE_431_527].FrqMin;
    }
    else if(ui->rbRfRange_861_1054->isChecked())
    {
        fmax = FrqTableRange[FRQRANGE_861_1054].FrqMax;
        fmin = FrqTableRange[FRQRANGE_861_1054].FrqMin;
    }
    else // (ui->rbRfRange_2152_2635->isChecked())
    {
        fmax = FrqTableRange[FRQRANGE_2152_2635].FrqMax;
        fmin = FrqTableRange[FRQRANGE_2152_2635].FrqMin;
    };

    // Update Gui Controls
    // Frq Start<->Stop Limit
    ui->spFrqStartValue->setMaximum(fmax);
    ui->spFrqStartValue->setMinimum(fmin);
    ui->spFrqStopValue->setMaximum(fmax);
    ui->spFrqStopValue->setMinimum(fmin);
    // Frq Span/2<-FCenter->Span/2 Limit
    ui->spFrqCenterValue->setMinimum(fmin + ((double)ui->sbFrqSpanInput->minimum()/(double)2.0));
    ui->spFrqCenterValue->setMaximum(fmax  - ((double)ui->sbFrqSpanInput->minimum()/(double)2.0));
    // No Span min/max span allows users to enter higher values and the GUI corrects it.
}

void MainWindow::guiFrqBand2StartStop(void)
{
    sDeviceInfo devInfo;

    deviceCtrl->deviceGetDevInfo(&devInfo);
    guiFrqUpdateLimits();

    // Update Gui Controls
    const QSignalBlocker fcenterBlocker(ui->spFrqCenterValue);
    if(ui->rbRfRange_431_527->isChecked())
    {
        ui->spFrqCenterValue->setValue(FrqTableRange[FRQRANGE_431_527].FrqStart);
        devInfo.ActiveRBWTable = &devInfo.RBWTableBand0;
    }
    else if(ui->rbRfRange_861_1054->isChecked())
    {
        ui->spFrqCenterValue->setValue(FrqTableRange[FRQRANGE_861_1054].FrqStart);
        devInfo.ActiveRBWTable = &devInfo.RBWTableBand0;
    }
    else // (ui->rbRfRange_2152_2635->isChecked())
    {
        ui->spFrqCenterValue->setValue(FrqTableRange[FRQRANGE_2152_2635].FrqStart);
        devInfo.ActiveRBWTable = &devInfo.RBWTableBand1;
    };

    // Load RBW list
    guiUpdateRBWList(&devInfo);
    guiFrqSpan2StartStop();
}

void MainWindow::guiFrqSpan2StartStop(void)
{
    volatile double fcenter = ui->spFrqCenterValue->value();
    double fspan;

    // Calculate fspan
    if(!ui->bttnRfModeEasyRf->isEnabled())
    {
        fspan = FrqTableSpan[ui->cbFrqSpanValue->currentIndex()].SpanMHz;
    }
    else // Expert RF Mode
    {
        double fspanMax = (ui->spFrqStopValue->maximum() - ui->spFrqStartValue->minimum());
        fspan  = ui->sbFrqSpanInput->value();

        if(fspan > fspanMax)
        {
            const QSignalBlocker fspanBlocker(ui->sbFrqSpanInput);
            ui->sbFrqSpanInput->setValue((int)fspanMax);
            fspan = fspanMax;
        };
    };

    // Update Gui Controls
    const QSignalBlocker fcenterBlocker(ui->spFrqCenterValue);
    if(fcenter + (fspan/(double)2.0) > ui->spFrqStopValue->maximum())
    {
        ui->spFrqCenterValue->setValue(ui->spFrqStopValue->maximum() - fspan/(double)2.0);
    }
    else if(fcenter - (fspan/(double)2.0) < ui->spFrqStartValue->minimum())
    {
        ui->spFrqCenterValue->setValue(ui->spFrqStartValue->minimum() + fspan/(double)2.0);
    };

    guiFrqCenter2StartStop();
}

void MainWindow::guiFrqExpertRFSpan2StepWidth(void)
{
    double fstep = ui->sbFrqStepwidthInput->value();
    double fstepMin;
    double fstepHwDividendMin;
    sDeviceInfo devInfo;

    // Adjust to minimum fstep for current fspan
    if(deviceCtrl->deviceGetDevInfo(&devInfo))
    {
        fstepMin = ((double)ui->sbFrqSpanInput->value()*(double)1000.0)/(double)(devInfo.MaxSpecLength-1);
        // Find minimum even step dividend
        fstepHwDividendMin = ceil((fstepMin/(double)1000.0)*(double)65536.0);
        fstepHwDividendMin += (unsigned short)fstepHwDividendMin%2;
        fstepMin = (fstepHwDividendMin/(double)65536.0)*(double)1000.0;

        if (fstep < fstepMin)
        {
            fstep = fstepMin;
        };
    };

    // Update Gui Controls
    const QSignalBlocker fstepBlocker(ui->sbFrqStepwidthInput);
    ui->sbFrqStepwidthInput->setValue(fstep);
}

bool MainWindow::guiFwUpdate(QString ComPort)
{
    bool done = false;

    // Parameter Check
    if(!ComPort.isEmpty())
    {
        QMessageBox::information(this, tr("Auto Firmware Updater"), tr("Beginning SA1350 Spectrum Firmware Update"));

        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Auto Firmware Updater"), tr("Do you need to install the firmware update drivers (one-time)?"));
        if(reply == QMessageBox::Yes)
        {
            guiDrvrInstall(ComPort);
        }
        else
        {
            appFwUpdater updater(this);
            deviceCtrl->deviceDisconnect();
            updater.SetParameter(ComPort.toStdString().c_str(), "/fwupdate_windows_64", "/dslite.bat");
            switch(updater.exec())
            {
            case appFwUpdater::Done:
                QMessageBox::information(this, tr("Auto Firmware Updater"), tr("The SA1350 Spectrum Firmware is now up to date\nConnect the SA1350 to proceed"));
                done = true;
                break;
            case appFwUpdater::UPDPTFAIL:
                QMessageBox::warning(this, tr("Auto Firmware Updater"), tr("The SA1350 Spectrum Firmware Update failed\nConnect the SA1350 to proceed"));
                break;

            default:
                break;
            };
        };
    };

    return(done);
}

bool MainWindow::guiDrvrInstall(QString ComPort)
{
    bool done = false;

    // Parameter Check
    if(!ComPort.isEmpty())
    {
        appFwUpdater updater(this);
        deviceCtrl->deviceDisconnect();
        updater.SetParameter(ComPort.toStdString().c_str(), "/fwupdate_windows_64", "/one_time_setup.bat");
        switch(updater.exec())
        {
        case appFwUpdater::Done:
            QMessageBox::information(this, tr("Auto Firmware Updater"),tr("The SA1350 Firmware Update drivers are now installed\nConnect the SA1350 again or restart the SA1350 Spectrum Firmware Update to proceed"));
            done = true;
            break;
        case appFwUpdater::UPDPTFAIL:
            QMessageBox::warning(this, tr("Auto Firmware Updater"),tr("The SA1350 Spectrum Firmware Update drivers failed to install\nContact the TI support"));
            break;

        default:
            break;
        };
    };

    return(done);
}

void MainWindow::eventRBWchanged(int index)
{
    sDeviceInfo devInfo;

    // Confirm RBW table is populated
    if(index>-1)
    {
        if(deviceCtrl->deviceGetDevInfo(&devInfo))
        {
            ui->lIFreqValue->setText(QString::number(devInfo.ActiveRBWTable->at(index).IfFrqKHz));
        };
    };
}
