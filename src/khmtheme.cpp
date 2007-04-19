/*
   Copyright (C) 2007      Pino Toscano <pino@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

//project headers
#include "khmtheme.h"

#include <klocale.h>

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
        return QRect(0, windowsize.height()-windowsize.height()*126/535,
                     windowsize.width()*417/700, windowsize.height()*126/535);
    }

    virtual QColor letterColor() const
    {
        return QColor(148, 156, 167);
    }

    virtual QColor fontColor() const
    {
        return QColor(148, 156, 167);
    }

    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const
    {
        return QPoint(popupPos.x() + windowsize.width()*250/700,
                      popupPos.y() + windowsize.height()*485/535);
    }

    virtual QPalette palette(PaletteKind kind) const
    {
        QPalette pal;
        switch (kind)
        {
            case GuessButtonPalette:
            {
                pal.setBrush( QPalette::Window, QColor( 115,  64,  49));
                pal.setBrush( QPalette::WindowText, QColor(148, 156, 16));
                break;
            }
            case LetterInputPalette:
            {
                pal.setBrush( QPalette::WindowText, QColor(  83,  40,  14));
                break;
            }
        }
        return pal;
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
        return QRect(0, windowsize.height()-windowsize.height()*126/535,
                     windowsize.width()*327/700, windowsize.height()*126/535);
    }

    virtual QColor letterColor() const
    {
        return QColor(87, 0, 0);
    }

    virtual QColor fontColor() const
    {
        return QColor(87, 0, 0);
    }

    virtual QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const
    {
        return QPoint(popupPos.x() + windowsize.width()*200/700,
                      popupPos.y() + windowsize.height()*485/535);
    }

    virtual QPalette palette(PaletteKind kind) const
    {
        QPalette pal;
        switch (kind)
        {
            case GuessButtonPalette:
            {
                pal.setBrush( QPalette::Window, QColor( 205, 214, 90));
                pal.setBrush( QPalette::WindowText, QColor(87,   0,  0));
                break;
            }
            case LetterInputPalette:
            {
                pal.setBrush( QPalette::WindowText, QColor(  87,   0,  0));
                break;
            }
        }
        return pal;
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




