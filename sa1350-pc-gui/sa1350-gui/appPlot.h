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
/*! \file appPlot.h */
#pragma once

#include "appCurve.h"
#include "appGrid.h"
#include "appMarker.h"
#include "appTypedef.h"

/*!
 \brief Add brief

 \typedef struct sMarkerCfg sMarkerCfg
*/
/*!
 \brief Add brief

 \struct sMarkerCfg appPlot.h "appPlot.h"
*/
typedef struct sMarkerCfg
{
    bool            flagUpdate; /*!< Add in-line comment */
    bool            On;         /*!< Add in-line comment */
    QColor          Color;      /*!< Add in-line comment */
    int             Pos;        /*!< Add in-line comment */
    eTrace          Trace;      /*!< Add in-line comment */
    QVector<double> *DataX;     /*!< Add in-line comment */
    QVector<double> *DataY;     /*!< Add in-line comment */
}sMarkerCfg;

/*!
 \brief Add brief

 \typedef struct sTraceCfg sTraceCfg
*/
/*!
 \brief Add brief

 \struct sTraceCfg appPlot.h "appPlot.h"
*/
typedef struct sTraceCfg
{
    bool               flagUpdate; /*!< Add in-line comment */
    bool               On;         /*!< Add in-line comment */
    bool               flagHold;   /*!< Add in-line comment */
    QColor             TextColor;  /*!< Add in-line comment */
    QColor             Color;      /*!< Add in-line comment */
    eTraceMode         Mode;       /*!< Add in-line comment */
    appMarker          *Marker[3]; /*!< Add in-line comment */
    QVector<double>    *DataX;     /*!< Add in-line comment */
    QVector<double>    *DataY;     /*!< Add in-line comment */
    QVector<double>    DataHoldY;  /*!< Add in-line comment */
}sTraceCfg;

/*!
 \brief Add brief

 \class appPlot appPlot.h "appPlot.h"
*/
class appPlot
{
public:
    /*!
     \brief Constructor

     \param qwtPlot Add param
    */
    appPlot(QwtPlot *qwtPlot);
    /*!
     \brief Destructor

    */
    ~appPlot();

    // Public Title Function Decleration
    /*!
     \brief Add brief

     \param strTitle Add param
    */
    void SetTitle(QString *strTitle);
    /*!
     \brief Add brief

     \param strTitle Add param
    */
    void GetTitle(QString *strTitle);

    // Public Spectrum Function Decleration
    /*!
     \brief Add brief

     \param SpecId Add param
     \param NewParameters Add param
     \param CalibrationData Add param
     \return bool
    */
    bool SetNewSpectrumParameter(int SpecId,sFrqValues *NewParameters,sCalibrationData *CalibrationData);
    /*!
     \brief Add brief

     \param NewSpectrum Add param
     \return bool
    */
    bool SetSpectrumData(sSpectrum *NewSpectrum);

    // Public Grid Function Decleration
    /*!
     \brief Add brief

    */
    void GridOff(void);
    /*!
     \brief Add brief

    */
    void GridLines(void);
    /*!
     \brief Add brief

    */
    void GridDotted(void);

    // Public Marker Function Decleration
    /*!
     \brief Add brief

     \param marker Add param
    */
    void MarkerOn(eMarker marker);
    /*!
     \brief Add brief

     \param marker Add param
    */
    void MarkerOff(eMarker marker);
    /*!
     \brief Add brief

     \param marker Add param
    */
    void MarkerSetActive(eMarker marker);
    /*!
     \brief Add brief

     \param index Add param
     \return int
    */
    int MarkerSetActivePos(int index);
    /*!
     \brief Add brief

     \return int
    */
    int MarkerGetActivePos(void);
    /*!
     \brief Add brief

     \param marker Add param
     \param Color Add param
    */
    void MarkerSetColor(eMarker marker, QColor Color);
    /*!
     \brief Add brief

     \param marker Add param
     \return QColor
    */
    QColor MarkerGetColor(eMarker marker);
    /*!
     \brief Add brief

     \param marker Add param
     \param traceNr Add param
     \return bool
    */
    bool  MarkerSetTrace(eMarker marker, eTrace traceNr);
    /*!
     \brief Add brief

     \param marker Add param
     \param index Add param
    */
    void MarkerSetPos(eMarker marker, int index);
    /*!
     \brief Add brief

     \param marker Add param
     \return int
    */
    int  MarkerGetPos(eMarker marker);
    /*!
     \brief Add brief

     \param marker Add param
     \param xData Add param
     \param yData Add param
     \return bool
    */
    bool MarkerGetPosData(eMarker marker, double &xData, double &yData);
    /*!
     \brief Add brief

     \param marker Add param
    */
    void MarkerUpdate(eMarker marker);
    /*!
     \brief Add brief

    */
    void MarkerUpdatePos(void);
    /*!
     \brief Add brief

     \param mInfo Add param
    */
    void MarkerGetInfo(sMarkerInfo *mInfo);

