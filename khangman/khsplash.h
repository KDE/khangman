/***************************************************************************
                          khsplash.h  -  description
                             -------------------
    begin                : Fri Jul 27 2001
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

#ifndef KHSPLASH_H
#define KHSPLASH_H

//standard C++ connections...
#include <stdlib.h>

//qt stuff...
#include <qwidget.h>
#include <qframe.h>
#include <qtimer.h>

//kde /custom stuff...
#include <kapplication.h>
#include "khangman.h"

/**
  *@author Anne-Marie Mahfouf
  */

class KhSplash : public QFrame  {
   Q_OBJECT

public: 
	KhSplash(QWidget *parent=0, const char *name=0);
    ~KhSplash();

public slots: // Public slots
  /** No descriptions */
  void slotHide();
};

#endif
