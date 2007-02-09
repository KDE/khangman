/*
   Copyright (C) 2007      Pino Toscano <pino@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

//project headers
#include "langutils.h"

bool LangUtils::hasSpecialChars(const QString& lang)
{
    if (lang == QLatin1String("en")
        || lang == QLatin1String("it")
        || lang == QLatin1String("nl")
        || lang == QLatin1String("ru")
        || lang == QLatin1String("bg"))
        return true;
    return false;
}

bool LangUtils::hasAccentedLetters(const QString& lang)
{
    if (lang == QLatin1String("es")
        || lang == QLatin1String("ca")
        || lang == QLatin1String("pt")
        || lang == QLatin1String("pt_BR"))
        return true;
    return false;
}

QFont LangUtils::fontForLanguage(const QString& lang)
{
    QFont f;
    if (lang == QLatin1String("tg"))
        f.setFamily( "URW Bookman" );
    else
        f.setFamily( "Sans Serif" );
    return f;
}

QString LangUtils::capitalize(const QString& str, const QString& lang, bool uppercase)
{
    // If German, make upper case, otherwise make lower case.
    QString ret;
    if (uppercase && lang == QLatin1String("de"))
        ret = str.toUpper();
    else
        ret = str.toLower();
    return ret;
}


