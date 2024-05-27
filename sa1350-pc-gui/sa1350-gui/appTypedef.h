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
/*! \file appTypedef.h */
#pragma once
#include <QColor>
#include <QVector>

#define MAX_TRACES  4 /*!< Add in-line comment */
#define MAX_MARKERS 4 /*!< Add in-line comment */

#define MAX_ITEMS_REFLEVEL    8 /*!< Add in-line comment */
#define MAX_ITEMS_SPAN        9 /*!< Add in-line comment */
#define MAX_ITEMS_RANGE       3 /*!< Add in-line comment */
#define MAX_ITEMS_RBW        21 /*!< Add in-line comment */

#define EXPERT_RF_MODE  MAX_ITEMS_SPAN /*!< Add in-line comment */
                                /* TODO: Extract this value from the target on connect */
/*!
 \brief Add brief

 \enum eMarker
*/
enum eMarker
{
    M_0 = 0,           /*!< Add in-line comment */
    M_1,               /*!< Add in-line comment */
    M_2,               /*!< Add in-line comment */
    M_3,               /*!< Add in-line comment */
    M_OFF,             /*!< Add in-line comment */
    M_PEAK,            /*!< Add in-line comment */
    M_ALL,             /*!< Add in-line comment */
    M_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eMarkerTrace
*/
enum eMarkerTrace
{
    M_TRACE_CLRWRITE = 0,    /*!< Add in-line comment */
    M_TRACE_MAXHOLD,         /*!< Add in-line comment */
    M_TRACE_AVERAGE,         /*!< Add in-line comment */
    M_TRACE_PEAK,            /*!< Add in-line comment */
    M_TRACE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eMarkerMode
*/
enum eMarkerMode
{
    M_MODE_0 = 0,           /*!< Add in-line comment */
    M_MODE_1,               /*!< Add in-line comment */
    M_MODE_2,               /*!< Add in-line comment */
    M_MODE_3,               /*!< Add in-line comment */
    M_MODE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eGridMode
*/
enum eGridMode
{
    GRID_OFF=0,           /*!< Add in-line comment */
    GRID_LINES,           /*!< Add in-line comment */
    GRID_DOTTED,          /*!< Add in-line comment */
    GRID_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eTrace
*/
enum eTrace
{
    TRACE_0=0,             /*!< Add in-line comment */
    TRACE_1,               /*!< Add in-line comment */
    TRACE_2,               /*!< Add in-line comment */
    TRACE_3,               /*!< Add in-line comment */
    TRACE_OFF,             /*!< Add in-line comment */
    TRACE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eTraceMode
*/
enum eTraceMode
{
    T_MODE_OFF = 0,         /*!< Add in-line comment */
    T_MODE_CLEARWRITE,      /*!< Add in-line comment */
    T_MODE_MAXHOLD,         /*!< Add in-line comment */
    T_MODE_AVERAGE,         /*!< Add in-line comment */
    T_MODE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eFrqRange
*/
enum eFrqRange
{
    FRQRANGE_431_527= 0,      /*!< Add in-line comment */
    FRQRANGE_861_1054,         /*!< Add in-line comment */
    FRQRANGE_2152_2635,         /*!< Add in-line comment */
    FRQRANGE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \enum eFrqInputMode
*/
enum eFrqInputMode
{
    FRQMODE_CENTERSPAN= 0,   /*!< Add in-line comment */
    FRQMODE_STARTSTOP,       /*!< Add in-line comment */
    FRQMODE_UNDEFINED = 0xff /*!< Add in-line comment */
};

/*!
 \brief Add brief

 \typedef struct _sFrqRefLevel sFrqRefLevel
*/
/*!
 \brief Add brief

 \struct _sFrqRefLevel appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqRefLevel
{
    signed char Value;      /*!< Add in-line comment */
    unsigned char RegValue; /*!< Add in-line comment */
}sFrqRefLevel;

/*!
 \brief Add brief

 \typedef struct _sFrqSpan sFrqSpan
*/
/*!
 \brief Add brief

 \struct _sFrqSpan appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqSpan
{
    double SpanMHz;         /*!< Add in-line comment */
    double FSWkHz;          /*!< Add in-line comment */
    unsigned char RBWindex; /*!< Add in-line comment */
}sFrqSpan;

/*!
 \brief Add brief

 \typedef struct _sBrdRfLimits sBrdRfLimits
*/
/*!
 \brief Add brief

 \struct _sBrdRfLimits appTypedef.h "appTypedef.h"
*/
typedef struct _sBrdRfLimits
{
    QString BoardID; /*!< Add in-line comment */
    bool Band900Tuned;  /*!< Add in-line comment */
    bool Band440Tuned;  /*!< Add in-line comment */
} sBrdRfLimits;

/*!
 \brief Add brief

 \typedef struct _sFrqRange sFrqRange
*/
/*!
 \brief Add brief

 \struct _sFrqRange appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqRange
{
    double    FrqMin;   /*!< Add in-line comment */
    double    FrqMax;   /*!< Add in-line comment */
    double    FrqStart; /*!< Add in-line comment */
}sFrqRange;

/*!
 \brief Add brief

 \typedef struct _sFrqRBW sFrqRBW
*/
/*!
 \brief Add brief

 \struct _sFrqRBW appTypedef.h "appTypedef.h"
*/
/* Table of RBW values from TRM */
typedef struct _sFrqRBW
{
    double          FrqKHz;     /*!< Add in-line comment */
    unsigned short  IfFrqKHz;   /*!< Add in-line comment */
    unsigned char   RegValue;   /*!< Add in-line comment */
}sFrqRBW;

/*!
 \brief Add brief

 \typedef struct _sFrqRefLevGain sFrqRefLevGain
*/
/*!
 \brief Add brief

 \struct _sFrqRefLevGain appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqRefLevGain
{
    double RefLevel; /*!< Add in-line comment */
    double Gain;     /*!< Add in-line comment */
}sFrqRefLevGain;

/*!
 \brief Add brief

 \typedef struct _sFrqValues sFrqValues
*/
/*!
 \brief Add brief

 \struct _sFrqValues appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqValues
{
    QString        ProfileName;       /*!< Add in-line comment */
    bool           flagModeStartStop; /*!< Add in-line comment */
    bool           flagModeEasyRf;    /*!< Add in-line comment */
    bool           flagModeContinuous;/*!< Add in-line comment */
    int            FrqRange;          /*!< Add in-line comment */
    double         FrqStart;          /*!< Add in-line comment */
    double         FrqStop;           /*!< Add in-line comment */
    double         FrqCenter;         /*!< Add in-line comment */
    int            FrqSpanIndex;      /*!< Add in-line comment */
    double         FrqSpan;           /*!< Add in-line comment */
    double         FrqStepWidth;      /*!< Add in-line comment */
    int            RefDcLevelIndex;   /*!< Add in-line comment */
    signed char    RefDcLevel;        /*!< Add in-line comment */
    int            RBWIndex;          /*!< Add in-line comment */
    double         RBW;               /*!< Add in-line comment */
}sFrqValues;

/*!
 \brief Add brief

 \typedef struct _sFrqParameterBuffer sFrqParameterBuffer
*/
/*!
 \brief Add brief

 \struct _sFrqParameterBuffer appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqParameterBuffer
{
    unsigned char  FrqRange;     /*!< Add in-line comment */
    unsigned long  FrqStart;     /*!< Add in-line comment */
    unsigned long  FrqStop;      /*!< Add in-line comment */
    unsigned long  FrqCenter;    /*!< Add in-line comment */
    unsigned char  RBW;          /*!< Add in-line comment */
    unsigned long  FrqStepWidth; /*!< Add in-line comment */
    unsigned short FrqStepCount; /*!< Add in-line comment */
    unsigned short FrqSpan;      /*!< Add in-line comment */
    unsigned char  SpanIndex;    /*!< Add in-line comment */
}sFrqParameterBuffer;

/*!
 \brief Add brief

 \typedef struct _sFrqSetting sFrqSetting
*/
/*!
 \brief Add brief

 \struct _sFrqSetting appTypedef.h "appTypedef.h"
*/
typedef struct _sFrqSetting
{
    QString        Name;   /*!< Add in-line comment */
    sFrqValues     Values; /*!< Add in-line comment */
}sFrqSetting;

/*!
 \brief Add brief

 \typedef struct _sSettingMarker sSettingMarker
*/
/*!
 \brief Add brief

 \struct _sSettingMarker appTypedef.h "appTypedef.h"
*/
typedef struct _sSettingMarker
{
    bool        On;    /*!< Add in-line comment */
    eMarker     Nr;    /*!< Add in-line comment */
    eMarkerMode Mode;  /*!< Add in-line comment */
    eTrace      Trace; /*!< Add in-line comment */
    int    Index;      /*!< Add in-line comment */
    QColor Color;      /*!< Add in-line comment */
}sSettingMarker;

/*!
 \brief Add brief

 \typedef struct _sMarkerInfo sMarkerInfo
*/
/*!
 \brief Add brief

 \struct _sMarkerInfo appTypedef.h "appTypedef.h"
*/
typedef struct _sMarkerInfo
{
    bool        On[3];       /*!< Add in-line comment */
    double      FrqValue[3]; /*!< Add in-line comment */
    double      dBmValue[3]; /*!< Add in-line comment */
}sMarkerInfo;

/*!
 \brief Add brief

 \typedef struct _sSettingTrace sSettingTrace
*/
/*!
 \brief Add brief

 \struct _sSettingTrace appTypedef.h "appTypedef.h"
*/
typedef struct _sSettingTrace
{
    bool       On;    /*!< Add in-line comment */
    eTrace     Nr;    /*!< Add in-line comment */
    eTraceMode Mode;  /*!< Add in-line comment */
    QColor     Color; /*!< Add in-line comment */
}sSettingTrace;

/*!
 \brief Add brief

 \typedef struct _sSettingGrid sSettingGrid
*/
/*!
 \brief Add brief

 \struct _sSettingGrid appTypedef.h "appTypedef.h"
*/
typedef struct _sSettingGrid
{
    bool       On;   /*!< Add in-line comment */
    eGridMode  Mode; /*!< Add in-line comment */
}sSettingGrid;

/*!
 \brief Add brief

 \typedef struct _sSettingGui sSettingGui
*/
/*!
 \brief Add brief

 \struct _sSettingGui appTypedef.h "appTypedef.h"
*/
typedef struct _sSettingGui
{
    sSettingGrid   Grid;                /*!< Add in-line comment */
    sSettingTrace  Trace[MAX_TRACES];   /*!< Add in-line comment */
    sSettingMarker Marker[MAX_MARKERS]; /*!< Add in-line comment */
    sFrqSetting    DefaultFrqSetting;   /*!< Add in-line comment */
}sSettingGui;

/*!
 \brief Add brief

 \typedef struct _sSpectrum sSpectrum
*/
/*!
 \brief Add brief

 \struct _sSpectrum appTypedef.h "appTypedef.h"
*/
typedef struct _sSpectrum
{
    int SpecId;            /*!< Add in-line comment */
    QVector<double>  Data; /*!< Add in-line comment */
}sSpectrum;

/*!
 \brief Add brief

 \typedef struct _sSpectrumOffset sSpectrumOffset
*/
/*!
 \brief Add brief

 \struct _sSpectrumOffset appTypedef.h "appTypedef.h"
*/
typedef struct _sSpectrumOffset
{
    int SpecId;              /*!< Add in-line comment */
    QVector<double>  Offset; /*!< Add in-line comment */
}sSpectrumOffset;

/*!
 \brief Add brief

 \typedef struct _sDeviceInfo sDeviceInfo
*/
/*!
 \brief Add brief

 \struct _sDeviceInfo appTypedef.h "appTypedef.h"
*/
typedef struct _sDeviceInfo
{
    QString  DeviceVersion;             /*!< Add in-line comment */
    unsigned short  FWVersion;          /*!< Add in-line comment */
    QVector<sFrqRBW> *ActiveRBWTable;   /*!< Add in-line comment */
    QVector<sFrqRBW> RBWTableBand0;     /*!< Add in-line comment */
    QVector<sFrqRBW> RBWTableBand1;     /*!< Add in-line comment */
    unsigned short MaxSpecLength;       /*!< Add in-line comment */
}sDeviceInfo;

// Typedef Struct - Flash Calibration Data
/*!
 \brief Add brief

 \typedef struct sRefLevGain sRefLevGain
*/
/*!
 \brief Add brief

 \struct sRefLevGain appTypedef.h "appTypedef.h"
*/
typedef struct sRefLevGain
{
    signed char   RefLevel; /*!< Add in-line comment */
    unsigned char Gain;     /*!< Add in-line comment */
}sRefLevGain;

/*!
 \brief Add brief

 \typedef struct sCalCoeffs sCalCoeffs
*/
/*!
 \brief Add brief

 \struct sCalCoeffs appTypedef.h "appTypedef.h"
*/
typedef struct sCalCoeffs
{
    unsigned char DcSelect;  /*!< Add in-line comment */
    double        Values[8]; /*!< Add in-line comment */
}sCalCoeffs;

/*!
 \brief Add brief

 \typedef struct sCalFrqRange sCalFrqRange
*/
/*!
 \brief Add brief

 \struct sCalFrqRange appTypedef.h "appTypedef.h"
*/
typedef struct sCalFrqRange
{
    unsigned long  FStart;   /*!< Add in-line comment */
    unsigned long  FStop;    /*!< Add in-line comment */
    unsigned long  FSamples; /*!< Add in-line comment */
}sCalFrqRange;

/*!
 \brief Add brief

 \typedef struct sCalibrationData sCalibrationData
*/
/*!
 \brief Add brief

 \struct sCalibrationData appTypedef.h "appTypedef.h"
*/
typedef struct sCalibrationData
{
    unsigned short     CalFormatVer;              /*!< Add in-line comment */
    char               CalDate[16];               /*!< Add in-line comment */
    unsigned short     CalSwVer;                  /*!< Add in-line comment */
    unsigned char      CalProdSide;               /*!< Add in-line comment */
    sCalFrqRange       CalFrqRange[3];            /*!< Add in-line comment */
    sRefLevGain        CalRefGainTable[8];        /*!< Add in-line comment */
    unsigned long      DevHwId;                   /*!< Add in-line comment */
    char               DevUsbSerNr[16];           /*!< Add in-line comment */
    unsigned long      DevFxtal;                  /*!< Add in-line comment */
    short              DevFxtalppm;               /*!< Add in-line comment */
    unsigned char      DevTempStart[6];           /*!< Add in-line comment */
    unsigned char      DevTempStop[6];            /*!< Add in-line comment */
    sCalCoeffs         DevCalCoeffsFrqGain[3][8]; /*!< Add in-line comment */
}sCalibrationData;

/*!
 \brief Add brief

 \typedef struct _sProgHeader sProgHeader
*/
/*!
 \brief Add brief

 \struct _sProgHeader appTypedef.h "appTypedef.h"
*/
typedef struct _sProgHeader
{
    unsigned short MemStartAddr; /*!< Add in-line comment */
    unsigned short MemLength;    /*!< Add in-line comment */
    unsigned short MemType;      /*!< Add in-line comment */
    unsigned short TypeVersion;  /*!< Add in-line comment */
    unsigned short Crc16;        /*!< Add in-line comment */
}sProgHeader;

/*!
 \brief Add brief

 \typedef struct _sCalMemory sCalMemory
*/
/*!
 \brief Add brief

 \struct _sCalMemory appTypedef.h "appTypedef.h"
*/
typedef struct _sCalMemory
{
    sProgHeader      Header;  /*!< Add in-line comment */
    sCalibrationData CalData; /*!< Add in-line comment */
}sCalMemory;

extern const sFrqRefLevel   FrqTableRefLevel[MAX_ITEMS_REFLEVEL]; /*!< Add in-line comment */
extern const sFrqSpan       FrqTableSpan[MAX_ITEMS_SPAN];         /*!< Add in-line comment */
extern const sFrqRange      FrqTableRange[MAX_ITEMS_RANGE];       /*!< Add in-line comment */
extern const QVector<sBrdRfLimits> BrdTableRFLimits;              /*!< Add in-line comment */
