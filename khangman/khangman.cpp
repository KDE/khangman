/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 /************************************/
 //Please save as utf8 encoding
 /************************************/
 
//Qt headers
#include <qcheckbox.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qstringlist.h>
//KDE headers
#include <kactionclasses.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
//Project headers
#include "khangman.h"
#include "prefs.h"
#include "khnewstuff.h"
#include "advanced.h"

const int IDS_LEVEL      = 100;
const int IDS_LANG       = 101;
const int IDS_ACCENTS = 102;
const int IDS_HINT =         103;

KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{
	levelString = "";
	modeString = "";
	mNewStuff = 0;
	setLanguages();
	// tell the KMainWindow that this is indeed the main widget
	setCentralWidget(m_view);
	//selectedLanguage is the language saved in Settings otherwise it is default or en if no default
	// then, setup our actions, must be done after the language search
	setupActions();
	// set up the status bar
	statusBar( )->insertItem("   ",IDS_LEVEL, 0);
	statusBar( )->insertItem("   ",IDS_LANG, 0);
	statusBar( )->insertItem("   ",IDS_ACCENTS, 0);
	statusBar( )->insertItem("   ",IDS_HINT, 0);
	//toolbar for special characters
	secondToolbar = toolBar("Special Characters");
	secondToolbar->setBarPos(KToolBar::Bottom);
	connect(m_view, SIGNAL(signalChangeLanguage(int)), this, SLOT(changeLanguage(int)));
	connect(m_view, SIGNAL(signalKvtml(bool)), this, SLOT(enableHint(bool)));
	loadSettings();
	loadLangToolBar();
	setupLangMenu();
	slotHint();
	newGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
	newAct = new KAction(i18n("&New"), "file_new", CTRL+Key_N , this, SLOT(newGame()), actionCollection(), "file_new");
	KGlobal::iconLoader()->loadIcon("knewstuff", KIcon::Small);
	new KAction( i18n("Get data in a new language..."), "knewstuff", 0, this, SLOT( downloadNewStuff() ), actionCollection(), "downloadnewstuff" );
	KStdAction::quit(this, SLOT(slotClose()), actionCollection());
	
	createStandardStatusBarAction();
	setStandardToolBarMenuEnabled(true);
	langAct = new KSelectAction(i18n("&Languages"), 0, this, SLOT(slotLanguage()), actionCollection(), "combo_lang");
	langAct->setItems(m_sortedNames);
	
	KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
	
	m_pFullScreen = KStdAction::fullScreen( 0, 0, actionCollection(), this);
	connect( m_pFullScreen, SIGNAL( toggled( bool )), this, SLOT( slotSetFullScreen( bool )));
	
	levelAct = new KSelectAction(i18n("Level"), 0, this, SLOT(changeLevel()), actionCollection(), "combo_level");
	levelAct->setToolTip(i18n( "Choose the level" ));
	levelAct->setWhatsThis(i18n( "Choose the level of difficulty" ));
	
	KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
	
	QStringList modes;
	modeAct = new KSelectAction(i18n("Look & Feel"), 0, this, SLOT(changeMode()),  actionCollection(), "combo_mode");
	modes += i18n("No Background");
	modes += i18n("Blue Theme");
	modes += i18n("Nature Theme");
	modeAct->setItems(modes);
	modeAct->setToolTip(i18n( "Choose the look and feel" ));
	modeAct->setWhatsThis(i18n( "Check the look and feel" ));
	setAutoSaveSettings("General");
	createGUI("khangmanui.rc");
}

void KHangMan::setupLangMenu()
{
	langPopup = static_cast<QPopupMenu*>(factory()->container("languages", this));
	langPopup->clear();
	for (uint index = 0; index < m_sortedNames.count(); index++)
		langPopup->insertItem(m_sortedNames[index], m_languageNames.findIndex(m_sortedNames[index]), index);
	langPopup->setItemChecked(m_languages.findIndex(selectedLanguage), true);
	connect(langPopup, SIGNAL(activated(int)), this, SLOT(changeLanguage(int)) );
}

void KHangMan::newGame()
{
  	m_view->slotNewGame();
}

void KHangMan::optionsConfigureKeys()
{
    	KKeyDialog::configure(actionCollection());
}

void KHangMan::optionsConfigureToolbars()
{
	// use the standard toolbar editor
	saveMainWindowSettings( KGlobal::config(), autoSaveGroup() );
	KEditToolbar dlg(actionCollection());
	connect(&dlg, SIGNAL(newToolbarConfig()), this, SLOT(newToolbarConfig()));
	dlg.exec();
}

