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
#include "drvSA1350.h"

#include <QDebug>

#include "../sa1350-dll/sa1350Cmd.h"

#define MIN_FW_VERSION		((unsigned short)(0x0103)) /*!<  FW version number in High_byte.Low_byte format  */
#define NULL_FW_VERSION     ((unsigned short)(0xFFFF)) /*!<  Invalid/Unknown FW version number */

drvSA1350::drvSA1350()
{
    Status.flagInit             = false;
    flagThreadExit              = false;

    Status.flagSpecTrigger      = false;
    Status.flagSpecIsBusy       = false;
    Status.flagSpecNewParameter = false;

    Status.flagDevInfoLoaded    = false;

    signalDeviceOpen            = new cThreads::cEvent(true);

    signalSpecIsBusy            = new cThreads::cEvent(true);
    signalSpecNewParameter      = new cThreads::cEvent(true);
    signalSpecTrigger           = new cThreads::cEvent(true);

    currentSpectrumId = 0;
    DecoderSpectrumBuffer.clear();
    SpectrumBuffer.clear();

    sa1350Init();
    if(sa1350IsInit())
    {
        Status.flagInit = true;
        // Start Thread
        this->start();
    };
}

drvSA1350::~drvSA1350()
{
    if(Status.flagInit)
    {
        flagThreadExit = true;
        while(!this->isFinished());
        sa1350DeInit();
        Status.flagInit = false;
    };

    delete signalDeviceOpen;
    delete signalSpecIsBusy;
    delete signalSpecNewParameter;
    delete signalSpecTrigger;
}

// Public Function Defintion
unsigned short drvSA1350::GetDllVer(void)
{
    return(sa1350GetDllVersion());
}

bool drvSA1350::deviceSearch(unsigned short &NumDevs)
{
    bool ok = false;
    DrvAccess.lock();
    ok = sa1350FindDevices(NumDevs);
    DrvAccess.unlock();
    return(ok);
}

bool drvSA1350::deviceGetInfo(unsigned short DevNr,sa1350UsbDevice *DevInfo)
{
    bool ok = false;
    DrvAccess.lock();
    ok = sa1350GetDeviceInfo(DevNr,DevInfo);
    DrvAccess.unlock();
    return(ok);
}

bool drvSA1350::deviceConnect(sa1350UsbDevice *usbInfo)
{
    if(!usbInfo)
        return(false);

    if(!sa1350IsInit())
    {
        emit signalErrorMsg("SA1350** DLL Not initialized !!");
        return(false);
    };

    if(sa1350IsConnected())
    {
        emit signalErrorMsg("One Device is already Connected!!");
        return(false);
    };

    if(sa1350Connect(usbInfo->PortName))
    {
        Status.activeUsbInterface = *usbInfo;
        Status.flagDevConnected   = true;

        signalSpecTrigger->Reset();
        signalSpecNewParameter->Reset();
        signalSpecIsBusy->Reset();

        Status.flagSpecTrigger       = false;
        Status.flagSpecIsBusy        = false;
        Status.flagSpecNewParameter  = false;

        currentSpectrumId     = 0;
        DecoderSpectrumBuffer.clear();
        SpectrumBuffer.clear();

        signalDeviceOpen->Signal();
        return(true);
    }
    else
    {
        deviceDisconnect();
        emit signalErrorMsg(QString("Failed to open Port: %0").arg(usbInfo->PortName));
    };

    return(false);
}

bool drvSA1350::deviceGetDevInfo(sDeviceInfo *devInfo)
{
    bool done = false;
    if(!devInfo || !Status.flagDevConnected)
        return(done);

    if(Status.flagDevInfoLoaded)
    {
        *devInfo = Status.activeDeviceInfo;
        done = true;
    };

    return(done);
}

bool drvSA1350::deviceGetUsbInfo(sa1350UsbDevice *usbInfo)
{
    bool done = false;

    if(!usbInfo || !Status.flagDevConnected)
        return(done);

    *usbInfo = Status.activeUsbInterface;

    done = true;

    return(done);
}

bool drvSA1350::deviceGetCalData(sCalibrationData *calData)
{
    bool done = false;

    if(!calData || !Status.flagDevConnected)
        return(done);

    if(Status.flagDevInfoLoaded)
    {
        *calData = Status.activeCalData;
        done = true;
    };

    return(done);
}

bool drvSA1350::deviceDisconnect(void)
{
    bool done = false;

    if(signalDeviceOpen->Check())
    {

        if(sa1350IsConnected())
        {
            if(Status.flagDevInfoLoaded == true)
            {
                cmdDisconnect();
            }
            sa1350Disconnect();
        };
        signalDeviceOpen->Reset();
        Status.flagDevConnected = false;
        Status.flagDevInfoLoaded= false;
        done = true;
    };
    State = STATE_OPEN;
    emit signalDisconnected();

    return(done);
}

bool drvSA1350::spectrumSetParameter(sFrqValues *FrqValues)
{
    bool done = false;
    if(signalDeviceOpen->Check())
    {
        Status.activeFrqValues = *FrqValues;

        Status.flagSpecNewParameter = true;
        done = true;
    };

    return(done);
}

