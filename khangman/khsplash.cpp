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
#include "khsplash.moc"
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kaudioplayer.h>
#include <kmessagebox.h>
#include <stdlib.h>
#include <kdebug.h>

KhSplash::KhSplash(QWidget *parent, const char *name ) : QFrame(parent,name,QWidget::WStyle_NoBorder | QWidget::WStyle_Customize)
{
//before anything; We have to check for the images, if they are installed correctly...
	if (locate("data","khangman/pics/khm_splash.png")=="")
	{
		KMessageBox::error(this,"Sorry... KHangMan is not installed correctly\nPlease install it properly with --prefix=/your/KDE/dir...");
		exit(0);
	}
	setBackgroundPixmap( QPixmap( locate("data","khangman/pics/khm_splash.png") ) ); //preload is senseless here...
	QRect desk = KGlobalSettings::splashScreenDesktopGeometry();
	setGeometry ( desk.width()/2-160, desk.height()/2-120, 349, 173 );
	setFrameStyle( QFrame::Box | QFrame::Raised );
	setLineWidth(1);
	KAudioPlayer::play(locate("data","khangman/sounds/splash.ogg"));
	show(); //only after play...

//allow the splash screen to be displayed 400 seconds or closed by mouseclick...
	QTimer *timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()), this, SLOT(slotHide()) );
	
	timer->start( 2000, TRUE );
}

KhSplash::~KhSplash()
{
}


/** Hide the splash screen and call the game */
void KhSplash::slotHide()
{
	this->hide();
	KHangMan *khangman = new KHangMan();
	khangman->show();
}
