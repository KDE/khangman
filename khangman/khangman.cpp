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

//Qt headers
#include <qdir.h>
#include <qlineedit.h>
//KDE headers
#include <kactionclasses.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kedittoolbar.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
//Project headers
#include "khangman.h"

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

    //the program scans in khangman/data/ to see what languages data is found
    QStringList dirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
    for (QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
	QDir dir(*it);
	m_languages += dir.entryList(QDir::Dirs, QDir::Name);
    }
    m_languages.remove(m_languages.find("."));
    m_languages.remove(m_languages.find(".."));
    //we look in $KDEDIR/share/locale/all_languages from /kdelibs/kdecore/all_languages
    //to find the name of the country
    //corresponding to the code and the language the user set
    KConfig entry(locate("locale", "all_languages"));
    for (QStringList::Iterator it = m_languages.begin(); it != m_languages.end(); ++it) {
	entry.setGroup(*it);
	m_languageNames.append(entry.readEntry("Name"));
    }
    m_sortedNames = m_languageNames;
    m_sortedNames.sort();

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
    KStdAction::quit(this, SLOT(slotClose()), actionCollection());

    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);
    langAct = new KSelectAction(i18n("&Languages"), 0, this, SLOT(slotLanguage()), actionCollection(), "combo_lang");
    langAct->setItems(m_sortedNames);
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[selectedLanguage]));

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    m_bFullScreen = false;
#ifdef KHANGMAN_KDE_3_2_0
    m_pFullScreen = KStdAction::fullScreen( 0, 0, actionCollection(), this);
    connect( m_pFullScreen, SIGNAL( toggled( bool )), this, SLOT( slotSetFullScreen( bool )));
#else
    m_pFullScreen = new KToggleAction( i18n( "&Full-Screen Mode" ), "window_fullscreen", CTRL+SHIFT+Key_F, 0,
        0, actionCollection(), "fullscreen" );
    connect( m_pFullScreen, SIGNAL( toggled( bool )), this, SLOT( slotSetFullScreen( bool )));
#endif
    transAct = new KToggleAction(i18n("&Transparent Pictures"), CTRL+Key_T, this, SLOT(slotTransparent()), actionCollection(), "transparent");
    softAct = new KToggleAction(i18n("&Softer Hangman Pictures"), CTRL+Key_S, this, SLOT(slotSofter()), actionCollection(), "softer");
    hintAct = new KToggleAction(i18n("Enable &Hint"), CTRL+Key_H, this, SLOT(slotChooseHint()), actionCollection(), "hint");
    accentsAct = new KToggleAction(i18n("Type A&ccented Letters"), CTRL+Key_A, this, SLOT(slotAccents()), actionCollection(), "accents");

    levelAct = new KSelectAction(i18n("Level"), 0, this, SLOT(changeLevel()), actionCollection(), "combo_level");
    levelAct->setToolTip(i18n( "Choose the level" ));
    levelAct->setWhatsThis(i18n( "Choose the level of difficulty" ));

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
    for (uint index = 0; index < m_sortedNames.count(); index++)
	langPopup->insertItem(m_sortedNames[index], m_languageNames.findIndex(m_sortedNames[index]), index);
    langPopup->setItemChecked(selectedLanguage, true);
    connect(langPopup, SIGNAL(activated(int)), this, SLOT(changeLanguage(int)) );
}

void KHangMan::saveProperties(KConfig *)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored
}

void KHangMan::readProperties(KConfig *)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'
}

void KHangMan::newGame()
{
 	// Start a new game
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

//when combo is changed, levelString is changed
//and written in config
void KHangMan::changeLevel()
{
	static const char *levelStrings[] = {
		I18N_NOOP("Easy"),
		I18N_NOOP("Medium"),
		I18N_NOOP("Hard"),
		I18N_NOOP("Animals"),
	};
	currentLevel = levelAct->currentItem();
	levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).lower());

	m_view->levelFile = levelString +".txt";
	changeStatusbar(i18n("Level: ") + levelString, IDS_LEVEL);
        KConfigBase *conf = kapp->config();
        if( conf ) {
     	  conf->setGroup( "General" );
     	  conf->writeEntry("level", currentLevel);
	  conf->writeEntry("levelFile", m_view->levelFile);
	}
	newGame();
}

