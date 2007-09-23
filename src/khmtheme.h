/***************************************************************************
 *   Copyright (C) Copyright (C) 2007      Pino Toscano <pino@kde.org>                                            *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef KHMTHEME_H
#define KHMTHEME_H

#include <QPalette>

class KHMTheme
{
public:
    KHMTheme();
    virtual ~KHMTheme();

    ///The name of theme as in the folder
    virtual QString name() const = 0;
    ///The name of the theme in the menu
    virtual QString uiName() const = 0;
    ///Get the svg name of the theme
    virtual QString svgFileName() const = 0;
    ///Set the position and size for drawing the word to guess
    virtual QRect wordRect(const QSize& windowsize) const = 0;
    ///Set the position and size for drawing the hanged K
    virtual QRect kRect(const QSize& windowsize) const = 0;
    ///Set the color for the word and the missed letters
    virtual QColor letterColor() const = 0;
    ///Set the color of the Guess word
    virtual QColor guessButtonTextColor() const = 0;
    ///Set the color of the Guess button background
    virtual QColor guessButtonColor() const = 0;
    ///Set the color of the Guess button background when the mouse is over it
    virtual QColor guessButtonHoverColor() const = 0;
    ///Set the color of the input text in the input widget
    virtual QColor letterInputTextColor() const = 0;
    ///Set the already guessed popup position
    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const = 0;
};

class KHMThemeFactory
{
public:
    static KHMThemeFactory* instance();
    ~KHMThemeFactory();

    KHMTheme* buildTheme(int id) const;
    QStringList themeList() const;

private:
    KHMThemeFactory();
};

#endif

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