bool drvSA1350::spectrumGetParameter(sFrqValues *FrqValues, sFrqValues *FrqCorrected)
{
    bool done = false;
    if(signalDeviceOpen->Check())
    {
        *FrqValues    = Status.activeFrqValues;
        *FrqCorrected = Status.activeFrqValuesCorrected;
        done = true;
    };

    return(done);
}

bool drvSA1350::spectrumTriggerOn(void)
{
    bool done = false;
    Status.flagSpecTrigger = true;

    return(done);
}

bool drvSA1350::spectrumTriggerOff(void)
{
    bool done = false;
    Status.flagSpecTrigger = false;

    return(done);
}

bool drvSA1350::spectrumGetData(sSpectrum *Spectrum)
{
    bool done = false;
    if(!Spectrum || SpectrumBuffer.isEmpty())
        return(done);

    *Spectrum =   SpectrumBuffer.first();
    SpectrumBuffer.pop_front();
    done = true;

    return(done);
}

// Public Signals Function Definition

// Public Slot Function Definiton

// Protected Function Defintion
void drvSA1350::run(void)
{
    flagThreadExit = false;
    State = STATE_INIT;
    sa1350Status Status;

    qDebug()<<"drvSA1350: Start Driver Thread";

    do
    {
        switch(State)
        {
        case STATE_INIT:
            stateInit();
            break;
        case STATE_OPEN:
            stateOpen();
            break;
        case STATE_SETUP:
            stateSetup();
            break;
        case STATE_RUN:
            stateRun();
            break;
        case STATE_EXIT:
            stateExit();
            break;

        default:
            break;
        };

        if(sa1350GetStatus(Status))
        {
            if(Status.flagUsbRemoval)
            {
                deviceDisconnect();
                emit signalErrorMsg("Device removed from USB Interface !!");
            };
        };

        this->msleep(1);
    }while(!flagThreadExit);

    qDebug()<<"drvSA1350: Exit Driver Thread";

}

// Private Function Defintion
// Private SA1350 Task Function Definition
bool drvSA1350::stateInit(void)
{
    if(sa1350IsInit())
    {
        qDebug()<<"drvSA1350: Init";
        State = STATE_OPEN;
    }
    else
    {
        qDebug()<<"drvSA1350: Failed to Init";
        emit signalErrorMsg("sa1350Drv: Failed to init Interface Dll");
    };
    return(false);
}

bool drvSA1350::stateOpen(void)
{
    if(signalDeviceOpen->CheckSignal(1))
    {
        qDebug()<<"drvSA1350: Is Open";
        State = STATE_SETUP;
    };
    return(false);
}

bool drvSA1350::stateSetup(void)
{
    bool done = false;
    bool valid_RBW = false;

    // Clear Settings
    Status.activeDeviceInfo.DeviceVersion.clear();
    Status.activeDeviceInfo.FWVersion = 0;
    Status.activeDeviceInfo.RBWTableBand0.clear();
    Status.activeDeviceInfo.RBWTableBand1.clear();
    Status.activeDeviceInfo.MaxSpecLength = 0;

    // Firmware Function First
    if(cmdGetFWVersion(&Status.activeDeviceInfo.FWVersion))
    {
        if(cmdGetDeviceVersion(&Status.activeDeviceInfo.DeviceVersion))
        {
            // Try to read RBW table for each possible band
            if (cmdGetRFParameters(&Status.activeDeviceInfo.RBWTableBand0,
                                           &Status.activeDeviceInfo.MaxSpecLength, 0))
            { // RBW Table for band0 read correctly

                // Set active RBW table pointer to TableBand0
                Status.activeDeviceInfo.ActiveRBWTable = &Status.activeDeviceInfo.RBWTableBand0;

                if (cmdGetRFParameters(&Status.activeDeviceInfo.RBWTableBand1,
                                               &Status.activeDeviceInfo.MaxSpecLength, 1))
                { // RBW Table for band1 read correctly
                    valid_RBW = true;
                }
            }

            if(valid_RBW)
            {
                if(FwVersionIsOk())
                {
                    if(cmdConnect())
                    {
                        Status.flagDevInfoLoaded = true;
                        done = true;
                    }
                    else
                    {
                        emit signalErrorMsg("Failed to receive connect confirmation");
                    };
                }
                else
                {
                    emit signalDeviceUpdateRequired(QString(Status.activeUsbInterface.PortName));
                    emit signalErrorMsg("Wrong Firmware Version");
                }
            }
            else
            {
                emit signalErrorMsg("Failed to receive RF Parameters");
            };
        }
        else
        {
            emit signalErrorMsg("Failed to receive Device Version");
        };
    }
    else
    {
        emit signalDeviceUpdateRequired(QString(Status.activeUsbInterface.PortName));
        emit signalErrorMsg(QString("Device on Port %0 unresponsive").arg(Status.activeUsbInterface.PortName));
    };

    if(done)
    {
        //        cmdBlink();
        State = STATE_RUN;
        emit signalConnected();
    }
    else
    {
        deviceDisconnect();
    };

    return(done);
}

