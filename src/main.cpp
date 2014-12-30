/*************************************************************************************
 * Copyright (C) 2001-2009 Anne-Marie Mahfouf <annma@kde.org>                        *
 * Copyright (C) 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>                  *
 *                                                                                   *
 * This program is free software; you can redistribute it and/or modify              *
 * it under the terms of the GNU General Public License as published by              *
 * the Free Software Foundation; either version 2 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * This program is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU General Public License for more details.                                      *
 *                                                                                   *
 * You should have received a copy of the GNU General Public License                 *
 * along with this program; if not, write to the Free Software                       *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.   *
 *************************************************************************************/


#include "khangman.h"
#include "version.h"

#include <KAboutData>

#include <KLocalizedString>

#include <QApplication>
#include <QFontDatabase>
#include <QFontInfo>
#include <QStandardPaths>
#include <QQmlEngine>

static const char description[] =
        I18N_NOOP("Classical hangman game for KDE");

int main(int argc, char **argv)
{
    QApplication::setApplicationName("khangman");
    QApplication::setApplicationVersion(KHM_VERSION);
    QApplication::setOrganizationDomain("kde.org");
    QApplication::setApplicationDisplayName(i18n("KHangMan"));
    QApplication::setWindowIcon(QIcon::fromTheme("khangman"));

    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    KAboutData aboutData(I18N_NOOP("khangman"),
                         i18n("KHangMan"),
                         I18N_NOOP(KHM_VERSION),
                         i18n(description),
                         KAboutLicense::GPL,
                         i18n("(c) 2001-2011, Anne-Marie Mahfouf"));
    aboutData.addAuthor(i18n("Primoz Anzur"), i18n("Previous maintainer"),
            "zerokode@gmx.net");

    // Write the names with accents and all and this file save with
    // utf8 encoding.
    aboutData.addAuthor(i18n("Anne-Marie Mahfouf"), i18n("Original Author"),
                        "annma@kde.org", "http://annma.blogspot.com", "annma");
    aboutData.addAuthor(i18n("Rahul Chowdhury"), i18n("Developer"),
                        "rahul.chowdhury@kdemail.net", "http://rahulc93.wordpress.com/", "rahulch");
    aboutData.addCredit(i18n("Stefan Böhmann"),
                        i18n("Coding help"), "kde@hilefoks.org", "http://www.hilefoks.org", "hilefoks");
    aboutData.addCredit(i18n("Stefan Asserhäll"),
                        i18n("Swedish data files, coding help, transparent pictures and i18n fixes"),
                                "stefan.asserhall@telia.com");
    aboutData.addCredit(i18n("eXParTaKus"),
                        i18n("Spanish data files"), "expartakus@expartakus.com");
    aboutData.addCredit(i18n("Erik Kjær Pedersenn"),
                        i18n("Danish data files"), "erik@mpim-bonn.mpg.de" );
    aboutData.addCredit(i18n("Niko Lewman"),
                        i18n("Finnish data files"), "niko.lewman@edu.hel.fi" );
    aboutData.addCredit(i18n("João Sebastião de Oliveira Bueno"),
                        i18n("Brazilian Portuguese data files"), "gwidion@mpc.com.br" );
    aboutData.addCredit(i18n("Antoni Bella"),
                        i18n("Catalan data files"), "bella5@teleline.es");
    aboutData.addCredit(i18n("Giovanni Venturi"),
                        i18n("Italian data files"), "jumpyj@tiscali.it");
    aboutData.addCredit(i18n("Rinse"),
                        i18n("Dutch data files"), "rinse@kde.nl");
    aboutData.addCredit(i18n("Lukáš Tinkl"),
                        i18n("Czech data files"), "lukas@kde.org");
    aboutData.addCredit(i18n("Kristóf Kiszel"),
                        i18n("Hungarian data files"), "ulysses@kubuntu.org");
    aboutData.addCredit(i18n("Torger Åge Sinnes"),
                        i18n("Norwegian (Bokmål) data files"), "org-a-s@online.no");
    aboutData.addCredit(i18n("Roger Kovacs"),
                        i18n("Tajik data files"), "rkovacs@khujand.org");
    aboutData.addCredit(i18n("Chusslove Illich"),
                        i18n("Serbian (Cyrillic and Latin) data files"), "chaslav@sezampro.yu");
    aboutData.addCredit(i18n("Jure Repinc"),
                        i18n("Slovenian data files"), "jlp@holodeck1.com");
    aboutData.addCredit(i18n("Pedro Morais"),
                        i18n("Portuguese data files"), "Morais@kde.org");
    aboutData.addCredit(i18n("Gaute Hvoslef Kvalnes"),
                        i18n("Norwegian (Nynorsk) data files"), "gaute@verdsveven.com");
    aboutData.addCredit(i18n("Mehmet Özel"),
                        i18n("Turkish data files"), "mehmet_ozel2003@hotmail.com");
    aboutData.addCredit(i18n("Черепанов Андрей"),
                        i18n("Russian data files"), "sibskull@mail.ru");
    aboutData.addCredit(i18n("Radostin Radnev"),
                        i18n("Bulgarian data files"), "radnev@yahoo.com");
    aboutData.addCredit(i18n("Kevin Patrick Scannell"),
                        i18n("Irish (Gaelic) data files"), "scannell@slu.edu");
    aboutData.addCredit(i18n("Matt Howe"),
                        i18n("Softer Hangman Pictures"), "mdhowe@bigfoot.com");
    aboutData.addCredit(i18n("Benjamin Meyer"),
                        i18n("Coding help"), "ben@meyerhome.net");
    aboutData.addCredit(i18n("Robert Gogolok"),
                        i18n("Coding help"), "robertgogolok@gmx.de");
    aboutData.addCredit(i18n("Lubos Lunàk"),
                        i18n("Coding help"), "l.lunak@kde.org");
    aboutData.addCredit(i18n("Albert Astals Cid"),
                        i18n("Coding help, fixed a lot of things"), "aacid@kde.org");
    aboutData.addCredit(i18n("Peter Hedlund"),
                        i18n("Code for generating icons for the characters toolbar"), "peter.hedlund@kdemail.net");
    aboutData.addCredit(i18n("Inge Wallin"),
                        i18n("Code cleaning"), "inge@lysator.liu.se");

    KAboutData::setApplicationData(aboutData);

    QFont f("Domestic Manners", 12, QFont::Normal, true);
    if (!QFontInfo(f).exactMatch())
    {
        QFontDatabase::addApplicationFont(QStandardPaths::locate(QStandardPaths::DataLocation, "fonts/Domestic_Manners.ttf"));
    }

    QFont fd("Dustismo Roman", 12, QFont::Normal, true);
    if (!QFontInfo(fd).exactMatch())
    {
        QFontDatabase::addApplicationFont(QStandardPaths::locate(QStandardPaths::DataLocation, "fonts/Dustismo_Roman.ttf"));
    }

    KHangMan hangman;
    QObject::connect(hangman.getEngine(), SIGNAL(quit()), &app, SLOT(quit()));
    hangman.show();
    return app.exec();
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

