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


KHangMan::KHangMan()
    : KMainWindow( 0, "KHangMan" ),
      m_view(new KHangManView(this))
{

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

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
	readSettings();
	//kdDebug()<<"modeString = " << modeString <<endl;
	isLevel();
	isMode();
	fileNew();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
    newAct = new KAction(i18n("&New"), "file_new", 0 , this, SLOT(fileNew()), actionCollection(), "file_new");
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    createStandardStatusBarAction();
    setStandardToolBarMenuEnabled(true);

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

	createGUI("khangmanui.rc");
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
    if (dlg.exec())
    {
        // redo your settings
    }
	readSettings();
	isLevel();
	isMode();
	if (dlg.cancelBool==false) fileNew();
}

void KHangMan::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->message(text);
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
	changeStatusbar("level: "+levelString);
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
    m_view->levelFile=config->readEntry( "levelFile");
	modeString=config->readEntry("mode");
    if (m_view->levelFile.isEmpty()) //if no config file
    {
		levelString="easy";
		m_view->levelFile="easy.txt";
		//kdDebug()<<"in no config file routine"<<endl;
    }
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
     	conf->sync();
  	}
}

//when config is read, set the KComboBox to the right item
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
	changeStatusbar("level: "+levelString);
}

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

#include "khangman.moc"
