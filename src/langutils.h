/*
   Copyright 2007      Pino Toscano <pino@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
*/

#ifndef LANGUTILS_H
#define LANGUTILS_H

#include <QFont>

class LangUtils
{
public:
    static bool hasSpecialChars(const QString& lang);
    static bool hasAccentedLetters(const QString& lang);
    static QFont fontForLanguage(const QString& lang);
    static QString capitalize(const QString& str, const QString& lang);
};

#endif

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

