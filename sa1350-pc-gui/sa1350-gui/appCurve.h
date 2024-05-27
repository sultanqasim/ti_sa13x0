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
/*! \file appCurve.h */
#pragma once

#include "qwt_plot_curve.h"

/*!
 \brief Add brief

 \class appCurve appCurve.h "appCurve.h"
*/
class appCurve
{
public:
    /*!
     \brief Constructor

     \param qwtPlot Add param
     \param Label Add param
     \param Color Add param
    */
    appCurve(QwtPlot *qwtPlot, QString Label, QColor Color);
    /*!
     \brief Destructor

    */
    ~appCurve();

    /*!
     \brief Add brief

    */
    void   On(void);
    /*!
     \brief Add brief

    */
    bool   IsOn(void);
    /*!
     \brief Add brief

    */
    void   Off(void);
    /*!
     \brief Add brief

     \param Color Add param
    */
    void   SetColor(QColor Color);
    /*!
     \brief Add brief

    */
    QColor GetColor(void);
    /*!
     \brief Add brief

     \param DataX Add param
     \param DataY Add param
    */
    void   SetData(QVector<double> *DataX, QVector<double> *DataY);

private:
    bool            flagVisible; /*!< Add in-line comment */
    bool            flagUpdate;  /*!< Add in-line comment */

    QwtPlot          *qwtCtrl;        /*!< Add in-line comment */
    QFont            *curveLabelFont; /*!< Add in-line comment */
    QwtText          *curveLabel;     /*!< Add in-line comment */
    QwtPlotCurve     *curveCtrl;      /*!< Add in-line comment */
    QColor           *curveColor;     /*!< Add in-line comment */
    QPen             *curvePen;       /*!< Add in-line comment */

    QVector<double>  *curveDataX; /*!< Add in-line comment */
    QVector<double>  *curveDataY; /*!< Add in-line comment */
};
