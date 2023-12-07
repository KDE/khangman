// SPDX-FileCopyrightText: 2010 Adam Rakowski <foo-script@o2.pl>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "khmthemefactory.h"
#include <QDebug>

bool KHMThemeFactory::addTheme(const QString &themeFile)
{
    QFile file (themeFile);
    if (!file.exists()) {
        qDebug() << "Themes file doesn't exist";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"Can't open themes file";
        return false;
    }

    QDomDocument tree(QStringLiteral("KHangManTheme")); //do we need it?
    if (!tree.setContent(&file)) {
        file.close();
        qDebug()<<"Can't set content for theme file";
        return false;
    }

    file.close();
    const QDomElement root=tree.documentElement();

    if (!checkTheme(root, QStringLiteral("1"))) {
        qDebug()<<"Incompatible version of theme loaded";
        return false;
    }

    const QString themeVersion=root.attribute(QStringLiteral("version"));
    for (QDomNode themeNode=root.firstChild(); !themeNode.isNull(); themeNode=themeNode.nextSibling()) {
        doTheme(themeNode.toElement(), themeVersion);
    }
    return true;
}

void KHMThemeFactory::walkDirectory(const QDir &dir)       //unused! (but works)
{
    if (dir.exists()) {
        const QStringList allowedExtenstion(QStringLiteral("*.xml"));
        const QFileInfoList xmlFilesList=dir.entryInfoList(allowedExtenstion, QDir::Files);

        for (const auto &file : xmlFilesList) {
            addTheme(file.absoluteFilePath());
        }
    }
}

int KHMThemeFactory::getQty() const
{
    return themesList.size();
}

QStringList KHMThemeFactory::getNames() const
{
    QStringList list;
    for (const auto &theme : themesList) {
        list.append(theme.name());
    }
    return list;
}

QStringList KHMThemeFactory::themeList() const
{
    QStringList list;
    for (const auto &theme : themesList) {
        list.append(theme.uiName());
    }
    return list;
}

const KHMTheme * KHMThemeFactory::getTheme(int id) const
{
    if (id >= 0 && id < themesList.size()) {
        return &themesList[id];
    }
    return nullptr;
}

QRect KHMThemeFactory::makeRect(const QDomElement &element, const QString &propertyName)
{
    const QDomElement rect=element.firstChildElement(propertyName);

    return QRect(
        (int)(rect.attribute(QStringLiteral("xratio")).toDouble()*10000),
        (int)(rect.attribute(QStringLiteral("yratio")).toDouble()*10000),  //*10000 cause ratios are float
        (int)(rect.attribute(QStringLiteral("wratio")).toDouble()*10000),
        (int)(rect.attribute(QStringLiteral("hratio")).toDouble()*10000)  //QPoint,QRect constructors expect ints
    );
}

QColor KHMThemeFactory::getColor(const QDomElement &element, const QString &propertyName)
{
    const QDomElement color=element.firstChildElement(propertyName);

    return QColor ( color.attribute(QStringLiteral("r")).toInt(), color.attribute(QStringLiteral("g")).toInt(), color.attribute(QStringLiteral("b")).toInt());
}

bool KHMThemeFactory::checkTheme(const QDomElement &root, const QString &themeVersion)
{
    const QString rootName=root.tagName();

    return (rootName==QLatin1String("KHangManThemes")) && (themeVersion==root.attribute(QStringLiteral("version")));
}

void KHMThemeFactory::doTheme(const QDomElement &theme, const QString &version)   //fetch all the properties from .xml and stick it together
//"theme" means <theme></theme> section
{
    //Names of elements are camelCase, but
    //attributes are always lowercase

    const QString name=theme.attribute(QStringLiteral("name"));
    const QString uiName=theme.attribute(QStringLiteral("uiname"));
    const QString svgFileName=theme.attribute(QStringLiteral("svgfilename"));
    const QString author=theme.attribute(QStringLiteral("author"));

    const QDomElement colors=theme.firstChildElement(QStringLiteral("colors"));
    const QColor letterColor=getColor(colors, QStringLiteral("letterColor"));
    const QColor guessButtonTextColor=getColor(colors, QStringLiteral("guessButtonTextColor"));
    const QColor guessButtonColor=getColor(colors, QStringLiteral("guessButtonColor"));
    const QColor guessButtonHoverColor=getColor(colors, QStringLiteral("guessButtonHoverColor"));
    const QColor letterInputTextColor=getColor(colors, QStringLiteral("letterInputTextColor"));

    const QDomElement coords=theme.firstChildElement(QStringLiteral("coords"));
    const QRect wordRect=makeRect(coords, QStringLiteral("wordRect"));
    const QRect hintRect=makeRect(coords, QStringLiteral("hintRect"));
    const QRect kRect=makeRect(coords, QStringLiteral("kRect"));

    const QDomElement wordPos = coords.firstChildElement(QStringLiteral("goodWordPos"));
    const QPoint goodWordPos = QPoint(	wordPos.attribute(QStringLiteral("wratio")).toDouble()*10000, wordPos.attribute(QStringLiteral("hratio")).toDouble()*10000	);

    const KHMTheme newTheme (name, uiName, svgFileName, author, version, wordRect, hintRect, kRect, letterColor, guessButtonTextColor, guessButtonColor, guessButtonHoverColor, letterInputTextColor, goodWordPos);
    themesList.append(newTheme);
}
