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
#include "appSettings.h"
#include <QDir>
#include <QFile>

/*!
 \brief Add brief

 \param device
 \param map
 \return bool
*/
static bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
/*!
 \brief Add brief

 \param device
 \param map
 \return bool
*/
static bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);

appSettings::appSettings(QObject *parent) :
    QObject(parent)
{
    // Init custom QSettings File Format "XML"
    XmlFormat = QSettings::registerFormat("xml",&readXmlFile,&writeXmlFile);
    QSettings::setDefaultFormat(XmlFormat);
    QSettings::setPath(XmlFormat, QSettings::UserScope, QDir::currentPath());

    // Generate XML QSettings Objects
    // Gui
    flagGuiUpdate  = false;
    FileGui    	= new QSettings(XmlFormat, QSettings::UserScope, "SA1350Gui");

    // Frq

    // FrqList

    // Load Settings
    this->loadGuiSettings();

    if(!QFile::exists("SA1350Gui.xml"))
    {// Generate Xml File
        this->saveGuiSettings();
        FileGui->sync();
    };
    this->loadGuiSettings();

    // if(!QFile::exists("SA1350Frq.xml"))
    // {// Generate Xml File
    //  this->saveFrqSettings();
    //  FileFrq->sync();
    // };
    this->loadFrqSettings();
}

appSettings::~appSettings()
{
}

// Gui Settings Function Definition
bool appSettings::LoadGuiSettings(void)
{
    loadGuiSettings();
    return(true);
}

bool appSettings::SaveGuiSettings(void)
{
    saveGuiSettings();
    return(true);
}

bool appSettings::GuiSettingFileExists(void)
{
    return(true);
}

// Frequency Settings Function Definition
bool appSettings::LoadFrqSettings(QString FileName,sFrqSetting *FrqListItem)
{
    bool done = false;
    QString test;
    if(FrqListItem && !FileName.isEmpty())
    {
        QFile fileXml(FileName);
        if(!fileXml.open(QIODevice::ReadOnly))
            return(false);
        QXmlStreamReader reader(&fileXml);

        do
        {
            // N?chsten Token lesen
            reader.readNext();
            // Wenn Token ein Startelement
            test = reader.name().toString();
            if(reader.isStartElement() /*&& reader.name().toString().compare("FrqProfileSettings")==0*/)
            {
                if(xmlReadItem(&reader,QString("ProfileName"       ),FrqListItem->Name                  )){};
                if(xmlReadBool(&reader,QString("flagModeEasyRf"    ),FrqListItem->Values.flagModeEasyRf )){};
                if(xmlReadBool(&reader,QString("flagModeContinuous"),FrqListItem->Values.flagModeContinuous)){};
                if(xmlReadBool(&reader,QString("flagModeStartStop" ),FrqListItem->Values.flagModeStartStop)){};
                if(xmlReadInt (&reader,QString("FrqRange"          ),FrqListItem->Values.FrqRange       )){};
                if(xmlReadDouble(&reader,QString("FrqCenter"       ),FrqListItem->Values.FrqCenter      )){};
                if(xmlReadDouble(&reader,QString("FrqSpan"         ),FrqListItem->Values.FrqSpan        )){};
                if(xmlReadInt(&reader,QString("FrqSpanIndex"       ),FrqListItem->Values.FrqSpanIndex   )){};
                if(xmlReadDouble(&reader,QString("FrqStart"        ),FrqListItem->Values.FrqStart       )){};
                if(xmlReadDouble(&reader,QString("FrqStop"         ),FrqListItem->Values.FrqStop        )){};
                if(xmlReadDouble(&reader,QString("FrqStepWidth"    ),FrqListItem->Values.FrqStepWidth   )){};
                if(xmlReadInt(&reader,QString("RefDcLevelIndex"    ),FrqListItem->Values.RefDcLevelIndex)){};
                if(xmlReadDouble(&reader,QString("RBW"             ),FrqListItem->Values.RBW            )){};
                if(xmlReadInt(&reader,QString("RBWIndex"           ),FrqListItem->Values.RBWIndex       )){};
                {
                    done = true;
                };
            };
        }while(!reader.isEndElement() && !reader.hasError() && done != true);
        fileXml.close();
    };

    return(done);
}