//When changing background, the game stays as it is
void KHangMan::changeMode()
{
	switch (modeAct->currentItem()) {
    	case 0:
	 		modeString="nobg";
			m_view->slotNoBkgd();
    		break;

    	case 1:
      			modeString="blue";
			m_view->slotSetPixmap(m_view->bluePix);
   			break;
    	case 2:
      		modeString="nature";
			m_view->slotSetPixmap(m_view->naturePix);
   			break;
	}
        transAct->setEnabled( modeAct->currentItem() != 0 );
	KConfigBase *conf = kapp->config();
        if( conf ) {
     	  conf->setGroup( "General" );
	  conf->writeEntry( "mode", modeString);
        }
}

//read settings from config file khangmanrc
void KHangMan::loadSettings()
{
    // Language
    //load the kdeglobals config file - safer way would be to load that one read-only
    KConfigBase *globalConf = KGlobal::config();
    globalConf->setGroup("Locale");
    userLanguage = globalConf->readEntry("Language");
    //keep only the first 2 characters
    userLanguage = userLanguage.left(2);
    setSelectedLanguage(userLanguage);

    KConfig *config = kapp->config();
    config->setGroup("Language");
    selectedLanguage = config->readNumEntry("selectedLanguage", defaultLang);
    if (selectedLanguage >= (int) m_languages.count())
                selectedLanguage = 3;
     setLanguage(selectedLanguage);

    config->setGroup( "General" );
    // Level
    currentLevel = config->readNumEntry("level", 1); //default is easy

    // Show/hide characters toolbar
    m_bCharToolbar = config->readBoolEntry( "showCharToolbar", false);
    if (m_bCharToolbar) secondToolbar->show();
    else secondToolbar->hide();
    //see if language has special accented letters
    setAccentBool();
    m_view->accent_b = config->readBoolEntry( "accentedLetters", false);
    accentsAct->setChecked(m_view->accent_b);
    if (m_view->m_accent && m_view->accent_b)
	changeStatusbar(i18n("Type accented letters"), IDS_ACCENTS);
    
    loadDataFiles();
    //Enable hint or not
    m_view->hintBool= config->readBoolEntry( "hint", true);
    if (m_view->hintBool) hintAct->setChecked(true);
    else hintAct->setChecked(false);
    
    m_view->levelFile = config->readEntry( "levelFile", "easy.txt");
    levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).lower());
    setLevel_WindowState();

     // Background
    QString oldMode = modeString;
    modeString = config->readEntry("mode", "nobg");
    if(oldMode != modeString)
    	setMode_WindowState();

      // Transparency
     config->setGroup("Settings");
      if(m_view->transparent != config->readBoolEntry( "transparent", true)) {
      m_view->transparent = config->readBoolEntry( "transparent", true);
      m_view->slotTransparent();
    }
    transAct->setChecked(m_view->transparent);

    // Softer Pictures
    config->setGroup("Settings");
    if(m_view->softer != config->readBoolEntry( "softer", true)) {
    m_view->softer = config->readBoolEntry( "softer", true);
    m_view->slotSofter();
    }
 }

void KHangMan::setSelectedLanguage(QString mLanguage)
{
    defaultLang = m_languages.findIndex(mLanguage);
    if (defaultLang == -1)
	defaultLang = 3; //if no default, set it to English
}

//when config is read, set the KComboBox to the right level
//and update statusbar
void KHangMan::setLevel_WindowState()
{
    if (currentLevel>levels.count())
        currentLevel = levels.count();
    levelAct->setCurrentItem(currentLevel);
    changeStatusbar(i18n("Level: ") + i18n(levelString.latin1()), IDS_LEVEL);
}

//when config is read, set the KComboBox to the right background
//and call the corresponding slot in the main view to set the background
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
    transAct->setEnabled( modeAct->currentItem() != 0 );
}

void KHangMan::slotLanguage()
{
    changeLanguage(m_languageNames.findIndex(m_sortedNames[langAct->currentItem()]));
}

