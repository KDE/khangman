/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */


//KDE headers
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstddirs.h>
//Qt headers
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qradiobutton.h>
//Project headers
#include "pref.h"


KHangManPreferences::KHangManPreferences()
    : KDialogBase(IconList, i18n("KHangMan Preferences"),
                  Help|Default|Ok|Apply|Cancel, Ok)
{
     //disable the Apply button before any changes are made
     enableButton( Apply, false);

    QFrame *frame;
    QVBoxLayout *layout;

    frame = addPage(i18n("General"), i18n("General"), BarIcon("gear", KIcon::SizeMedium));
    layout = new QVBoxLayout(frame, 0, 0);
    m_pageOne = new pref1ui(frame);
    frame->setMinimumHeight(220);

    frame = addPage(i18n("Language"), i18n("Data language"), BarIcon ("locale", KIcon::SizeMedium));
    layout = new QVBoxLayout(frame, 0, 0);
    m_pageTwo = new pref2ui(frame);
    frame->setMinimumHeight(220);

    //get the available languages
    QStringList langs;
    QStringList dirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
    for (QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it ) {
	QDir dir(*it);
	langs += dir.entryList(QDir::Dirs, QDir::Name);
    }
    langs.remove(langs.find("."));
    langs.remove(langs.find(".."));

    KConfig entry(locate("locale", "all_languages"));
    for (QStringList::Iterator it = langs.begin(); it != langs.end(); ++it ) {
	entry.setGroup(*it);
        languageNames.append(entry.readEntry("Name"));
    }
    m_pageTwo->langBox->insertStringList(languageNames);
    m_pageTwo->langBox->sort();

    readConfig();
    //set the values read in config file
    slotSet();

    QObject::connect(m_pageOne->buttonGroup, SIGNAL(clicked(int)), this, SLOT(slotMode(int)));
    QObject::connect(m_pageOne->buttonGroup, SIGNAL(clicked(int)), this, SLOT(slotChanged()));
    QObject::connect(m_pageOne->levelBox, SIGNAL(activated(int)), this, SLOT(slotLevel(int)));
    QObject::connect(m_pageOne->levelBox, SIGNAL(activated(int)), this, SLOT(slotChanged()));
    QObject::connect(m_pageOne->transparentBox, SIGNAL(toggled(bool)), this, SLOT(slotTransparent(bool)));
    QObject::connect(m_pageOne->transparentBox, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
    QObject::connect(m_pageTwo->langBox, SIGNAL(highlighted(const QString &)), this, SLOT(slotLang(const QString &)));
    QObject::connect(m_pageTwo->langBox, SIGNAL(highlighted(const QString &)), this, SLOT(slotChanged()));
}

void KHangManPreferences::readConfig()
{
   	KConfigBase *conf = kapp->config();
	if (conf)
	{
  		conf->setGroup("Settings");
   		levelString = conf->readEntry("level");
		modeString = conf->readEntry("mode");
		transparent = conf->readBoolEntry( "transparent", false);
		conf->setGroup("Language");
   		langNum = conf->readNumEntry("myLanguage");
		defaultLang = conf->readNumEntry("defaultLang");
		if (langNum < 0 || langNum >= (int)m_pageTwo->langBox->count())
			langNum = 0;
	}
	if( !levelString )
  		slotDefault();
}

//Set the system defaults
void KHangManPreferences::slotDefault()
{
	readConfig();
	levelChanged = true;
	levelString="easy";
	modeString="nobg";
	langNum = defaultLang;
	if (langNum < 0 || langNum >= (int)m_pageTwo->langBox->count())
			langNum = 0;
	slotSet();
	slotChanged();
}

//Apply changes made in the dialog and close the dialog
void KHangManPreferences::slotOk()
{
	if( configChanged )
     	   slotApply();
	accept();
}

//Apply changes made in the dialog and disable the Apply button
void KHangManPreferences::slotApply()
{
	enableButton( Apply, false );
        configChanged = false;
	writeConfig();
	//emit a signal which is connected to a slot in khangman.cpp
	emit aClicked();
}

//Don't validate the new changes in the dialog (after Apply was hit) and close the dialog
void KHangManPreferences::slotCancel()
{
	cancelBool=true;
	accept();
}

void KHangManPreferences::slotSet()
{
	if (levelString=="easy")
		m_pageOne->levelBox->setCurrentItem(0);
	if (levelString=="medium")
		m_pageOne->levelBox->setCurrentItem(1);
	if (levelString=="hard")
		m_pageOne->levelBox->setCurrentItem(2);
	if (levelString=="animals")
		m_pageOne->levelBox->setCurrentItem(3);
	//if (levelString=="own")
	//	m_pageOne->levelBox->setCurrentItem(4);
	if (modeString=="blue")
		m_pageOne->blueBox->setChecked(true);
	if (modeString=="nobg")
		m_pageOne->noBgBox->setChecked(true);
	if (modeString=="nature")
		m_pageOne->natureBox->setChecked(true);
	m_pageOne->transparentBox->setChecked(transparent);
	m_pageOne->transparentBox->setEnabled(!m_pageOne->noBgBox->isChecked());

	m_pageTwo->langBox->setCurrentItem(m_pageTwo->langBox->findItem(languageNames[langNum]));
}

void KHangManPreferences::slotMode(int id)
{
	switch(id){
	case 0:
		modeString = "nobg";
		break;
	case 1:
		modeString = "blue";
		break;
	case 2:
		modeString = "nature";
		break;
	}
	m_pageOne->transparentBox->setEnabled( id != 0 );
	enableButton( Apply, false );
        configChanged = false;
}

void KHangManPreferences::slotLang(const QString &item)
{
	langNum = languageNames.findIndex(item);
	slotSet();
	langChanged = true;
	enableButton( Apply, false );
        configChanged = false;
}

void KHangManPreferences::slotLevel(int id)
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
	enableButton( Apply, false );
        configChanged = false;
	levelChanged = true;
}

void KHangManPreferences::slotTransparent(bool on)
{
	transparent = on;
}

//Write settings in config
void KHangManPreferences::writeConfig()
{
	KConfigBase *conf = kapp->config();
	if (conf)
	{
		conf->setGroup( "Settings");
		conf->writeEntry( "level", levelString);
		conf->writeEntry( "mode", modeString);
		conf->writeEntry( "transparent", transparent);
		conf->writeEntry("levelFile", levelString+".txt");
		conf->setGroup("Language");
		conf->writeEntry("myLanguage", langNum);
		conf->sync();
	}
}

//called whenever a change has been made in the settings
//Apply button is enabled so it can be clicked to apply the changes
void KHangManPreferences::slotChanged()
{
    enableButton( Apply, true );
    configChanged = true;
}

#include "pref.moc"
