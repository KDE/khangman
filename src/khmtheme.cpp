/***************************************************************************
 *   Copyright (C) 2007      Pino Toscano <pino@kde.org>                   *
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

#include <KLocale>

KHMTheme::KHMTheme()
{
}

KHMTheme::~KHMTheme()
{
}

/// The 'sea' theme
class KHMThemeSea : public KHMTheme
{
public:
    KHMThemeSea()
            : KHMTheme()
    {
    }

    virtual QString name() const
    {
        return "sea";
    }

    virtual QString uiName() const
    {
        return i18n("Sea Theme");
    }

    virtual QString svgFileName() const
    {
        return "khangman_sea.svg";
    }

    virtual QRect wordRect(const QSize& windowsize) const
    {
        return QRect(0, windowsize.height()-windowsize.height()*15/100,
                windowsize.width()*417/700, windowsize.height()*15/100);
    }

    virtual QRect hintRect(const QSize& windowsize) const
    {
        return QRect(0, windowsize.height()-windowsize.height()*210/700,
                     windowsize.width()*410/700, windowsize.height()*130/700);
    }

    virtual QRect kRect(const QSize& windowsize) const
    {
        return QRect(0, 0, windowsize.width()*630/700, windowsize.height()*285/535);
    }

    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const
    {
        return QPoint(popupPos.x() + windowsize.width()*250/700,
                popupPos.y() + windowsize.height()*485/535);
    }

    virtual QColor letterColor() const
    {
        return QColor(148, 156, 167);
    }

    virtual QColor guessButtonColor() const
    {
        return QColor( 115,  64,  49);
    }

    virtual QColor guessButtonTextColor() const
    {
        return QColor( 165, 165, 165);
    }

    virtual QColor guessButtonHoverColor() const
    {
        return QColor( 115, 55, 55);
    }

    virtual QColor letterInputTextColor() const
    {
        return QColor(  83,  40,  14);
    }
};


/// The 'desert' theme
class KHMThemeDesert : public KHMTheme
{
public:
    KHMThemeDesert()
            : KHMTheme()
    {
    }

    virtual QString name() const
    {
        return "desert";
    }

    virtual QString uiName() const
    {
        return i18n("Desert Theme");
    }

    virtual QString svgFileName() const
    {
        return "khangman_desert.svg";
    }

    virtual QRect wordRect(const QSize& windowsize) const
    {
        return QRect(0, windowsize.height()-windowsize.height()*15/100,
                windowsize.width()*327/700, windowsize.height()*15/100);
    }

    virtual QRect hintRect(const QSize& windowsize) const
    {
        return QRect(0, windowsize.height()-windowsize.height()*210/700,
                     windowsize.width()*320/700, windowsize.height()*130/700);
    }

    virtual QRect kRect(const QSize& windowsize) const
    {
        return QRect(windowsize.width()*68/700, windowsize.height()*170/535,
                windowsize.width()*200/700, windowsize.height()*220/535);
    }

    virtual QColor letterColor() const
    {
        return QColor(87, 0, 0);
    }

    virtual QColor guessButtonColor() const
    {
        return QColor( 205, 214, 90);
    }

    virtual QColor guessButtonTextColor() const
    {
        return QColor( 82, 119, 70);
    }

    virtual QColor guessButtonHoverColor() const
    {
        return QColor( 205, 220, 100);
    }

    virtual QColor letterInputTextColor() const
    {
        return QColor( 82, 119, 70);
    }

    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const
    {
        return QPoint(popupPos.x() + windowsize.width()*200/700,
                popupPos.y() + windowsize.height()*485/535);
    }
};


KHMThemeFactory* KHMThemeFactory::instance()
{
    static KHMThemeFactory factory;
    return &factory;
}

KHMThemeFactory::KHMThemeFactory()
{
}

KHMThemeFactory::~KHMThemeFactory()
{
}

KHMTheme* KHMThemeFactory::buildTheme(int id) const
{
    switch (id)
    {
        case 0:
            return new KHMThemeSea();
        case 1:
            return new KHMThemeDesert();
    }
    return 0;
}

#define ADD_THEME_NAME( themeclass, list ) \
{ \
   themeclass x; \
   list.append( x.uiName() ); \
}
QStringList KHMThemeFactory::themeList() const
{
    QStringList ret;
    ADD_THEME_NAME( KHMThemeSea, ret )
    ADD_THEME_NAME( KHMThemeDesert, ret )
    return ret;
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