// Switch to another language using Languages menu
void KHangMan::changeLanguage(int newLanguage)
{
    // Do not accept to switch to same language
    if (newLanguage == selectedLanguage)
	return;

    // Unselect preceding language
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[newLanguage]));
    for (int id = 0; id < (int) m_languageNames.count(); id++)
    	langPopup->setItemChecked(id, id == newLanguage);

    selectedLanguage = newLanguage;
    // Change language in the config file
    KConfigBase *conf = kapp->config();
    if( conf ) {
	conf->setGroup("Language");
	conf->writeEntry("selectedLanguage", selectedLanguage);
        conf->writeEntry("defaultLang", defaultLang);
    }
    //load the different data files in the Level combo for the new language
    loadDataFiles();
    bool fileExistBool = false;
    //check if the name of the file exists in the new language. If not, set it to Easy.
    //TODO: save level per language
    for (int id = 0; id < (int) levels.count(); id++)
    if (levels[id].lower()==levelString)
    	fileExistBool = true;
    if (!fileExistBool) {
    	levelString = "easy";
	m_view->levelFile = levelString +".txt";
	currentLevel = 1;
	changeStatusbar(i18n("Level: ") + levelString, IDS_LEVEL);
        KConfigBase *conf = kapp->config();
        if( conf ) {
     		conf->setGroup( "General" );
     	  	conf->writeEntry("level", currentLevel);
	  	conf->writeEntry("levelFile", m_view->levelFile);
	}
    }
    //update the Levels in Level combobox as well
    setLevel_WindowState();
    setLanguage(newLanguage);
    if (m_view->hintBool && m_view->kvtmlBool) 
    	hintAct->setChecked(true);
    slotHint();
    setAccentBool();
    if (m_view->m_accent) 
    		slotAccents();
    else {
    	changeStatusbar("", IDS_ACCENTS);
	loadLangToolBar();
	newGame();
    }
}

void KHangMan::setLanguage(int lang)
{
    	if (lang >= 0 && lang < (int) m_languages.count()) {
		m_view->language = m_languages[lang];
		changeStatusbar(i18n("Language: ")+m_languageNames[lang], IDS_LANG);
    	}
}

void KHangMan::slotTransparent()
{
        m_view->transparent = transAct->isChecked();
        m_view->slotTransparent();
        //write transparent in the config file
        KConfigBase *conf = kapp->config();
    	if( conf ) {
        	conf->setGroup( "Settings" );
        	conf->writeEntry( "transparent", m_view->transparent);
        }
}

void KHangMan::loadDataFiles()
{
    //build the Level combobox menu dynamically depending of the data for each language
    //TODO: ask Stefan about i18n() for dynamic items
    //should the file names be diesctly translated like animaux.txt in Fr for animals.txt?
    levels.clear();//initialize QStringList levels
    KStandardDirs *dirs = KGlobal::dirs();
    QStringList mfiles = dirs->findAllResources("data","khangman/data/" + m_languages[selectedLanguage] + "/*.txt");
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
}

void KHangMan::slotSetFullScreen( bool set )
{
   m_bFullScreen = set;
   if( m_bFullScreen ){
      showFullScreen();
      menuBar()->hide();
#ifndef KHANGMAN_KDE_3_2_0
      m_pFullScreen->setText( i18n( "Exit Full-Screen Mode" ) );
      m_pFullScreen->setIcon( "window_nofullscreen" );
#endif
   } else {
      showNormal();
      menuBar()->show();
#ifndef KHANGMAN_KDE_3_2_0
      m_pFullScreen->setText( i18n( "Full-Screen Mode" ) );
      m_pFullScreen->setIcon( "window_fullscreen" );
#endif
   }
}

