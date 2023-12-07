// SPDX-FileCopyrightText: 2007-2010 Pino Toscano <pino@kde.org>
// SPDX-FileCopyCopyright: 2010 Adam Rakowski <foo-script@o2.pl>
// SPDX-License-Identifier: GPL-2.0-or-later

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
    QString name() const { return KHMname; }
    ///The name of the theme in the menu
    QString uiName() const;
    ///Get the svg n22 the theme
    QString svgFileName() const { return KHMsvgFileName; }
    ///Set the position and size for drawing the word to guess
    QRect wordRect(const QSize& windowsize) const;

    QRect hintRect(const QSize& windowsize) const;

    ///Set the position and size for drawing the hanged K
    QRect kRect(const QSize& windowsize) const;
    ///Set the color for the word and the missed letters
    QColor letterColor() const { return KHMletterColor; }
    ///Set the color of the Guess word
    QColor guessButtonTextColor() const { return KHMguessButtonTextColor; }
    ///Set the color of the Guess button background
    QColor guessButtonColor() const { return KHMguessButtonColor; }
    ///Set the color of the Guess button background when the mouse is over it
    QColor guessButtonHoverColor() const { return KHMguessButtonHoverColor; }
    ///Set the color of the input text in the input widget
    QColor letterInputTextColor() const { return KHMletterInputTextColor; }
    ///Set the already guessed popup position
    QPoint goodWordPos(const QSize& windowsize, const QPoint& popupPos) const;
    //Find out who's the author
    QString getAuthor() const { return KHMauthor; }
    //Version is equal to a KHMTheme format version declared in XML file
    QString getThemeVersion() const { return KHMthemeVersion; }
};

#endif

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

