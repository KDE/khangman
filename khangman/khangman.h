/***************************************************************************
                          khangman.h  -  description
                             -------------------
    begin                : Thu Jul 19 16:42:53 EDT 2001
    copyright            : (C) 2001 by Anne-Marie Mahfouf
    email                : a-m.mahfouf@lineone.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KHANGMAN_H
#define KHANGMAN_H

#define KHM_VERSION "0.79"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//standard C++ connections...
#include <stdlib.h>

//extern GUI...
#include "mainw.h"
#include "info.h"

//Qt headers
#include <qcombobox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qtextstream.h>

//KDE headers
#include <kapplication.h>
#include <kaudioplayer.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <krandomsequence.h>


class KHangMan : public MainW
{
  Q_OBJECT
public:
    /** construtor */
    KHangMan(QWidget* parent=0, const char *name=0);
     /** destructor */
    ~KHangMan();
	QString levelFile;
	QString word;
	QString goodWord;
	QString missedL;
	int missedChar;
	QStringList allWords;

public slots:
	virtual void slotClose();
	virtual void slotGetLevel(int level);
	virtual void slotInfo();
	virtual void slotNewGame();
	virtual void slotTry();
	void game();
	void wipeout();

private:
	KRandomSequence random;
	QPixmap px[13];

protected:
	KHelpMenu *helpMenu;
	void closeEvent(QCloseEvent *);

};

#endif