void KHangMan::newToolbarConfig()
{
	// this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
	// recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
	createGUI();
	setupLangMenu();
	applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
}

void KHangMan::changeStatusbar(const QString& text, int id)
{
	// display the text on the statusbar
	statusBar()->changeItem(text, id);
}

void KHangMan::changeCaption(const QString& text)
{
	// display the text on the caption
	setCaption(text);
}

void KHangMan::changeLevel()
{
	static const char *levelStrings[] = {
		I18N_NOOP("Easy"),
		I18N_NOOP("Medium"),
		I18N_NOOP("Hard"),
		I18N_NOOP("Animals"),
	};
	currentLevel = levelAct->currentItem();
	levelString = levels[currentLevel];
	levelString.replace(0, 1, levelString.left(1).lower());
	
	m_view->levelFile = levelString +".txt";
	changeStatusbar(i18n(levels[currentLevel].utf8()), IDS_LEVEL);
	if (m_view->levelFile == "world_capitals.txt" || m_view->levelFile == "departements.txt")
		changeStatusbar(i18n("First letters must be upper case"), IDS_ACCENTS);	
	else
		changeStatusbar(i18n(""), IDS_ACCENTS);
	Prefs::setLevel( currentLevel);
	Prefs::setLevelFile(m_view->levelFile);
	Prefs::writeConfig();
        newGame();
}

void KHangMan::changeMode()
{
	switch (modeAct->currentItem()) {
    	case 0:
	 		modeString="nobg";
			m_view->slotNoBkgd();
			Prefs::setMode("nobg");
    		break;

    	case 1:
      			modeString="blue";
			m_view->slotSetPixmap(m_view->bluePix);
			Prefs::setMode("blue");
   			break;
    	case 2:
      		modeString="nature";
			m_view->slotSetPixmap(m_view->naturePix);
			Prefs::setMode("nature");
   			break;
	}
	Prefs::writeConfig();
	if (dialog)
	mNormal->kcfg_Transparent->setEnabled( modeAct->currentItem() != 0 );
}

void KHangMan::loadSettings()
{
    	// Language
    	selectedLanguage = Prefs::selectedLanguage();
	if (m_languages.grep(selectedLanguage).isEmpty())
		selectedLanguage = "en";
     	setLanguage(selectedLanguage);
    	// Level
    	currentLevel = Prefs::level(); //default is easy
     	// Show/hide characters toolbar
   	m_bCharToolbar = Prefs::showCharToolbar();
   	if (m_bCharToolbar) 
		secondToolbar->show();
  	else
		secondToolbar->hide();
    	//see if language has special accented letters
    	setAccentBool();
    	m_view->accent_b = Prefs::accentedLetters();
    	if (m_view->m_accent && m_view->accent_b)
		changeStatusbar(i18n("Type accented letters"), IDS_ACCENTS);
    
    	loadDataFiles();
 
	if (locate("data", "khangman/data/"+selectedLanguage+"/"+Prefs::levelFile()).isEmpty()) {
		Prefs::setLevelFile(levels[0].replace(0, 1, levels[0].left(1).lower())+".txt");
		Prefs::writeConfig();
		currentLevel = 0;
		}
	m_view->levelFile = Prefs::levelFile();
	kdDebug() << "--------- LevelFile -------- " << m_view->levelFile << endl;
    	levelString = levels[currentLevel];
    	levelString.replace(0, 1, levelString.left(1).lower());
    	setLevel_WindowState();

     	// Background
    	QString oldMode = modeString;
    	modeString = Prefs::mode();
   	if(oldMode != modeString)
    		setMode_WindowState();
	m_view->transparent = Prefs::transparent();
	m_view->slotTransparent();
	m_view->softer = Prefs::softer();
	m_view->slotSofter();
	slotAccents();	
	m_view->hintBool= Prefs::hint();
	m_view->b_oneLetter = Prefs::oneLetter();
 }

void KHangMan::setLevel_WindowState()
{
	if (currentLevel>levels.count())
		currentLevel = levels.count();
	levelAct->setCurrentItem(currentLevel);
	changeStatusbar(i18n(levels[currentLevel].utf8()), IDS_LEVEL);
}

