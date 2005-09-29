/*
    This file is from KOrganizer.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2004-2005 Anne-Marie Mahfouf <annma@kde.org> 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <kapplication.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktar.h>
#include <qdir.h>
#include <kaction.h>

#include "prefs.h"
#include "khnewstuff.h"

KHNewStuff::KHNewStuff( KHangManView *view ) :
  KNewStuff( "khangman", view ),
  m_view( view )
{
}

bool KHNewStuff::install( const QString &fileName )
{
    kdDebug(5850) << "KHNewStuff::install(): " << fileName << endl;
    
    KTar archive( fileName );
    if ( !archive.open( IO_ReadOnly ) )
                    return false;
    const KArchiveDirectory *archiveDir = archive.directory();
    KStandardDirs myStdDir;
    const QString destDir =myStdDir.saveLocation("data", kapp->instanceName() + "/data/", true);      
    KStandardDirs::makeDir( destDir );
    archiveDir->copyTo(destDir);
    archive.close();
        //look for languages dirs installed
    m_view->khangman->setLanguages();
    //refresh Languages menu
    m_view->khangman->m_languageAction->setItems(m_view->khangman->m_languageNames);
    m_view->khangman->slotChangeLanguage(m_view->khangman->m_languages.findIndex(Prefs::selectedLanguage()));
    m_view->khangman->m_languageAction->setCurrentItem(m_view->khangman->m_languages.findIndex(Prefs::selectedLanguage()));
    return true;
}

bool KHNewStuff::createUploadFile( const QString &/*fileName*/ )
{
    //return mView->saveCalendar( fileName );
    return true;
}
