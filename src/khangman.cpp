/***************************************************************************
 *   Copyright (C) 2001-2006 Anne-Marie Mahfouf <annma@kde.org> *
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
#include "timer.h"
#include "khnewstuff.h"
#include "khmtheme.h"
#include "langutils.h"

#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QPainter>
#include <QDir>
#include <kselectaction.h>
#include <kactioncollection.h>

#include <kconfigdialog.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kicon.h>
#include <kglobal.h>

KHangMan::KHangMan()
    : KMainWindow(),
      m_view(new KHangManView(this))
{
    setObjectName(QLatin1String("KHangMan"));

    m_newStuff = 0;
    currentLevel = -1;

    setCentralWidget(m_view);
    setLanguages();
    setupStatusbar();
    setupActions();

    // Toolbar for special characters
    secondToolbar = toolBar("secondToolBar");
    addToolBar ( Qt::BottomToolBarArea, secondToolbar);

    loadSettings();
    setAccent();
    loadLangToolBar();
    loadLevels();

    // set the theme
    slotChangeMode(Prefs::mode());

    // Start a new game.
    m_view->slotNewGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    // Game->New
    QAction *newAct = KStandardAction::openNew(m_view, SLOT(slotNewGame()),
                                       actionCollection());
    newAct->setToolTip(i18n( "Play with a new word" ));
    //connect(newAct, SIGNAL(triggered(bool)), m_view, SLOT(slotNewGame()));

    // Game->Get Words in New Language
    KAction *newStuffAct  = new KAction(i18n("&Get Words in New Language..."), this);
    actionCollection()->addAction("downloadnewstuff", newStuffAct );
    newStuffAct->setIcon(KIcon("knewstuff"));
    newStuffAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
    connect(newStuffAct, SIGNAL(triggered(bool)), this, SLOT(slotDownloadNewStuff()));

    KStandardAction::quit(this, SLOT(slotQuit()), actionCollection());

    m_levelAction  = new KSelectAction(i18n("Le&vel"), this);
    actionCollection()->addAction("combo_level", m_levelAction );
    connect(m_levelAction, SIGNAL(triggered(int)), this, SLOT(slotChangeLevel(int)));
    m_levelAction->setToolTip(i18n( "Choose the level" ));
    m_levelAction->setWhatsThis(i18n( "Choose the level of difficulty" ));

    // Action for selecting language.
    m_languageAction  = new KSelectAction(i18n("&Language"), this);
    actionCollection()->addAction("languages", m_languageAction );
    m_languageAction->setItems(m_languageNames);
    m_languageAction->setCurrentItem(m_languages.indexOf(Prefs::selectedLanguage()));
    connect(m_languageAction, SIGNAL(triggered(int)), this, SLOT(slotChangeLanguage(int)));

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // Mode.
    m_modeAction  = new KSelectAction(i18n("L&ook"), this);
    actionCollection()->addAction("combo_mode", m_modeAction );
    connect(m_modeAction, SIGNAL(triggered(int)), this, SLOT(slotChangeMode(int)));
    m_modeAction->setItems(KHMThemeFactory::instance()->themeList());
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
    qApp->closeAllWindows();
}


void KHangMan::slotChangeLevel(int index)
{
    static const char *levelStrings[] = {
        I18N_NOOP("Animals"),
        I18N_NOOP("Easy"),
        I18N_NOOP("Medium"),
        I18N_NOOP("Hard"),
    };
    levelString = levels[index];
    levelString.replace(0, 1, levelString.left(1).toLower());
    changeStatusbar(i18n(levelStrings[index]), IDS_LEVEL);
#if 0
    if (m_view->levelFile == "world_capitals.kvtml"
	|| m_view->levelFile == "departements.kvtml")
        changeStatusbar(i18n("First letter upper case"), IDS_ACCENTS);
    else
        changeStatusbar("", IDS_ACCENTS);
#endif
    Prefs::setCurrentLevel( index);
    Prefs::setLevelFile(levelString +".kvtml");
    Prefs::writeConfig();
    m_view->slotNewGame();
}

void KHangMan::slotChangeLanguage(int index)
{
    //good when no in English
    kDebug() << "Change to " << m_languages[m_languageNames.indexOf(m_languageNames[index])] << endl;
    Prefs::setSelectedLanguage(m_languages[m_languageNames.indexOf(m_languageNames[index])]);
    Prefs::writeConfig();
    loadLevels();
    loadLangToolBar();
    changeStatusbar(m_languageNames[m_languages.indexOf(Prefs::selectedLanguage())], IDS_LANG);
    setAccent();
    setMessages();
    m_view->slotNewGame();
}

void KHangMan::slotChangeMode(int index)
{
    Prefs::setMode(index);
    Prefs::writeConfig();
    m_view->setTheme(KHMThemeFactory::instance()->buildTheme(index));
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
        m_languages.removeAll(".");
        m_languages.removeAll("..");
    }
    m_languages.sort();

    kDebug() << "languages :" << m_languages << endl;

    if (m_languages.isEmpty())
	return;
    //find duplicated entries in KDEDIR and KDEHOME

    QStringList temp_languages;
    for (int i=0;  i<m_languages.count(); i++) {
        if (m_languages.count(m_languages[i])>1) {
            temp_languages.append(m_languages[i]);
            m_languages.removeAt(i);
        }
    }

    for (int i=0;  i<temp_languages.count(); i++) {
	// Append 1 of the 2 instances found.
        if (i%2==0)
            m_languages.append(temp_languages[i]);
    }
    temp_languages.clear();

    // Write the present languages in config so they cannot be downloaded.
    KSharedConfig::Ptr config = KGlobal::config();
    config->setGroup("KNewStuffStatus");
    for (int i=0;  i<m_languages.count(); i++) {
        QString tmp = m_languages[i];
        if (config->readEntry(tmp, QString()).isEmpty())
            config->writeEntry(tmp, QDate::currentDate().toString(Qt::ISODate));
    }

    // We look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.
    KConfig entry(KStandardDirs::locate("locale", "all_languages"));
    const QStringList::ConstIterator itEnd = m_languages.end();
    for (QStringList::Iterator it = m_languages.begin();
	 it != m_languages.end(); ++it) {
        entry.setGroup(*it);
        if (*it == "sr")
            m_languageNames.append(entry.readEntry("Name")+" ("+i18n("Cyrillic")+')');
        else if (*it == "sr@Latn") {
            entry.setGroup("sr");
	    m_languageNames.append(entry.readEntry("Name")
				   + " ("+i18n("Latin")+')');
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
    if (m_languages.filter(selectedLanguage).isEmpty())
            selectedLanguage = "en";
    changeStatusbar(m_languageNames[m_languages.indexOf(Prefs::selectedLanguage())], IDS_LANG);
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
    if (currentLevel > levels.count())
        currentLevel= 0;
    levelString = levels[currentLevel];
    levelString.replace(0, 1, levelString.left(1).toLower());
    levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).toLower()) ;
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
            int location = f.fileName().lastIndexOf("/");
            //strip the string to keep only the filename and not the path
            QString mString = f.fileName().right(f.fileName().length()-location-1);
            if (mString == Prefs::levelFile())
                levelBool = true;
            mString = mString.left(mString.length()-6);
            //Put the first letter in Upper case
            mString = mString.replace(0, 1, mString.left(1).toUpper());
        levels+=mString;
    }
    //TODO else tell no files had been found
    }
    
    // Sort easy, medium, hard at bottom, with the other categories at the top
    levels.sort();
    QString replace[3] = { "Easy", "Medium", "Hard" };
    for ( int i = 0; i < 3; ++i )
    {
        if ( levels.contains( replace[i] ) )
        {
            levels.removeAll( replace[i] );
            levels.append( replace[i] );
        }
    }
    
    //find duplicated entries in KDEDIR and KDEHOME
    for (int i=0;  i<levels.count(); i++)
    {
        if (levels.count(levels[i])>1)
            levels.removeAt(i);
    }
    if (currentLevel!=-1 && currentLevel>levels.count())
        currentLevel = levels.count();
    if (levelBool == false)
    {
        Prefs::setLevelFile(levels[0].replace(0, 1, levels[0].left(1).toLower())+".kvtml");
        Prefs::setCurrentLevel(0);
        currentLevel =0;
        Prefs::writeConfig();
    }
    QStringList translatedLevels;
    for (QStringList::Iterator it = levels.begin(); it != levels.end(); ++it )
        translatedLevels+=i18n((*it).toUtf8());
    m_levelAction->setItems(translatedLevels);
    m_levelAction->setCurrentItem(Prefs::currentLevel());

    setLevel();
    QString m_lstring = translatedLevels[currentLevel].toUtf8();
    m_lstring.replace(0, 1, m_lstring.left(1).toUpper());
    changeStatusbar(m_lstring, IDS_LEVEL);
}


void KHangMan::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
	ui_language.kcfg_Hint->setEnabled(m_view->hintExists());
        ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());
	/*
        if (Prefs::selectedLanguage() == "de")
            languageSettingsDlg->kcfg_UpperCase->setEnabled(true);
        else
            languageSettingsDlg->kcfg_UpperCase->setEnabled(false);
	*/
        return;
    }

    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog( this, "settings",  Prefs::self() );
    // Add the General Settings page
    QWidget *generalSettingsDlg = new QWidget;
    ui_general.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "colorize");

    // Add the Language Settings page
    QWidget *languageSettingsDlg = new QWidget;
    ui_language.setupUi(languageSettingsDlg);
    dialog->addPage(languageSettingsDlg, i18n("Languages"), "kvoctrain");

    ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());
    ui_language.kcfg_Hint->setEnabled( m_view->hintExists() );
    /*
    if (Prefs::selectedLanguage() == "de")
        languageSettingsDlg->kcfg_UpperCase->setEnabled(true);
    else
        languageSettingsDlg->kcfg_UpperCase->setEnabled(false);
    */

    Timer *m_timer = new Timer();
    dialog->addPage(m_timer, i18n("Timers"), "clock");

    connect(dialog, SIGNAL(settingsChanged( const QString &)), this, SLOT(updateSettings()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void KHangMan::updateSettings()
{
    //after upperCase() changed, reload new game
    setAccent();
    if (Prefs::selectedLanguage() == "de")
        loadLangToolBar();
    setMessages();
    m_view->slotNewGame();
}

void KHangMan::slotDownloadNewStuff()
{
    if ( !m_newStuff )
        m_newStuff = new KHNewStuff( this );
    m_newStuff->download();
}

void KHangMan::loadLangToolBar()
{
    QString lang = Prefs::selectedLanguage();
    m_noSpecialChars = LangUtils::hasSpecialChars(lang);

    if (secondToolbar->isVisible() && !m_noSpecialChars) {
	Prefs::setShowCharToolbar(true);
	Prefs::writeConfig();
    }

    secondToolbar->clear();

    m_allData.clear();
    if (!m_noSpecialChars) {
	QString myString=QString("khangman/%1.txt").arg(lang);
	QFile myFile;
	myFile.setFileName(KStandardDirs::locate("data", myString));

	// Let's look in local KDEHOME dir then
	if (!myFile.exists()) {
	    QString  myString=QString("khangman/data/%1/%2.txt")
		.arg(lang)
		.arg(lang);
	    myFile.setFileName(KStandardDirs::locate("data",myString));
	    kDebug() << myString << endl;
	}

	if (!myFile.exists()) {
	    QString mString=i18n("File $KDEDIR/share/apps/khangman/%1.txt not found;\n"
				 "check your installation.", lang);
	    KMessageBox::sorry( this, mString,
				i18n("Error") );
	    qApp->quit();
	}
	update();

	// We open the file and store info into the stream...
	QFile openFileStream(myFile.fileName());
	openFileStream.open(QIODevice::ReadOnly);
	QTextStream readFileStr(&openFileStream);
	readFileStr.setCodec("UTF-8");

	// m_allData contains all the words from the file
	// FIXME: Better name
	m_allData = readFileStr.readAll().split("\n");
	openFileStream.close();
	if (Prefs::selectedLanguage() == "de" && Prefs::upperCase())
	    for (int i=0; i<(int) m_allData.count(); i++)
		m_allData[i] = m_allData[i].toUpper();

        for (int i=0; i<m_allData.count(); ++i)
        {
            QAction *act = secondToolbar->addAction(m_allData.at(i));
            act->setIcon(charIcon(m_allData.at(i).at(0)));
            // used to carry the id
            act->setData(i);
            connect(act, SIGNAL(triggered(bool)), this, SLOT(slotPasteChar()));
        }
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
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    bool ok = true;
    int id = act->data().toInt(&ok);
    if (!ok || id < 0 || id >= m_allData.count())
        return;

    m_view->enterLetter(m_allData.at(id));
}

QIcon KHangMan::charIcon(const QChar & c) const
{
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
    p.end();

    ///Create transparency mask
    QBitmap bm(128, 128);
    bm.fill(Qt::color0);
    QPainter b(&bm);
    b.setFont(font);
    b.setPen(Qt::color1);
    b.drawText(r, Qt::AlignCenter, (QString) c);
    b.end();

    ///Mask the pixmap
    pm.setMask(bm);

    return QIcon(pm);
}

void KHangMan::setAccent()
{
    kDebug() << "in slot accent  " << endl;
    m_view->setAccentedLetters(LangUtils::hasAccentedLetters(Prefs::selectedLanguage()));
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