bool drvSA1350::stateExit(void)
{
    qDebug()<<"drvSA1350: Exit Driver Task";
    return(false);
}

bool drvSA1350::stateRun(void)
{
    if(!Status.flagSpecIsBusy)
    {// Ready to set new spectrum parameter
        if(Status.flagSpecNewParameter)
        {
            if(cmdSetFrq(&Status.activeFrqValues,&Status.activeFrqValuesCorrected))
            {// Successfully set new spectrum parameter
                DecoderSpectrumBuffer.clear();
                currentSpectrumId++;
                emit signalNewParameterSet(true,currentSpectrumId);
                Status.flagSpecNewParameter=false;
                cmdGetSpectrum();
                Status.flagSpecIsBusy=true;
            }
            else
            {// Failed to set new spectrum parameter
                emit signalNewParameterSet(false,0);
                emit signalErrorMsg("Failed to set new spectrum parameter !!");
            };
            Status.flagSpecNewParameter = false;
        }
        else
        {// Ready to trigger new spectrum if requested
            if(Status.flagSpecTrigger)
            {
                Status.flagSpecTrigger = false;
                Status.flagSpecIsBusy  = true;
                cmdGetSpectrum();

            };
        };
    }
    else
    {// Wait for requested spectrum data
        if(sa1350IsFrameAvailable())
        {
            DecoderFrame.Cmd = 0xff;
            DecoderFrame.Crc = 0;
            DecoderFrame.Length = 0;
            memset(&DecoderFrame.Data[0],0,FRAME_MAX_DATA_LENGTH);
            if(sa1350GetFrame(&DecoderFrame))
            {
                switch(DecoderFrame.Cmd)
                {
                case CMD_GETSPECNOINIT:
                    DecoderSpectrumBuffer.append(DecoderFrame);
                    break;
                case CMD_GETLASTERROR:
                    if(DecoderFrame.Length==2)
                    {// End of requeted spectrum
                        Status.flagSpecIsBusy = false;
                        specSave(&DecoderSpectrumBuffer);
                        DecoderSpectrumBuffer.clear();
                    };
                    break;
                default:
                    break;
                };
            };
        };
    };
    return(false);
}

// Private SA1350 Command Function Definition
bool drvSA1350::IsFrameAvailable(void)
{
    bool ok = false;
    DrvAccess.lock();
    ok = sa1350IsFrameAvailable();
    DrvAccess.unlock();
    return(ok);
}

bool drvSA1350::GetFrame(sa1350Frame *frame)
{
    bool ok = false;
    DrvAccess.lock();
    ok = sa1350GetFrame(frame);
    DrvAccess.unlock();
    return(ok);
}

bool drvSA1350::cmdWaitForConfirmation(unsigned char Cmd,unsigned long ms)
{
    bool done = false;
    sa1350Frame rcvFrame;

    ms = ms;

    do
    {
        if(sa1350IsFrameAvailable())
            if(sa1350GetFrame(&rcvFrame))
                if(rcvFrame.Cmd == Cmd && rcvFrame.Length == 0)
                    done = true;

        if(ms==0 && !done)
            return(false);
        usleep(1000);
        ms--;
    }while(!done);

    return(done);
}

bool drvSA1350::cmdWaitForData(unsigned char Cmd,sa1350Frame *dataFrame, unsigned long ms)
{
    bool done = false;
    sa1350Frame tmpFrame;

    do
    {
        if(sa1350IsFrameAvailable())
            if(sa1350GetFrame(&tmpFrame))
                if(tmpFrame.Cmd == Cmd)
                    if(tmpFrame.Length)
                    {
                        *dataFrame = tmpFrame;
                        done = true;
                    };
        if(ms==0 && !done)
            return(false);
        usleep(1000);
        ms--;
    }while(!done);

    return(done);
}

bool drvSA1350::cmdSetU8(unsigned char Cmd,unsigned char U8)
{
    bool done = false;

    unsigned char  u8 = U8;

    if(sa1350SendCmd(Cmd,&u8,1))
        if(cmdWaitForConfirmation(Cmd,500))
            done = true;

    return(done);
}

bool drvSA1350::cmdSetU16(unsigned char Cmd,unsigned short U16)
{
    bool done = false;

    unsigned char  u8[2];

    u8[0] = (unsigned char) (U16>>8    );
    u8[1] = (unsigned char) (U16 & 0xff);

    if(sa1350SendCmd(Cmd,&u8[0],2))
        if(cmdWaitForConfirmation(Cmd,500))
            done = true;


    return(done);
}

bool drvSA1350::cmdGetU16(unsigned char Cmd, unsigned short *U16)
{
    bool done = false;
    sa1350Frame  rxData;

    *U16 = 0;

    if(sa1350SendCmd(Cmd,NULL,0))
        if(cmdWaitForConfirmation(Cmd,500))
            if(cmdWaitForData(Cmd,&rxData,500))
            {
                *U16  = rxData.Data[0] << 8;
                *U16 |= rxData.Data[1];
                done = true;
            };

    return(done);
}

