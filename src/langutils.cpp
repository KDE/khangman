// SPDX-FileCopyrightText: 2007 Pino Toscano <pino@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

//project headers
#include "langutils.h"

bool LangUtils::hasSpecialChars(const QString& lang)
{
    if (lang == QLatin1String("en")
            || lang == QLatin1String("en_GB")
            || lang == QLatin1String("it")
            || lang == QLatin1String("nl")
            || lang == QLatin1String("ru")
            || lang == QLatin1String("bg")
            || lang == QLatin1String("uk")
            || lang == QLatin1String("el")
            || lang == QLatin1String("ro"))
    {
        return false;
    }
    return true;
}

QFont LangUtils::fontForLanguage(const QString& lang)
{
    QFont f;
    if (lang == QLatin1String("tg")) {
        f.setFamily( QStringLiteral("URW Bookman") );
    }
    else {
        f.setFamily( QStringLiteral("Sans Serif") );
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

