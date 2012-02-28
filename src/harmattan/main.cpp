/******************************************************************************
 * This file is part of the KHangMan project
 * Copyright (c) 2012 Laszlo Papp <lpapp@kde.org>
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

#include "khangmanenginehelper.h"

#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <KDE/KComponentData>
#include <kdeclarative.h>

#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QtGui/QApplication>

#include <QtCore/QUrl>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    app.setOrganizationName( "KDE KHangMan" );
    app.setApplicationName( "KHangMan Harmattan Frontend" );

    QDeclarativeView view;
    QDeclarativeContext *ctxt = view.rootContext();

    KGlobal::dirs()->addResourceDir("data", "/opt/khangman/share/");

    KHangMan *khangmanEngine = new KHangManEngine();
    ctxt->setContextProperty("khangmanEngine", khangmanEngine);

    KHangManEngineHelper khangmanEngineHelper(khangmanEngine);
    ctxt->setContextProperty("khangmanEngineHelper", &khangmanEngineHelper);

    KComponentData data("khangman-harmattan");

    KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(view.engine());
    kdeclarative.initialize();
    // Binds things like KConfig and Icons
    kdeclarative.setupBindings();

    view.setSource(QUrl("qrc:/main.qml"));
    view.showFullScreen();

    return app.exec();
}
