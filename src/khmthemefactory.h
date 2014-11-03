/***************************************************************************
 *   Copyright (C) 2010 Adam Rakowski <foo-script@o2.pl>  (GSoC 2010)      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef KHMTHEMEFACTORY_H
#define KHMTHEMEFACTORY_H

#include <QRect>
#include <QColor>
#include <QList>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>

#include "khmtheme.h"

class KHMTheme;

class KHMThemeFactory
{
public:
    KHMThemeFactory();
    ~KHMThemeFactory();

    bool addTheme(QString filePath);    //returns "true" if theme has been added successfuly, "false" otherwise
    void walkDirectory(QDir dir);   //walks the directory loads valid themes files. No recursion

    int getQty() const; //returns quantity of list
    QStringList getNames(); //returns short names(worknames) of all the themes
    QStringList themeList();    //returns user interface names of all the themes
    KHMTheme * buildTheme(int id);  //Returns theme at "index". An "index" must exists

private:
    QList<KHMTheme> themesList;

    QRect makeRect(QDomElement element, QString propertyName);
    QColor getColor(QDomElement element, QString propertyName);
    bool checkTheme(QDomElement root, QString themeVersion);
    void doTheme(QDomElement theme, QString version);
};

#endif // KHMTHEMEFACTORY_H
