/*
 * Copyright (C) 2001-2005 Anne-Marie Mahfouf <annma@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "khangman.h"
#include "version.h"

#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

static const char description[] =
    I18N_NOOP("Classical hangman game for KDE");

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData aboutData( "khangman", I18N_NOOP("KHangMan"),
                              KHM_VERSION, description, KAboutData::License_GPL,
                              "(c) 2001-2003, Anne-Marie Mahfouf", 0, "http://edu.kde.org/khangman");
    aboutData.addAuthor("Primoz Anzur", I18N_NOOP("Previous maintainer"), "zerokode@gmx.net");
    //write the names with accents and all and this file save with utf8 encoding
    aboutData.addAuthor("Anne-Marie Mahfouf", I18N_NOOP("Actual maintainer, author"), "annma@kde.org");
    aboutData.addCredit("Renaud Blanchard",
                            I18N_NOOP("Graphics"), "kisukuma@chez.com");
    aboutData.addCredit("Stefan Asserhäll",
                            I18N_NOOP("Swedish data files, coding help, transparent pictures and i18n fixes"), "stefan.asserhall@telia.com");
    aboutData.addCredit("Joe Bolin",
                            I18N_NOOP("Nature theme background"), "jbolin@zoominternet.net");
    aboutData.addCredit("Primoz Anzur",
                            I18N_NOOP("Blue theme background, icons"), "zerokode@gmx.net");
    aboutData.addCredit("eXParTaKus",
                            I18N_NOOP("Spanish data files"), "expartakus@expartakus.com");
    aboutData.addCredit("Erik Kjær Pedersenn",
                            I18N_NOOP("Danish data files"), "erik@mpim-bonn.mpg.de" );
    aboutData.addCredit("Niko Lewman",
                            I18N_NOOP("Finnish data files"), "niko.lewman@edu.hel.fi" );
    aboutData.addCredit("João Sebastião de Oliveira Bueno",
                            I18N_NOOP("Brazilian Portuguese data files"), "gwidion@mpc.com.br" );
    aboutData.addCredit("Antoni Bella",
                            I18N_NOOP("Catalan data files"), "bella5@teleline.es");
    aboutData.addCredit("Giovanni Venturi",
                            I18N_NOOP("Italian data files"), "jumpyj@tiscali.it");
    aboutData.addCredit("Rinse",
                            I18N_NOOP("Dutch data files"), "rinse@kde.nl");
    aboutData.addCredit("Lukáš Tinkl",
                            I18N_NOOP("Czech data files"), "lukas@kde.org");
    aboutData.addCredit("Tamas Szanto",
                            I18N_NOOP("Hungarian data files"), "tszanto@mol.hu");
    aboutData.addCredit("Torger Åge Sinnes",
                            I18N_NOOP("Norwegian (Bokmål) data files"), "org-a-s@online.no");
    aboutData.addCredit("Roger Kovacs",
                            I18N_NOOP("Tajik data files"), "rkovacs@khujand.org");
    aboutData.addCredit("Chusslove Illich",
                            I18N_NOOP("Serbian (Cyrillic and Latin) data files"), "chaslav@sezampro.yu");
    aboutData.addCredit("Jure Repinc",
                            I18N_NOOP("Slovenian data files"), "jlp@holodeck1.com");
    aboutData.addCredit("Pedro Morais",
                            I18N_NOOP("Portuguese data files"), "Morais@kde.org");
    aboutData.addCredit("Gaute Hvoslef Kvalnes",
                            I18N_NOOP("Norwegian (Nynorsk) data files"), "gaute@verdsveven.com");
    aboutData.addCredit("Mehmet Özel",
                            I18N_NOOP("Turkish data files"), "mehmet_ozel2003@hotmail.com");
     aboutData.addCredit("Черепанов Андрей",
                            I18N_NOOP("Russian data files"), "sibskull@mail.ru"); 
     aboutData.addCredit("Radostin Radnev",
			 I18N_NOOP("Bulgarian data files"), "radnev@yahoo.com");
     aboutData.addCredit("Kevin Patrick Scannell",
			 I18N_NOOP("Irish (Gaelic) data files"), "scannell@slu.edu");
     aboutData.addCredit("Matt Howe",
                            I18N_NOOP("Softer Hangman Pictures"), "mdhowe@bigfoot.com");
    aboutData.addCredit("Benjamin Meyer",
                            I18N_NOOP("Coding help"), "ben@meyerhome.net");
    aboutData.addCredit("Robert Gogolok",
                            I18N_NOOP("Coding help"), "robertgogolok@gmx.de");
    aboutData.addCredit("Lubos Lunàk",
                            I18N_NOOP("Coding help"), "l.lunak@kde.org");
    aboutData.addCredit("Albert Astals Cid",
                            I18N_NOOP("Coding help"), "tsdgeos@terra.es");
   aboutData.addCredit("Danny Allen",
                             I18N_NOOP("svg icon"), "danny@dannyallen.co.uk");
   aboutData.addCredit("Peter Hedlund",
                             I18N_NOOP("Code for generating icons for the characters toolbar"), "peter@peterandlinda.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    // register ourselves as a dcop client
    app.dcopClient()->registerAs(app.name(), false);

    // see if we are starting with session management
    if (app.isRestored())
        RESTORE(KHangMan)
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            KHangMan *widget = new KHangMan;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++)
            {
                KHangMan *widget = new KHangMan;
                widget->show();
            }
        }
        args->clear();
    }

    return app.exec();
}