void KHangMan::setMode_WindowState()
{
	if (modeString=="nobg")
	{
		modeAct->setCurrentItem(0);
		m_view->slotNoBkgd();
	}
	else if (modeString=="blue")
	{
		modeAct->setCurrentItem(1);
		m_view->slotSetPixmap(m_view->bluePix);
	}
	else if (modeString=="nature")
	{
		modeAct->setCurrentItem(2);
		m_view->slotSetPixmap(m_view->naturePix);
	}
}

void KHangMan::slotLanguage()
{
    	changeLanguage(m_languageNames.findIndex(m_sortedNames[langAct->currentItem()]));
}

void KHangMan::changeLanguage(int newLanguage)
{
    	// Do not accept to switch to same language
   	 if (newLanguage == m_languageNames.findIndex(selectedLanguage))
		return;

    	// Unselect preceding language
    	langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[newLanguage]));
   	for (int id = 0; id < (int) m_languageNames.count(); id++)
    		langPopup->setItemChecked(id, id == newLanguage);

    	selectedLanguage = m_languages[newLanguage];
	Prefs::setSelectedLanguage(selectedLanguage);
	Prefs::writeConfig();
    	//load the different data files in the Level combo for the new language
   	loadDataFiles();
	//at the moment, check if currentLevel exists (< levels.count()) if not, set it to 0
	//but if medium in tg -> hard in other so 
    	//check if the name of the file exists in the new language. If not, set it to levels[0].
    	//TODO: save level per language

	if (currentLevel > (uint) levels.count()) currentLevel= 0;
	levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).lower()) ;
	m_view->levelFile = levelString +".txt";
	Prefs::setLevel(currentLevel);
        Prefs::setLevelFile(m_view->levelFile);
	Prefs::writeConfig();
    	
	//update the Levels in Level combobox as well
    	setLevel_WindowState();
    	setLanguage(selectedLanguage);
	
    	if (m_view->kvtmlBool&& dialog) 
	      mNormal->kcfg_Hint->setEnabled( true);
    	slotHint();
    	setAccentBool();
	m_bCharToolbar = Prefs::showCharToolbar();
   	 if (m_view->m_accent) 
    		slotAccents();
    	else {
		if (dialog)
			mNormal->kcfg_AccentedLetters->setEnabled(false);
    		changeStatusbar("", IDS_ACCENTS);
		loadLangToolBar();
		newGame();
    	}
}

void KHangMan::setLanguage(QString lang)
{
	m_view->language = lang;
	changeStatusbar(m_languageNames[m_languages.findIndex(lang)], IDS_LANG);
}

void KHangMan::slotTransparent()
{
        m_view->transparent = Prefs::transparent();//transAct->isChecked();
        m_view->slotTransparent();
}

void KHangMan::loadDataFiles()
{
	//build the Level combobox menu dynamically depending of the data for each language
	levels.clear();//initialize QStringList levels
	KStandardDirs *dirs = KGlobal::dirs();
	QStringList mfiles = dirs->findAllResources("data","khangman/data/" + selectedLanguage + "/*.txt");
	if (!mfiles.isEmpty())
	{
	for (QStringList::Iterator it = mfiles.begin(); it != mfiles.end(); ++it ) {
		QFile f( *it);
		//find the last / in the file name
		int location = f.name().findRev("/");
		//strip the string to keep only the filename and not the path
		QString mString = f.name().right(f.name().length()-location-1);
		mString = mString.left(mString.length()-4);
		//Put the first letter in Upper case
		mString = mString.replace(0, 1, mString.left(1).upper());
		levels+=mString;
	}
	//TODO else tell no files had been found
	}
	levels.sort();
	levelAct->setItems(levels);
	QStringList translatedLevels;
	for (QStringList::Iterator it = levels.begin(); it != levels.end(); ++it )
		translatedLevels+=i18n((*it).utf8());
	levelAct->setItems(translatedLevels);
}

void KHangMan::slotSetFullScreen( bool set )
{
	if( set ){
		showFullScreen();
		menuBar()->hide();
		m_pFullScreen->setText( i18n( "Exit Full-Screen Mode" ) );
		m_pFullScreen->setIcon( "window_nofullscreen" );
	} else {
		showNormal();
		menuBar()->show();
		m_pFullScreen->setText( i18n( "Full-Screen Mode" ) );
		m_pFullScreen->setIcon( "window_fullscreen" );
	}
}

