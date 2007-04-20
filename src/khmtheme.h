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

#ifndef KHANGMANTHEME_H
#define KHANGMANTHEME_H

#include <QPalette>
#include <QStringList>

class KHMTheme
{
public:
    KHMTheme();
    virtual ~KHMTheme();

    enum PaletteKind
    {
        GuessButtonPalette,
        LetterInputPalette
    };

    virtual QString name() const = 0;
    virtual QString uiName() const = 0;
    virtual QString svgFileName() const = 0;
    virtual QRect wordRect(const QSize& windowsize) const = 0;
    virtual QColor letterColor() const = 0;
    virtual QColor fontColor() const = 0;
    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const = 0;
    virtual QPalette palette(PaletteKind kind) const = 0;
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