bool drvSA1350::cmdSetU32(unsigned char Cmd,unsigned long U32)
{
    bool done = false;
    unsigned char u8[4];

    u8[0] =(unsigned char)(U32>>24);
    u8[1] =(unsigned char)(U32>>16);
    u8[2] =(unsigned char)(U32>> 8);
    u8[3] =(unsigned char)(U32);

    if(sa1350SendCmd(Cmd,&u8[0],4))
        if(cmdWaitForConfirmation(Cmd,500))
            done = true;

    return(done);
}

bool drvSA1350::cmdGetU32(unsigned char Cmd, unsigned long *U32)
{
    bool done = false;
    sa1350Frame  rxData;

    *U32 = 0;

    if(sa1350SendCmd(Cmd,NULL,0))
        if(cmdWaitForConfirmation(Cmd,500))
            if(cmdWaitForData(Cmd,&rxData,500))
            {
                *U32  = (unsigned long)(((unsigned char)rxData.Data[0]) << 24);
                *U32 |= (unsigned long)(((unsigned char)rxData.Data[1]) << 16);
                *U32 |= (unsigned long)(((unsigned char)rxData.Data[2]) << 8);
                *U32 |= (unsigned long)(((unsigned char)rxData.Data[3]));
                done = true;
            };

    return(done);
}

bool drvSA1350::cmdSetX(unsigned char Cmd, unsigned char *ptrData, int Size)
{
    bool done = false;

    if(sa1350SendCmd(Cmd,ptrData,Size))
    {
        if(cmdWaitForConfirmation(Cmd,1000))
        {
            done = true;
        };
    };

    return(done);
}

// Private SA1350 Upper API Calls Function Definition
bool drvSA1350::cmdConnect(void)
{
    bool done = false;

    if(sa1350SendCmd(CMD_CONNECT,NULL,0))
    {
        if(cmdWaitForConfirmation(CMD_CONNECT,500))
        {
            done = true;
        };
    };

    return(done);
}

bool drvSA1350::cmdDisconnect(void)
{
    bool done = false;

    if(sa1350SendCmd(CMD_DISCONNECT,NULL,0))
    {
        done = true;
    };

    return(done);
}

//bool drvSA1350::cmdGetDeviceVersion(unsigned short *VerDevice)
//{
//    bool done = false;

//    *VerDevice = 0;

//    if(cmdGetU16(CMD_GETDEVICEVER,VerDevice))
//        done = true;

//    return(done);
//}

bool drvSA1350::cmdGetDeviceVersion(QString *strDeviceVersion)
{
    bool done = false;
    sa1350Frame rcvFrame;

    strDeviceVersion->clear();

    if(sa1350SendCmd(CMD_GETDEVICEVER,NULL,0))
    {
        if(cmdWaitForConfirmation(CMD_GETDEVICEVER,500))
        {
            if(cmdWaitForData(CMD_GETDEVICEVER,&rcvFrame,500))
            {
                rcvFrame.Data[rcvFrame.Length]=0;
                strDeviceVersion->append((const char*)&rcvFrame.Data[0]);
                done = true;
            };
        };
    };

    return(done);
}

bool drvSA1350::cmdGetFWVersion(unsigned short *VerFW)
{
    bool done = false;

    *VerFW = 0;

    if(cmdGetU16(CMD_GETFWVER,VerFW))
        done = true;

    return(done);
}

bool drvSA1350::cmdGetRFSizes(unsigned char *rbwSize, unsigned short *rssiSize)
{
    bool        done = false;
    sa1350Frame rfSizeFrame;
    QByteArray  rfSizeData;

    if(cmdWaitForData(CMD_GETRFPARAMS,&rfSizeFrame,500))
    {
        rfSizeData.clear();
        for(int index=0;index<rfSizeFrame.Length;index++)
        {
            rfSizeData.append(rfSizeFrame.Data[index]);
        }

        if(rfSizeData.size() == (sizeof(*rbwSize) + sizeof(*rssiSize)))
        {
            SetGetDataPtr(&rfSizeData);
            *rbwSize = GetDataU8();
            *rssiSize = GetDataU16();

            done = true;
        }
    }

    return done;
}

bool drvSA1350::cmdGetRFRBWs(QVector<sFrqRBW> *RBWs, unsigned char RBWCount)
{
    bool        done = false;
    sa1350Frame rbwEntryFrame;
    QByteArray  rbwEntryData;
    sFrqRBW     rbw;

    for(int index=0;index<RBWCount;index++)
    {
        if(cmdWaitForData(CMD_GETRFPARAMS,&rbwEntryFrame,500))
        {
            rbwEntryData.clear();
            for(int index=0;index<rbwEntryFrame.Length;index++)
            {
                rbwEntryData.append(rbwEntryFrame.Data[index]);
            }

            if(rbwEntryData.size() == (sizeof(double) + sizeof(unsigned short) + sizeof(unsigned char)))
            {
                SetGetDataPtr(&rbwEntryData);
                rbw.FrqKHz = GetDataDouble();
                rbw.IfFrqKHz = GetDataU16();
                rbw.RegValue = GetDataU8();
                RBWs->append(rbw);
            }
        }
        else
        {
            break;
        }
    }

    if(RBWs->size() == RBWCount)
    {
        done = true;
    }

    return done;
}

