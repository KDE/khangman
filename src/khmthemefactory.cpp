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
#include <KDebug>

KHMThemeFactory::KHMThemeFactory()
{    
}

bool KHMThemeFactory::addTheme(QString themeFile)
{
    QFile file (themeFile);
    QDomDocument tree("KHangManTheme");	//do we need it?
    QDomElement root;
    QDomNode themeNode;
    QDomElement themeElement;
    
    if (!file.exists()) { 
        kDebug() << "Themes file doesn't exist"; 
        return false; 
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        kDebug()<<"Can't open themes file"; 
        return false;
    }
    
    if (!tree.setContent(&file)) {
        file.close();
        kDebug()<<"Can't set content for theme file";
        return false;
    }
    
    file.close();
    root=tree.documentElement();
    
    if (!checkTheme(root, "1")) {
        kDebug()<<"Incompatible version of theme loaded"; 
        return false; 
    }
    
    QString themeVersion=root.attribute("version");
    themeNode=root.firstChild();

    do {
        doTheme(themeNode.toElement(), themeVersion);
        themeNode=themeNode.nextSibling();
    } while (!themeNode.isNull());
    return true;
}

void KHMThemeFactory::walkDirectory(QDir dir)		//unused! (but works)
{    
    QFileInfoList xmlFilesList;
    QStringList allowedExtenstion("*.xml");
    
    if (dir.exists()) {
        xmlFilesList=dir.entryInfoList(allowedExtenstion, QDir::Files);
      
        for (QFileInfoList::iterator i=xmlFilesList.begin(); i!=xmlFilesList.end(); i++) {
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
    return new KHMTheme(themesList[id]);
}

QRect KHMThemeFactory::makeRect(QDomElement element, QString propertyName)
{
    QDomElement rect=element.firstChildElement(propertyName);
  
    return QRect(	(int)(rect.attribute("xratio").toDouble()*10000),	(int)(rect.attribute("yratio").toDouble()*10000),	//*10000 cause ratios are float
		(int)(rect.attribute("wratio").toDouble()*10000),	(int)(rect.attribute("hratio").toDouble()*10000));	//QPoint,QRect constructors expect ints
}

QColor KHMThemeFactory::getColor(QDomElement element, QString propertyName)
{
    QDomElement color=element.firstChildElement(propertyName);
  
    return QColor ( color.attribute("r").toInt(), color.attribute("g").toInt(), color.attribute("b").toInt());
}

bool KHMThemeFactory::checkTheme(QDomElement root, QString themeVersion)
{
    QString rootName=root.tagName();
  
    return (rootName.compare("KHangManThemes")==0)	&&	(themeVersion.compare(root.attribute("version"))==0);
}

void KHMThemeFactory::doTheme(QDomElement theme, QString version)	//fetch all the properties from .xml and stick it together
									//"theme" means <theme></theme> section
{
    QDomElement coords=theme.firstChildElement("coords");

    //Names of elements are camelCase, but
    //attributes are always lowercase
    
    QString name=theme.attribute("name");
    QString uiName=theme.attribute("uiname");
    QString svgFileName=theme.attribute("svgfilename");
    QString author=theme.attribute("author");
    QString themeVersion=version;

    QColor letterColor=getColor(theme.firstChildElement("colors"), "letterColor");
    QColor guessButtonTextColor=getColor(theme.firstChildElement("colors"), "guessButtonTextColor");
    QColor guessButtonColor=getColor(theme.firstChildElement("colors"), "guessButtonColor");
    QColor guessButtonHoverColor=getColor(theme.firstChildElement("colors"), "guessButtonHoverColor");
    QColor letterInputTextColor=getColor(theme.firstChildElement("colors"), "letterInputTextColor");
    
    QRect wordRect=makeRect(theme.firstChildElement("coords"), "wordRect");
    QRect hintRect=makeRect(theme.firstChildElement("coords"), "hintRect");
    QRect kRect=makeRect(theme.firstChildElement("coords"), "kRect");
    
    QDomElement wordPos = theme.firstChildElement("coords").firstChildElement("goodWordPos");
    QPoint goodWordPos = QPoint(	wordPos.attribute("wratio").toDouble()*10000, wordPos.attribute("hratio").toDouble()*10000	);

    KHMTheme * newTheme = new KHMTheme	(	name, uiName, svgFileName, author, themeVersion, wordRect, hintRect, kRect,
						letterColor, guessButtonTextColor, guessButtonColor, guessButtonHoverColor,
						letterInputTextColor, goodWordPos	);
    themesList.append(*newTheme);
}

KHMThemeFactory::~KHMThemeFactory()
{
    themesList.clear();
}