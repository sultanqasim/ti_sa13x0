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
#include "cDeviceDriver.h"

using namespace std;

cDeviceDriver::cDeviceDriver()
{
    // Init Variables
    flagCreated            = false;
    flagOpen               = false;
    flagExitThread         = false;

    // Generate Objects
    Drv                    = new cDriver();

    eDrvError              = new cEvent(true);
    eDrvOpen               = new cEvent(true);

    eDecoderInit           = new cEvent(false);
    eDecoderStart          = new cEvent(false);
    eDecoderStop           = new cEvent(false);
    eDecoderExit           = new cEvent(false);

    eventFrameReceived     = new cEvent(true);
    eventFrameErrorCrc     = new cEvent(true);
    eventFrameErrorTimeOut = new cEvent(true);

    FrameFifo              = new list<sFrame>;
    ThreadHandle           = new TThread<cDeviceDriver>(*this,&cDeviceDriver::run);

    // Check if Objects are created
    if(ThreadHandle && Drv && eDrvError && eDrvOpen && eDecoderInit && eDecoderStart && eDecoderStop && eDecoderExit && FrameFifo )
    {
        flagCreated = true;
    };

    // Start Driver Thread;
    if(flagCreated)
    {
        ThreadHandle->StartAndWait();
        if(ThreadHandle->IsRunning())
            flagCreated = true;
        else
            flagCreated = false;
    };
}

cDeviceDriver::~cDeviceDriver()
{
    // Close Thread
    if(ThreadHandle->IsRunning())
    {
        flagExitThread = true;
        ThreadHandle->WaitUntilTerminate();
    };
    // Close Driver
    if(Drv)
    {
        Drv->Close();
        delete Drv;
        Drv = NULL;
    };
    // Clean up objects
    delete ThreadHandle;
    delete eDrvError;
    delete eDrvOpen;
    delete eDecoderInit;
    delete eDecoderStart;
    delete eDecoderStop;
    delete eDecoderExit;
    delete FrameFifo;
    delete Drv;
    delete eventFrameErrorCrc;
    delete eventFrameErrorTimeOut;
    delete eventFrameReceived;
}

// Public Function Defintion
bool cDeviceDriver::IsInit(void)
{
    return(flagCreated);
}

bool cDeviceDriver::Open(std::string strPort)
{
    if(!drvOpen(strPort))
    {
        flagOpen = false;
    }
    else
    {
        // Clear FrameFifo
        FrameFifoAccess.Lock();
        FrameFifo->clear();
        FrameFifoAccess.Unlock();

        eventFrameErrorCrc->Reset();
        eventFrameErrorTimeOut->Reset();
        eventFrameReceived->Reset();

        flagOpen = true;
    };
    return(flagOpen);
}

bool cDeviceDriver::IsOpen(void)
{
    return(flagOpen);
}

bool cDeviceDriver::IsDrvOk(void)
{
    return(!this->eDrvError->Check());
}

bool cDeviceDriver::Close(void)
{
    if(this->drvClose())
    {
        flagOpen = false;
        return(true);
    };
    return(false);
}

bool cDeviceDriver::Reset(void)
{
    return(false);
}

bool cDeviceDriver::SendFrame(unsigned char Cmd, unsigned char *Data, unsigned short size)
{
    bool ok = false;
    sFrame newFrame;

    MakeFrame(&newFrame,Cmd,Data,size);

    ok = drvSendFrame(&newFrame);

    return(ok);
}

bool cDeviceDriver::IsFrameFifoEmpty(void)
{
    bool answ = false;

    if(Drv)
    {
        FrameFifoAccess.Lock();
        answ = FrameFifo->empty();
        FrameFifoAccess.Unlock();
    };

    return(answ);
}

bool cDeviceDriver::GetFrameFifoSize(unsigned short &Size)
{
    if(Drv)
    {
        FrameFifoAccess.Lock();
        Size = FrameFifo->size();
        FrameFifoAccess.Unlock();
        return(true);
    };

    Size = 0;

    return(false);
}
bool cDeviceDriver::GetFrame(sFrame *Frame)
{
    if(Drv && Frame!=NULL)
    {
        FrameFifoAccess.Lock();
        if(!FrameFifo->empty())
        {
            *Frame = FrameFifo->front();
            FrameFifo->pop_front();
        };
        FrameFifoAccess.Unlock();
        return(true);
    };

    return(false);
}

bool cDeviceDriver::HasFrameReceived(void)
{
    return(eventFrameReceived->Check());
}

bool cDeviceDriver::HasFrameCrcError(void)
{
    return(eventFrameErrorCrc->Check());
}

