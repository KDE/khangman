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

#include "khmthemefactory.h"
#include <QDebug>

KHMThemeFactory::KHMThemeFactory()
{
}

bool KHMThemeFactory::addTheme(const QString &themeFile)
{
    QFile file (themeFile);
    QDomDocument tree(QStringLiteral("KHangManTheme")); //do we need it?
    QDomElement root;
    QDomNode themeNode;
    QDomElement themeElement;

    if (!file.exists()) {
        qDebug() << "Themes file doesn't exist";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"Can't open themes file";
        return false;
    }

    if (!tree.setContent(&file)) {
        file.close();
        qDebug()<<"Can't set content for theme file";
        return false;
    }

    file.close();
    root=tree.documentElement();

    if (!checkTheme(root, QStringLiteral("1"))) {
        qDebug()<<"Incompatible version of theme loaded";
        return false;
    }

    QString themeVersion=root.attribute(QStringLiteral("version"));
    themeNode=root.firstChild();

    do {
        doTheme(themeNode.toElement(), themeVersion);
        themeNode=themeNode.nextSibling();
    } while (!themeNode.isNull());
    return true;
}

void KHMThemeFactory::walkDirectory(const QDir &dir)       //unused! (but works)
{
    QFileInfoList xmlFilesList;
    QStringList allowedExtenstion(QStringLiteral("*.xml"));

    if (dir.exists()) {
        xmlFilesList=dir.entryInfoList(allowedExtenstion, QDir::Files);

        for (QFileInfoList::iterator i=xmlFilesList.begin(); i!=xmlFilesList.end(); ++i) {
            addTheme(i->absoluteFilePath());
        }
    }
}

int KHMThemeFactory::getQty() const
{
    return themesList.size();
}

QStringList KHMThemeFactory::getNames()
{
    QStringList list;
    for (int i=0; i<themesList.size(); i++) {
        list.append(themesList[i].name());
    }

    return list;
}

QStringList KHMThemeFactory::themeList()
{
    QStringList list;
    for (int i=0; i<themesList.size(); i++) {
        list.append(themesList[i].uiName());
    }

    return list;
}

KHMTheme * KHMThemeFactory::buildTheme (int id)
{
    if (id >= 0 && id < themesList.size())
        return new KHMTheme(themesList[id]);
    else
        return nullptr;
}

QRect KHMThemeFactory::makeRect(const QDomElement &element, const QString &propertyName)
{
    QDomElement rect=element.firstChildElement(propertyName);

    return QRect(
        (int)(rect.attribute(QStringLiteral("xratio")).toDouble()*10000),
        (int)(rect.attribute(QStringLiteral("yratio")).toDouble()*10000),  //*10000 cause ratios are float
        (int)(rect.attribute(QStringLiteral("wratio")).toDouble()*10000),
        (int)(rect.attribute(QStringLiteral("hratio")).toDouble()*10000)  //QPoint,QRect constructors expect ints
    );
}

QColor KHMThemeFactory::getColor(const QDomElement &element, const QString &propertyName)
{
    QDomElement color=element.firstChildElement(propertyName);

    return QColor ( color.attribute(QStringLiteral("r")).toInt(), color.attribute(QStringLiteral("g")).toInt(), color.attribute(QStringLiteral("b")).toInt());
}

bool KHMThemeFactory::checkTheme(const QDomElement &root, const QString &themeVersion)
{
    QString rootName=root.tagName();

    return (rootName.compare(QLatin1String("KHangManThemes"))==0)	&&	(themeVersion.compare(root.attribute(QStringLiteral("version")))==0);
}

void KHMThemeFactory::doTheme(const QDomElement &theme, const QString &version)   //fetch all the properties from .xml and stick it together
//"theme" means <theme></theme> section
{
    QDomElement coords=theme.firstChildElement(QStringLiteral("coords"));

    //Names of elements are camelCase, but
    //attributes are always lowercase

    QString name=theme.attribute(QStringLiteral("name"));
    QString uiName=theme.attribute(QStringLiteral("uiname"));
    QString svgFileName=theme.attribute(QStringLiteral("svgfilename"));
    QString author=theme.attribute(QStringLiteral("author"));
    QString themeVersion=version;

    QColor letterColor=getColor(theme.firstChildElement(QStringLiteral("colors")), QStringLiteral("letterColor"));
    QColor guessButtonTextColor=getColor(theme.firstChildElement(QStringLiteral("colors")), QStringLiteral("guessButtonTextColor"));
    QColor guessButtonColor=getColor(theme.firstChildElement(QStringLiteral("colors")), QStringLiteral("guessButtonColor"));
    QColor guessButtonHoverColor=getColor(theme.firstChildElement(QStringLiteral("colors")), QStringLiteral("guessButtonHoverColor"));
    QColor letterInputTextColor=getColor(theme.firstChildElement(QStringLiteral("colors")), QStringLiteral("letterInputTextColor"));

    QRect wordRect=makeRect(theme.firstChildElement(QStringLiteral("coords")), QStringLiteral("wordRect"));
    QRect hintRect=makeRect(theme.firstChildElement(QStringLiteral("coords")), QStringLiteral("hintRect"));
    QRect kRect=makeRect(theme.firstChildElement(QStringLiteral("coords")), QStringLiteral("kRect"));

    QDomElement wordPos = theme.firstChildElement(QStringLiteral("coords")).firstChildElement(QStringLiteral("goodWordPos"));
    QPoint goodWordPos = QPoint(	wordPos.attribute(QStringLiteral("wratio")).toDouble()*10000, wordPos.attribute(QStringLiteral("hratio")).toDouble()*10000	);

    KHMTheme * newTheme = new KHMTheme (name, uiName, svgFileName, author, themeVersion, wordRect, hintRect, kRect, letterColor, guessButtonTextColor, guessButtonColor, guessButtonHoverColor, letterInputTextColor, goodWordPos);
    themesList.append(*newTheme);
}

KHMThemeFactory::~KHMThemeFactory()
{
    themesList.clear();
}
