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

  //levelFile is the text file containing the data
	QString levelFile;
	//language is the current language for data file
	QString language;
	//word is the random word to be guessed
	QString word;
	//goodWord is the hidden word that is filled in
	QString goodWord;
	QString missedL;
	int missedChar;
	//allWords contains all letters already guessed
	QStringList allWords;
	//mode
	QString mode;
	QPixmap bluePix;
	QPixmap naturePix;
	//store the previous word to check it's not the same twice
	QString temp;

private:

	KRandomSequence random;
	//the hangman pictures
	QPixmap px[11];

  bool containsChar(QString &);
  void replaceLetters(QString);

public slots:

	void slotNewGame();
	void slotTry();
	void slotBlue(QPixmap& );
	void slotNoBkgd();

private slots:

    void game();
    void wipeout();
};

#endif // _KHANGMANVIEW_H_
