/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */

//Qt headers
#include <qdir.h>
//KDE headers
#include <kaction.h>
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
    //setLanguage(selectedLanguage); //seems useless 20/05
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

    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);

    langAct = new KSelectAction(i18n("&Languages"), 0, this, SLOT(slotLanguage()), actionCollection(), "combo_lang");
    langAct->setItems(m_sortedNames);
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[selectedLanguage]));

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    transAct = new KToggleAction(i18n("&Transparent Pictures"), CTRL+Key_T, this, SLOT(slotTransparent()), actionCollection(), "transparent");

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
	/*static const char *levelStrings[] = {
		I18N_NOOP("easy"),
		I18N_NOOP("medium"),
		I18N_NOOP("hard"),
		I18N_NOOP("animals"),
	};*/
	//TODO i18n()
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
    bool startNewGame = false;
    // Language
    //load the kdeglobals config file - safer way would be to load that one read-only
    KConfigBase *globalConf = KGlobal::config();
    globalConf->setGroup("Locale");
    userLanguage = globalConf->readEntry("Language");
    //keep only the first 2 characters
    userLanguage = userLanguage.left(2);
    kdDebug() << userLanguage << endl;
    setSelectedLanguage(userLanguage);

    KConfig *config = kapp->config();
    config->setGroup("Language");
    selectedLanguage = config->readNumEntry("selectedLanguage", defaultLang);
     if (selectedLanguage >= (int) m_languages.count())
                selectedLanguage = 1;
     setLanguage(selectedLanguage);

    config->setGroup( "General" );
    // Level
    currentLevel = config->readNumEntry("level", 1); //default is easy
    QString oldLevel = levelString;

    loadDataFiles();

    levelString = levels[currentLevel].replace(0, 1, levels[currentLevel].left(1).lower());
    kdDebug() << levelString << endl;
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

     if(startNewGame) newGame();
 }

void KHangMan::setSelectedLanguage(QString mLanguage)
{
    defaultLang = m_languages.findIndex(mLanguage);
    if (defaultLang == -1)
	defaultLang = 1; //if no default, set it to English
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

    // Unselect preceeding language
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
    //update the Levels in Level combobox as well
    setLevel_WindowState();
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

#include "khangman.moc"
