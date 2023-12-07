// SPDX-FileCopyrightText: 2010 Adam Rakowski <foo-script@o2.pl>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KHMTHEMEFACTORY_H
#define KHMTHEMEFACTORY_H

#include <QRect>
#include <QColor>
#include <QList>
#include <QDir>
#include <QDomElement>

#include "khmtheme.h"

class KHMTheme;

class KHMThemeFactory
{
public:
    bool addTheme(const QString &filePath);    //returns "true" if theme has been added successfully, "false" otherwise
    void walkDirectory(const QDir &dir);   //walks the directory loads valid themes files. No recursion

    int getQty() const; //returns quantity of list
    QStringList getNames() const; //returns short names(worknames) of all the themes
    QStringList themeList() const;    //returns user interface names of all the themes
    const KHMTheme * getTheme(int id) const;  //Returns theme at "index". An "index" must exists

private:
    QList<KHMTheme> themesList;

    static QRect makeRect(const QDomElement &element, const QString &propertyName);
    static QColor getColor(const QDomElement &element, const QString &propertyName);
    static bool checkTheme(const QDomElement &root, const QString &themeVersion);
    void doTheme(const QDomElement &theme, const QString &version);
};

#endif // KHMTHEMEFACTORY_H
