/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */

//Qt headers
#include <qdir.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
//KDE headers
#include <kaccel.h>
#include <kaction.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kedittoolbar.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <kstddirs.h>
#include <ktoolbar.h>
//Project headers
#include "khangman.h"

const int IDS_LEVEL     = 100;
const int IDS_LANG       = 101;

KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{
    levelString = "";
    modeString = "";
    // set up the status bar
    statusBar( )->insertItem("   ",IDS_LEVEL, 0);
    statusBar( )->insertItem("   ",IDS_LANG, 0);
    statusBar()->show();

    //the program scans in khangman/data/ to see what languages data is found
    QStringList dirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
    for (QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
	QDir dir(*it);
	m_languages += dir.entryList(QDir::Dirs, QDir::Name);
    }
    m_languages.remove(m_languages.find("."));
    m_languages.remove(m_languages.find(".."));

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
    setLanguage(selectedLanguage);

     // then, setup our actions, must be done after the language search
    setupActions();
    loadSettings();

    setupLangMenu();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    newAct = new KAction(i18n("&New"), "file_new", CTRL+Key_N , this, SLOT(newGame()), actionCollection(), "file_new");
    KStdAction::quit(this, SLOT(close()), actionCollection());
    //KStdAction::quit(this, SLOT(slotQuit()), actionCollection());

    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);

    langAct = new KSelectAction(i18n("&Languages"), 0, this, SLOT(slotLanguage()), actionCollection(), "combo_lang");
    langAct->setItems(m_sortedNames);
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[selectedLanguage]));

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    transAct = new KToggleAction(i18n("&Transparent pictures"), CTRL+Key_T, this, SLOT(slotTransparent()), actionCollection(), "transparent");

    QStringList levels;
    levelAct = new KSelectAction(i18n("Level"), 0, this, SLOT(changeLevel()), actionCollection(), "combo_level");
    levels += i18n("Easy");
    levels += i18n("Medium");
    levels += i18n("Hard");
    levels += i18n("Animals");
    //levels += i18n("Own");
    levelAct->setItems(levels);
    levelAct->setToolTip(i18n( "Choose the level" ));
    levelAct->setWhatsThis(i18n( "Choose the level of difficulty" ));

    QStringList modes;
    modeAct = new KSelectAction(i18n("Look and Feel"), 0, this, SLOT(changeMode()),  actionCollection(), "combo_mode");
    modes += i18n("No Background");
    modes += i18n("Blue Theme");
    modes += i18n("Nature Theme");
    modeAct->setItems(modes);
    modeAct->setToolTip(i18n( "Choose the Look and Feel" ));
    modeAct->setWhatsThis(i18n( "Check the Look and Feel" ));

    createGUI("khangmanui.rc");
}

void KHangMan::setupLangMenu()
{
    langPopup = static_cast<QPopupMenu*>(factory()->container("languages",this));
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
    KKeyDialog::configureKeys(actionCollection(), "khangmanui.rc");
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
		I18N_NOOP("easy"),
		I18N_NOOP("medium"),
		I18N_NOOP("hard"),
		I18N_NOOP("animals"),
		/* I18N_NOOP("own"), */
	};
	int id = levelAct->currentItem();

	m_view->levelFile = QString(levelStrings[id])+".txt";
	changeStatusbar(i18n("Level: ") + i18n(levelStrings[id]), IDS_LEVEL);
        KConfigBase *conf = kapp->config();
        if( conf ) {
     	  conf->setGroup( "General" );
     	  conf->writeEntry("level", id);// levelStrings[id]);
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
			m_view->slotBlue(m_view->bluePix);
   			break;
    	case 2:
      		modeString="nature";
			m_view->slotBlue(m_view->naturePix);
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
    bool startNewGame = false;

    KConfig *config = kapp->config();
    config->setGroup( "General" );
    // Level
    currentLevel = config->readNumEntry("level", 0);
    QString oldLevel = levelString;
    switch(currentLevel){
      default:
	currentLevel=0;
      case 0:
        levelString="easy";
        break;
      case 1:
        levelString="medium";
        break;
      case 2:
        levelString="hard";
        break;
      case 3:
        levelString="animals";
    }
    m_view->levelFile = config->readEntry( "levelFile", "easy.txt");
    m_view->levelFile = QString("%1.%1").arg(levelString).arg("txt");
    if (oldLevel!=levelString) {
      setLevel_WindowState();
      startNewGame = true;
    }

     // Background
    QString oldMode = modeString;
    modeString = config->readEntry("mode", "nobg");
    if(oldMode != modeString)
      setMode_WindowState();

       // Transparency
     config->setGroup("Settings");
      if(m_view->transparent != config->readBoolEntry( "transparent", true)){
      m_view->transparent = config->readBoolEntry( "transparent", true);
      m_view->slotTransparent();
    }
    transAct->setChecked(m_view->transparent);
    //load the kdeglobals config file
    //safer way would be to load that one read-only
    KConfigBase *globalConf = KGlobal::config();
    globalConf->setGroup("Locale");
    userLanguage = globalConf->readEntry("Language");
    //keep only the first 2 characters
    userLanguage = userLanguage.left(2);
    setSelectedLanguage(userLanguage);

    config->setGroup("Language");
    selectedLanguage = config->readNumEntry("selectedLanguage", defaultLang);
     if (selectedLanguage >= (int) m_languages.count())
                selectedLanguage = 0;
     setLanguage(selectedLanguage);
     if(startNewGame) newGame();
 }

void KHangMan::setSelectedLanguage(QString mLanguage)
{
    defaultLang = m_languages.findIndex(mLanguage);
    if (defaultLang == -1)
	defaultLang = 0;
}

//when config is read, set the KComboBox to the right level
//and update statusbar
void KHangMan::setLevel_WindowState()
{
    if (levelString=="easy")
 	levelAct->setCurrentItem(0);
    if (levelString=="medium")
 	levelAct->setCurrentItem(1);
    if (levelString=="hard")
 	levelAct->setCurrentItem(2);
    if (levelString=="animals")
 	levelAct->setCurrentItem(3);
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
	m_view->slotBlue(m_view->bluePix);
    }
    else if (modeString=="nature")
    {
	modeAct->setCurrentItem(2);
	m_view->slotBlue(m_view->naturePix);
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

    // Unselect preceeding language
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[newLanguage]));
    for (int id = 0; id < (int) m_languageNames.count(); id++)
    	langPopup->setItemChecked(id, id == newLanguage);

    // Change language in the config file
    selectedLanguage = newLanguage;
    // Change language in the config file
    KConfigBase *conf = kapp->config();
    if( conf ) {
	conf->setGroup("Language");
	conf->writeEntry("selectedLanguage", selectedLanguage);
        conf->writeEntry("defaultLang", defaultLang);
    }
    setLanguage(newLanguage);
    newGame();
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

#include "khangman.moc"
