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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapp.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qevent.h>
#include <kaccel.h>
#include <qmainwindow.h>

class KHangMan : public QWidget
{
  Q_OBJECT
  public:
    /** construtor */
    KHangMan(QWidget* parent=0, const char *name=0);

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
    QMainWindow *helpla;
    QPushButton *exit1;
    QLabel *helpla1;
//    KhSplash *splash;

  public slots:
    void slotquit();
    void slotTreat();
    void slotShow();
    void slotNext();
    void choice(int index);
    void choice1();
    void choice2();
    void choice3();
    void slotInfo();
    void slotNewgame();
    void game();
    void warning();
    void slotHelp();
    void test1();
    void animals();
  /** No descriptions */
 // void slotDel();
//void slotGo();

  protected:
    void keyPressEvent(QKeyEvent *);
     /** destructor */
    ~KHangMan();
};

#endif
