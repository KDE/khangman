/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */

//Qt headers
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
    languages = 0;

    // and a status bar
    statusBar( )->insertItem("   ",IDS_LEVEL, 0);
    statusBar( )->insertItem("   ", 102, 0);
    statusBar( )->insertItem("   ",IDS_LANG, 0);
    statusBar()->show();

    readSettings();
    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);
    //selectedLanguage is the language saved in Settings otherwise it is default or en if no default
    setLanguage(selectedLanguage);

    bool enabled;
    //enabled=true if language is found
    //the program scans in khangman/data/ to see what languages data is found
    enabled = locate("data", "khangman/data/en/") != 0;
    registerLanguage(i18n("English"), "data_en", enabled);
    enabled = locate("data", "khangman/data/fr/") != 0;
    registerLanguage(i18n("French"), "data_fr", enabled);
    enabled = locate("data", "khangman/data/sp/") != 0;
    registerLanguage(i18n("Spanish"), "data_sp", enabled);

    // then, setup our actions, must be done after the language search
    setupActions();

	toolBar()->insertSeparator(-1, 1); //id=1 for separator
	toolBar()->insertCombo(i18n("Easy"), 2, false, SIGNAL(activated(int)), this, SLOT(slotLevel(int)));
	combo = toolBar()->getCombo(2);
	combo->insertItem(i18n("Medium"), 1);
	combo->insertItem(i18n("Hard"), 2);
	combo->insertItem(i18n("Animals"), 3);
	//combo->insertItem("own", 4);
       QToolTip::add( combo, i18n( "Choose the level" ) );
       QWhatsThis::add( combo, i18n( "Choose the level of difficulty" ) );
	toolBar()->insertSeparator(-1, 3);
	toolBar()->insertCombo(i18n("No Background"), 4, false, SIGNAL(activated(int)), this, SLOT(slotMode(int)));
	comboMode = toolBar()->getCombo(4);
	comboMode->insertItem(i18n("Blue Theme"), 1);
	comboMode->insertItem(i18n("Nature Theme"), 2);
	QToolTip::add( comboMode, i18n( "Choose the Look and Feel" ) );
        QWhatsThis::add( comboMode, i18n( "Check the Look and Feel" ) );

	isLevel();
	isMode();
	fileNew();
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

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    createGUI("khangmanui.rc");
}

// Register an available language
void KHangMan::registerLanguage(const QString &menuItem, const char *actionId, bool enabled)
{
  KToggleAction *t = 0;

  switch (languages)
  {
  	case 0: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language0()), actionCollection(), actionId);
		break;
  	case 1: t =  new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language1()), actionCollection(), actionId);
		break;
	case 2: t =  new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language2()), actionCollection(), actionId);
		break;
  }

  if( t ) {
      if (languages == selectedLanguage) t->setChecked(true);
      t->setEnabled(enabled);
      languageActions[languages] = actionId;
      languages++;
  }
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
void KHangMan::slotLevel(int id)
{
	switch(id){
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
		break;
	/*case 4:
		levelString="own";
		break;*/
	}
	m_view->levelFile = levelString+".txt";
	changeStatusbar(i18n("Level: ") + levelString, IDS_LEVEL);
	writeSettings();
	fileNew();
}

//When changing background, the game stays as it is
void KHangMan::slotMode(int index)
{
 	switch ( index ) {
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
    config->setGroup("Language");
    selectedLanguage = config->readNumEntry("myLanguage");
    if ( selectedLanguage > 2)
    	selectedLanguage = 0;
    if (m_view->levelFile.isEmpty()) //if no config file
    {
	levelString = "easy";
	m_view->levelFile = "easy.txt";
	//selectedLanguage = 0;
    }
    //see what language the user has set for kde
    //if fr or en or sp, set that as default at first run
    if (config->hasKey("myLanguage")==false)
    {
	 //load the kdeglobals config file
	 KConfigBase *config = KGlobal::config();
	 config->setGroup("Locale");
         userLanguage = config->readEntry("Language");//, QString::fromLatin1(""), true, true);
	 //keep only the first 2 characters
	 userLanguage = userLanguage.left(2);
	 setSelectedLanguage(userLanguage);
    }
    if (modeString.isEmpty())
    	modeString = "nobg";
}

void KHangMan::setSelectedLanguage(QString mLanguage)
{
	if (mLanguage == "en")
		selectedLanguage = 0;
	else if (mLanguage == "fr")
		selectedLanguage  = 1;
	else if (mLanguage == "sp")
		selectedLanguage = 2;
	else selectedLanguage = 0;
	defaultLang = selectedLanguage;
	writeSettings();
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
 	combo->setCurrentItem(0);
    if (levelString=="medium")
 	combo->setCurrentItem(1);
    if (levelString=="hard")
 	combo->setCurrentItem(2);
    if (levelString=="animals")
 	combo->setCurrentItem(3);
    //if (levelString=="own")
 //	combo->setCurrentItem(4);
    changeStatusbar(i18n("Level: ") + levelString, IDS_LEVEL);
}

//when config is read, set the KComboBox to the right background
//and call the corresponding slot in the main view to set the background
void KHangMan::isMode()
{
    if (modeString=="nobg")
    {
	comboMode->setCurrentItem(0);
	m_view->slotNoBkgd();
    }
    if (modeString=="blue")
    {
	comboMode->setCurrentItem(1);
	m_view->slotBlue(m_view->bluePix);
    }
    if (modeString=="nature")
    {
	comboMode->setCurrentItem(2);
	m_view->slotBlue(m_view->naturePix);
    }
}

void KHangMan::language0()
{
    changeLanguage(0);
}

void KHangMan::language1()
{
    changeLanguage(1);
}

void KHangMan::language2()
{
    changeLanguage(2);
}

// Switch to another language using Languages menu
void KHangMan::changeLanguage(uint newLanguage)
{
   // Do not accept to switch to same language
   if (newLanguage == selectedLanguage)
   {
    	// newLanguage should stay checked
    	((KToggleAction*) actionCollection()->action(languageActions[newLanguage].latin1()))->setChecked(true);
   	 return;
    }
    // Unselect preceeding language
    ((KToggleAction*) actionCollection()->action(languageActions[selectedLanguage].latin1()))->setChecked(false);

    // Change language in the config file
    selectedLanguage = newLanguage;
    writeSettings();
    setLanguage(newLanguage);
    fileNew();
}

void KHangMan::setLanguage(int lang)
{
    QString language;
    switch ( lang ) {
    	case 0:
	m_view->language="en";
	language = i18n("English");
	break;
	case 1:
	m_view->language="fr";
	language = i18n("French");
	break;
	case 2:
	m_view->language="sp";
	language = i18n("Spanish");
	break;
	}
    changeStatusbar(i18n("Language: ")+language, IDS_LANG);
}

//when Apply button in Preferences dialog is clicked, refresh view
void KHangMan::slotClickApply()
{
	KHangManPreferences dlg;
	modeString = dlg.modeString;
	setLanguage(dlg.langNum);
	((KToggleAction*) actionCollection()->action(languageActions[selectedLanguage].latin1()))->setChecked(false);
	((KToggleAction*) actionCollection()->action(languageActions[dlg.langNum].latin1()))->setChecked(true);
	selectedLanguage = dlg.langNum;
	isMode();
	if (dlg.levelChanged)
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