bool cDeviceDriver::HasFrameTimeoutError(void)
{
    return(eventFrameErrorTimeOut->Check());
}

// Private Function Definition
bool cDeviceDriver::drvOpen(std::string strPort)
{
    bool ok = false;

    if(Drv)
    {
        if(eDrvOpen->Check())
            drvClose();

        DrvAccess.Lock();
        drvSignalErrorReset();
        if(Drv->Open(strPort,B115200,DAT_8,PAR_NONE,STOP_1,FLOW_OFF))
        {
            ok = true;
        }
        else
        {
            drvSignalError(Drv->GetLastErrorCode(),Drv->GetLastErrorString());
        };
        DrvAccess.Unlock();
    };
    if(ok)
    {
        this->eDrvOpen->Signal();
    }

    return(ok);
}

bool cDeviceDriver::drvGetData(std::string *rxFifo)
{
    bool ok = false;
    unsigned long size=0;

    DrvAccess.Lock();
    if(Drv->GetRcvBufferSize(size))
    {
        if(size>0)
        {
            rxFifo->resize(rxFifo->size()+size);
            if(Drv->ReadData((unsigned char*)(rxFifo->data()),(unsigned short)size))
            {
                ok = true;
            }
            else
            {
                drvSignalError(Drv->GetLastErrorCode(),Drv->GetLastErrorString());
            };
        };
    };
    DrvAccess.Unlock();

    if(!Drv->IsOk())
    {
        drvSignalError(Drv->GetLastErrorCode(),Drv->GetLastErrorString());
        ok = false;
    };

    return(ok);
}

bool cDeviceDriver::drvClose(void)
{
    bool ok = false;

    if(Drv)
    {
        DrvAccess.Lock();
        ok = Drv->Close();
        DrvAccess.Unlock();
        this->eDrvOpen->Reset();
        this->eDrvError->Reset();
    };

    return(ok);
}

void cDeviceDriver::Raw2Frame(std::string &Raw, sFrame &Frame)
{
    if(Raw.size()<4)
    {
        return;
    };
    Frame.Cmd      = Raw.at(1);
    Frame.Length   = Raw.at(2);
    Frame.Data.clear();
    if(Frame.Length>0)
    {
        Frame.Data.append((Raw.data()+4),Frame.Length);
        Frame.Crc = (unsigned short)((Raw.at(3+Frame.Length)<<8) + (Raw.at(4+Frame.Length)<<8));
    }
    else
    {
        Frame.Crc = (unsigned short)((Raw.at(3)<<8) + (Raw.at(4)<<8));
    };
}

void cDeviceDriver::Frame2Raw(sFrame &Frame, std::string &Raw)
{
    Raw.clear();

    Raw.resize(3);

    Raw[0] = 0x2A;
    Raw[1] = Frame.Length;
    Raw[2] = Frame.Cmd;

    if(Frame.Data.size()>0)
    {
        Raw.append(Frame.Data);
    };
    Raw.resize(Raw.size()+2);
    Frame.Crc = calcCrc16(Raw);
    Raw[Raw.size()-2] = (unsigned char) (Frame.Crc >>8);
    Raw[Raw.size()-1] = (unsigned char) (Frame.Crc & 0xff);
}

void cDeviceDriver::crc16AddByte(unsigned short &crc, unsigned char u8)
{
    crc  = (unsigned char)(crc>>8)|(crc<<8);
    crc ^=  u8;
    crc ^= (unsigned char)(crc & 0xff)>>4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4)<< 1;
}

unsigned short cDeviceDriver::calcCrc16(std::string &Raw)
{
    int index;
    int datalength;
    unsigned short crc = 0x2A;

    datalength = Raw.size()-2;

    for(index=1 ; index<datalength; index++)
    {
        crc16AddByte(crc,(unsigned char)Raw.at(index));
    };
    return(crc);
}

bool cDeviceDriver::drvSendFrame(sFrame *Frame)
{
    bool ok = false;
    std::string rawFrame;

    this->Frame2Raw(*Frame,rawFrame);

    if(Drv)
    {
        DrvAccess.Lock();
        if(Drv->WriteData((unsigned char*)rawFrame.data(),(unsigned short)rawFrame.size()))
        {
            ok = true;
        }
        else
        {
            drvSignalError(Drv->GetLastErrorCode(),Drv->GetLastErrorString());
            ok =false;
        };
        DrvAccess.Unlock();
    };

    return(ok);
}

void cDeviceDriver::drvSignalError(unsigned int Code, std::string Msg)
{
    Error.Code = Code;
    Error.Msg  = Msg;
    eDrvOpen->Reset();
    Drv->Close();
    eDrvError->Signal();
}