bool drvSA1350::cmdGetRFParameters(QVector<sFrqRBW> *RBWEntries, unsigned short *MaxRSSICount,
                                   unsigned char band)
{
    bool          done = false;
    QByteArray    rfParamData;
    unsigned char rbwEntryCount;

    RBWEntries->clear();
    *MaxRSSICount = 0;

    if(cmdSetU8(CMD_GETRFPARAMS, band))
    {
        if(cmdGetRFSizes(&rbwEntryCount, MaxRSSICount))
        {
            if(rbwEntryCount > 0)
            { // Only read values if there are values to read
                if(cmdGetRFRBWs(RBWEntries, rbwEntryCount))
                {
                    done = true;
                }
            }
        }
    }
    return(done);
}

bool drvSA1350::cmdModeSpectrumInit(void)
{
    bool done = false;

    if(sa1350SendCmd(CMD_INITPARAMETER,NULL,0))
    {
        if(cmdWaitForConfirmation(CMD_INITPARAMETER,500))
        {
            done = true;
        };
    };

    return(done);
}

bool drvSA1350::cmdGetSpectrum(void)
{
    bool done = false;

    if(sa1350SendCmd(CMD_GETSPECNOINIT,NULL,0))
    {
        if(cmdWaitForConfirmation(CMD_GETSPECNOINIT,500))
        {
            done = true;
        };
    };

    return(done);
}

bool drvSA1350::cmdFlashRead(unsigned short AddrStart, QByteArray *Data, unsigned short Size)
{
    bool done = false;
    unsigned char cmdData[4];
    unsigned short NrOfBlocksToRead;
    unsigned short tmpLength;
    unsigned short tmpAddr;
    unsigned short tmpDataSize;
    Q_UNUSED(tmpDataSize)

    sa1350Frame dataFrame;

    NrOfBlocksToRead = Size/255;
    tmpLength = Size;

    for(unsigned short blockIndex=0;blockIndex<=NrOfBlocksToRead;blockIndex++)
    {
        done = false;
//        if(tmpLength>255)
//        {
//            tmpDataSize = 255;
//        }
//        else
//        {
//            tmpDataSize = tmpLength;
//        };
        tmpAddr   = AddrStart+(blockIndex*255);
        cmdData[0] = tmpAddr >> 8;
        cmdData[1] = tmpAddr & 0xff;
        cmdData[2] = tmpLength >> 8;
        cmdData[3] = tmpLength & 0x00FF;
        if(sa1350SendCmd(CMD_FLASH_READ, &cmdData[0], 4))
            if(cmdWaitForConfirmation(CMD_FLASH_READ,500))
            {
                if(cmdWaitForData(CMD_FLASH_READ,&dataFrame,500))
                {
                    for(int index=0;index<dataFrame.Length;index++)
                    {
                        Data->append(dataFrame.Data[index]);
                        tmpLength--;
                    };
                    done = true;
                }
                else
                {
                    blockIndex = 0xffff;
                    done = false;
                    Data->clear();
                };
            };
    };

    return(done);
}

