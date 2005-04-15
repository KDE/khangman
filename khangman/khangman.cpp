/***************************************************************************
 *   Copyright (C) 2001-2005 Anne-Marie Mahfouf <annma@kde.org> *
 *   annemarie.mahfouf@free.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "khangman.h"
#include "prefs.h"
#include "advanced.h"

#include <qcheckbox.h>
#include <qdir.h>

#include <kapplication.h>
#include <kactionclasses.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>

const int IDS_LEVEL      = 100;
const int IDS_LANG       = 101;
const int IDS_ACCENTS = 102;
const int IDS_HINT =         103;

KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{
    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);
    
    setupStatusbar();
    setLanguages();
    setupActions();

    loadSettings();
    loadLevels();
    //setupToolbars();
    connect(m_view, SIGNAL(signalKvtml(bool)), this, SLOT(enableHint(bool)));
    m_view->slotNewGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    newAct = new KAction(i18n("&New"), "filenew", CTRL+Key_N , m_view, SLOT(slotNewGame()), actionCollection(), "file_new");
    newAct->setToolTip(i18n( "Play with a new word" ));
    new KAction( i18n("&Get Words in New Language..."), "knewstuff", CTRL+Key_G, this, SLOT( slotDownloadNewStuff() ), actionCollection(), "downloadnewstuff" );
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());
    
    levelAct = new KSelectAction(i18n("Le&vel"), 0, this, SLOT(slotChangeLevel()), actionCollection(), "combo_level");
    levelAct->setToolTip(i18n( "Choose the level" ));
    levelAct->setWhatsThis(i18n( "Choose the level of difficulty" ));

    m_languageAction = new KSelectAction(i18n("&Language"), 0, actionCollection(), "languages");
    m_languageAction->setItems(m_languageNames);
    m_languageAction->setCurrentItem(m_languages.findIndex(Prefs::selectedLanguage()));
    connect(m_languageAction, SIGNAL(activated(int)), this, SLOT(slotChangeLanguage(int)));
    
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
    
    QStringList modes;
    modeAct = new KSelectAction(i18n("L&ook"), 0, this, SLOT(slotChangeMode()),  actionCollection(), "combo_mode");
    modes += i18n("&Sea Theme");
    modes += i18n("&Desert Theme");
    modeAct->setItems(modes);
    modeAct->setCurrentItem(Prefs::mode());
    modeAct->setToolTip(i18n( "Choose the look and feel" ));
    modeAct->setWhatsThis(i18n( "Check the look and feel" ));
    
    setupGUI();
}

void KHangMan::setupStatusbar()
{
    // set up the status bar
    statusBar( )->insertItem("   ",IDS_LEVEL, 0);
    statusBar( )->insertItem("   ",IDS_LANG, 0);
    statusBar( )->insertItem("   ",IDS_ACCENTS, 0);
    statusBar( )->insertItem("   ",IDS_HINT, 0);
}

void KHangMan::changeStatusbar(const QString& text, int id)
{
    statusBar()->changeItem(text, id);
}

void KHangMan::slotChangeLevel()
{
    static const char *levelStrings[] = {
        I18N_NOOP("Animals"),
        I18N_NOOP("Easy"),
        I18N_NOOP("Hard"),
        I18N_NOOP("Medium"),
    };
    currentLevel = levelAct->currentItem();
    levelString = levels[currentLevel];
    levelString.replace(0, 1, levelString.left(1).lower());
    changeStatusbar(i18n(levelStrings[currentLevel]), IDS_LEVEL);
   // if (m_view->levelFile == "world_capitals.kvtml" || m_view->levelFile == "departements.kvtml")
    //    changeStatusbar(i18n("First letter upper case"), IDS_ACCENTS);
   // else
   //     changeStatusbar("", IDS_ACCENTS);
    Prefs::setCurrentLevel( currentLevel);
    Prefs::setLevelFile(levelString +".kvtml");
    Prefs::writeConfig();
    m_view->slotNewGame();
}

void KHangMan::slotChangeLanguage(int index)
{
    //good when no in English
    kdDebug() << "Change to " << m_languages[m_languageNames.findIndex(m_languageNames[index])] << endl;
    Prefs::setSelectedLanguage(m_languages[m_languageNames.findIndex(m_languageNames[index])]);
    Prefs::writeConfig();
    loadLevels();
    changeStatusbar(m_languageNames[m_languages.findIndex(Prefs::selectedLanguage())], IDS_LANG);
    m_view->slotNewGame();   
}

void KHangMan::slotChangeMode()
{
    if (modeAct->currentItem() ==0)
        Prefs::setMode(Prefs::EnumMode::sea);
    else
        Prefs::setMode(Prefs::EnumMode::desert);
    Prefs::writeConfig();
    m_view->setTheme();
}

void KHangMan::setLanguages()
{
    m_languages.clear();
    m_languageNames.clear();
    m_sortedNames.clear();
    //the program scans in khangman/data/ to see what languages data is found
    QStringList mdirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
    if (mdirs.isEmpty()) return;
    for (QStringList::Iterator it =mdirs.begin(); it !=mdirs.end(); ++it ) {
        QDir dir(*it);
        m_languages += dir.entryList(QDir::Dirs, QDir::Name);
        m_languages.remove(m_languages.find("."));
        m_languages.remove(m_languages.find(".."));
    }
    m_languages.sort();
    kdDebug() << "languages :" << m_languages << endl;
    if (m_languages.isEmpty()) return;
    //find duplicated entries in KDEDIR and KDEHOME
    QStringList temp_languages;
    for (uint i=0;  i<m_languages.count(); i++)
    {
        if (m_languages.contains(m_languages[i])>1) {
            temp_languages.append(m_languages[i]);
            m_languages.remove(m_languages[i]);
        }
    }
    for (uint i=0;  i<temp_languages.count(); i++)
    {
        if (i%2==0)
            m_languages.append(temp_languages[i]);//append 1 of the 2 instances found
    }
    temp_languages.clear();
    //write the present languages in config so they cannot be downloaded
    KConfig *config=kapp->config();
    config->setGroup("KNewStuffStatus");
    for (uint i=0;  i<m_languages.count(); i++)
    {
        QString tmp = m_languages[i];
        if (!config->readEntry(tmp))
            config->writeEntry(tmp, QDate::currentDate().toString());
    }
    //we look in $KDEDIR/share/locale/all_languages from /kdelibs/kdecore/all_languages
    //to find the name of the country corresponding to the code and the language the user set
    KConfig entry(locate("locale", "all_languages"));
    const QStringList::ConstIterator itEnd = m_languages.end();
    for (QStringList::Iterator it = m_languages.begin(); it != m_languages.end(); ++it) {
        entry.setGroup(*it);
        if (*it == "sr")
            m_languageNames.append(entry.readEntry("Name")+" ("+i18n("Cyrillic")+")");
        else if (*it == "sr@Latn") {
            entry.setGroup("sr");
        m_languageNames.append(entry.readEntry("Name")+" ("+i18n("Latin")+")");
        }
        else
        m_languageNames.append(entry.readEntry("Name"));
    }
    //never sort m_languageNames as it's m_languages translated 
    m_sortedNames = m_languageNames;
}

void KHangMan::loadSettings()
{
    // Language //TODO is selectedLanguage necessary??? only used here
    selectedLanguage = Prefs::selectedLanguage();
    if (m_languages.grep(selectedLanguage).isEmpty())
            selectedLanguage = "en";
    changeStatusbar(m_languageNames[m_languages.findIndex(Prefs::selectedLanguage())], IDS_LANG);
}

void KHangMan::setLevel()
{
    currentLevel = Prefs::currentLevel();
    if (currentLevel > (uint) levels.count()) currentLevel= 0;
    levelString = levels[currentLevel];
    levelString.replace(0, 1, levelString.left(1).lower());
    levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).lower()) ;
}

void KHangMan::loadLevels()
{
    //build the Level combobox menu dynamically depending of the data for each language
    levels.clear();//initialize QStringList levels
    KStandardDirs *dirs = KGlobal::dirs();
    QStringList mfiles = dirs->findAllResources("data","khangman/data/" + Prefs::selectedLanguage() + "/*.kvtml");
    bool levelBool = false;
    if (!mfiles.isEmpty())
    {
        for (QStringList::Iterator it = mfiles.begin(); it != mfiles.end(); ++it ) {
            QFile f( *it);
            //find the last / in the file name
            int location = f.name().findRev("/");
            //strip the string to keep only the filename and not the path
            QString mString = f.name().right(f.name().length()-location-1);
            if (mString == Prefs::levelFile())
                levelBool = true;
            mString = mString.left(mString.length()-6);
            //Put the first letter in Upper case
            mString = mString.replace(0, 1, mString.left(1).upper());
        levels+=mString;
    }
    //TODO else tell no files had been found
    }
    levels.sort();
    //find duplicated entries in KDEDIR and KDEHOME
    for (uint i=0;  i<levels.count(); i++)
    {
        if (levels.contains(levels[i])>1)
            levels.remove(levels[i]);
    }
    if (currentLevel>levels.count())
        currentLevel = levels.count();
    if (levelBool == false)
    {
        Prefs::setLevelFile(levels[0].replace(0, 1, levels[0].left(1).lower())+".kvtml");
        Prefs::setCurrentLevel(0);
        currentLevel =0;
        Prefs::writeConfig();
    }    
    QStringList translatedLevels;
    for (QStringList::Iterator it = levels.begin(); it != levels.end(); ++it )
        translatedLevels+=i18n((*it).utf8());
    levelAct->setItems(translatedLevels);
    levelAct->setCurrentItem(Prefs::currentLevel());
    
    setLevel();
    QString m_lstring = translatedLevels[currentLevel].utf8();
    m_lstring.replace(0, 1, m_lstring.left(1).upper());
    changeStatusbar(m_lstring, IDS_LEVEL);
}

void KHangMan::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog( this, "settings",  Prefs::self() );
    //normal *mNormal =  new normal( 0, "Normal Settings" );
    //dialog->addPage(mNormal, i18n("Look & Feel"), "colorize");
    advanced *mAdvanced=  new advanced( 0, "Advanced" );
    mAdvanced->kcfg_Hint->setEnabled( m_view->hintBool);
    mAdvanced->kcfg_AccentedLetters->setEnabled(m_view->m_accent);
    dialog->addPage(mAdvanced, i18n("Advanced Settings"), "wizard");
    connect(dialog, SIGNAL(settingsChanged()), this, SLOT(updateSettings()));
    dialog->show();
}

#include "khangman.moc"
