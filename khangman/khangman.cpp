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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/


#include "khangman.h"
#include "prefs.h"
#include "advanced.h"
#include "normal.h"
#include "timer.h"
#include "khnewstuff.h"

#include <qbitmap.h>
#include <qcheckbox.h>
#include <qpainter.h>
#include <qdir.h>

#include <kapplication.h>
#include <kactionclasses.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktoolbarbutton.h>


KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{
    m_newStuff = 0;

    setCentralWidget(m_view);
    setLanguages();
    setupStatusbar();
    setupActions();

    // Toolbar for special characters
    secondToolbar = toolBar("secondToolBar");
    secondToolbar->setBarPos(KToolBar::Bottom);
    loadSettings();
    setAccent();
    loadLangToolBar();
    loadLevels();

    // Start a new game.
    m_view->slotNewGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    // Game->New
    KAction *action = new KAction(i18n("&New"), "filenew", CTRL+Key_N , m_view, SLOT(slotNewGame()), actionCollection(), "file_new");
    action->setToolTip(i18n( "Play with a new word" ));

    // Game->Get Words in New Language
    new KAction( i18n("&Get Words in New Language..."), "knewstuff", CTRL+Key_G, this, SLOT( slotDownloadNewStuff() ), actionCollection(), "downloadnewstuff" );

    KStdAction::quit(this, SLOT(slotQuit()), actionCollection());
    
    m_levelAction = new KSelectAction(i18n("Le&vel"), 0,  actionCollection(), "combo_level");
    m_levelAction->setToolTip(i18n( "Choose the level" ));
    m_levelAction->setWhatsThis(i18n( "Choose the level of difficulty" ));
	connect(m_levelAction, SIGNAL(activated(int)), this, SLOT(slotChangeLevel(int)));

    // Action for selecting language.
    m_languageAction = new KSelectAction(i18n("&Language"), 0, actionCollection(), "languages");
    m_languageAction->setItems(m_languageNames);
    m_languageAction->setCurrentItem(m_languages.findIndex(Prefs::selectedLanguage()));
    connect(m_languageAction, SIGNAL(activated(int)), this, SLOT(slotChangeLanguage(int)));
    
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // Mode. Currently hard coded into Sea and Desert themes.
    QStringList modes;
    m_modeAction = new KSelectAction(i18n("L&ook"), 0, this, SLOT(slotChangeMode()),  actionCollection(), "combo_mode");
    modes += i18n("&Sea Theme");
    modes += i18n("&Desert Theme");
    m_modeAction->setItems(modes);
    m_modeAction->setCurrentItem(Prefs::mode());
    m_modeAction->setToolTip(i18n( "Choose the look and feel" ));
    m_modeAction->setWhatsThis(i18n( "Choose the look and feel" ));
    
    setupGUI();
}

// Set up the status bar with 4 different fields.
void KHangMan::setupStatusbar()
{
    // set up the status bar
    statusBar( )->insertItem("   ",IDS_LEVEL,   0);
    statusBar( )->insertItem("   ",IDS_LANG,    0);
    statusBar( )->insertItem("   ",IDS_ACCENTS, 0);
    statusBar( )->insertItem("   ",IDS_HINT,    0);
}


// FIXME: Make this into a slot?
void KHangMan::changeStatusbar(const QString& text, int id)
{
    statusBar()->changeItem(text, id);
}


// ----------------------------------------------------------------
//                               Slots


void KHangMan::slotQuit()
{
    Prefs::setShowCharToolbar( secondToolbar->isVisible());
    Prefs::writeConfig();
    kapp->quit();
}


void KHangMan::slotChangeLevel(int index)
{
    levelString = levels[index];
    changeStatusbar(levelString, IDS_LEVEL);
#if 0
    if (m_view->levelFile == "world_capitals.kvtml" 
	|| m_view->levelFile == "departements.kvtml")
        changeStatusbar(i18n("First letter upper case"), IDS_ACCENTS);
    else
        changeStatusbar("", IDS_ACCENTS);
#endif
    Prefs::setCurrentLevel(index);
	levelString.replace(0, 1, levelString.left(1).lower());
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
    loadLangToolBar();
    changeStatusbar(m_languageNames[m_languages.findIndex(Prefs::selectedLanguage())], IDS_LANG);
    setAccent();
    setMessages();
    m_view->slotNewGame();   
}

void KHangMan::slotChangeMode()
{
    if (m_modeAction->currentItem() == 0)
        Prefs::setMode(Prefs::EnumMode::sea);
    else
        Prefs::setMode(Prefs::EnumMode::desert);

    Prefs::writeConfig();
    m_view->setTheme();
}


