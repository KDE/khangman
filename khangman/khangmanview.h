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
	///transparent=true indicates that pictures should be transparent
	bool transparent;
	///language is the current language for data file
	QString language;
	////word is the random word to be guessed
	QString word;
	///goodWord is the hidden word that is filled in
	QString goodWord;
	QString missedL;
	int missedChar;
	///allWords contains all letters already guessed
	QStringList allWords;
	///mode is the background mode: "nobg", "blue" or "nature"
	QString mode;
        ///contains the pixmap for the blue background mode
	QPixmap bluePix;
        ///contains the pixmap for the nature background mode
	QPixmap naturePix;
	///store temporarily the previous word to check it's not the same twice
	QString temp;

private:
	///necessary to have it all resize correctly
	void resizeEvent(QResizeEvent *);
	///KDE random generator
	KRandomSequence random;
	///the hangman pictures
	QPixmap px[11];
        ///the background image
	QPixmap bgPixmap;

  bool containsChar(QString &);
  void replaceLetters(QString);

public slots:

	void slotNewGame();
	void slotTry();
        ///set the background pixmap to the QPixmap argument
	void slotSetPixmap(QPixmap& );
	void slotNoBkgd();
	void slotTransparent();

private slots:

    void slotValidate(const QString &);
    void game();
    void wipeout();
};

#endif // _KHANGMANVIEW_H_