bool drvSA1350::cmdLoadCalData(sCalibrationData *CalData)
{
    bool        done = false;
    QByteArray  dataBuffer;
    sProgHeader ProgHeader;

    if(!CalData)
        return(done);

    dataBuffer.clear();

    // Try to load Calibration ProgHeader
    if(cmdFlashRead(CALDATA_FLASH_START,&dataBuffer,(unsigned short)sizeof(sProgHeader)))
    {
        ProgHeader.MemStartAddr = (unsigned short)(dataBuffer[0] + (dataBuffer[1]<<8));
        ProgHeader.MemLength    = (unsigned short)(dataBuffer[2] + (dataBuffer[3]<<8));
        ProgHeader.MemType      = (unsigned short)(dataBuffer[4] + (dataBuffer[5]<<8));
        ProgHeader.TypeVersion  = (unsigned short)(dataBuffer[6] + (dataBuffer[7]<<8));
        ProgHeader.Crc16        = (unsigned short)(dataBuffer[8] + (dataBuffer[9]<<8));
        // Verify Header For Valid Type
        if(ProgHeader.MemType==PROGTYPE_CALC)
        {
            done = true;
        };
    };

    if(done)
    {
        dataBuffer.clear();

        // Try to load Calibration Data itself
        if(cmdFlashRead((unsigned short)(CALDATA_FLASH_START+sizeof(sProgHeader)),&dataBuffer,(unsigned short)sizeof(sCalibrationData)))
        {
            SetGetDataPtr(&dataBuffer);
            // Get Index 1: Calibration File Format Version
            CalData->CalFormatVer    = GetDataU16();

            // Get Index 2: Calibration Date Information
            for(int index=0;index<16;index++)
                CalData->CalDate[index] = GetDataU8();

            // Get Index 3: Calibration Software Version
            CalData->CalSwVer        = GetDataU16();

            // Get Index 4: Calibration Production Side
            CalData->CalProdSide     = GetDataU8();

            // Get Index 5: Calibration FrqRange Start Stop NrOfSamples
            for(int frqrangeIndex=0;frqrangeIndex<3;frqrangeIndex++)
            {
                CalData->CalFrqRange[frqrangeIndex].FStart   = GetDataU32();
                CalData->CalFrqRange[frqrangeIndex].FStop    = GetDataU32();
                CalData->CalFrqRange[frqrangeIndex].FSamples = GetDataU32();
            };

            // Add Index 6: RefLevel, Gain and IF LookUpTable
            for(int rgIndex=0;rgIndex<8;rgIndex++)
            {
                CalData->CalRefGainTable[rgIndex].RefLevel      = (signed char  )GetDataU8();
                CalData->CalRefGainTable[rgIndex].Gain          = (unsigned char)GetDataU8();
            };

            // Get Index 7: Device Production Hardware Id Information
            CalData->DevHwId             = GetDataU32();

            // Get Index 8: Device USB TUSB3410 Serialnumber
            for(int index=0;index<16;index++)
                CalData->DevUsbSerNr[index] = GetDataU8();

            // Get Index 9: Device FXtal Frequency Information
            CalData->DevFxtal            = GetDataU32();

            // Get Index 10: Device FXtal Frequency Information
            CalData->DevFxtalppm         = GetDataU16();

            // Get Index 11: Device Calibration Start Temerature
            for(int tempIndex=0;tempIndex<6;tempIndex++)
                CalData->DevTempStart[tempIndex] = GetDataU8();

            // Get Index 12: Device Calibration Stop  Temerature
            for(int tempIndex=0;tempIndex<6;tempIndex++)
                CalData->DevTempStop[tempIndex] = GetDataU8();

            // Get Index 13: Device Calibration Coeff and DcSelect Index Data per Frequency Range and Gain Level
            for(int frqIndex=0;frqIndex<3;frqIndex++)
                for(int gainIndex=0;gainIndex<8;gainIndex++)
                {
                    CalData->DevCalCoeffsFrqGain[frqIndex][gainIndex].DcSelect = GetDataU8();
                    for(int valueIndex=0;valueIndex<8;valueIndex++)
                    {
                        CalData->DevCalCoeffsFrqGain[frqIndex][gainIndex].Values[valueIndex] = (double)GetDataDouble();
                    };
                };

        };
    };

    return(done);
}

bool drvSA1350::cmdSetFrq(sFrqValues *FrqSetting, sFrqValues *FrqCorrected)
{
    bool           done = false;
    sFrqParameterBuffer FrqData;

    // Correct New Frq Settings
    FrqData.FrqRange     = FrqSetting->FrqRange;
    FrqData.FrqStart     = _calcFrqStart(FrqSetting);
    FrqData.FrqStop      = _calcFrqStop(FrqSetting);
    FrqData.FrqCenter    = _calcFrqCenter(FrqSetting);
    FrqData.RBW          = _calcFrqRBW(FrqSetting);
    FrqData.FrqStepWidth = _calcFrqStep(FrqSetting);
    FrqData.FrqStepCount = _calcFrqCount(FrqSetting);
    FrqData.FrqSpan      = FrqSetting->FrqSpan;
    FrqData.SpanIndex    = EXPERT_RF_MODE;

    // Return Correct Frq Settings
    FrqCorrected->FrqRange         = FrqSetting->FrqRange;
    FrqCorrected->FrqStart         = _calcFrqCorrect(FrqData.FrqStart);
    FrqCorrected->FrqStop          = _calcFrqCorrect(FrqData.FrqStop);
    FrqCorrected->FrqCenter        = _calcFrqCorrect(FrqData.FrqCenter);
    FrqCorrected->FrqSpanIndex     = FrqSetting->FrqSpanIndex;
    FrqCorrected->FrqSpan          = FrqSetting->FrqSpan;
    FrqCorrected->FrqStepWidth     = _calcFrqCorrect(FrqData.FrqStepWidth)*(double)1000.0;
    FrqCorrected->RefDcLevelIndex  = FrqSetting->RefDcLevelIndex;
    FrqCorrected->RefDcLevel       = FrqSetting->RefDcLevel;
    FrqCorrected->RBWIndex         = FrqSetting->RBWIndex;
    FrqCorrected->RBW              = FrqSetting->RBW;

    if(cmdSetU8(CMD_SETFRANGE,FrqData.FrqRange))
        if(cmdSetU32(CMD_SETFSTART,FrqData.FrqStart))
            if(cmdSetU32(CMD_SETFSTOP,FrqData.FrqStop))
                if(cmdSetU8(CMD_SETRBW,FrqData.RBW))
                    if(cmdSetU32(CMD_SETFSTEP,FrqData.FrqStepWidth))
                        if(cmdSetU16(CMD_SETSTEPCOUNT,FrqData.FrqStepCount))
                            if(cmdSetU16(CMD_SETSPAN,FrqData.FrqSpan))
                                if(cmdSetU8(CMD_SETSPANINDEX,FrqData.SpanIndex))
                                {
                                    done = true;
                                };

    return(done);
}