// ----------------------------------------------------------------


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

    if (m_languages.isEmpty())
	return;
    //find duplicated entries in KDEDIR and KDEHOME

    QStringList temp_languages;
    for (uint i=0;  i<m_languages.count(); i++) {
        if (m_languages.contains(m_languages[i])>1) {
            temp_languages.append(m_languages[i]);
            m_languages.remove(m_languages[i]);
        }
    }

    for (uint i=0;  i<temp_languages.count(); i++) {
	// Append 1 of the 2 instances found.
        if (i%2==0)
            m_languages.append(temp_languages[i]);
    }
    temp_languages.clear();

    // Write the present languages in config so they cannot be downloaded.
    KConfig *config=kapp->config();
    config->setGroup("KNewStuffStatus");
    for (uint i=0;  i<m_languages.count(); i++) {
        QString tmp = m_languages[i];
        if (!config->readEntry(tmp))
            config->writeEntry(tmp, QDate::currentDate().toString(Qt::ISODate));
    }

    // We look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.
    KConfig entry(locate("locale", "all_languages"));
    const QStringList::ConstIterator itEnd = m_languages.end();
    for (QStringList::Iterator it = m_languages.begin(); 
	 it != m_languages.end(); ++it) {
        entry.setGroup(*it);
        if (*it == "sr")
            m_languageNames.append(entry.readEntry("Name")+" ("+i18n("Cyrillic")+")");
        else if (*it == "sr@Latn") {
            entry.setGroup("sr");
	    m_languageNames.append(entry.readEntry("Name") 
				   + " ("+i18n("Latin")+")");
        }
        else
	    m_languageNames.append(entry.readEntry("Name"));
    }

    // Never sort m_languageNames as it's m_languages translated 
    m_sortedNames = m_languageNames;
}


void KHangMan::loadSettings()
{
    // Language //TODO is selectedLanguage necessary??? only used here
    selectedLanguage = Prefs::selectedLanguage();
    if (m_languages.grep(selectedLanguage).isEmpty())
            selectedLanguage = "en";
    changeStatusbar(m_languageNames[m_languages.findIndex(Prefs::selectedLanguage())], IDS_LANG);
    // Show/hide characters toolbar
    if (Prefs::showCharToolbar())
        secondToolbar->show();
    else
        secondToolbar->hide();
    setMessages();
}

void KHangMan::setLevel()
{
    currentLevel = Prefs::currentLevel();
    if (currentLevel > (uint) levels.count()) 
        currentLevel= 0;
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

    // Sort easy, medium, hard at bottom, with the other categories at the top
    levels.sort();
    QString replace[3] = { "Easy", "Medium", "Hard" };
    for ( int i = 0; i < 3; ++i )
    {
        if ( levels.findIndex( replace[i] ) > -1 )
        {
            levels.remove( replace[i] );
            levels.append( replace[i] );
        }
    }

    //find duplicated entries in KDEDIR and KDEHOME
    QString last;
    for ( QStringList::Iterator it = levels.begin(); it != levels.end(); )
    {
        QStringList::Iterator it2 = it++;
        if (*it2 == last)
        {
            // remove duplicate
            levels.remove(it2);
        }
        else
        {
            last = *it2;
        }
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
    m_levelAction->setItems(translatedLevels);
    m_levelAction->setCurrentItem(Prefs::currentLevel());
    
    setLevel();
    QString m_lstring = translatedLevels[currentLevel].utf8();
    m_lstring.replace(0, 1, m_lstring.left(1).upper());
    changeStatusbar(m_lstring, IDS_LEVEL);
}


void KHangMan::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
	mAdvanced->kcfg_Hint->setEnabled(m_view->hintExists());
        mAdvanced->kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());
        return;
    }

    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog( this, "settings",  Prefs::self() );
    // Add the Normal Settings page
    normal *mNormal =  new normal( 0, "Normal Settings" );
    dialog->addPage(mNormal, i18n("General"), "colorize");

    // Add the Advanced Settings page
    mAdvanced = new advanced( 0, "Advanced" );
    mAdvanced->kcfg_Hint->setEnabled( m_view->hintExists() );
    mAdvanced->kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());

    dialog->addPage(mAdvanced, i18n("Languages"), "kvoctrain");

    Timer *m_timer = new Timer();
    dialog->addPage(m_timer, i18n("Timers"), "clock");

    connect(dialog, SIGNAL(settingsChanged()), this, SLOT(updateSettings()));

    dialog->show();
}

void KHangMan::updateSettings()
{
    //after upperCase() changed, reload new game
    setAccent();
    setMessages();
    m_view->slotNewGame();
}

void KHangMan::slotDownloadNewStuff()
{
    if ( !m_newStuff )
        m_newStuff = new KHNewStuff( m_view );
    m_newStuff->download();
}