bool appSettings::SaveFrqSettings(sFrqSetting *FrqListItem)
{
    bool done = false;

    if(FrqListItem)
    {
        if(!FrqListItem->Name.isEmpty())
        {
            QFile   fileXml(FrqListItem->Name);
            if(!fileXml.open(QIODevice::WriteOnly))
                return(false);
            QXmlStreamWriter writer(&fileXml);
            writer.setAutoFormatting(true);
            writer.writeStartDocument("1.0");
            writer.writeStartElement("FrqProfileSettings");
            xmlWriteItem(&writer,"ProfileName"      ,QString("%0").arg((QString)FrqListItem->Name));
            xmlWriteItem(&writer,"flagModeEasyRf"   ,QString("%0").arg((bool  )FrqListItem->Values.flagModeEasyRf ));
            xmlWriteItem(&writer,"flagModeContinuous",QString("%0").arg((bool  )FrqListItem->Values.flagModeContinuous));
            xmlWriteItem(&writer,"flagModeStartStop",QString("%0").arg((bool  )FrqListItem->Values.flagModeStartStop));
            xmlWriteItem(&writer,"FrqRange"         ,QString("%0").arg((int   )FrqListItem->Values.FrqRange        ));
            xmlWriteItem(&writer,"FrqCenter"        ,QString("%0").arg((double)FrqListItem->Values.FrqCenter      ));
            xmlWriteItem(&writer,"FrqSpan"          ,QString("%0").arg((double)FrqListItem->Values.FrqSpan        ));
            xmlWriteItem(&writer,"FrqSpanIndex"     ,QString("%0").arg((int   )FrqListItem->Values.FrqSpanIndex   ));
            xmlWriteItem(&writer,"FrqStart"         ,QString("%0").arg((double)FrqListItem->Values.FrqStart       ));
            xmlWriteItem(&writer,"FrqStop"          ,QString("%0").arg((double)FrqListItem->Values.FrqStop        ));
            xmlWriteItem(&writer,"FrqStepWidth"     ,QString("%0").arg((double)FrqListItem->Values.FrqStepWidth   ));
            xmlWriteItem(&writer,"RefDcLevelIndex"  ,QString("%0").arg((int   )FrqListItem->Values.RefDcLevelIndex));
            xmlWriteItem(&writer,"RBW"              ,QString("%0").arg((double)FrqListItem->Values.RBW            ));
            xmlWriteItem(&writer,"RBWIndex"         ,QString("%0").arg((int   )FrqListItem->Values.RBWIndex       ));
            writer.writeEndElement();
            writer.writeEndDocument();
            fileXml.close();
            done = true;
        };
    };
    return(done);
}

bool appSettings::FrqSettingFileExists(void)
{

    return(true);
}

