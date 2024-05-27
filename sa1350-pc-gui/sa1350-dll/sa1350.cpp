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
/*! \file sa1350.cpp
 \brief Defines the exported functions for the DLL application.
*/
#include "sa1350.h"
#include "cDeviceDriver.h"
#include "cUsbDetect.h"

using namespace std;

#define DLL_VERSION		((unsigned short)(0x0103)) /*!< DLL version number in High_byte.Low_byte format */

volatile bool           flagInit      = false; /*!< Add in-line comment */
volatile bool           flagConnected = false; /*!< Add in-line comment */

cDeviceDriver           *Device    = NULL; /*!< Add in-line comment */
cRegAccess		RegAccess; /*!< Add in-line comment */
cUsbDetect		*UsbDetect = NULL; /*!< Add in-line comment */

lComPortList            ComPortList; /*!< Add in-line comment */
vUsbDeviceList          UsbDeviceList; /*!< Add in-line comment */

SA1350_API unsigned short API_CALL sa1350GetDllVersion(void)
{
    return(DLL_VERSION);
}

SA1350_API bool API_CALL sa1350FindDevices(unsigned short &NumDevices)
{
    bool ok = false;

    ComPortList.clear();
    sUsbDeviceInfo usbdeviceInfo;

    NumDevices = 0;

    RegAccess.LoadComPortList(&ComPortList);

    if(RegAccess.LoadRegDeviceList(&UsbDeviceList))
    {
        for(vUsbDeviceList::iterator itemDevice= UsbDeviceList.begin(); itemDevice!= UsbDeviceList.end(); itemDevice++)
        {
            if(RegAccess.LoadRegDeviceDetails(itemDevice))
            {
                NumDevices++;
                if(!ComPortList.empty())
                    for(lComPortList::iterator itemCom = ComPortList.begin(); itemCom!= ComPortList.end(); itemCom++)
                    {
                        if(itemDevice->PortName.compare(itemCom->Nr)==0 && itemDevice->PortName.size()!=0)
                        {
                            itemDevice->Connected = true;
                            ok = true;
                            break;
                        }
                        else
                        {
                            itemDevice->Connected = false;
                            ok = true;
                        };
                    };
            };
        };
    };
    NumDevices = UsbDeviceList.size();

    return(ok);
}

SA1350_API bool API_CALL sa1350GetDeviceInfo(unsigned short DevNr, SA1350UsbDevice *DevInfo)
{
    bool ok = false;
    if(DevInfo && DevNr<UsbDeviceList.size())
    {
        strcpy(&DevInfo->SerialNr[0],UsbDeviceList.at(DevNr).SerialNr.c_str());
        strcpy(&DevInfo->DevDesc[0],UsbDeviceList.at(DevNr).DevDesc.c_str());
        strcpy(&DevInfo->LocationInformation[0],UsbDeviceList.at(DevNr).LocationInformation.c_str());
        strcpy(&DevInfo->PortName[0],UsbDeviceList.at(DevNr).PortName.c_str());
        DevInfo->Connected = UsbDeviceList.at(DevNr).Connected;
        ok = true;
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350Init(void)
{
    if(flagInit)
        return(false);

    flagConnected = false;

    if(!UsbDetect)
        UsbDetect = new cUsbDetect();

    if(!Device)
        Device    = new cDeviceDriver();

    if(Device && UsbDetect)
    {
        if(Device->IsInit())
        {
            flagInit = true;
            return(true);
        };
    };

    return(false);
}

SA1350_API bool API_CALL sa1350IsInit(void)
{
    return(flagInit);
}

SA1350_API void API_CALL sa1350DeInit(void)
{
    flagInit      = false;
    flagConnected = false;

    if(Device)
    {
        if(Device->IsOpen())
            Device->Close();
        delete Device;
        Device = NULL;
    };
    if(UsbDetect)
    {
        UsbDetect->Off();
        delete UsbDetect;
        UsbDetect = NULL;
    };
}

SA1350_API bool API_CALL sa1350GetStatus(SA1350Status &Status)
{
    if(Device)
    {
        if(UsbDetect)
        {
            Status.flagUsbRemoval = UsbDetect->EventUnplugged();
            if(Status.flagUsbRemoval)
            {
                sa1350Disconnect();
                Status.flagUsbRemoval  = true;
            };
        }
        else
        {
            Status.flagUsbRemoval  = false;
        };

        Status.flagInit			= flagInit;
        Status.flagOpen			= flagConnected;
        Status.flagDrvError			= !Device->IsDrvOk();
        Status.flagFrameError		= false;
        Status.flagFrameReceived		= Device->HasFrameReceived();
        Status.flagFrameErrorCrc		= Device->HasFrameCrcError();
        Status.flagFrameErrorTimeOut         = Device->HasFrameTimeoutError();
    }
    else
    {
        Status.flagInit			= false;
        Status.flagOpen			= false;
        Status.flagDrvError			= false;
        Status.flagFrameError		= false;
        Status.flagFrameReceived		= false;
        Status.flagFrameErrorCrc		= false;
        Status.flagFrameErrorTimeOut  = false;
        Status.flagUsbRemoval	 = false;
    };

    return(true);
}

SA1350_API bool API_CALL sa1350Connect(const char *strPort)
{
    bool ok = false;

    if(flagConnected)
        return(false);

    if(!Device)
        return(false);

    ok = Device->Open(strPort);

    if(ok)
    {
        if(UsbDetect->On(strPort))
        {
            ok = true;
            flagConnected = true;
        }
        else
        {
            ok = false;
        };
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350IsConnected(void)
{
    bool ok = false;

    if(!flagInit)
        return(false);

    if(Device)
    {
        ok = Device->IsOpen();
    };

    if(ok)
    {
        flagConnected = true;
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350Disconnect(void)
{
    bool ok = false;

    if(flagInit && flagConnected)
    {
        if(Device)
        {
            ok = Device->Close();
        };
    };

    if(ok)
    {
        UsbDetect->Off();
        flagConnected = false;
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350IsFrameAvailable(void)
{
    bool ok = false;

    if(flagInit && flagConnected)
    {
        if(Device)
        {
            ok = !Device->IsFrameFifoEmpty();
        };
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350GetFrameFifoSize(unsigned short &Size)
{
    bool ok = false;

    if(flagInit && flagConnected)
    {
        if(Device)
        {
            ok = Device->GetFrameFifoSize(Size);
        };
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350GetFrame(SA1350Frame *Frame)
{
    bool ok = false;
    sFrame srcFrame;
    if(flagInit && flagConnected)
    {
        if(Device)
        {
            if(Device->GetFrame(&srcFrame))
            {
                Frame->Cmd		= srcFrame.Cmd;
                Frame->Crc		= srcFrame.Crc;
                Frame->Length	= srcFrame.Length;
                for(unsigned int index=0;index<srcFrame.Data.size();index++)
                {
                    Frame->Data[index] = srcFrame.Data[index];
                };
                ok = true;
            };
        };
    };

    return(ok);
}

SA1350_API bool API_CALL sa1350SendCmd(unsigned char Cmd, unsigned char *Data, unsigned short Size)
{
    bool ok = false;

    if(flagInit && flagConnected)
    {
        if(Device)
        {
            ok = Device->SendFrame(Cmd,Data,(unsigned long)Size);
        };
    };

    return(ok);
}
