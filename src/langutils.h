// SPDX-FileCopyrightText: 2007 Pino Toscano <pino@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef LANGUTILS_H
#define LANGUTILS_H

#include <QFont>

class LangUtils
{
public:
    static bool hasSpecialChars(const QString& lang);
    static QFont fontForLanguage(const QString& lang);
    static QString capitalize(const QString& str, const QString& lang);
};

#endif

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

