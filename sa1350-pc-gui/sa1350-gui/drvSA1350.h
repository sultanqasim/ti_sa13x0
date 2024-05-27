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
/*! \file drvSA1350.h */
#pragma once
#include <QMutex>
#include <QThread>

#include "../sa1350-dll/sa1350.h"
#include "appTypedef.h"
#include "cEvent.h"

/*!
 \brief Add brief

 \enum eDrvState
*/
enum  eDrvState
{
    STATE_INIT = 0, /*!< Add in-line comment */
    STATE_OPEN,     /*!< Add in-line comment */
    STATE_SETUP,    /*!< Add in-line comment */
    STATE_RUN,      /*!< Add in-line comment */
    STATE_EXIT,     /*!< Add in-line comment */
};

#define CALDATA_FORMATVER     ((unsigned short)(0x0110)) /*!< Add in-line comment */
#define LIB_VERSION	      ((unsigned short)(0x0140))     /*!< Add in-line comment */

#define CALDATA_FLASH_START   ((unsigned short)(0xD400)) /*!< Add in-line comment */
#define CALDATA_FLASH_END     ((unsigned short)(0xEBFF)) /*!< Add in-line comment */

#define CMD_FLASH_READ        ((unsigned char)  ( 10))   /*!< Add in-line comment */
#define CMD_FLASH_WRITE       ((unsigned char)  ( 11))   /*!< Add in-line comment */
#define CMD_FLASH_ERASE       ((unsigned char)  ( 12))   /*!< Add in-line comment */
#define CMD_FLASH_GETCRC      ((unsigned char)  ( 13))   /*!< Add in-line comment */
#define FLASH_SEGMENT_SIZE    ((unsigned short) (512))   /*!< Add in-line comment */
#define PROGTYPE_CALC         ((unsigned short) ( 62))   /*!< Add in-line comment */

/*!
 \brief Add brief

 \typedef struct sStatusSA1350 sStatusSA1350
*/
/*!
 \brief Add brief

 \struct sStatusSA1350 drvSA1350.h "drvSA1350.h"
*/
typedef struct sStatusSA1350
{
    bool   flagInit;                            /*!< Add in-line comment */
    bool   flagDevConnected;                    /*!< Add in-line comment */
    bool   flagSpecTrigger;                     /*!< Add in-line comment */
    bool   flagSpecIsBusy;                      /*!< Add in-line comment */
    bool   flagSpecNewParameter;                /*!< Add in-line comment */
    bool   flagSpecContinuousModeOn;            /*!< Add in-line comment */
    bool   flagDevInfoLoaded;                   /*!< Add in-line comment */
    sCalibrationData  activeCalData;            /*!< Add in-line comment */
    sa1350UsbDevice   activeUsbInterface;       /*!< Add in-line comment */
    sDeviceInfo       activeDeviceInfo;         /*!< Add in-line comment */
    sFrqSetting       activeFrqSetting;         /*!< Add in-line comment */
    sFrqValues        activeFrqValues;          /*!< Add in-line comment */
    sFrqValues        activeFrqValuesCorrected; /*!< Add in-line comment */
}sStatusSA1350;

/*!
 \brief Add brief

 \class drvSA1350 drvSA1350.h "drvSA1350.h"
*/
class drvSA1350: public QThread
{
    Q_OBJECT

public:
    /*!
     \brief Constructor

    */
    drvSA1350();
    /*!
     \brief Destructor

    */
    ~drvSA1350();

