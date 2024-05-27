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
/*! \file main.cpp
 \brief Defines the entry point for the GUI application.
*/
#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QTimer>

/*!
 \brief Add brief

 \param argc
 \param argv
 \return int
*/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setFont(QFont("Arial,Helvetica Neue",8));

    MainWindow mainWindow;

    // Display the splash screen, scaled to 35% of the default screen width
    QRect screenGeometry = QApplication::desktop()->availableGeometry(0);
    int splashScreenWidth = (screenGeometry.width() * 35) / 100;
    QPixmap splashScreenImage = QPixmap(":/img/TiSplash").scaledToWidth(splashScreenWidth, Qt::SmoothTransformation);
    QSplashScreen *splash = new QSplashScreen(&mainWindow,splashScreenImage,Qt::WindowStaysOnTopHint);

    QTimer::singleShot(2000, splash, SLOT(close()));
    QTimer::singleShot(100, &mainWindow, SLOT(show()));
    splash->show();

    return app.exec();
}