void KHangMan::loadLangToolBar()
{
    if (Prefs::selectedLanguage() == "en"
	|| Prefs::selectedLanguage() == "it"
	|| Prefs::selectedLanguage() == "nl"
	|| Prefs::selectedLanguage() == "ru"
	|| Prefs::selectedLanguage() == "bg")
	m_noSpecialChars = true;
    else
	m_noSpecialChars = false;

    if (secondToolbar->isVisible() && !m_noSpecialChars) {
	Prefs::setShowCharToolbar(true);
	Prefs::writeConfig();
    }

    secondToolbar->clear();

    m_allData.clear();
    if (!m_noSpecialChars) {
	QString myString=QString("khangman/%1.txt").arg(Prefs::selectedLanguage());
	QFile myFile;
	myFile.setName(locate("data", myString));

	// Let's look in local KDEHOME dir then
	if (!myFile.exists()) {
	    QString  myString=QString("khangman/data/%1/%1.txt")
		.arg(Prefs::selectedLanguage())
		.arg(Prefs::selectedLanguage());
	    myFile.setName(locate("data",myString));
	    kdDebug() << myString << endl;
	}

	if (!myFile.exists()) {
	    QString mString=i18n("File $KDEDIR/share/apps/khangman/%1.txt not found;\n"
				 "check your installation.").arg(Prefs::selectedLanguage());
	    KMessageBox::sorry( this, mString,
				i18n("Error") );
	    kapp->quit();
	}
	update();

	// We open the file and store info into the stream...
	QFile openFileStream(myFile.name());
	openFileStream.open(IO_ReadOnly);
	QTextStream readFileStr(&openFileStream);
	readFileStr.setEncoding(QTextStream::UnicodeUTF8);

	// m_allData contains all the words from the file
	// FIXME: Better name
	m_allData = QStringList::split("\n", readFileStr.read(), true);
	openFileStream.close();

	for (int i=0; i<(int) m_allData.count(); i++)
	    secondToolbar->insertButton (charIcon(m_allData[i].at(0)), i, 
					 SIGNAL( clicked() ), this, 
					 SLOT( slotPasteChar()), true,  
					 i18n("Inserts the character %1").arg(m_allData[i]), i+1 );
    }

    if (Prefs::showCharToolbar())
	secondToolbar->show();
    else
	secondToolbar->hide();

    // Hide toolbar for special characters if the language doesn't have them.
    if (m_noSpecialChars)
	secondToolbar->hide();
}


void KHangMan::slotPasteChar()
{
	KToolBarButton *charBut = (KToolBarButton* ) sender();
	m_view->enterLetter(m_allData[charBut->id()]);
}

QString KHangMan::charIcon(const QChar & c)
{
    ///Create a name and path for the icon
    QString s = locateLocal("icon", "char" + QString::number(c.unicode()) + ".png");

    QRect r(4, 4, 120, 120);

    ///A font to draw the character with
    QFont font;
    font.setFamily( "Sans Serif" );
    font.setPointSize(96);
    font.setWeight(QFont::Normal);

    ///Create the pixmap
    QPixmap pm(128, 128);
    pm.fill(Qt::white);
    QPainter p(&pm);
    p.setFont(font);
    p.setPen(Qt::black);
    p.drawText(r, Qt::AlignCenter, (QString) c);

    ///Create transparency mask
    QBitmap bm(128, 128);
    bm.fill(Qt::color0);
    QPainter b(&bm);
    b.setFont(font);
    b.setPen(Qt::color1);
    b.drawText(r, Qt::AlignCenter, (QString) c);

    ///Mask the pixmap
    pm.setMask(bm);

    ///Save the icon to disk
    pm.save(s, "PNG");

    return s;
}

void KHangMan::setAccent()
{
    if (Prefs::selectedLanguage()=="es"
	|| Prefs::selectedLanguage() == "ca"
	|| Prefs::selectedLanguage() == "pt"
	|| Prefs::selectedLanguage() == "pt_BR")
        m_view->setAccentedLetters( true );
    else
        m_view->setAccentedLetters( false );
}


void KHangMan::setMessages()
{
    // Tell the user about if there is a hint.
    if (Prefs::hint() && m_view->hintExists())
        changeStatusbar(i18n("Hint on right-click"), IDS_HINT);
    else if (m_view->hintExists() && !Prefs::hint() )
        changeStatusbar(i18n("Hint available"), IDS_HINT);
    else
        changeStatusbar("", IDS_HINT);

    // Tell the user about accented characters
    if (m_view->accentedLetters() && Prefs::accentedLetters())
        changeStatusbar(i18n("Type accented letters"), IDS_ACCENTS);
    else
        changeStatusbar("", IDS_ACCENTS);
}

#include "khangman.moc"