// Private Function Defintion
bool appSettings::loadGuiSettings(void)
{
    int index = 0;

    // Grid
    FileGui->beginGroup("Grid");
    Gui.Grid.On   = FileGui->value("On"   ,false   ).toBool();
    Gui.Grid.Mode = static_cast<eGridMode>(FileGui->value("Mode" ,GRID_OFF).toInt());
    FileGui->endGroup();

    // Traces
    FileGui->beginGroup("Traces");
    for(index=0;index<MAX_TRACES;index++)
    {
        FileGui->beginGroup(QString("Trace%0").arg(index));
        Gui.Trace[index].On   = FileGui->value("On"   ,false    ).toBool();
        Gui.Trace[index].Nr   = static_cast<eTrace>(FileGui->value("Nr",static_cast<eTrace>(index)).toInt());
        Gui.Trace[index].Mode = static_cast<eTraceMode>(FileGui->value("Color",T_MODE_OFF).toInt());
        FileGui->endGroup();
    };
    FileGui->endGroup();

    // Markers
    FileGui->beginGroup("Markers");
    for(index=0;index<MAX_MARKERS;index++)
    {
        FileGui->beginGroup(QString("Marker%0").arg(index));
        Gui.Marker[index].On    = FileGui->value("On"   ,false ).toBool();
        Gui.Marker[index].Index = FileGui->value("Nr"   ,index ).toInt();
        Gui.Marker[index].Mode  = static_cast<eMarkerMode>(FileGui->value("Mode" ,M_MODE_UNDEFINED  ).toInt());
        Gui.Marker[index].Trace = static_cast<eTrace>(FileGui->value("Trace",M_TRACE_UNDEFINED ).toInt());
        Gui.Marker[index].Color = /*static_cast<QColor>*/FileGui->value("Color",QColor(Qt::white)).value<QColor>();
        FileGui->endGroup();
    };
    FileGui->endGroup();

    // Default Frequency Settings
    // FileGui->beginGroup("Defaults");
    //  FileGui->beginGroup("FrqSetting");
    //   Gui.DefaultFrqSetting.flagExportMode          = FileGui->value("ExportMode"     , false            ).toBool();
    //   Gui.DefaultFrqSetting.flagValueCorrectionDone = FileGui->value("ValueCorrection", false            ).toBool();
    //   Gui.DefaultFrqSetting.FrqInputMode            = static_cast<eFrqInputMode>(FileGui->value("FrqInputMode"   , FRQMODE_STARTSTOP).toInt());
    //   Gui.DefaultFrqSetting.FrqRange                = static_cast<eFrqRange>(FileGui->value("FrqRange"       , FRQRANGE_300_348 ).toInt());
    //   FileGui->beginGroup("FrqValues");
    //    Gui.DefaultFrqSetting.FrqSetValues.FrqCenter    = FileGui->value("FrqCenter"   , 324).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.FrqSpan      = FileGui->value("FrqSpan"     ,  20).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.FrqStart     = FileGui->value("FrqStart"    , 300).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.FrqStop      = FileGui->value("FrqStop"     , 348).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.RefDcLevelIndex   = FileGui->value("RefDcLevel"  ,   0).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.RBWIndex     = FileGui->value("ResBandWidth",   0).toDouble();
    //    Gui.DefaultFrqSetting.FrqSetValues.FrqStepWidth = FileGui->value("StepWidth"   ,   0).toDouble();
    //   FileGui->endGroup();
    //  FileGui->endGroup();
    // FileGui->endGroup();

    return(true);
}

bool appSettings::saveGuiSettings(void)
{
    int index = 0;
    // Grid
    FileGui->beginGroup("Grid");
    FileGui->setValue("On"  , Gui.Grid.On);
    FileGui->setValue("Mode", Gui.Grid.Mode);
    FileGui->endGroup();
    // Traces
    FileGui->beginGroup("Traces");
    for(index=0;index<MAX_TRACES;index++)
    {
        FileGui->beginGroup(QString("Trace%0").arg(index));
        FileGui->setValue("On"    , Gui.Trace[index].On);
        FileGui->setValue("Nr"    , Gui.Trace[index].Nr);
        FileGui->setValue("Mode"  , Gui.Trace[index].Mode);
        FileGui->setValue("Color" , Gui.Trace[index].Color);
        FileGui->endGroup();
    };
    FileGui->endGroup();
    // // Markers
    FileGui->beginGroup("Markers");
    for(index=0;index<MAX_MARKERS;index++)
    {
        FileGui->beginGroup(QString("Marker%0").arg(index));
        FileGui->setValue("On"   , Gui.Marker[index].On);
        FileGui->setValue("Nr"   , Gui.Marker[index].Index);
        FileGui->setValue("Mode" , Gui.Marker[index].Mode);
        FileGui->setValue("Trace", Gui.Marker[index].Trace);
        FileGui->endGroup();
    };
    FileGui->endGroup();
    // Default Frequency Settings
    // FileGui->beginGroup("Defaults");
    //  FileGui->beginGroup("FrqSetting");
    //  FileGui->setValue("ExportMode"     , Gui.DefaultFrqSetting.flagExportMode);
    //  FileGui->setValue("ValueCorrection", Gui.DefaultFrqSetting.flagValueCorrectionDone);
    //  FileGui->setValue("FrqInputMode"   , Gui.DefaultFrqSetting.FrqInputMode);
    //  FileGui->setValue("FrqRange"       , Gui.DefaultFrqSetting.FrqRange);
    //  FileGui->beginGroup("FrqValues");
    //   FileGui->setValue("FrqCenter"      , Gui.DefaultFrqSetting.FrqSetValues.FrqCenter);
    //   FileGui->setValue("FrqSpan"        , Gui.DefaultFrqSetting.FrqSetValues.FrqSpan);
    //   FileGui->setValue("FrqStart"       , Gui.DefaultFrqSetting.FrqSetValues.FrqStart);
    //   FileGui->setValue("FrqStop"        , Gui.DefaultFrqSetting.FrqSetValues.FrqStop);
    //   FileGui->setValue("RefDcLevel"     , Gui.DefaultFrqSetting.FrqSetValues.RefDcLevel);
    //   FileGui->setValue("ResBandWidth"   , Gui.DefaultFrqSetting.FrqSetValues.RBWIndex);
    //   FileGui->setValue("StepWidth"      , Gui.DefaultFrqSetting.FrqSetValues.FrqStepWidth);
    //   FileGui->endGroup();
    //  FileGui->endGroup();
    // FileGui->endGroup();

    return(true);
}

