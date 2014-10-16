/***************************************************************************
 *   Copyright (C) 2007-2010 Pino Toscano <pino@kde.org>                   *
 *   Updated by Adam Rakowski <foo-script@o2.pl> (GSoC 2010)               *
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

//project headers
#include "khmtheme.h"

#include <KLocalizedString>

KHMTheme::KHMTheme( QString name, QString uiName, QString svgFileName, QString author, QString themeVersion,
            QRect wordRect, QRect hintRect, QRect kRect,
            QColor letterColor, QColor guessButtonTextColor, QColor guessButtonColor, QColor guessButtonHoverColor, QColor letterInputTextColor,
            QPoint goodWordPos)
{
    KHMname=name;
    KHMuiName=uiName;
    KHMsvgFileName=svgFileName;
    KHMauthor=author;
    KHMthemeVersion=themeVersion;
    KHMwordRect=wordRect;
    KHMhintRect=hintRect;
    KHMkRect=kRect;
    KHMletterColor=letterColor;
    KHMguessButtonTextColor=guessButtonTextColor;
    KHMguessButtonColor=guessButtonColor;
    KHMguessButtonHoverColor=guessButtonHoverColor;
    KHMletterInputTextColor=letterInputTextColor;
    KHMgoodWordPos=goodWordPos;
}

QString KHMTheme::name()
{
    return KHMname;
}

QString KHMTheme::uiName()
{
    return i18n(KHMuiName.toLatin1());
}

QString KHMTheme::svgFileName()
{
    return KHMsvgFileName;
}

QColor KHMTheme::letterInputTextColor()
{
    return KHMletterInputTextColor;
}

QString KHMTheme::getAuthor()
{
    return KHMauthor;
}

QString KHMTheme::getThemeVersion()
{
    return KHMthemeVersion;
}

QRect KHMTheme::wordRect(const QSize& windowsize)
{
    return QRect(windowsize.width()*KHMwordRect.x()/10000,    windowsize.height()*KHMwordRect.y()/10000,
           windowsize.width()*KHMwordRect.width()/10000,    windowsize.height()*KHMwordRect.height()/10000);
}

QRect KHMTheme::hintRect(const QSize& windowsize)
{
    return QRect(windowsize.width()*KHMhintRect.x()/10000,    windowsize.height()*KHMhintRect.y()/10000,
           windowsize.width()*KHMhintRect.width()/10000,    windowsize.height()*KHMhintRect.height()/10000);
}

QRect KHMTheme::kRect(const QSize& windowsize)
{
    return QRect(windowsize.width()*KHMkRect.x()/10000,       windowsize.height()*KHMkRect.y()/10000,
            windowsize.width()*KHMkRect.width()/10000,       windowsize.height()*KHMkRect.height()/10000);
}

QColor KHMTheme::letterColor()
{
    return KHMletterColor;
}

QColor KHMTheme::guessButtonTextColor()
{
    return KHMguessButtonTextColor;
}

QColor KHMTheme::guessButtonColor()
{
    return KHMguessButtonColor;
}

QColor KHMTheme::guessButtonHoverColor()
{
    return KHMguessButtonHoverColor;
}

QPoint KHMTheme::goodWordPos(const QSize& windowsize, const QPoint& popupPos)   //works good
{
    return QPoint(popupPos.x() + windowsize.width()*KHMgoodWordPos.x()/10000,
                    popupPos.y() + windowsize.height()*KHMgoodWordPos.y()/10000);
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

