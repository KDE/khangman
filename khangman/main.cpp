/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Jul 19 16:42:53 EDT 2001
    copyright            : (C) 2001 by Anne-Marie Mahfouf
    email                : a-m.mahfouf@lineone.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "khsplash.h"

static const char *description =
    I18N_NOOP("Hangman Game");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

static const char *version = "0.71";


static KCmdLineOptions options[] =
    {
        { 0, 0, 0 }
        // INSERT YOUR COMMANDLINE OPTIONS HERE
    };

int main(int argc, char *argv[])
{

	KAboutData aboutData( "khangman", I18N_NOOP("KHangMan"),
	                      version, description, KAboutData::License_GPL,
	                      "(c) 2001, Anne-Marie Mahfouf", 0, "http://edu.kde.org/khangman");
	aboutData.addAuthor("Anne-Marie Mahfouf", I18N_NOOP("Maintainer, Author"), "annma@kde.org");
	aboutData.addCredit("Renaud Blanchard",
	                    I18N_NOOP("Graphics"), "kisukuma@chez.com");
    aboutData.addCredit("Primoz Primoz Anzur",
	                    I18N_NOOP("Splash screen"), "zerokode@yahoo.com");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication a;
	KhSplash *splash=new KhSplash();
	splash->show();
	return a.exec();
}