bool appSettings::loadFrqSettings(void)
{
    // int NrOfItems=0;
    // int itemIndex=0;
    // sFrqSetting tmpFrqProfile;

    // // Save Frq Setting List Items
    // FileGui->beginGroup("FrqSetList");
    //  NrOfItems = FileFrq->value("NumberOfItems", 0).toInt();
    // if(NrOfItems > 0)
    // {
    //  FileFrq->beginReadArray("Profiles");
    //  for(itemIndex=0;itemIndex<NrOfItems;itemIndex++)
    //  {
    //   //itemIndex = FileFrq->setValue("ItemIndex"     , );
    //   tmpItem.Name = FileFrq->value("Name","Unkown_").toString();
    //   FileFrq->beginGroup("FrqSetting");
    //    tmpItem.flagExportMode = FileFrq->value("ExportMode", false).toBool();
    //    tmpItem.flagValueCorrectionDone = FileFrq->value("ValueCorrection",false).toBool();
    //    tmpItem.FrqInputMode = static_cast<eFrqInputMode>(FileFrq->value("FrqInputMode",FRQMODE_STARTSTOP).toInt());
    //    tmpItem.FrqRange     = static_cast<eFrqRange>(FileFrq->value("FrqRange",FRQRANGE_300_348).toInt());
    //     FileFrq->beginGroup("FrqValues");
    //      tmpItem.FrqSetValues.FrqCenter    = FileFrq->value("FrqCenter"   ,  0).toDouble();
    //      tmpItem.FrqSetValues.FrqSpan      = FileFrq->value("FrqSpan"     ,  0).toDouble();
    //      tmpItem.FrqSetValues.FrqStart     = FileFrq->value("FrqStart"    ,300).toDouble();
    //      tmpItem.FrqSetValues.FrqStop      = FileFrq->value("FrqStop"     ,348).toDouble();
    //      tmpItem.FrqSetValues.RefDcLevel   = FileFrq->value("RefDcLevel"  ,  0).toDouble();
    //      tmpItem.FrqSetValues.RBWIndex     = FileFrq->value("ResBandWidth",  0).toDouble();
    //      tmpItem.FrqSetValues.FrqStepWidth = FileFrq->value("StepWidth"   ,  0).toDouble();
    //     FileFrq->endGroup();
    //   FileFrq->endGroup();
    //   ListFrqSettings.append(tmpItem);
    //  };
    //  FileFrq->endArray();
    // };

    return(true);
}

bool appSettings::saveFrqSettings(void)
{
    //int itemIndex=0;
    // Save Frq Setting List Items

    return(true);
}

void appSettings::xmlWriteItem(QXmlStreamWriter *writer, QString Item ,QString Value)
{
    writer->writeTextElement(Item,Value);
}

bool appSettings::xmlReadInt(QXmlStreamReader *reader, QString Item ,int &iValue)
{
    bool done = false;
    QString strValue;

    iValue = 0;

    if(xmlReadItem(reader,Item,strValue))
    {
        iValue = (int)strValue.toInt();
        done = true;
    }
    return(done);
}

bool appSettings::xmlReadBool(QXmlStreamReader *reader, QString Item ,bool &bValue)
{
    bool done = false;
    QString strValue;

    bValue = false;
    if(xmlReadItem(reader,Item,strValue))
    {
        bValue = (bool)strValue.toInt();
        done = true;
    };
    return(done);
}

bool appSettings::xmlReadDouble(QXmlStreamReader *reader, QString Item ,double &dValue)
{
    bool done = false;
    QString strValue;

    dValue = 0;
    if(xmlReadItem(reader,Item,strValue))
    {
        dValue = strValue.toDouble();
        done = true;
    };
    return(done);
}

