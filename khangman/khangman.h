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

#define KHM_VERSION "0.72"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//Qt headers
#include <qcombobox.h>
#include <qevent.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmainwindow.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtooltip.h>
#include <qvariant.h>
#include <qwidget.h>

//KDE headers
#include <kaccel.h>
#include <kapplication.h>
#include <kaudioplayer.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <krandomsequence.h>


class KHangMan : public QWidget
{
  Q_OBJECT
  public:
    /** construtor */
    KHangMan(QWidget* parent=0, const char *name=0);
     /** destructor */
    ~KHangMan();

     QString file1;

  public slots:
    void slotquit();
    void slotTreat();
    void slotShow();
    void slotNext();
    void choice(int index);
    void choice1();
    void slotInfo();
    void slotNewgame();
    void game();
    void warning();
    void test1();

 private:
	KRandomSequence random;
    QComboBox*level;
    QLabel *but[20];
    QLabel *guess[20];
    QLabel *lab1;
    QLabel *lab2;
    QLabel *lab3[26];
    QLabel *lab4;
    QLabel *lab8;
    QLineEdit *line;
    QPushButton *help;
    QPushButton *info;
    QPushButton *lab5;
    QPushButton *lab6;
    QPushButton *lab7;
    QPushButton *quit;
    QPixmap px[14];
    KAccel *accel1;
    KAccel *accel2;
    QPushButton *exit1;
    QLabel *helpla1;
    KDialog *helpla;


  protected:
	KHelpMenu *helpMenu;
    void keyPressEvent(QKeyEvent *);
     void resizeEvent(QResizeEvent *);
     void closeEvent(QCloseEvent *);

};

#endif
