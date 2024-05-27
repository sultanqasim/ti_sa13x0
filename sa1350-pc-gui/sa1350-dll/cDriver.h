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
/*! \file cDriver.h */
#pragma once
#include <string>

using namespace std;

#include "cEvent.h"

/*Serial Line Status Bits*/
#define LS_CTS  0x01 /*!< Add in-line comment */
#define LS_DSR  0x02 /*!< Add in-line comment */
#define LS_DCD  0x04 /*!< Add in-line comment */
#define LS_RI   0x08 /*!< Add in-line comment */
#define LS_RTS  0x10 /*!< Add in-line comment */
#define LS_DTR  0x20 /*!< Add in-line comment */
#define LS_ST   0x40 /*!< Add in-line comment */
#define LS_SR   0x80 /*!< Add in-line comment */

/*Serial Port Error Constant's*/
#define E_NO_ERROR                   0 /*!< Add in-line comment */
#define E_INVALID_FD                 1 /*!< Add in-line comment */
#define E_NO_MEMORY                  2 /*!< Add in-line comment */
#define E_CAUGHT_NON_BLOCKED_SIGNAL  3 /*!< Add in-line comment */
#define E_PORT_TIMEOUT               4 /*!< Add in-line comment */
#define E_INVALID_DEVICE             5 /*!< Add in-line comment */
#define E_BREAK_CONDITION            6 /*!< Add in-line comment */
#define E_FRAMING_ERROR              7 /*!< Add in-line comment */
#define E_IO_ERROR                   8 /*!< Add in-line comment */
#define E_BUFFER_OVERRUN             9 /*!< Add in-line comment */
#define E_RECEIVE_OVERFLOW          10 /*!< Add in-line comment */
#define E_RECEIVE_PARITY_ERROR      11 /*!< Add in-line comment */
#define E_TRANSMIT_OVERFLOW         12 /*!< Add in-line comment */
#define E_READ_FAILED               13 /*!< Add in-line comment */
#define E_WRITE_FAILED              14 /*!< Add in-line comment */

/*!
 \brief Valid Baudrate Values

 \enum BaudRateType
*/
enum BaudRateType
{
    B50,           /*!< Add in-line comment */ //POSIX ONLY
    B75,           /*!< Add in-line comment */ //POSIX ONLY
    B110,          /*!< Add in-line comment */
    B134,          /*!< Add in-line comment */ //POSIX ONLY
    B150,          /*!< Add in-line comment */ //POSIX ONLY
    B200,          /*!< Add in-line comment */ //POSIX ONLY
    B300,          /*!< Add in-line comment */
    B600,          /*!< Add in-line comment */
    B1200,         /*!< Add in-line comment */
    B1800,         /*!< Add in-line comment */ //POSIX ONLY
    B2400,         /*!< Add in-line comment */
    B4800,         /*!< Add in-line comment */
    B9600,         /*!< Add in-line comment */
    B14400,        /*!< Add in-line comment */ //WINDOWS ONLY
    B19200,        /*!< Add in-line comment */
    B38400,        /*!< Add in-line comment */
    B56000,        /*!< Add in-line comment */ //WINDOWS ONLY
    B57600,        /*!< Add in-line comment */
    B76800,        /*!< Add in-line comment */ //POSIX ONLY
    B115200,       /*!< Add in-line comment */
    B128000,       /*!< Add in-line comment */ //WINDOWS ONLY
    B256000,       /*!< Add in-line comment */ //WINDOWS ONLY
    B926100=926100 /*!< Add in-line comment */
};

/*!
 \brief Std. data bits types

 \enum DataBitsType
*/
enum  DataBitsType
{
    DAT_5, /*!< Add in-line comment */
    DAT_6, /*!< Add in-line comment */
    DAT_7, /*!< Add in-line comment */
    DAT_8  /*!< Add in-line comment */
};

/*!
 \brief Std. parity types

 \enum ParityType
*/
enum ParityType
{
    PAR_NONE, /*!< Add in-line comment */
    PAR_ODD,  /*!< Add in-line comment */
    PAR_EVEN, /*!< Add in-line comment */
    PAR_MARK, /*!< Add in-line comment */               //WINDOWS ONLY
    PAR_SPACE /*!< Add in-line comment */
};

/*!
 \brief Std. stop bit types

 \enum StopBitsType
*/
enum StopBitsType
{
    STOP_1,   /*!< Add in-line comment */
    STOP_1_5, /*!< Add in-line comment */               //WINDOWS ONLY
    STOP_2    /*!< Add in-line comment */
};

