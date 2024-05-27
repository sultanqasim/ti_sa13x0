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
/*! \file appSettings.h */
#pragma once
#include <QSettings>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "appTypedef.h"

/*!
 \brief Add brief

 \class appSettings appSettings.h "appSettings.h"
*/
class appSettings : public QObject
{
    Q_OBJECT

public:
    /*!
     \brief Constructor

     \param parent
    */
    explicit appSettings(QObject *parent = 0);
    /*!
     \brief Destructor

    */
    ~appSettings();

    // Gui Settings Function Decleration
    /*!
     \brief Add brief

     \return bool
    */
    bool LoadGuiSettings(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool SaveGuiSettings(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool GuiSettingFileExists(void);

    // Frequency Settings Function Decleration
    /*!
     \brief Add brief

     \param FileName
     \param FrqListItem
     \return bool
    */
    bool LoadFrqSettings(QString FileName,sFrqSetting *FrqListItem);
    /*!
     \brief Add brief

     \param FrqListItem
     \return bool
    */
    bool SaveFrqSettings(sFrqSetting *FrqListItem);
    /*!
     \brief Add brief

     \return bool
    */
    bool FrqSettingFileExists(void);

signals:

public slots:

private:
    QSettings::Format XmlFormat; /*!< Add in-line comment */

    // Gui Settings
    bool                flagGuiUpdate; /*!< Add in-line comment */
    QSettings          *FileGui;       /*!< Add in-line comment */
    sSettingGui         Gui;           /*!< Add in-line comment */

    /*!
     \brief Add brief

     \return bool
    */
    bool loadGuiSettings(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool saveGuiSettings(void);

    // Frq Settings
    /*!
     \brief Add brief

     \return bool
    */
    bool loadFrqSettings(void);
    /*!
     \brief Add brief

     \return bool
    */
    bool saveFrqSettings(void);

    /*!
     \brief Add brief

     \param writer
     \param Item
     \param Value
    */
    void xmlWriteItem(QXmlStreamWriter * writer, QString Item ,QString Value);
    /*!
     \brief Add brief

     \param reader
     \param Item
     \param iValue
     \return bool
    */
    bool xmlReadInt(QXmlStreamReader *reader, QString Item ,int &iValue);
    /*!
     \brief Add brief

     \param reader
     \param Item
     \param bValue
     \return bool
    */
    bool xmlReadBool(QXmlStreamReader *reader, QString Item ,bool &bValue);
    /*!
     \brief Add brief

     \param reader
     \param Item
     \param dValue
     \return bool
    */
    bool xmlReadDouble(QXmlStreamReader *reader, QString Item ,double &dValue);
    /*!
     \brief Add brief

     \param reader
     \param Item
     \param Value
     \return bool
    */
    bool xmlReadItem(QXmlStreamReader *reader, QString Item ,QString &Value);
};
