/*
    This file is from KOrganizer.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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

#include <kcomponentdata.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kselectaction.h>
#include <ktar.h>

#include "prefs.h"
#include "khnewstuff.h"
#include "khangman.h"

KHNewStuff::KHNewStuff( KHangMan *hangMan ) :
  KNewStuff( "khangman", hangMan ),
  m_hangMan( hangMan )
{
}

bool KHNewStuff::install( const QString &fileName )
{
    kDebug(5850) << "KHNewStuff::install(): " << fileName << endl;
    
    KTar archive( fileName );
    if ( !archive.open( QIODevice::ReadOnly ) )
                    return false;
    const KArchiveDirectory *archiveDir = archive.directory();
    KStandardDirs myStdDir;
    const QString destDir =myStdDir.saveLocation("data", KGlobal::mainComponent().componentName() + "/data/", true);      
    KStandardDirs::makeDir( destDir );
    archiveDir->copyTo(destDir);
    archive.close();
        //look for languages dirs installed
    m_hangMan->setLanguages();
    //refresh Languages menu
    m_hangMan->m_languageAction->setItems(m_hangMan->m_languageNames);
    m_hangMan->slotChangeLanguage(m_hangMan->m_languages.indexOf(Prefs::selectedLanguage()));
    m_hangMan->m_languageAction->setCurrentItem(m_hangMan->m_languages.indexOf(Prefs::selectedLanguage()));
    return true;
}

bool KHNewStuff::createUploadFile( const QString &/*fileName*/ )
{
    //return mView->saveCalendar( fileName );
    return true;
}