/*!
 \brief Std. flow types

 \enum FlowType
*/
enum FlowType
{
    FLOW_OFF,      /*!< Add in-line comment */
    FLOW_HARDWARE, /*!< Add in-line comment */
    FLOW_XONXOFF   /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \typedef struct sPortSetting sPortSetting
*/
/*!
 \brief Add brief

 \struct sPortSetting cDriver.h "cDriver.h"
*/
typedef struct sPortSetting
{
    BaudRateType	BaudRate;   /*!< Add in-line comment */
    DataBitsType	DataBits;   /*!< Add in-line comment */
    ParityType	Parity;     /*!< Add in-line comment */
    StopBitsType	StopBits;   /*!< Add in-line comment */
    FlowType	FlowControl;/*!< Add in-line comment */
    long		TimeoutMs;  /*!< Add in-line comment */
}sPortSetting;


/*!
 \brief Add brief

 \typedef struct sError sError
*/
/*!
 \brief Add brief

 \struct sError cDriver.h "cDriver.h"
*/
typedef struct sError
{
    int         Code;  /*!< Add in-line comment */
    std::string Msg;   /*!< Add in-line comment */
}sError;

/*!
 \brief Add brief

 \class cDriver cDriver.h "cDriver.h"
*/
class cDriver
{
public:
    /*!
     \brief Constructor

    */
    cDriver();
    /*!
     \brief Destructor

    */
    ~cDriver();
    /*!
     \brief Open Comport based on given parameter

     \param strPort Port String
     \param Baud Baudrate
     \param Bits Data Bit Length
     \param Parity Parity Bit
     \param Stopbits Number of Stop Bits
     \param FlowCtrl Flow Control
     \return bool true:comport is open false:failed to open
    */
    bool Open(std::string strPort, BaudRateType Baud=B115200, DataBitsType Bits=DAT_8, ParityType Parity=PAR_NONE, StopBitsType Stopbits=STOP_1, FlowType FlowCtrl=FLOW_OFF);
    /*!
     \brief Close active comport

     \return bool
    */
    bool Close(void);
    /*!
     \brief Checks if comport driver has no error

     \return bool true: No Error false: Error occured
    */
    bool IsOk(void);
    /*!
     \brief Returns last error code

     \return int
    */
    int GetLastErrorCode(void);
    /*!
     \brief Returns last error code string

     \return std::string
    */
    std::string GetLastErrorString(void);
    /*!
     \brief Returns number of received bytes from the comport

     \param Size Add param
     \return bool
    */
    bool GetRcvBufferSize(unsigned long &Size);
    /*!
     \brief Set DTR signal to high

     \param state Add param
     \return int
    */
    int  SetDtr(bool state=true);
    /*!
     \brief Set RTS signal to high

     \param state Add param
     \return int
    */
    int  SetRts(bool state=true);
    /*!
     \brief Get CTS signal state

     \return bool
    */
    bool GetCts(void);
    /*!
     \brief Flush current receive and transmit buffer

     \return bool
    */
    bool Flush(void);
    /*!
     \brief Write data to the comport

     \param Data Add param
     \param size Add param
     \return bool
    */
    bool WriteData(unsigned char *Data, unsigned short size);
    /*!
     \brief Read data from the comport

     \param Data Add param
     \param size Add param
     \return bool
    */
    bool ReadData(unsigned char *Data, unsigned short size);
    /*!
    \brief Reset comport driver

     \return bool
    */
    bool Reset(void);

private:
    HANDLE hPort;              /*!< Add in-line comment */
    sPortSetting PortSetting;  /*!< Add in-line comment */
    COMMCONFIG   PortConfig;   /*!< Add in-line comment */
    COMMTIMEOUTS PortTimeouts; /*!< Add in-line comment */

    sError      Error;         /*!< Add in-line comment */
    std::string rxFifo;        /*!< Add in-line comment */
    cEvent      *eErrorSignal; /*!< Add in-line comment */

    /*!
     \brief Signal a comport error

     \param Code Add param
     \param Msg Add param
    */
    void SignalError(int Code, std::string Msg);
    /*!
     \brief Resets the comport error signal

    */
    void SignalErrorReset(void);
    /*!
     \brief Checks if comport open

     \return bool true:Open false:Not Open
    */
    bool isOpen(void);
    /*!
     \brief Set the comport flow control

     \param FlowType Add param
    */
    void setFlowControl(FlowType);
    /*!
     \brief Set the comport parity bits

     \param parity Add param
    */
    void setParity(ParityType parity);
    /*!
     \brief Set the comport parity bits

     \param DataBitsType Add param
    */
    void setDataBits(DataBitsType);
    /*!
     \brief Set number of comport stop bits

     \param StopBitsType Add param
    */
    void setStopBits(StopBitsType);
    /*!
     \brief Set comport baudrate

     \param BaudRateType Add param
    */
    void setBaudRate(BaudRateType);
    /*!
     \brief Set comport timeout

     \param ms Add param
    */
    void setTimeout( long ms);
};