bool appSettings::xmlReadItem(QXmlStreamReader *reader, QString Item, QString &Value)
{
    Q_UNUSED(Item)

    bool done= false;
    QString item;
    QString value;

    if(reader)
    {
        do
        {
            reader->readNext();
            if(reader->isStartElement())
            {
                item = reader->name().toString();
            }
            else if (reader->isEndElement())
            {
                if(reader->name() == item.toStdString().c_str())
                {
                    done = true;
                };
            }
            else if (reader->isCharacters() && !reader->isWhitespace())
            {
                value += reader->text().toString();
            };
        }while(!reader->atEnd() && !reader->hasError() && !done);
        Value = value;
    };

    return(done);
}

static bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QXmlStreamReader xmlReader(&device);
    QStringList elements;

    // Solange Ende nicht erreicht und kein Fehler aufgetreten ist
    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        // N?chsten Token lesen
        xmlReader.readNext();
        // Wenn Token ein Startelement
        if (xmlReader.isStartElement() && xmlReader.name() != "Settings")
        {
            // Element zur Liste hinzuf?gen
            elements.append(xmlReader.name().toString());
            // Wenn Token ein Endelement
        }
        else if (xmlReader.isEndElement())
        {
            // Letztes Element l?schen
            if(!elements.isEmpty())
                elements.removeLast();
            // Wenn Token einen Wert enth?lt
        }
        else if (xmlReader.isCharacters() && !xmlReader.isWhitespace())
        {
            QString key;
            // Elemente zu String hinzuf?gen
            for(int i = 0; i < elements.size(); i++)
            {
                if(i != 0)
                    key += "/";
                key += elements.at(i);
            };
            // Wert in Map eintragen
            map[key] = xmlReader.text().toString();
        };
    };

    // Bei Fehler Warnung ausgeben
    if (xmlReader.hasError())
    {
        //qWarning() << xmlReader.errorString();
        return(false);
    };

    return(true);
}

//static bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map)
//{
// QXmlStreamReader reader(&device);
// QString key;
// while(!reader.atEnd())
// {
//  if(reader.isStartElement() && reader.tokenString() != "Settings")
//  {
//   if(reader.text().isNull())
//    {
//     // key = Settings
//     if(key.isEmpty())
//      {
//       key = reader.tokenString();
//      }
//     else
//      {
//       key += "/" + reader.tokenString();
//      };
//    }
//   else
//    {
//     map.insert(key, reader.text().data());
//    };
//  }
// };
// return(true);
//}

static bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    QXmlStreamWriter xmlWriter(&device);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Settings");

    QStringList prev_elements;
    QSettings::SettingsMap::ConstIterator map_i;

    // Alle Elemente der Map durchlaufen
    for (map_i = map.begin(); map_i != map.end(); map_i++)
    {
        QStringList elements = map_i.key().split("/");
        int x = 0;
        // Zu schlie?ende Elemente ermitteln
        while(x < prev_elements.size() && elements.at(x) == prev_elements.at(x))
        {
            x++;
        };
        // Elemente schlie?en
        for(int i = prev_elements.size() - 1; i >= x; i--)
        {
            xmlWriter.writeEndElement();
        };
        // Elemente ?ffnen
        for (int i = x; i < elements.size(); i++)
        {
            xmlWriter.writeStartElement(elements.at(i));
        };
        // Wert eintragen
        xmlWriter.writeCharacters(map_i.value().toString());
        prev_elements = elements;
    };

    // Noch offene Elemente schlie?en
    for(int i = 0; i < prev_elements.size(); i++)
    {
        xmlWriter.writeEndElement();
    };

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    return(true);
}

//static bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map)
//{
//   QXmlStreamWriter writer(&device);
// writer.writeStartDocument("1.0");
// writer.writeStartElement("Settings");
// foreach(QString key, map.keys())
// {
//  foreach(QString elementKey, key.split("/"))
//  {
//   writer.writeStartElement(elementKey);
//  };
//  writer.writeCDATA(map.value(key).toString());
//  writer.writeEndElement();
// };
// writer.writeEndElement();
// writer.writeEndDocument();

// return(true);
//}
