/***************************************************************************
                          khsplash.cpp  -  description
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

#include "khsplash.h"
#include <kstddirs.h>

KhSplash::KhSplash(QWidget *parent, const char *name ) : QFrame(parent,name,QWidget::WStyle_NoBorder | QWidget::WStyle_Customize)
{
   QPixmap pm;
   pm.load(locate("data","beta.png"));
	setBackgroundPixmap(pm);
	setGeometry( QApplication::desktop()->width ()/2-160,
				QApplication::desktop()->height()/2-120,
							349, 173 );
   setFrameStyle( QFrame::Box | QFrame::Raised );
	setLineWidth(1);
	show();
    //allow the splash screen to be displayed 400 seconds
    QTimer *timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()),
    this, SLOT(slotHide()) );
    timer->start( 3000, TRUE );
}

KhSplash::~KhSplash(){
}


/** Hide the splash screen and call the game */
void KhSplash::slotHide()
{
    this->hide();
    KHangMan *khangman = new KHangMan();
    khangman->setGeometry(20,20,720,400);
    khangman->show();
}