    /*!
     \brief Add brief

     \return unsigned short
    */
    unsigned short GetDllVer(void);
    /*!
     \brief Add brief

     \param NumDevs
     \return bool
    */
    bool deviceSearch(unsigned short &NumDevs);
    /*!
     \brief Add brief

     \param DevNr
     \param DevInfo
     \return bool
    */
    bool deviceGetInfo(unsigned short DevNr,sa1350UsbDevice *DevInfo);
    /*!
     \brief Add brief

     \param usbInfo
     \return bool
    */
    bool deviceConnect(sa1350UsbDevice *usbInfo);
    /*!
     \brief Add brief

     \param devInfo
     \return bool
    */
    bool deviceGetDevInfo(sDeviceInfo *devInfo);
    /*!
     \brief Add brief

     \param usbInfo
     \return bool
    */
    bool deviceGetUsbInfo(sa1350UsbDevice *usbInfo);
    /*!
     \brief Add brief

     \param calData
     \return bool
    */
    bool deviceGetCalData(sCalibrationData *calData);
    /*!
     \brief Add brief

     \return bool
    */
    bool deviceDisconnect(void);
    /*!
     \brief Add brief

     \param FrqValues
     \return bool
    */
    bool spectrumSetParameter(sFrqValues *FrqValues);
    /*!
     \brief Add brief

     \param FrqValues
     \param FrqCorrected
     \return bool
    */
    bool spectrumGetParameter(sFrqValues *FrqValues, sFrqValues *FrqCorrected);
    /*!
     \brief Add brief

     \return bool
    */
    bool spectrumTriggerOn(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool spectrumTriggerOff(void);
    /*!
     \brief Add brief

     \param Spectrum
     \return bool
    */
    bool spectrumGetData(sSpectrum *Spectrum);

signals:
    /*!
     \brief Add brief

    */
    void signalConnected(void);
    /*!
     \brief Add brief

    */
    void signalDisconnected(void);
    /*!
     \brief Add brief

     \param ComPort
    */
    void signalDeviceUpdateRequired(QString ComPort);
    /*!
     \brief Add brief

     \param Done
     \param SpecId
    */
    void signalNewParameterSet(bool Done, int SpecId);
    /*!
     \brief Add brief

    */
    void signalSpectrumReceived(void);
    /*!
     \brief Add brief

     \param Msg
    */
    void signalErrorMsg(QString Msg);

public slots:

protected:
    /*!
     \brief Add brief

    */
    void run(void);

private:
    sStatusSA1350  Status;                      /*!< Add in-line comment */

    cThreads::cEvent *signalDeviceOpen;         /*!< Add in-line comment */

    cThreads::cEvent *signalSpecIsBusy;         /*!< Add in-line comment */
    cThreads::cEvent *signalSpecNewParameter;   /*!< Add in-line comment */
    cThreads::cEvent *signalSpecTrigger;        /*!< Add in-line comment */

    sa1350Frame          DecoderFrame;          /*!< Add in-line comment */
    int                 currentSpectrumId;      /*!< Add in-line comment */
    QList<sa1350Frame>   DecoderSpectrumBuffer; /*!< Add in-line comment */
    QList<sSpectrum>    SpectrumBuffer;         /*!< Add in-line comment */
    sSpectrumOffset     SpectrumOffset;         /*!< Add in-line comment */
    QMutex DrvAccess;                           /*!< Add in-line comment */

    volatile eDrvState State;                   /*!< Add in-line comment */
    volatile bool      flagThreadExit;          /*!< Add in-line comment */

    // SA1350 Task Function Declaration
    /*!
     \brief Add brief

     \return bool
    */
    bool stateInit(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool stateOpen(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool stateSetup(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool stateExit(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool stateRun(void);

    // SA1350 Command Function Declaration
    /*!
     \brief Add brief

     \return bool
    */
    bool IsFrameAvailable(void);
    /*!
     \brief Add brief

     \param frame
     \return bool
    */
    bool GetFrame(sa1350Frame *frame);
    /*!
     \brief Add brief

     \param Cmd
     \param ms
     \return bool
    */
    bool cmdWaitForConfirmation(unsigned char Cmd,unsigned long ms);
    /*!
     \brief Add brief

     \param Cmd
     \param dataFrame
     \param ms
     \return bool
    */
    bool cmdWaitForData(unsigned char Cmd,sa1350Frame *dataFrame, unsigned long ms);
    /*!
     \brief Add brief

     \param Cmd
     \param U8
     \return bool
    */
    bool cmdSetU8(unsigned char Cmd,unsigned char U8);
    /*!
     \brief Add brief

     \param Cmd
     \param U16
     \return bool
    */
    bool cmdSetU16(unsigned char Cmd,unsigned short U16);
    /*!
     \brief Add brief

     \param Cmd
     \param U16
     \return bool
    */
    bool cmdGetU16(unsigned char Cmd, unsigned short *U16);
    /*!
     \brief Add brief

     \param Cmd
     \param U32
     \return bool
    */
    bool cmdSetU32(unsigned char Cmd,unsigned long U32);
    /*!
     \brief Add brief

     \param Cmd
     \param U32
     \return bool
    */
    bool cmdGetU32(unsigned char Cmd, unsigned long *U32);
    /*!
     \brief Add brief

     \param Cmd
     \param ptrData
     \param Size
     \return bool
    */
    bool cmdSetX(unsigned char Cmd, unsigned char *ptrData, int Size);

    // SA1350 Upper API Calls Function Declaration
    /*!
     \brief Add brief

     \return bool
    */
    bool cmdConnect(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool cmdDisconnect(void);
    /*!
     \brief Add brief

     \param strDeviceVersion
     \return bool
    */
    bool cmdGetDeviceVersion(QString *strDeviceVersion);
    /*!
     \brief Add brief

     \param VerFW
     \return bool
    */
    bool cmdGetFWVersion(unsigned short *VerFW);
    /*!
     \brief Add brief

     \param rbwSize
     \param rssiSize
     \return bool
    */
    bool cmdGetRFSizes(unsigned char *rbwSize, unsigned short *rssiSize);
    /*!
     \brief Add brief

     \param RBWs
     \param RBWCount
     \return bool
    */
    bool cmdGetRFRBWs(QVector<sFrqRBW> *RBWs, unsigned char RBWCount);
    /*!
     \brief Add brief

     \param RBWEntries
     \param MaxRSSICount
     \param band - {0, 1, 2} Indicates a specific one of the supported bands
     \return bool
    */
    bool cmdGetRFParameters(QVector<sFrqRBW> *RBWEntries, unsigned short *MaxRSSICount,
                            unsigned char band);
    /*!
     \brief Add brief

     \return bool
    */
    bool cmdModeSpectrumInit(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool cmdGetSpectrum(void);
    /*!
     \brief Add brief

     \param AddrStart
     \param Data
     \param Size
     \return bool
    */
    bool cmdFlashRead(unsigned short AddrStart, QByteArray *Data,unsigned short Size);
    /*!
     \brief Add brief

     \param CalData
     \return bool
    */
    bool cmdLoadCalData(sCalibrationData *CalData);
    /*!
     \brief Add brief

     \param Values
     \param FrqCorrected
     \return bool
    */
    bool cmdSetFrq(sFrqValues *Values , sFrqValues *FrqCorrected);

    // SA1350 SetFrq Helper Function Declaration
    /*!
     \brief Add brief

     \param frq
     \return double
    */
    double _calcFrqCorrect(double frq);
    /*!
     \brief Add brief

     \param Values
     \return unsigned long
    */
    unsigned long _calcFrqCenter(sFrqValues *Values);
    /*!
     \brief Add brief

     \param Values
     \return unsigned long
    */
    unsigned long _calcFrqStart(sFrqValues *Values);
    /*!
     \brief Add brief

     \param Values
     \return unsigned long
    */
    unsigned long _calcFrqStop(sFrqValues *Values);
    /*!
     \brief Add brief

     \param Values
     \return unsigned long
    */
    unsigned long _calcFrqStep(sFrqValues *Values);
    /*!
     \brief Add brief

     \param Values
     \return unsigned char
    */
    unsigned char _calcFrqRBW(sFrqValues *Values);
    /*!
     \brief Add brief

     \param Values
     \return unsigned short
    */
    unsigned short _calcFrqCount(sFrqValues *Values);
    /*!
     \brief Add brief

     \param RefDcLevelIndex
     \return unsigned char
    */
    unsigned char _calcFrqGain(unsigned char RefDcLevelIndex);
    /*!
     \brief Add brief

     \param value
     \param par
    */
    void u16toPar(unsigned short value, unsigned char *par);
    /*!
     \brief Add brief

     \param value
     \param par
    */
    void u32toPar(unsigned long value, unsigned char *par);

    // SA1350 LoadCalData Helper Function Declaration
    QByteArray GetData; /*!< Add in-line comment */
    /*!
     \brief Add brief

     \param DataBuffer
    */
    void    SetGetDataPtr(QByteArray *DataBuffer);
    /*!
     \brief Add brief

     \return quint8
    */
    quint8  GetDataU8(void);
    /*!
     \brief Add brief

     \return quint16
    */
    quint16 GetDataU16(void);
    /*!
     \brief Add brief

     \return quint32
    */
    quint32 GetDataU32(void);
    /*!
     \brief Add brief

     \return quint64
    */
    quint64 GetDataU64(void);
    /*!
     \brief Add brief

     \return double
    */
    double  GetDataDouble(void);
    /*!
     \brief Add brief

     \param Str
     \param Length
     \return bool
    */
    bool    GetDataStr(char *Str , int Length);

    // SA1350 Spectrum Function Declaration
    /*!
     \brief Add brief

     \param DecoderBuffer
    */
    void specSave(QList<sa1350Frame> *DecoderBuffer);
    /*!
     \brief Add brief

     \param SpecId
     \param FrqValues
    */
    void specCalcOffset(int SpecId,sFrqValues *FrqValues);
    /*!
     \brief Add brief

     \param Spectrum
    */
    void specOffset(sSpectrum *Spectrum);
    // SA1350 Firmware Updater Declaration
    /*!
     \brief Add brief

     \return bool
    */
    bool FwVersionIsOk(void);

};