// Private SA1350 SetFrq Helper Function Definition
double drvSA1350::_calcFrqCorrect(double frq)
{
    unsigned long frqint = (unsigned long)frq>>16;
    double frqfrac       = (double)((unsigned long)frq & 0xffff)/(double)65536.0;
    double        dfrq;

    dfrq = (double)frqint + frqfrac;

    return(dfrq);
}

unsigned long drvSA1350::_calcFrqCenter(sFrqValues *Values)
{
    unsigned short sfcenter;
    unsigned short sfraccenter;
    unsigned long fcenter;

    sfcenter    = (unsigned short)Values->FrqCenter;
    sfraccenter = (unsigned short)lround((Values->FrqCenter - (double)sfcenter)*(double)65536.0);

    fcenter     = (unsigned long)((sfcenter<<16) | sfraccenter);

    return fcenter;
}

unsigned long drvSA1350::_calcFrqStart(sFrqValues *Values)
{
    unsigned short sfstart;
    unsigned short sfracstart;
    unsigned long fstart;
    unsigned long len = (unsigned long)(Values->FrqSpan/(Values->FrqStepWidth/(double)1000.0)) + 1;
    double fsw        = (double)_calcFrqStep(Values)/(double)65536.0;
    double dfstart    = (double)_calcFrqCorrect(_calcFrqCenter(Values)) - (fsw*((double)(len - 1)/(double)2.0));

    sfstart    = (unsigned short)dfstart;
    sfracstart = (unsigned short)lround((dfstart - (double)sfstart)*(double)65536.0);

    fstart     = (unsigned long)((sfstart<<16) | sfracstart);

    return(fstart);
}

unsigned long drvSA1350::_calcFrqStop(sFrqValues *Values)
{
    unsigned short sfstop;
    unsigned short sfracstop;
    unsigned long fstop;
    unsigned long len = (unsigned long)(Values->FrqSpan/(Values->FrqStepWidth/(double)1000.0)) + 1;
    double fsw        = (double)_calcFrqStep(Values)/(double)65536.0;
    double dfstop     = (double)_calcFrqCorrect(_calcFrqCenter(Values)) + (fsw*((double)(len - 1)/(double)2.0));

    sfstop    = (unsigned short)dfstop;
    sfracstop = (unsigned short)lround((dfstop - (double)sfstop)*(double)65536.0);

    fstop     = (unsigned long)((sfstop<<16) | sfracstop);

    return(fstop);
}

unsigned long drvSA1350::_calcFrqStep(sFrqValues *Values)
{
    unsigned long fstep;

    fstep  = lround((Values->FrqStepWidth/(double)1000.0)*(double)65536.0);

    return(fstep);
}

unsigned char drvSA1350::_calcFrqRBW(sFrqValues *Values)
{
    unsigned char rbw = Status.activeDeviceInfo.ActiveRBWTable->last().RegValue;

    if(Values->flagModeEasyRf)
    {
        // Find nearest device RBW greater than or equal to current value
        foreach(sFrqRBW devRbw, *Status.activeDeviceInfo.ActiveRBWTable)
        {
            if(devRbw.FrqKHz >= Values->RBW)
            {
                rbw = devRbw.RegValue;
                break;
            };
        };
    }
    else
    {
        rbw = Status.activeDeviceInfo.ActiveRBWTable->at(Values->RBWIndex).RegValue;
    };

    return(rbw);
}

unsigned short drvSA1350::_calcFrqCount(sFrqValues *Values)
{
    unsigned short sfcount;

    sfcount = (unsigned short)lround((double)1000.0/Values->FrqStepWidth);

    return(sfcount);
}

unsigned char  drvSA1350::_calcFrqGain(unsigned char RefDcLevelIndex)
{
    return(FrqTableRefLevel[RefDcLevelIndex].RegValue);
}

void drvSA1350::u16toPar(unsigned short value, unsigned char *par)
{
    *par    = (unsigned char) ((value>>8) & 0xff);
    *(par+1)= (unsigned char) (value & 0xff);
}

void drvSA1350::u32toPar(unsigned long value, unsigned char *par)
{
    *par    = (unsigned char) ((value>>24) & 0xff);
    *(par+1)= (unsigned char) ((value>>16) & 0xff);
    *(par+2)= (unsigned char) ((value>>8) & 0xff);
    *(par+3)= (unsigned char) (value & 0xff);
}

// SA1350 LoadCalData Helper Function Declaration
void drvSA1350::SetGetDataPtr(QByteArray *DataBuffer)
{
    GetData = *DataBuffer;
}

quint8  drvSA1350::GetDataU8(void)
{
    unsigned char u8 = 0;

    if(GetData.size()>=1)
    {
        u8 = GetData.at(0);
        GetData.remove(0,1);
    };

    return(u8);
}

quint16 drvSA1350::GetDataU16(void)
{
    quint16 u16 = 0;

    if(GetData.size()>=2)
    {
        u16  = (quint16)((unsigned char)GetData.at(0)<< 8);
        u16 |= (quint16)((unsigned char)GetData.at(1) & 0xff);
        GetData.remove(0,2);
    };

    return(u16);
}

