/***************************************************************************
                          info.h  -  description
                             -------------------
    begin                : Wed Apr 17 2002
    copyright            : (C) 2002 by Whitehawk Stormchaser
    email                : zerokode@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INFO_H
#define INFO_H

#include <qwidget.h>
#include <infodlg.h>

/**
  *@author Whitehawk Stormchaser
  */

class Info : public InfoDlg  {
   Q_OBJECT
public: 
	Info(QWidget *parent=0, const char *name=0);
	~Info();
};

#endif
