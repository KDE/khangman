/*
 * Copyright (C) 2003 Anne-Marie Mahfouf <annma@kde.org>
 */

#ifndef _KHANGMANVIEW_H_
#define _KHANGMANVIEW_H_

#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

#include <krandomsequence.h>

#include "mainw.h"


/**
 * This is the main view class for KHangMan.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 */

class KHangManView : public MainW
{
    Q_OBJECT
public:

    KHangManView(QWidget *parent=0, const char *name=0);

    virtual ~KHangManView();

	QString levelFile;
	QString language;
	QString word;
	QString goodWord;
	QString missedL;
	int missedChar;
	QStringList allWords;
	QString mode;
	QPixmap bluePix;
	QPixmap naturePix;

private:

	KRandomSequence random;
	QPixmap px[13];

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

public slots:

	void slotNewGame();
	void slotTry();
	void slotBlue(QPixmap& );
	void slotNoBkgd();

private slots:

    void slotSetTitle(const QString& title);
	void game();
	void wipeout();
};

#endif // _KHANGMANVIEW_H_
