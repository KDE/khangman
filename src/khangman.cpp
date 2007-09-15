/***************************************************************************
 *   Copyright (C) 2001-2007 Anne-Marie Mahfouf <annma@kde.org>            *
 *   annemarie.mahfouf@free.fr                                             *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


#include "khangman.h"
#include "prefs.h"
#include "timer.h"
#include "khmtheme.h"
#include "langutils.h"
#include "khangmanview.h"
#include "sharedkvtmlfiles.h"

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
#include <KLocale>
#include <kxmlguiwindow.h>
#include <kmessagebox.h>
#include <kstandardaction.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kicon.h>
#include <kglobal.h>
#include <knewstuff2/engine.h>

KHangMan::KHangMan()
    : KXmlGuiWindow(), m_currentLevel(-1),
      m_view(new KHangManView(this))
{
    setObjectName(QLatin1String("KHangMan"));

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
    show();
    // Start a new game.
    m_view->newGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    // Game->New
    QAction *newAct = KStandardAction::openNew(this, SLOT(slotNewGame()),
                                       actionCollection());
    newAct->setToolTip(i18n( "Play with a new word" ));

    // Game->Get Words in New Language
    KAction *newStuffAct  = new KAction(i18n("&Get Words in New Language..."), this);
    actionCollection()->addAction("downloadnewstuff", newStuffAct );
    newStuffAct->setIcon(KIcon("get-hot-new-stuff"));
    newStuffAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
    connect(newStuffAct, SIGNAL(triggered(bool)), this, SLOT(slotDownloadNewStuff()));

    KStandardAction::quit(this, SLOT(slotQuit()), actionCollection());

    m_levelAction  = new KSelectAction(i18n("&Category"), this);
    actionCollection()->addAction("combo_level", m_levelAction );
    connect(m_levelAction, SIGNAL(triggered(int)), this, SLOT(slotChangeLevel(int)));
    m_levelAction->setToolTip(i18n( "Choose the category" ));
    m_levelAction->setWhatsThis(i18n( "Choose the category of words" ));

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
    statusBar( )->insertPermanentItem("   ",IDS_LANG,    0);
    statusBar( )->insertPermanentItem("   ",IDS_LEVEL,   0);
    statusBar( )->insertPermanentItem("   ",IDS_ACCENTS, 0);
    statusBar( )->insertItem("   ", IDS_HINT,    1);
    statusBar( )->insertItem("   ", IDS_WINS,    1);
    statusBar( )->insertItem("   ", IDS_LOSSES,  1);
    statusBar()->setItemAlignment(IDS_HINT, Qt::AlignLeft);
}


void KHangMan::changeStatusbar(const QString& text, int id)
{
    statusBar()->changeItem(text, id);
}


// ----------------------------------------------------------------
//                               Slots


void KHangMan::slotQuit()
{
    Prefs::setShowCharToolbar( secondToolbar->isVisible());
    Prefs::self()->writeConfig();
    qApp->closeAllWindows();
}

void KHangMan::slotChangeLevel(int index)
{
    m_levelString = m_levels[index];
    changeStatusbar(m_titles[index], IDS_LEVEL);
    //m_levelString.replace(0, 1, m_levelString.left(1).toLower());
    Prefs::setCurrentLevel(index);
    Prefs::setLevelFile(m_levelString);
    Prefs::self()->writeConfig();
    m_view->readFile();
    m_view->newGame();
}

void KHangMan::slotChangeLanguage(int index)
{
    //good when no in English
    kDebug() << "Change to " << m_languages[m_languageNames.indexOf(m_languageNames[index])];
    Prefs::setSelectedLanguage(m_languages[m_languageNames.indexOf(m_languageNames[index])]);
    Prefs::self()->writeConfig();
    loadLevels();
    loadLangToolBar();
    changeStatusbar(m_languageNames[m_languages.indexOf(Prefs::selectedLanguage())], IDS_LANG);
    setAccent();
    setMessages();
    m_view->newGame();
}

void KHangMan::slotChangeMode(int index)
{
    Prefs::setMode(index);
    Prefs::self()->writeConfig();
    m_view->setTheme(KHMThemeFactory::instance()->buildTheme(index));
}


// ----------------------------------------------------------------


void KHangMan::setLanguages()
{
    m_languages.clear();
    m_languageNames.clear();
 	
    //the program scans in khangman/data/ to see what languages data is found
    m_languages = SharedKvtmlFiles::languages();
    kDebug() << "Languages " << m_languages << endl;
    if (m_languages.isEmpty())
		return;
    //find duplicated entries in KDEDIR and KDEHOME

    // Write the present languages in config so they cannot be downloaded.
    // FIXME: use pre-seeding here

    // We look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.

    KConfig entry(KStandardDirs::locate("locale", "all_languages"));
    for (int i = 0; i < m_languages.size(); ++i)
    {
	    KConfigGroup group = entry.group(m_languages[i]);
	    QString languageName = group.readEntry("Name");
	    if (languageName.isEmpty())
	    {
		    languageName = i18n("None");
	    }
	    m_languageNames.append(languageName);
    }
}


void KHangMan::loadSettings()
{
    // Language //TODO is selectedLanguage necessary??? only used here
    selectedLanguage = Prefs::selectedLanguage();
    if (!m_languages.contains(selectedLanguage))
            selectedLanguage = "en";
    int index = m_languages.indexOf(Prefs::selectedLanguage());
    if (index < 0)
    {
      // if the selected language is not available, use the first available one
      index = 0;
    }
    changeStatusbar(m_languageNames[index], IDS_LANG);
    // Show/hide characters toolbar
    if (Prefs::showCharToolbar())
        secondToolbar->show();
    else
        secondToolbar->hide();
    setMessages();
}

void KHangMan::setLevel()
{
    m_currentLevel = Prefs::currentLevel();
    if (m_currentLevel > m_levels.count())
        m_currentLevel= 0;
    m_levelString = m_titles[m_currentLevel];
    m_view->readFile();
}

void KHangMan::loadLevels()
{
    //build the Level combobox menu dynamically depending of the data for each language
	m_levels = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
	m_titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());
	
	if (!m_levels.contains(Prefs::levelFile()))
	{
        if (m_levels.size() == 0)
        {
            Prefs::setSelectedLanguage("en");
            m_levels = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
            m_titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());
        }

        Prefs::setLevelFile(m_levels[0]);
        Prefs::setCurrentLevel(0);
        m_currentLevel = 0;
        Prefs::self()->writeConfig();
	}

	// don't run off the end of the list
	if (m_currentLevel!=-1 && m_currentLevel > m_levels.count())
        m_currentLevel = m_levels.count();

	// titles should be translated in the data files themselves
    m_levelAction->setItems(m_titles);
    m_levelAction->setCurrentItem(Prefs::currentLevel());

    setLevel();
    changeStatusbar(m_titles[m_currentLevel], IDS_LEVEL);
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
    m_view->newGame();
}

void KHangMan::slotDownloadNewStuff()
{
    KNS::Entry::List entries = KNS::Engine::download();

    //look for languages dirs installed
    setLanguages();
    //refresh Languages menu
    m_languageAction->setItems(m_languageNames);
    slotChangeLanguage(m_languages.indexOf(Prefs::selectedLanguage()));
    m_languageAction->setCurrentItem(m_languages.indexOf(Prefs::selectedLanguage()));
    SharedKvtmlFiles::sortDownloadedFiles();
}

void KHangMan::loadLangToolBar()
{
    QString lang = Prefs::selectedLanguage();
    m_noSpecialChars = LangUtils::hasSpecialChars(lang);

    if (secondToolbar->isVisible() && !m_noSpecialChars) {
		Prefs::setShowCharToolbar(true);
		Prefs::self()->writeConfig();
    }

    secondToolbar->clear();

    m_allData.clear();
    if (!m_noSpecialChars) {
		QString myString=QString("khangman/%1.txt").arg(lang);
		QFile myFile;
		myFile.setFileName(KStandardDirs::locate("data", myString));

		// Let's look in local KDEHOME dir then
		if (!myFile.exists()) {
			QString  myString=QString("khangman/%1/%2.txt")
			.arg(lang)
			.arg(lang);
			myFile.setFileName(KStandardDirs::locate("data",myString));
			kDebug() << myString;
		}

		if (!myFile.exists()) {
			QString mString=i18n("File $KDEDIR/share/apps/khangman/%1.txt not found;\n"
					 "check your installation.", lang);
			KMessageBox::sorry( this, mString,
					i18n("Error") );
			//qApp->quit();
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
            if (!m_allData.at(i).isEmpty())
            {
                QAction *act = secondToolbar->addAction(m_allData.at(i));
                act->setIcon(charIcon(m_allData.at(i).at(0)));
                // used to carry the id
                act->setData(i);
                connect(act, SIGNAL(triggered(bool)), this, SLOT(slotPasteChar()));
            }
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
    kDebug() << "in slot accent  ";
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

void KHangMan::slotNewGame()
{
    m_view->lossCount++;
    statusBar()->changeItem(i18n("Losses: %1", m_view->lossCount), IDS_LOSSES);
    m_view->newGame();
}

#include "khangman.moc"