void KHangMan::loadLangToolBar()
{
	if (m_view->language == "en" || m_view->language == "it" || m_view->language == "nl" )
	noCharBool = true;
	else noCharBool = false;
	if (secondToolbar->isVisible() && !noCharBool)
	    m_bCharToolbar=true;
	secondToolbar->clear();

	 //accentsAct->setEnabled(m_view->m_accent);
	 
	if (m_view->language == "ca")	{
		secondToolbar->insertButton ("a_grave.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAgrave()), true,  i18n("Try ")+ QString::fromUtf8("à", -1), 1 );
		secondToolbar->insertButton ("c_cedil.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteCcedil()), true,  i18n("Try ")+ QString::fromUtf8("ç", -1), 2 );
		secondToolbar->insertButton ("e_grave.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteEgrave()), true,  i18n("Try ")+ QString::fromUtf8("è", -1), 3 );
		secondToolbar->insertButton ("i_acute.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true,  i18n("Try ")+ QString::fromUtf8("í", -1), 4 );
		secondToolbar->insertButton ("o_grave.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteOgrave()), true,  i18n("Try ")+ QString::fromUtf8("ò", -1), 5 );
		secondToolbar->insertButton ("o_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true,  i18n("Try ")+ QString::fromUtf8("ó", -1), 6 );
		secondToolbar->insertButton ("u_acute.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteUacute()), true,  i18n("Try ")+ QString::fromUtf8("ú", -1), 7 );
		secondToolbar->insertButton ("u_umlaut.png", 80, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ü", -1) ,8);
	}
	else if (m_view->language == "es")	{
		secondToolbar->insertButton ("a_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAacute()), true,  i18n("Try ")+ QString::fromUtf8("á", -1), 1 );
		secondToolbar->insertButton ("e_acute.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true,  i18n("Try ")+ QString::fromUtf8("é", -1), 2 );
		secondToolbar->insertButton ("i_acute.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true,  i18n("Try ")+ QString::fromUtf8("í", -1), 3 );
		secondToolbar->insertButton ("n_tilde.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteNtilde()), true,  i18n("Try ")+ QString::fromUtf8("ñ", -1), 4 );
		secondToolbar->insertButton ("o_acute.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true,  i18n("Try ")+ QString::fromUtf8("ó", -1), 5 );
		secondToolbar->insertButton ("u_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteUacute()), true, i18n("Try ")+ QString::fromUtf8("ú", -1), 6 );
		secondToolbar->insertButton ("u_umlaut.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ü", -1), 7 );
	}
	else if (m_view->language == "da")	{
		secondToolbar->insertButton ("o_cross.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteOcross()), true,  i18n("Try ")+ QString::fromUtf8("ø", -1), 1 );
		secondToolbar->insertButton ("a_withe.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAwithe()), true,  i18n("Try ")+ QString::fromUtf8("æ", -1), 2 );
		secondToolbar->insertButton ("a_circle.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteAcircle()), true,  i18n("Try ")+ QString::fromUtf8("å", -1), 3 );
	}
	else if (m_view->language == "fi")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ä", -1), 1 );
		secondToolbar->insertButton ("o_umlaut.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ö", -1), 2 );
	}
	else if (m_view->language == "sv")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ä", -1), 1 );
		secondToolbar->insertButton ("a_circle.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAcircle()), true,  i18n("Try ")+ QString::fromUtf8("å", -1), 2 );
		secondToolbar->insertButton ("o_umlaut.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ö", -1), 3 );
	}
	else if (m_view->language == "pt" || m_view->language == "pt_BR")	{
		secondToolbar->insertButton ("a_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAacute()), true,  i18n("Try ")+ QString::fromUtf8("á", -1), 1 );
		secondToolbar->insertButton ("a_tilde.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAtilde()), true,  i18n("Try ")+ QString::fromUtf8("ã", -1), 2 );
		secondToolbar->insertButton ("c_cedil.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteCcedil()), true,  i18n("Try ")+ QString::fromUtf8("ç", -1), 3 );
		secondToolbar->insertButton ("e_acute.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true,  i18n("Try ")+ QString::fromUtf8("é", -1), 4 );
		secondToolbar->insertButton ("e_circ.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteEcirc()), true,  i18n("Try ")+ QString::fromUtf8("ê", -1), 5 );
		secondToolbar->insertButton ("i_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true,  i18n("Try ")+ QString::fromUtf8("í", -1), 6 );
		secondToolbar->insertButton ("o_acute.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true,  i18n("Try ")+ QString::fromUtf8("ó", -1), 7 );
		secondToolbar->insertButton ("o_circ.png", 80, SIGNAL( clicked() ), this, SLOT( slotPasteOcirc()), true,  i18n("Try ")+ QString::fromUtf8("ô", -1), 8 );
		secondToolbar->insertButton ("o_tilde.png", 90, SIGNAL( clicked() ), this, SLOT( slotPasteOtilde()), true,  i18n("Try ")+ QString::fromUtf8("õ", -1), 9 );
	}
	else if (m_view->language == "fr")	{
		secondToolbar->insertButton ("e_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true,  i18n("Try ")+ QString::fromUtf8("é", -1), 1 );
		secondToolbar->insertButton ("e_grave.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteEgrave()), true,  i18n("Try ")+ QString::fromUtf8("è", -1), 2 );
	}
	else if (m_view->language == "de")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ä", -1), 1 );
		secondToolbar->insertButton ("o_umlaut.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ö", -1), 2);  
		secondToolbar->insertButton ("u_umlaut.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ü", -1), 3 );
		secondToolbar->insertButton ("sz_lig.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteSzlig()), true,  i18n("Try ")+ QString::fromUtf8("ß", -1), 4 );
	}
	else if (m_view->language == "cs")	{
		secondToolbar->insertButton ("a_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAacute()), true,  i18n("Try ")+ QString::fromUtf8("á", -1), 1 );
		secondToolbar->insertButton ("c_caron.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteCcaron()), true,  i18n("Try ")+ QString::fromUtf8("č", -1), 2 );
		secondToolbar->insertButton ("d_apos.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteDapos()), true,  i18n("Try ")+ QString::fromUtf8("ď", -1), 3);  
		secondToolbar->insertButton ("e_acute.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true,  i18n("Try ")+ QString::fromUtf8("é", -1), 4 );
		secondToolbar->insertButton ("e_caron.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteEcaron()), true,  i18n("Try ")+ QString::fromUtf8("ĕ", -1), 5 );
		secondToolbar->insertButton ("i_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true,  i18n("Try ")+ QString::fromUtf8("í", -1), 6 );
		secondToolbar->insertButton ("n_caron.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteNcaron()), true,  i18n("Try ")+ QString::fromUtf8("ň", -1), 7 );
		secondToolbar->insertButton ("o_acute.png", 80, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true,  i18n("Try ")+ QString::fromUtf8("ó", -1), 8 );
		secondToolbar->insertButton ("o_umlaut.png", 90, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ö", -1), 9); 
		secondToolbar->insertButton ("u_umlaut.png", 100, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true,  i18n("Try ")+ QString::fromUtf8("ü", -1), 10 );
		secondToolbar->insertButton ("r_caron.png", 110, SIGNAL( clicked() ), this, SLOT( slotPasteRcaron()), true,  i18n("Try ")+ QString::fromUtf8("ř", -1), 11 );
		secondToolbar->insertButton ("s_caron.png", 120, SIGNAL( clicked() ), this, SLOT( slotPasteScaron()), true,  i18n("Try ")+ QString::fromUtf8("š", -1), 12 );
		secondToolbar->insertButton ("u_dot.png", 130, SIGNAL( clicked() ), this, SLOT( slotPasteUdot()), true,  i18n("Try ")+ QString::fromUtf8("ů", -1), 13 );
		secondToolbar->insertButton ("y_acute.png", 140, SIGNAL( clicked() ), this, SLOT( slotPasteYacute()), true,  i18n("Try ")+ QString::fromUtf8("ý", -1), 14 );
		secondToolbar->insertButton ("z_caron.png", 150, SIGNAL( clicked() ), this, SLOT( slotPasteZcaron()), true, i18n("Try ")+ QString::fromUtf8("ž", -1), 15 );
	}
	else if (m_view->language == "tg")	{
		secondToolbar->insertButton ("x_desc.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteXdesc()), true,  i18n("Try ")+ QString::fromUtf8("ҳ", -1), 1 );
		secondToolbar->insertButton ("y_macron.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteYmacron()), true,  i18n("Try ")+ QString::fromUtf8("ӯ", -1), 2 );
		secondToolbar->insertButton ("che_desc.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteChedesc()), true,  i18n("Try ")+ QString::fromUtf8("ҷ", -1), 3);  
		secondToolbar->insertButton ("i_macron.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteImacron()), true,  i18n("Try ")+ QString::fromUtf8("ӣ", -1), 4 );
		secondToolbar->insertButton ("ghe_stroke.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteGhestroke()), true,  i18n("Try ")+ QString::fromUtf8("ғ", -1), 5 );
		secondToolbar->insertButton ("ka_desc.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteKadesc()), true,  i18n("Try ")+ QString::fromUtf8("қ", -1), 6 );
	}
	else if (m_view->language == "sl")	{
		secondToolbar->insertButton ("c_caron.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteCcaron()), true,  i18n("Try ")+ QString::fromUtf8("č", -1), 1 );
		secondToolbar->insertButton ("s_caron.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteScaron()), true,  i18n("Try ")+ QString::fromUtf8("š", -1), 2 );
		secondToolbar->insertButton ("z_caron.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteZcaron()), true, i18n("Try ")+ QString::fromUtf8("ž", -1), 3 );
	}
	else if (m_view->language == "sr@Latn")	{
		secondToolbar->insertButton ("c_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteCacute()), true,  i18n("Try ")+ QString::fromUtf8("ć", -1), 1 );
		secondToolbar->insertButton ("c_caron.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteCcaron()), true,  i18n("Try ")+ QString::fromUtf8("č", -1), 2 );
		secondToolbar->insertButton ("d_bar.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteDbar()), true,  i18n("Try ")+ QString::fromUtf8("đ", -1), 3); 
		secondToolbar->insertButton ("s_caron.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteScaron()), true,  i18n("Try ")+ QString::fromUtf8("š", -1), 4 );
		secondToolbar->insertButton ("z_caron.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteZcaron()), true, i18n("Try ")+ QString::fromUtf8("ž", -1), 5 );
	}
	if (m_bCharToolbar) {
		secondToolbar->show();
	}
	else secondToolbar->hide();
	Prefs::setShowCharToolbar( !secondToolbar->isVisible());
	Prefs::writeConfig();
	if (noCharBool)//no special chars in those languages
		secondToolbar->hide();
}

void KHangMan::slotPasteAacute()
{
	m_view->charWrite->setText(QString::fromUtf8("á", -1));
}

void KHangMan::slotPasteAcircle()
{
	m_view->charWrite->setText(QString::fromUtf8("å", -1));
}

void KHangMan::slotPasteAgrave()
{
	m_view->charWrite->setText(QString::fromUtf8("à", -1));
}

void KHangMan::slotPasteAtilde()
{
	m_view->charWrite->setText(QString::fromUtf8("ã", -1));
}

void KHangMan::slotPasteAumlaut()
{
	m_view->charWrite->setText(QString::fromUtf8("ä", -1));
}

void KHangMan::slotPasteAwithe()
{
	m_view->charWrite->setText(QString::fromUtf8("æ", -1));
}

void KHangMan::slotPasteCacute()
{
	m_view->charWrite->setText(QString::fromUtf8("ć", -1));
}

void KHangMan::slotPasteCcaron()
{
	m_view->charWrite->setText(QString::fromUtf8("č", -1));
}

void KHangMan::slotPasteCcedil()
{
	m_view->charWrite->setText(QString::fromUtf8("ç", -1));
}

void KHangMan::slotPasteDapos()
{
	m_view->charWrite->setText(QString::fromUtf8("ď", -1));
}

void KHangMan::slotPasteDbar()
{
	m_view->charWrite->setText(QString::fromUtf8("đ", -1));
}

void KHangMan::slotPasteEacute()
{
	m_view->charWrite->setText(QString::fromUtf8("é", -1));
}

void KHangMan::slotPasteEcaron()
{
	m_view->charWrite->setText(QString::fromUtf8("ĕ", -1));
}

void KHangMan::slotPasteEcirc()
{
	m_view->charWrite->setText(QString::fromUtf8("ê", -1));
}

void KHangMan::slotPasteEgrave()
{
	m_view->charWrite->setText(QString::fromUtf8("è", -1));
}

void KHangMan::slotPasteIacute()
{
	m_view->charWrite->setText(QString::fromUtf8("í", -1));
}

void KHangMan::slotPasteIgrave()
{
	m_view->charWrite->setText(QString::fromUtf8("ì", -1));
}

void KHangMan::slotPasteNcaron()
{
	m_view->charWrite->setText(QString::fromUtf8("ň", -1));
}

void KHangMan::slotPasteNtilde()
{
	m_view->charWrite->setText(QString::fromUtf8("ñ", -1));
}

void KHangMan::slotPasteOgrave()
{
	m_view->charWrite->setText(QString::fromUtf8("ò", -1));
}

void KHangMan::slotPasteOacute()
{
	m_view->charWrite->setText(QString::fromUtf8("ó", -1));
}

void KHangMan::slotPasteOcross()
{
	m_view->charWrite->setText(QString::fromUtf8("ø", -1));
}

void KHangMan::slotPasteOumlaut()
{
	m_view->charWrite->setText(QString::fromUtf8("ö", -1));
}

void KHangMan::slotPasteOtilde()
{
	m_view->charWrite->setText(QString::fromUtf8("õ", -1));
}

void KHangMan::slotPasteOcirc()
{
	m_view->charWrite->setText(QString::fromUtf8("ô", -1));
}

void KHangMan::slotPasteRcaron()
{
	m_view->charWrite->setText(QString::fromUtf8("ř", -1));
}

void KHangMan::slotPasteScaron()
{
	m_view->charWrite->setText(QString::fromUtf8("š", -1));
}

void KHangMan::slotPasteUacute()
{
	m_view->charWrite->setText(QString::fromUtf8("ú", -1));
}

void KHangMan::slotPasteUdot()
{
	m_view->charWrite->setText(QString::fromUtf8("ů", -1));
}

void KHangMan::slotPasteUumlaut()
{
	m_view->charWrite->setText(QString::fromUtf8("ü", -1));
}

void KHangMan::slotPasteSzlig()
{
	m_view->charWrite->setText(QString::fromUtf8("ß", -1));
}

void KHangMan::slotPasteYacute()
{
	m_view->charWrite->setText(QString::fromUtf8("ý", -1));
}

void KHangMan::slotPasteZcaron()
{
	m_view->charWrite->setText(QString::fromUtf8("ž", -1));
}

void KHangMan::slotPasteXdesc()
{
	m_view->charWrite->setText(QString::fromUtf8("ҳ", -1));
}

void KHangMan::slotPasteYmacron()
{
	m_view->charWrite->setText(QString::fromUtf8("ӯ", -1));
}

void KHangMan::slotPasteChedesc()
{
	m_view->charWrite->setText(QString::fromUtf8("ҷ", -1));
}

void KHangMan::slotPasteImacron()
{
	m_view->charWrite->setText(QString::fromUtf8("ӣ", -1));
}

void KHangMan::slotPasteGhestroke()
{
	m_view->charWrite->setText(QString::fromUtf8("ғ", -1));
}

void KHangMan::slotPasteKadesc()
{
	m_view->charWrite->setText(QString::fromUtf8("қ", -1));
}

void KHangMan::slotClose()
{
	Prefs::setShowCharToolbar(secondToolbar->isVisible());
	Prefs::setAccentedLetters(m_view->accent_b);
	Prefs::setHint(m_view->hintBool);
	Prefs::writeConfig();
	close();
}

void KHangMan::slotAccents()
{
 	m_view->accent_b = Prefs::accentedLetters();
	if (m_view->accent_b)
		changeStatusbar(i18n("Type accented letters"), IDS_ACCENTS);
	else changeStatusbar("", IDS_ACCENTS);
	loadLangToolBar();
	newGame();
}

void KHangMan::slotHint()
{
	kdDebug() << "kvtmlBool: " << m_view->kvtmlBool << endl;
	if (m_view->kvtmlBool)  {
		if(dialog) mNormal->kcfg_Hint->setEnabled(true);
		changeStatusbar(i18n("You can enable hints in Settings"), IDS_HINT);
	}
        m_view->hintBool=Prefs::hint();
	if ((m_view->kvtmlBool) && (m_view->hintBool)) 
		changeStatusbar(i18n("Hint enabled on right-click"), IDS_HINT);	
	kdDebug() << Prefs::hint() << "hint" << endl;
	if (!Prefs::hint())
		changeStatusbar(i18n("You can enable hints in Settings"), IDS_HINT);
}

void KHangMan::enableHint(bool m_bool)
{
	if (m_bool) {
		m_view->kvtmlBool = true;
		if(dialog) mNormal->kcfg_Hint->setEnabled(true);
	}
	else
	{
		m_view->kvtmlBool = false;
		changeStatusbar("", IDS_HINT);
		if(dialog) mNormal->kcfg_Hint->setEnabled(false);
	}
	slotHint();
}

void KHangMan::setAccentBool()
{
	if (m_view->language=="es" || m_view->language =="pt" || m_view->language == "ca" || m_view->language == "pt_BR") {
    		m_view->m_accent = true;
		if (dialog)
			mNormal->kcfg_AccentedLetters->setEnabled(true);
	}	
    	else {
		m_view->m_accent = false;
		if (dialog)
			mNormal->kcfg_AccentedLetters->setEnabled(false);
	}
}

void KHangMan::downloadNewStuff()
{
	if ( !mNewStuff ) 
		mNewStuff = new KHNewStuff( m_view );
 	mNewStuff->download();
}

void KHangMan::setLanguages()
{
	m_languages.clear();
	m_languageNames.clear();
	m_sortedNames.clear();
	kdDebug() << "in SetLanguages "<< endl;
	//the program scans in khangman/data/ to see what languages data is found
	QStringList mdirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
	if (mdirs.isEmpty()) return;
	kdDebug() << "mdirs= "<< mdirs << endl;
	for (QStringList::Iterator it =mdirs.begin(); it !=mdirs.end(); ++it ) {
		QDir dir(*it);
		m_languages += dir.entryList(QDir::Dirs, QDir::Name);
		m_languages.remove(m_languages.find("."));
		m_languages.remove(m_languages.find(".."));	
	}	
	if (m_languages.isEmpty()) return;
	//suppress duplicated entries of same language dir
	for (uint i=0;  i<m_languages.count(); i++)
	{
		QString tmp = m_languages[i];
		if (m_languages.grep(m_languages[i]).count() >1)  {
			m_languages.remove(m_languages .find(tmp));
		}
	}
	//write the present languages in config so they cannot be downloaded
	KConfig *config=kapp->config();
	config->setGroup("KNewStuffStatus");
	for (uint i=0;  i<m_languages.count(); i++)
	{
		QString tmp = m_languages[i];
		if (!config->readEntry(tmp))
			config->writeEntry(tmp, "Thu Apr 8 2004");
	}
	//we look in $KDEDIR/share/locale/all_languages from /kdelibs/kdecore/all_languages
	//to find the name of the country
	//corresponding to the code and the language the user set
	KConfig entry(locate("locale", "all_languages"));
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
	m_sortedNames = m_languageNames;
}

void KHangMan::optionsPreferences()
{
	if ( KConfigDialog::showDialog( "settings" ) ) 
        	return; 
 
	//KConfigDialog didn't find an instance of this dialog, so lets create it : 
	dialog = new KConfigDialog( this, "settings",  Prefs::self() );
	//dialog->setModal(true); //makes it modal even if it's not the default
	mNormal =  new normal( 0, "Kids Settings" ); 
	dialog->addPage(mNormal, i18n("Kids Settings"), "configure");
	mNormal->kcfg_Transparent->setEnabled( modeAct->currentItem() != 0);
	if (m_view->kvtmlBool) 
	      mNormal->kcfg_Hint->setEnabled( true);
	if (m_view->m_accent) mNormal->kcfg_AccentedLetters->setEnabled(true);
	dialog->addPage(new advanced(0, "Advanced"), i18n("Advanced Settings"), "wizard");
	connect(dialog, SIGNAL(settingsChanged()), this, SLOT(updateSettings()));
	
	dialog->show();	
}

void KHangMan::updateSettings()
{
     	// Transparency
	if (m_view->transparent != Prefs::transparent())  {
      		m_view->transparent = Prefs::transparent();
		kdDebug() << "Change Transparent mode   " << endl;
      		m_view->slotTransparent();
	}
    	// Softer Pictures
	if (m_view->softer != Prefs::softer()) {
    		m_view->softer = Prefs::softer();	
		kdDebug() << "Change Softer mode   " << endl;	
		m_view->slotSofter();
	}	
	//Accented Letters
	if (m_view->accent_b != Prefs::accentedLetters())
		slotAccents();
	
	//Enable hint or not
	if (m_view->hintBool != Prefs::hint())  {
    		m_view->hintBool= Prefs::hint();
		kdDebug() << "Change Hint mode   " << endl;
		slotHint();
	}
	//One letter only mode
	if (m_view->b_oneLetter != Prefs::oneLetter())  {
		kdDebug() << "Change One Letter mode   " << endl;
		m_view->b_oneLetter = Prefs::oneLetter();
		newGame();
	}
}

#include "khangman.moc"
