/*
 * Copyright (C) 2003 Anne-Marie Mahfouf,,, <annma@kde.org>
 */

#include "khangman.h"
#include "version.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description =
    I18N_NOOP("The classical hangman game for KDE-Edu");

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    { 0, 0, 0 }
};

int main(int argc, char **argv)
{
    KAboutData about("khangman", I18N_NOOP("KHangMan"), KHM_VERSION, description,
                     KAboutData::License_GPL, "(C) 2001 Anne-Marie Mahfouf,,,", 0, 0, "annma@kde.org");
    about.addAuthor( "Anne-Marie Mahfouf,,,", 0, "annma@kde.org" );
    KCmdLineArgs::init(argc, argv, &about);
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
