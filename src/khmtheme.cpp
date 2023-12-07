// SPDX-FileCopyrightText: 2007-2010 Pino Toscano <pino@kde.org>
// SPDX-FileCopyCopyright: 2010 Adam Rakowski <foo-script@o2.pl>
// SPDX-License-Identifier: GPL-2.0-or-later

//project headers
#include "khmtheme.h"

#include <KLocalizedString>

KHMTheme::KHMTheme( const QString &name, const QString &uiName, const QString &svgFileName, const QString &author, const QString &themeVersion,
            QRect wordRect, QRect hintRect, QRect kRect,
            QColor letterColor, QColor guessButtonTextColor, QColor guessButtonColor, QColor guessButtonHoverColor, QColor letterInputTextColor,
            QPoint goodWordPos)
    :KHMname(name),
      KHMuiName(uiName),
      KHMsvgFileName(svgFileName),
      KHMauthor(author),
      KHMthemeVersion(themeVersion),
      KHMwordRect(wordRect),
      KHMhintRect(hintRect),
      KHMkRect(kRect),
      KHMletterColor(letterColor),
      KHMguessButtonTextColor(guessButtonTextColor),
      KHMguessButtonColor(guessButtonColor),
      KHMguessButtonHoverColor(guessButtonHoverColor),
      KHMletterInputTextColor(letterInputTextColor),
      KHMgoodWordPos(goodWordPos)
{
}

QString KHMTheme::uiName() const
{
    return i18n(KHMuiName.toLatin1().constData());
}

QRect KHMTheme::wordRect(const QSize& windowsize) const
{
    return QRect(windowsize.width()*KHMwordRect.x()/10000,    windowsize.height()*KHMwordRect.y()/10000,
           windowsize.width()*KHMwordRect.width()/10000,    windowsize.height()*KHMwordRect.height()/10000);
}

QRect KHMTheme::hintRect(const QSize& windowsize) const
{
    return QRect(windowsize.width()*KHMhintRect.x()/10000,    windowsize.height()*KHMhintRect.y()/10000,
           windowsize.width()*KHMhintRect.width()/10000,    windowsize.height()*KHMhintRect.height()/10000);
}

QRect KHMTheme::kRect(const QSize& windowsize) const
{
    return QRect(windowsize.width()*KHMkRect.x()/10000,       windowsize.height()*KHMkRect.y()/10000,
            windowsize.width()*KHMkRect.width()/10000,       windowsize.height()*KHMkRect.height()/10000);
}

QPoint KHMTheme::goodWordPos(const QSize& windowsize, const QPoint& popupPos) const  //works good
{
    return QPoint(popupPos.x() + windowsize.width()*KHMgoodWordPos.x()/10000,
                    popupPos.y() + windowsize.height()*KHMgoodWordPos.y()/10000);
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

