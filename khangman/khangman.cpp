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
#include "pref.h"

const int IDS_LEVEL     = 100;
const int IDS_LANG       = 101;

KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{
    // and a status bar
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

    readSettings();
    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);
    //selectedLanguage is the language saved in Settings otherwise it is default or en if no default
    setLanguage(selectedLanguage);

     // then, setup our actions, must be done after the language search
    setupActions();
    setupLangMenu();

    isLevel();
    isMode();
    fileNew();
    m_view->slotTransparent();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    newAct = new KAction(i18n("&New"), "file_new", CTRL+Key_N , this, SLOT(fileNew()), actionCollection(), "file_new");
    KStdAction::quit(this, SLOT(slotQuit()), actionCollection());

    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);

    langAct = new KSelectAction(i18n("&Languages"), 0, this, SLOT(slotLanguage()), actionCollection(), "combo_lang");
    langAct->setItems(m_sortedNames);
    langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[selectedLanguage]));

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    QStringList levels;
    levelAct = new KSelectAction(i18n("Level"), 0, this, SLOT(slotLevel()), actionCollection(), "combo_level");
    levels += i18n("Easy");
    levels += i18n("Medium");
    levels += i18n("Hard");
    levels += i18n("Animals");
    //levels += i18n("Own");
    levelAct->setItems(levels);
    levelAct->setToolTip(i18n( "Choose the level" ));
    levelAct->setWhatsThis(i18n( "Choose the level of difficulty" ));

    QStringList modes;
    modeAct = new KSelectAction(i18n("Look and Feel"), 0, this, SLOT(slotMode()),  actionCollection(), "combo_mode");
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

void KHangMan::fileNew()
{
    // create a new window  (new KHangMan)->show();
	/**we want the game to be reset*/
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

void KHangMan::optionsPreferences()
{
    KHangManPreferences dlg;
    dlg.resize(450, 340);
    dlg.cancelBool=false;
    dlg.levelChanged = false;
    dlg.langChanged = false;
    QObject::connect(&dlg, SIGNAL(aClicked()), this, SLOT(slotClickApply()));
    if (dlg.exec())
    {
        // redo your settings
    }
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
void KHangMan::slotLevel()
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
	writeSettings();
	fileNew();
}

//When changing background, the game stays as it is
void KHangMan::slotMode()
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
	writeSettings();
}

//read settings from config file khangmanrc
void KHangMan::readSettings()
{
    KConfig *config = kapp->config();
    config->setGroup( "Settings" );
    levelString = config->readEntry( "level");
    m_view->levelFile = config->readEntry( "levelFile");
    modeString=config->readEntry("mode");
    //default background mode
    if (modeString.isEmpty())
    	modeString = "nobg";
    m_view->transparent = config->readBoolEntry( "transparent", true);
    config->setGroup("Language");
    selectedLanguage = config->readNumEntry("myLanguage");
    if (m_view->levelFile.isEmpty()) //if no config file
    {
	levelString = "easy";
	m_view->levelFile = "easy.txt";
    }
    //load the kdeglobals config file
    //safer way would be to load tht one read-only
    KConfigBase *globalConf = KGlobal::config();
    globalConf->setGroup("Locale");
    userLanguage = config->readEntry("Language");//, QString::fromLatin1(""), true, true);
    //keep only the first 2 characters
    userLanguage = userLanguage.left(2);
    setSelectedLanguage(userLanguage);
    //don't get confused between the 2 config files
    config->setGroup( "Language" );
    if (config->hasKey("myLanguage")==false)
    selectedLanguage = defaultLang;
    else
	if (selectedLanguage >= (int) m_languages.count())
    		selectedLanguage = 0;
    writeSettings();
}

void KHangMan::setSelectedLanguage(QString mLanguage)
{
    defaultLang = m_languages.findIndex(mLanguage);
    if (defaultLang == -1)
	defaultLang = 0;
}

//write current settings to config file
void KHangMan::writeSettings()
{
   KConfigBase *conf = kapp->config();
   if( conf )
   {
     	conf->setGroup( "Settings" );
     	conf->writeEntry( "level", levelString);
	conf->writeEntry("levelFile",m_view->levelFile);
	conf->writeEntry( "mode", modeString);
	conf->writeEntry( "transparent", m_view->transparent);
	conf->setGroup("Language");
	conf->writeEntry("myLanguage", selectedLanguage);
	conf->writeEntry("defaultLang", defaultLang);
     	conf->sync();
  }
}

//when config is read, set the KComboBox to the right level
//and update statusbar
void KHangMan::isLevel()
{
    if (levelString=="easy")
 	levelAct->setCurrentItem(0);
    if (levelString=="medium")
 	levelAct->setCurrentItem(1);
    if (levelString=="hard")
 	levelAct->setCurrentItem(2);
    if (levelString=="animals")
 	levelAct->setCurrentItem(3);
    //if (levelString=="own")
    //    levelAct->setCurrentItem(4);
    changeStatusbar(i18n("Level: ") + i18n(levelString.latin1()), IDS_LEVEL);
}

//when config is read, set the KComboBox to the right background
//and call the corresponding slot in the main view to set the background
void KHangMan::isMode()
{
    if (modeString=="nobg")
    {
	modeAct->setCurrentItem(0);
	m_view->slotNoBkgd();
    }
    if (modeString=="blue")
    {
	modeAct->setCurrentItem(1);
	m_view->slotBlue(m_view->bluePix);
    }
    if (modeString=="nature")
    {
	modeAct->setCurrentItem(2);
	m_view->slotBlue(m_view->naturePix);
    }
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
    writeSettings();
    setLanguage(newLanguage);
    fileNew();
}

void KHangMan::setLanguage(int lang)
{
    if (lang >= 0 && lang < (int) m_languages.count()) {
	m_view->language = m_languages[lang];
	changeStatusbar(i18n("Language: ")+m_languageNames[lang], IDS_LANG);
    }
}

//when Apply button in Preferences dialog is clicked, refresh view
void KHangMan::slotClickApply()
{
	KHangManPreferences dlg;
	modeString = dlg.modeString;
	setLanguage(dlg.langNum);
	selectedLanguage = dlg.langNum;
	langAct->setCurrentItem(m_sortedNames.findIndex(m_languageNames[selectedLanguage]));
	for (int id = 0; id < (int) m_languageNames.count(); id++)
		langPopup->setItemChecked(id, id == selectedLanguage);
	isMode();
	if (dlg.transparent!=m_view->transparent) {
		m_view->transparent = dlg.transparent;
		m_view->slotTransparent();
	}
	if (dlg.levelString!=levelString)
	{
		levelString = dlg.levelString;
		isLevel();
		m_view->levelFile = levelString+".txt";
		fileNew();
	}
	if (dlg.cancelBool == false || dlg.langChanged == true)
		fileNew();
}

void KHangMan::slotQuit()
{
	writeSettings();
	kapp->quit();
}

#include "khangman.moc"