void KHangMan::slotSofter()
{
	m_view->softer = softAct->isChecked();
        m_view->slotSofter();
        //write softer in the config file
        KConfigBase *conf = kapp->config();
    	if( conf ) {
        	conf->setGroup( "Settings" );
        	conf->writeEntry( "softer", m_view->softer);
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

	if (m_view->m_accent)
     		accentsAct->setEnabled(true);
    	else accentsAct->setEnabled(false);
	 
	if (m_view->language == "ca")	{
		secondToolbar->insertButton ("a_grave.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAgrave()), true, i18n(QString("Try à").utf8()), 1 );
		secondToolbar->insertButton ("c_cedil.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteCcedil()), true, i18n(QString("Try ç").utf8()), 2 );
		secondToolbar->insertButton ("e_grave.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteEgrave()), true, i18n(QString("Try è").utf8()), 3 );
		secondToolbar->insertButton ("i_acute.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true, i18n(QString("Try í").utf8()), 4 );
		secondToolbar->insertButton ("o_grave.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteOgrave()), true, i18n(QString("Try ò").utf8()), 5 );
		secondToolbar->insertButton ("o_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true, i18n(QString("Try ó").utf8()), 6 );
		secondToolbar->insertButton ("u_acute.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteUacute()), true, i18n(QString("Try ú").utf8()), 7 );
		secondToolbar->insertButton ("u_umlaut.png", 80, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true, i18n(QString("Try ü").utf8()),8);
	}
	else if (m_view->language == "es")	{
		secondToolbar->insertButton ("a_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAacute()), true, i18n(QString("Try á").utf8()), 1 );
		secondToolbar->insertButton ("e_acute.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true, i18n(QString("Try é").utf8()), 2 );
		secondToolbar->insertButton ("i_acute.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true, i18n(QString("Try í").utf8()), 3 );
		secondToolbar->insertButton ("n_tilde.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteNtilde()), true, i18n(QString("Try ñ").utf8()), 4 );
		secondToolbar->insertButton ("o_acute.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true, i18n(QString("Try ó").utf8()), 5 );
		secondToolbar->insertButton ("u_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteUacute()), true, i18n(QString("Try ú").utf8()), 6 );
		secondToolbar->insertButton ("u_umlaut.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true, i18n(QString("Try ü").utf8()), 7 );
	}
	else if (m_view->language == "da")	{
		secondToolbar->insertButton ("o_cross.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteOcross()), true, i18n(QString("Try ø").utf8()), 1 );
		secondToolbar->insertButton ("a_withe.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAwithe()), true, i18n(QString("Try æ").utf8()), 2 );
		secondToolbar->insertButton ("a_circle.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteAcircle()), true, i18n(QString("Try å").utf8()), 3 );
	}
	else if (m_view->language == "fi")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true, i18n(QString("Try ä").utf8()), 1 );
		secondToolbar->insertButton ("o_umlaut.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true, i18n(QString("Try ö").utf8()), 2 );
	}
	else if (m_view->language == "sv")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true, i18n(QString("Try ä").utf8()), 1 );
		secondToolbar->insertButton ("a_circle.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAcircle()), true, i18n(QString("Try å").utf8()), 2 );
		secondToolbar->insertButton ("o_umlaut.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true, i18n(QString("Try ö").utf8()), 3 );
	}
	else if (m_view->language == "pt" || m_view->language == "pt_BR")	{
		secondToolbar->insertButton ("a_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAacute()), true, i18n(QString("Try á").utf8()), 1 );
		secondToolbar->insertButton ("a_tilde.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteAtilde()), true, i18n(QString("Try ã").utf8()), 2 );
		secondToolbar->insertButton ("c_cedil.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteCcedil()), true, i18n(QString("Try ç").utf8()), 3 );
		secondToolbar->insertButton ("e_acute.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true, i18n(QString("Try é").utf8()), 4 );
		secondToolbar->insertButton ("e_circ.png", 50, SIGNAL( clicked() ), this, SLOT( slotPasteEcirc()), true, i18n(QString("Try ê").utf8()), 5 );
		secondToolbar->insertButton ("i_acute.png", 60, SIGNAL( clicked() ), this, SLOT( slotPasteIacute()), true, i18n(QString("Try í").utf8()), 6 );
		secondToolbar->insertButton ("o_acute.png", 70, SIGNAL( clicked() ), this, SLOT( slotPasteOacute()), true, i18n(QString("Try ó").utf8()), 7 );
		secondToolbar->insertButton ("o_circ.png", 80, SIGNAL( clicked() ), this, SLOT( slotPasteOcirc()), true, i18n(QString("Try ô").utf8()), 8 );
		secondToolbar->insertButton ("o_tilde.png", 90, SIGNAL( clicked() ), this, SLOT( slotPasteOtilde()), true, i18n(QString("Try õ").utf8()), 9 );
	}
	else if (m_view->language == "fr")	{
		secondToolbar->insertButton ("e_acute.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteEacute()), true, i18n(QString("Try é").utf8()), 1 );
		secondToolbar->insertButton ("e_grave.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteEgrave()), true, i18n(QString("Try è").utf8()), 2 );
	}
	else if (m_view->language == "de")	{
		secondToolbar->insertButton ("a_umlaut.png", 10, SIGNAL( clicked() ), this, SLOT( slotPasteAumlaut()), true, i18n(QString("Try ä").utf8()), 1 );
		secondToolbar->insertButton ("o_umlaut.png", 20, SIGNAL( clicked() ), this, SLOT( slotPasteOumlaut()), true, i18n(QString("Try ö").utf8()), 2);  
		secondToolbar->insertButton ("u_umlaut.png", 30, SIGNAL( clicked() ), this, SLOT( slotPasteUumlaut()), true, i18n(QString("Try ü").utf8()), 3 );
		secondToolbar->insertButton ("sz_lig.png", 40, SIGNAL( clicked() ), this, SLOT( slotPasteSzlig()), true, i18n(QString("Try ß").utf8()), 4 );
	}
	if (m_bCharToolbar) {
		secondToolbar->show();
	}
	else secondToolbar->hide();
	if (noCharBool)//no special chars in those languages
		secondToolbar->hide();
}

void KHangMan::slotPasteCcedil()
{
	m_view->charWrite->setText("ç");
}

void KHangMan::slotPasteAgrave()
{
	m_view->charWrite->setText("à");
}

void KHangMan::slotPasteAacute()
{
	m_view->charWrite->setText("á");
}

void KHangMan::slotPasteIgrave()
{
	m_view->charWrite->setText("ì");
}

void KHangMan::slotPasteIacute()
{
	m_view->charWrite->setText("í");
}

void KHangMan::slotPasteOgrave()
{
	m_view->charWrite->setText("ò");
}

void KHangMan::slotPasteOacute()
{
	m_view->charWrite->setText("ó");
}

void KHangMan::slotPasteUacute()
{
	m_view->charWrite->setText("ú");
}

void KHangMan::slotPasteOcross()
{
	m_view->charWrite->setText("ø");
}

void KHangMan::slotPasteAcircle()
{
	m_view->charWrite->setText("å");
}

void KHangMan::slotPasteAwithe()
{
	m_view->charWrite->setText("æ");
}

void KHangMan::slotPasteOumlaut()
{
	m_view->charWrite->setText("ö");
}

void KHangMan::slotPasteAumlaut()
{
	m_view->charWrite->setText("ä");
}

void KHangMan::slotPasteAtilde()
{
	m_view->charWrite->setText("ã");
}

void KHangMan::slotPasteUumlaut()
{
	m_view->charWrite->setText("ü");
}

void KHangMan::slotPasteEacute()
{
	m_view->charWrite->setText("é");
}

void KHangMan::slotPasteNtilde()
{
	m_view->charWrite->setText("ñ");
}

void KHangMan::slotPasteEgrave()
{
	m_view->charWrite->setText("è");
}

void KHangMan::slotPasteEcirc()
{
	m_view->charWrite->setText("ê");
}

void KHangMan::slotPasteOtilde()
{
	m_view->charWrite->setText("õ");
}

void KHangMan::slotPasteOcirc()
{
	m_view->charWrite->setText("ô");
}

void KHangMan::slotPasteSzlig()
{
	m_view->charWrite->setText("ß");
}

void KHangMan::slotClose()
{
	// save if characters toolbar is shown or not
   	KConfigBase *conf = kapp->config();
        if( conf ) {
     	  conf->setGroup( "General" );
	  conf->writeEntry( "showCharToolbar", secondToolbar->isVisible());
	  if (m_view->m_accent)
	  	conf->writeEntry( "accentedLetters", m_view->accent_b);
	  if (m_view->kvtmlBool)
	  	conf->writeEntry( "hint", m_view->hintBool);
        }
	// then close the main window
	close();
}

void KHangMan::slotAccents()
{
 	m_view->accent_b = accentsAct->isChecked();

	if (m_view->accent_b)
		changeStatusbar(i18n("Type accented letters"), IDS_ACCENTS);
	else changeStatusbar("", IDS_ACCENTS);
	loadLangToolBar();
	newGame();
}

void KHangMan::restoreAccentConfig()
{
	accentsAct->setChecked(!m_view->accent_b);
	slotAccents();
}

void KHangMan::slotHint()
{
	if ((m_view->kvtmlBool) && (m_view->hintBool)) {
		hintAct->setChecked(true);
		changeStatusbar(i18n("Hint enabled on right-click"), IDS_HINT);
		}	
	else if (m_view->hintBool==false) {
		hintAct->setChecked(false);
		changeStatusbar("", IDS_HINT);
	}
}

void KHangMan::enableHint(bool m_bool)
{
	if (m_bool) {
		hintAct->setEnabled(true);
		m_view->kvtmlBool = true;
	}
	else
	{
		hintAct->setChecked(false);
		hintAct->setEnabled(false);
		m_view->kvtmlBool = false;
		changeStatusbar("", IDS_HINT);
	}
	slotHint();
}

void KHangMan::slotChooseHint()
{
	//hintBool=true if the user has choosen to have hints
	if (hintAct->isChecked())  {
		m_view->hintBool=true;
		changeStatusbar(i18n("Hint enabled on right-click"), IDS_HINT);
	}
	else {
		m_view->hintBool =false;
		changeStatusbar("", IDS_HINT);
	}
}

//set a bool to true for languages that allow Typing Accented Letters
void KHangMan::setAccentBool()
{
	if (m_view->language=="es" || m_view->language =="pt" || m_view->language == "ca" || m_view->language == "pt_BR") 
    		m_view->m_accent = true;
    	else m_view->m_accent = false;
}

#include "khangman.moc"