    // Public Trace Function Decleration
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceOn(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
     \return bool
    */
    bool TraceIsOn(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceOff(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
     \param newColor Add param
    */
    void TraceSetTextColor(eTrace traceNr, QColor newColor);
    /*!
     \brief TODO Add brief

     \param traceNr TODO Add param
     \return QColor
    */
    QColor TraceGetTextColor(eTrace traceNr);
    /*!
     \brief TODO Add brief

     \param traceNr TODO Add param
     \param newColor TODO Add param
    */
    void TraceSetColor(eTrace traceNr, QColor newColor);
    /*!
     \brief Add brief

     \param traceNr Add param
     \return QColor
    */
    QColor TraceGetColor(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
     \param traceMode Add param
    */
    void TraceSetMode(eTrace traceNr, eTraceMode traceMode);
    /*!
     \brief Add brief

     \param traceNr Add param
     \return eTraceMode
    */
    eTraceMode TraceGetMode(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
     \param DataFrq Add param
     \param DatadBm Add param
     \return bool
    */
    bool TraceGetData(eTrace traceNr, QVector<double> *DataFrq, QVector<double> *DatadBm);
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceHoldTrigger(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceHoldReset(eTrace traceNr);
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceClear(eTrace traceNr);

    // Public Tools Function Decleration
    /*!
     \brief Add brief

     \param invert TODO Add param
    */
    void Export(bool invert);
    /*!
     \brief Add brief

     \param invert TODO Add param
    */
    void Print(bool invert);

private:
    QwtPlot     *qwtCtrl;          /*!< Add in-line comment */
    appGrid     *gridCtrl;         /*!< Add in-line comment */

    // Marker
    appMarker   *markerActiveCtrl; /*!< Add in-line comment */
    appMarker   *markerCtrl[3];    /*!< Add in-line comment */
    sMarkerCfg  *markerActiveCfg;  /*!< Add in-line comment */
    sMarkerCfg   markerCfg[3];     /*!< Add in-line comment */

    // Trace
    appCurve    *traceCtrl[4]; /*!< Add in-line comment */
    sTraceCfg    traceCfg[4];  /*!< Add in-line comment */
    /*!
     \brief Add brief

    */
    void initTrace(void);
    /*!
     \brief Add brief

     \param traceNr Add param
    */
    void TraceDataUpdate(eTrace traceNr);

    // Spectrum
    int          ActiveSpecId; /*!< Add in-line comment */

    // Data
    bool            flagDataFirstSpectrum; /*!< Add in-line comment */
    QVector<double> DataX;                 /*!< Add in-line comment */
    QVector<double> DataClrWrite;          /*!< Add in-line comment */
    QVector<double> DataAvarage;           /*!< Add in-line comment */
    QVector<double> DataMaxHold;           /*!< Add in-line comment */
    QVector<double> DataOffset;            /*!< Add in-line comment */
    /*!
     \brief Add brief

    */
    void initData(void);
    /*!
     \brief Add brief

     \param Data Add param
     \param newSize Add param
     \param fillValue Add param
    */
    void DataReset(QVector<double> *Data,int newSize, double fillValue);
    /*!
     \brief Add brief

     \param Data Add param
    */
    void DataCalcClrWrite(QVector<double> *Data);
    /*!
     \brief Add brief

     \param Data Add param
    */
    void DataCalcMaxHold(QVector<double> *Data);
    /*!
     \brief Add brief

     \param Data Add param
    */
    void DataCalcAvarage(QVector<double> *Data);
    /*!
     \brief Add brief

     \param Data Add param
    */
    void DataCalcPeak(QVector<double> *Data);

    // Private Axis Function Decleration
    /*!
     \brief Add brief

     \param FrqStart Add param
     \param FrqStop Add param
    */
    void SetFrequencyRange(double FrqStart, double FrqStop);
    /*!
     \brief Add brief

     \param dBmMax Add param
     \param dBmMin Add param
    */
    void SetAmplitudeRange(double dBmMax, double dBmMin);

    // Private Tools Function Defintion
    /*!
     \brief TODO Add brief

    */
    void InvertPlot(void);
    /*!
     \brief TODO Add brief

    */
    QColor invertColor(QColor color);

    // Private Calibration Function Decleration
    /*!
     \brief Add brief

     \param FrqValues Add param
     \param CalData Add param
     \param Data Add param
     \return bool
    */
    bool calcCalDataOffset(sFrqValues *FrqValues , sCalibrationData *CalData, QVector<double> &Data);
    /*!
     \brief Add brief

     \param FrqValue Add param
     \param Coeffs Add param
     \param dBm Add param
     \return bool
    */
    bool calcCoeff(double FrqValue,double *Coeffs, double &dBm);

    /*!
     \brief Add brief

    */
    void Init(void);
};
