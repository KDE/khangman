/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _KHANGMANVIEW_H_
#define _KHANGMANVIEW_H_

#include <qpixmap.h>

#include <krandomsequence.h>
//from libkdeedu
#include <keduvocdata.h>

#include "mainw.h"

class KPassivePopup;
class KHangMan;

/**
 * This is the main view class for KHangMan.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 */

class KHangManView : public MainW
{
    Q_OBJECT
public:

    KHangManView(KHangMan *parent=0, const char *name=0);

    virtual ~KHangManView();

        //levelFile is the text file containing the data
	QString levelFile;
	///transparent=true indicates that pictures should be transparent
	bool transparent;
	///softer=true indicates that pictures should be childish ones
	bool softer;
	///language is the current language for data file
	QString language;
	////word is the random word to be guessed
	QString word;
	///goodWord is the hidden word that is filled in
	QString goodWord;
	///the text on the MissedLetters label
	QString missedL;
	///how many times you missed, when it reaches 10, you are hanged
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
	///store the hint when there is one
	QString tip;
	
	QString stripWord, sword;
	int c, d;
	///true if Type accents is enabled for the languages that support it
	bool accent_b;
	///true if language = es, ca, pt or pt_BR
	bool m_accent;
	///true if hint is enabled for the fiels that support it
	bool hintBool;
	///if the data file is a kvtml file thus has hints
	bool kvtmlBool;
	///for tip to be seen
	KPassivePopup * myPopup;
	///tmp is to check if not twice the same random number
	int tmp;
	KHangMan *khangman;
signals:
	/**
	* Use this signal to change the content of the statusbar
	*/
	void signalChangeLanguage(int);
	//emit this signal to say if this is a kvtml file or not (hints enabled or not)
	void signalKvtml(bool);
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

  
protected:
        ///Enable hints on mouse right click if Hints exist
        virtual void mousePressEvent(QMouseEvent *mouse);

public slots:
	///call wipeout(), then call game()
	void slotNewGame();
	///called when a letter is typed in the input lineEdit and Enter is pressed
	void slotTry();
        ///set the background pixmap to the QPixmap argument
	void slotSetPixmap(QPixmap& );
	///called when the Combobox changes to No Background
	void slotNoBkgd();
	///called when Settings ->Transparent Pictures is checked
	void slotTransparent();
	///called when Settings->Softer HangMan Pictures is checked
	void slotSofter();
	///is called if the data file is kvtml hence has tips. read the word and its corrercponding tip from the kvtml file
	void readFile();

private slots:

    	void slotValidate(const QString &);
    	void game();
    	void wipeout();
};

#endif // _KHANGMANVIEW_H_
