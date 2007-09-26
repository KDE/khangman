/*
   Copyright 2007      Pino Toscano <pino@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
*/

//project headers
#include "langutils.h"

bool LangUtils::hasSpecialChars(const QString& lang)
{
    if (lang == QLatin1String("en")
            || lang == QLatin1String("en_GB")
            || lang == QLatin1String("it")
            || lang == QLatin1String("nl")
            || lang == QLatin1String("ru")
            || lang == QLatin1String("bg"))
    {
        return true;
    }
    return false;
}

bool LangUtils::hasAccentedLetters(const QString& lang)
{
    if (lang == QLatin1String("es")
            || lang == QLatin1String("ca")
            || lang == QLatin1String("pt")
            || lang == QLatin1String("pt_BR"))
    {
        return true;
    }
    return false;
}

QFont LangUtils::fontForLanguage(const QString& lang)
{
    QFont f;
    if (lang == QLatin1String("tg")) {
        f.setFamily( "URW Bookman" );
    }
    else {
        f.setFamily( "Sans Serif" );
    }
    return f;
}

QString LangUtils::capitalize(const QString& str, const QString& lang)
{
    // If German, make upper case, otherwise make lower case.
    QString ret;
    if (lang == QLatin1String("de")) {
        ret = str.toUpper();
    }
    else {
        ret = str.toLower();
    }
    return ret;
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

