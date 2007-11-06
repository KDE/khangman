/*
 * Copyright (C) 2001-2007 Anne-Marie Mahfouf <annma@kde.org>

     This program is free software; you can redistribute it and/or modify  
     it under the terms of the GNU General Public License as published by  
     the Free Software Foundation; either version 2 of the License, or     
     (at your option) any later version.                                   

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "khangman.h"
#include "version.h"

#include <kprocess.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <KLocale>

#include "fontchecker.h"

static const char description[] =
        I18N_NOOP("Classical hangman game for KDE");

static const char version[] = "1.7";


int main(int argc, char **argv)
{
    KAboutData aboutData( "khangman", 0, ki18n("KHangMan"),
            KHM_VERSION, ki18n(description), KAboutData::License_GPL,
            ki18n("(c) 2001-2007, Anne-Marie Mahfouf"), KLocalizedString(),
            "http://edu.kde.org/khangman");
    aboutData.addAuthor(ki18n("Primoz Anzur"), ki18n("Previous maintainer"),
            "zerokode@gmx.net");

    // Write the names with accents and all and this file save with
    // utf8 encoding.
    aboutData.addAuthor(ki18n("Anne-Marie Mahfouf"), ki18n("Current maintainer, author"), "annma@kde.org");
    aboutData.addCredit(ki18n("Stefan Böhmann"),
                        ki18n("Coding help"), "kde@hilefoks.org");
    aboutData.addCredit(ki18n("Stefan Asserhäll"),
                        ki18n("Swedish data files, coding help, transparent pictures and i18n fixes"),
                                "stefan.asserhall@telia.com");
    aboutData.addCredit(ki18n("eXParTaKus"),
                        ki18n("Spanish data files"), "expartakus@expartakus.com");
    aboutData.addCredit(ki18n("Erik Kjær Pedersenn"),
                        ki18n("Danish data files"), "erik@mpim-bonn.mpg.de" );
    aboutData.addCredit(ki18n("Niko Lewman"),
                        ki18n("Finnish data files"), "niko.lewman@edu.hel.fi" );
    aboutData.addCredit(ki18n("João Sebastião de Oliveira Bueno"),
                        ki18n("Brazilian Portuguese data files"), "gwidion@mpc.com.br" );
    aboutData.addCredit(ki18n("Antoni Bella"),
                        ki18n("Catalan data files"), "bella5@teleline.es");
    aboutData.addCredit(ki18n("Giovanni Venturi"),
                        ki18n("Italian data files"), "jumpyj@tiscali.it");
    aboutData.addCredit(ki18n("Rinse"),
                        ki18n("Dutch data files"), "rinse@kde.nl");
    aboutData.addCredit(ki18n("Lukáš Tinkl"),
                        ki18n("Czech data files"), "lukas@kde.org");
    aboutData.addCredit(ki18n("Tamas Szanto"),
                        ki18n("Hungarian data files"), "tszanto@mol.hu");
    aboutData.addCredit(ki18n("Torger Åge Sinnes"),
                        ki18n("Norwegian (Bokmål) data files"), "org-a-s@online.no");
    aboutData.addCredit(ki18n("Roger Kovacs"),
                        ki18n("Tajik data files"), "rkovacs@khujand.org");
    aboutData.addCredit(ki18n("Chusslove Illich"),
                        ki18n("Serbian (Cyrillic and Latin) data files"), "chaslav@sezampro.yu");
    aboutData.addCredit(ki18n("Jure Repinc"),
                        ki18n("Slovenian data files"), "jlp@holodeck1.com");
    aboutData.addCredit(ki18n("Pedro Morais"),
                        ki18n("Portuguese data files"), "Morais@kde.org");
    aboutData.addCredit(ki18n("Gaute Hvoslef Kvalnes"),
                        ki18n("Norwegian (Nynorsk) data files"), "gaute@verdsveven.com");
    aboutData.addCredit(ki18n("Mehmet Özel"),
                        ki18n("Turkish data files"), "mehmet_ozel2003@hotmail.com");
    aboutData.addCredit(ki18n("Черепанов Андрей"),
                        ki18n("Russian data files"), "sibskull@mail.ru");
    aboutData.addCredit(ki18n("Radostin Radnev"),
                        ki18n("Bulgarian data files"), "radnev@yahoo.com");
    aboutData.addCredit(ki18n("Kevin Patrick Scannell"),
                        ki18n("Irish (Gaelic) data files"), "scannell@slu.edu");
    aboutData.addCredit(ki18n("Matt Howe"),
                        ki18n("Softer Hangman Pictures"), "mdhowe@bigfoot.com");
    aboutData.addCredit(ki18n("Benjamin Meyer"),
                        ki18n("Coding help"), "ben@meyerhome.net");
    aboutData.addCredit(ki18n("Robert Gogolok"),
                        ki18n("Coding help"), "robertgogolok@gmx.de");
    aboutData.addCredit(ki18n("Lubos Lunàk"),
                        ki18n("Coding help"), "l.lunak@kde.org");
    aboutData.addCredit(ki18n("Albert Astals Cid"),
                        ki18n("Coding help, fixed a lot of things"), "tsdgeos@terra.es");
    aboutData.addCredit(ki18n("Peter Hedlund"),
                        ki18n("Code for generating icons for the characters toolbar"), "peter.hedlund@kdemail.net");
    aboutData.addCredit(ki18n("Inge Wallin"),
                        ki18n("Code cleaning"), "inge@lysator.liu.se");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    QFont f("Domestic Manners", 12, QFont::Normal, true);
    QFont fd("Dustismo Roman", 12, QFont::Normal, true);
    if (!fontChecker::checkInstalled(f, KStandardDirs::locate("appdata", "fonts/Domestic_Manners.ttf"))) {
        KProcess *proc = new KProcess;
        for (int i = 0; i < argc; i++) {
            *proc << argv[i];
        }
        proc->start();
    }
    else if (!fontChecker::checkInstalled(fd, KStandardDirs::locate("appdata", "fonts/Dustismo_Roman.ttf"))) {
        KProcess *proc = new KProcess;
        for (int i = 0; i < argc; i++) {
            *proc << argv[i];
        }
        proc->start();
    }
    else {
        if (app.isSessionRestored()) {
            RESTORE(KHangMan);
        }
        else {
            app.setTopWidget(new KHangMan());
            return app.exec();
        }
    }
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

