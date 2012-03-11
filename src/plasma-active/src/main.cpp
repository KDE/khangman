/******************************************************************************
 * This file is part of the KHangMan project
 * Copyright 2011 Sebastian Kügler <sebas@kde.org>
 * Copyright 2011 Marco Martin <mart@kde.org>
 * Copyright 2012 Laszlo Papp <lpapp@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>

#include "mainwindow.h"

int main(int argc, char **argv)
{
    KAboutData about("kanagram-active", 0, ki18n("KHangMan Active"), "0.1.0", ki18n("KHangMan Active for Tablets"),
                     KAboutData::License_GPL, ki18n("Copyright 2012 Laszlo Papp"), KLocalizedString(), 0, "lpapp@kde.org");
    about.addAuthor( ki18n("Laszlo Papp"), KLocalizedString(), "lpapp@kde.org" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
#ifndef QT_NO_OPENGL
    options.add("opengl", ki18n("use a QGLWidget for the viewport"));
#endif
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    MainWindow mainWindow;
    mainWindow.show();

    args->clear();
    return app.exec();
}