quint32  drvSA1350::GetDataU32(void)
{
    quint32 u32 = 0;
    quint8  u8[4];

    if(GetData.size()>=4)
    {
        u8[0]=(unsigned char) GetData.at(0);
        u8[1]=(unsigned char) GetData.at(1);
        u8[2]=(unsigned char) GetData.at(2);
        u8[3]=(unsigned char) GetData.at(3);

        u32  = (quint32)(u8[0]<<24);
        u32 |= (quint32)(u8[1]<<16);
        u32 |= (quint32)(u8[2]<< 8);
        u32 |= (quint32)(u8[3]    );
        GetData.remove(0,4);
    };

    return(u32);
}

quint64 drvSA1350::GetDataU64(void)
{
    quint64 u64 = 0;
    quint8  u8[8];

    if(GetData.size()>=8)
    {
        u8[0]=(unsigned char) GetData.at(0);
        u8[1]=(unsigned char) GetData.at(1);
        u8[2]=(unsigned char) GetData.at(2);
        u8[3]=(unsigned char) GetData.at(3);
        u8[4]=(unsigned char) GetData.at(4);
        u8[5]=(unsigned char) GetData.at(5);
        u8[6]=(unsigned char) GetData.at(6);
        u8[7]=(unsigned char) GetData.at(7);

        u64  = (quint64)((quint64)u8[0]<<56);
        u64 |= (quint64)((quint64)u8[1]<<48);
        u64 |= (quint64)((quint64)u8[2]<<40);
        u64 |= (quint64)((quint64)u8[3]<<32);
        u64 |= (quint64)((quint64)u8[4]<<24);
        u64 |= (quint64)((quint64)u8[5]<<16);
        u64 |= (quint64)((quint64)u8[6]<< 8);
        u64 |= (quint64)((quint64)u8[7]);
        GetData.remove(0,8);
    };

    return(u64);
}

double drvSA1350::GetDataDouble(void)
{
    double        dValue = 0;

    quint64 u64Value;
    double *ptrDoubleValue = (double*)&u64Value;

    if(GetData.size()>=8)
    {
        u64Value = (quint64)GetDataU64();
        dValue = (double)*ptrDoubleValue;
    };
    return(dValue);
}

bool drvSA1350::GetDataStr(char *Str , int Length)
{
    bool done = false;
    int index = 0;

    if(GetData.size()>=Length && Str!=NULL)
    {
        do
        {
            *Str = GetData.at(index);
            index++;
        }while(index<Length);
        GetData.remove(0,Length);
        done = true;
    };

    return(done);
}

// Private SA1350 Spectrum Function Definition
void drvSA1350::specSave(QList<sa1350Frame> *DecoderBuffer)
{
    int index;
    int steps;
    Q_UNUSED(steps)
    signed char tmpValue;
    sSpectrum spectrum;

    if(!DecoderBuffer)
        return;

    spectrum.SpecId = currentSpectrumId;
    spectrum.Data.clear();
    foreach(sa1350Frame item, *DecoderBuffer)
    {
        for(index=0;index<item.Length;index++)
        {
            tmpValue = (signed char) item.Data[index];
            spectrum.Data.append(tmpValue);
        };
    };

    specOffset(&spectrum);
    SpectrumBuffer.append(spectrum);
    emit signalSpectrumReceived();
}

void drvSA1350::specCalcOffset(int SpecId, sFrqValues *FrqValues)
{
    Q_UNUSED(SpecId)
    Q_UNUSED(FrqValues)
    // int NrOfSamples=0;
    // QVector<double> FrqList;

    // if(FrqValues)
    // {
    //  SpectrumOffset.SpecId = SpecId;
    //  // Caluclate required FrqSamples samples
    //  NrOfSamples = (FrqValues->FrqStop - FrqValues->FrqStart)/FrqValues->FrqStepWidth;
    //  FrqList.clear();
    //  // Generate FrqList
    //  for(int index=0;index<NrOfSamples;index++)
    //  {
    //   FrqList.append((double)(FrqValues->FrqStart+(index*FrqValues->FrqStepWidth)));
    //  };
    //  // Caluclate Offset List
    //  SpectrumOffset.Offset.clear();
    //  foreach(double frq,FrqList)
    //  {// ToDo: Add Polynom Caluclation here and take care to use the right one!!!!!!
    //   //SpectrumOffset.Offset
    //  };
    // };
}

void drvSA1350::specOffset(sSpectrum *Spectrum)
{
    Q_UNUSED(Spectrum)
    // if(Spectrum->SpecId == SpectrumOffset.SpecId)
    // {
    //  for(int index=0;index<Spectrum->Data.count();index++)
    //  {
    //   Spectrum->Data[index]+=SpectrumOffset.Offset[index];
    //  };
    // };
}

// Private SA1350 Firmware Updater Definition
bool drvSA1350::FwVersionIsOk(void)
{
    bool ok = false;

    if(
            (Status.activeDeviceInfo.FWVersion >= MIN_FW_VERSION)
            && (Status.activeDeviceInfo.FWVersion != NULL_FW_VERSION)
            )
    {
        ok = true;
    };

    return(ok);
}