void cDeviceDriver::drvSignalErrorReset(void)
{
    Error.Code = 0;
    Error.Msg  = "No Error";
    eDrvError->Reset();
}

void cDeviceDriver::FrameDecoderInit(void)
{
    DecoderState.Crc       = 0x2A;
    DecoderState.CrcIndex  = 0;
    DecoderState.DataIndex = 0;
    DecoderState.State     = DS_WAITSMARKER;
    ResetFrame(&DecoderState.Frame);
}

bool cDeviceDriver::FrameDecoder(std::string *rxFifo)
{
    bool exit = false;
    unsigned char u8Data = 0xff;

    while(!rxFifo->empty() && !exit)
    {
        u8Data = (char) rxFifo->at(0);
        rxFifo->erase(rxFifo->begin());
        switch(DecoderState.State)
        {
        case DS_WAITSMARKER:
            if(u8Data==0x2A)
            {
                DecoderState.Crc = 0x2A;
                DecoderState.CrcIndex = 0;
                DecoderState.DataIndex = 0;
                ResetFrame(&DecoderState.Frame);
                DecoderState.State = DS_WAITLENGTH;
            };
            break;
        case DS_WAITLENGTH:
            DecoderState.Frame.Length = u8Data;
            crc16AddByte(DecoderState.Crc,u8Data);
            DecoderState.State = DS_WAITCMD;
            break;
        case DS_WAITCMD:
            DecoderState.Frame.Cmd = u8Data;
            crc16AddByte(DecoderState.Crc,u8Data);
            if(DecoderState.Frame.Length > 0)
                DecoderState.State = DS_WAITDATA;
            else
                DecoderState.State = DS_CRC_HIGH;
            break;
        case DS_WAITDATA:
            DecoderState.Frame.Data.push_back(u8Data);
            crc16AddByte(DecoderState.Crc,u8Data);
            DecoderState.DataIndex++;
            if(DecoderState.DataIndex == DecoderState.Frame.Length)
            {
                DecoderState.State = DS_CRC_HIGH;
            };
            break;
        case DS_CRC_HIGH:
            DecoderState.Frame.Crc = (unsigned short)(u8Data<<8);
            DecoderState.State = DS_CRC_LOW;
            break;
        case DS_CRC_LOW:
            DecoderState.Frame.Length = DecoderState.Frame.Data.size();
            DecoderState.Frame.Crc += (unsigned short)(u8Data);
            DecoderState.State = DS_WAITSMARKER;
            if(DecoderState.Frame.Crc == DecoderState.Crc)
            {
                FrameFifoAccess.Lock();
                FrameFifo->push_back(DecoderState.Frame);
                eventFrameReceived->Signal();
                FrameFifoAccess.Unlock();
            }
            else
            {
                eventFrameErrorCrc->Signal();
            };
            break;
        default:
            break;
        };
    };

    return(false);
}

void cDeviceDriver::ResetFrame(sFrame *Frame)
{
    Frame->Cmd    = 0;
    Frame->Length = 0;
    Frame->Data.clear();
    Frame->Crc    = 0;
}

void cDeviceDriver::MakeFrame(sFrame *Frame, unsigned char Cmd, unsigned char *Data, unsigned short Length)
{
    Frame->Cmd  = Cmd;
    Frame->Data.clear();
    Frame->Length = (unsigned char) Length;
    if(Data && Length>0)
    {
        Frame->Data.append((const char*)Data, (int)Length);
    };
}

void cDeviceDriver::run(void)
{
    DriverState State;
    std::string RxFifo;

    RxFifo.clear();

    State = DRV_INIT;

    do
    {
        switch(State)
        {
        case DRV_INIT:
            if(this->eDrvOpen->CheckSignal(20))
            {
                RxFifo.clear();
                FrameDecoderInit();
                State = DRV_RUN;
            };
            break;
        case DRV_RUN:
            if(!this->drvGetData(&RxFifo) && RxFifo.empty())
            {
                Sleep(1);
                break;
            };
            FrameDecoder(&RxFifo);
            break;
        case DRV_STOP:
            Sleep(10);
            break;
        case DRV_EXIT:
            break;
        };
        // Check Thread Exit Signals
        if(this->eDecoderExit->Check())
            flagExitThread = true;
        else
            if(this->eDrvError->Check())
                State = DRV_INIT;
            else
                if(!this->eDrvOpen->Check())
                    State = DRV_INIT;
                else
                    if(this->eDecoderInit->Check())
                        State = DRV_INIT;
                    else
                        if(this->eDecoderStop->Check())
                            State = DRV_STOP;

    }while(!flagExitThread);
}
