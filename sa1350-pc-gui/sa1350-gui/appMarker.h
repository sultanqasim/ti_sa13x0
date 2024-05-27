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
/*! \file appMarker.h */
#pragma once

#include "qwt_plot_marker.h"

/*!
 \brief Add brief

 \class appMarker appMarker.h "appMarker.h"
*/
class appMarker
{
public:
    /*!
     \brief Constructor

     \param qwtPlot
     \param mNr
     \param markerColor
     \param txtOffset
    */
    appMarker(QwtPlot *qwtPlot,int mNr,const QColor markerColor,double txtOffset);
    /*!
     \brief Destructor

    */
    ~appMarker();

    /*!
     \brief Add brief

    */
    void   On(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool   IsOn(void);
    /*!
     \brief Add brief

    */
    void   Off(void);
    /*!
     \brief Add brief

    */
    void   TextOn(void);
    /*!
     \brief Add brief

    */
    void   TextOff(void);
    /*!
     \brief Add brief

     \param Color
    */
    void   TextColorSet(QColor Color);
    /*!
     \brief TODO Add brief

     \return QColor
    */
    QColor   TextColorGet(void);
    /*!
     \brief TODO Add brief

     \param Color
    */
    void   ColorSet(QColor Color);
    /*!
     \brief Add brief

     \return QColor
    */
    QColor ColorGet(void);
    /*!
     \brief Add brief

     \param ValueMHz
     \param ValuedBm
    */
    void   Move(double ValueMHz, double ValuedBm);

private:
    bool        flagOn;        /*!< Add in-line comment */
    bool        flagTextOn;    /*!< Add in-line comment */
    int         markerNr;      /*!< Add in-line comment */
    double      labelOffset;   /*!< Add in-line comment */
    QColor      textColor;     /*!< Add in-line comment */
    QColor      markerColor;   /*!< Add in-line comment */
    QwtSymbol   *markerSymbol; /*!< Add in-line comment */
    QwtPlot     *qwtCtl;       /*!< Add in-line comment */
    QwtText     *markerText;   /*!< Add in-line comment */

    QwtPlotMarker *markerCtrl; /*!< Add in-line comment */
    QwtPlotMarker *symbolCtrl; /*!< Add in-line comment */

    /*!
     \brief Add brief

    */
    void Init(void);
};
