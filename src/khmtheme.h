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

#ifndef KHMTHEME_H
#define KHMTHEME_H

#include <QPalette>

class KHMTheme
{
  private:
    QString KHMname, KHMuiName, KHMsvgFileName, KHMauthor, KHMthemeVersion;
    QRect KHMwordRect, KHMhintRect, KHMkRect;
    QColor KHMletterColor, KHMguessButtonTextColor, KHMguessButtonColor, KHMguessButtonHoverColor, KHMletterInputTextColor;
    QPoint KHMgoodWordPos;
    
  public:
    KHMTheme(   const QString &name, const QString &uiName, const QString &svgFileName, const QString &author, const QString &themeVersion,
                QRect wordRect, QRect hintRect, QRect kRect,
                QColor letterColor, QColor guessButtonTextColor, QColor guessButtonColor, QColor guessButtonHoverColor, QColor letterInputTextColor,
                QPoint goodWordPos);

    ///The name of theme as in the folder
    QString name();
    ///The name of the theme in the menu
    QString uiName();
    ///Get the svg n22 the theme
    QString svgFileName();
    ///Set the position and size for drawing the word to guess
    QRect wordRect(const QSize& windowsize);

    QRect hintRect(const QSize& windowsize);

    ///Set the position and size for drawing the hanged K
    QRect kRect(const QSize& windowsize);
    ///Set the color for the word and the missed letters
    QColor letterColor();
    ///Set the color of the Guess word
    QColor guessButtonTextColor();
    ///Set the color of the Guess button background
    QColor guessButtonColor();
    ///Set the color of the Guess button background when the mouse is over it
    QColor guessButtonHoverColor();
    ///Set the color of the input text in the input widget
    QColor letterInputTextColor();
    ///Set the already guessed popup position
    QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos);
    //Find out who's the author
    QString getAuthor();
    //Version is equal to a KHMTheme format version declared in XML file
    QString getThemeVersion();
    
};

#endif

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

