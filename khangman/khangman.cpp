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
#include <qbitmap.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qpainter.h>
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
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
#include <ktoolbarbutton.h>
//Project headers
#include "khangman.h"
#include "prefs.h"
#include "khnewstuff.h"
#include "advanced.h"
#include "normal.h"
//standard C++ headers
#include <stdlib.h>

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
	// tell the KMainWindow that this is indeed the main widget
	setCentralWidget(m_view);
	
	setLanguages();
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
	slotAccents();
	newGame();
}

KHangMan::~KHangMan()
{
}

void KHangMan::setupActions()
{
	newAct = new KAction(i18n("&New"), "file_new", CTRL+Key_N , this, SLOT(newGame()), actionCollection(), "file_new");
	KGlobal::iconLoader()->loadIcon("knewstuff", KIcon::Small);
	new KAction( i18n("&Get data in a new language..."), "knewstuff", CTRL+Key_G, this, SLOT( downloadNewStuff() ), actionCollection(), "downloadnewstuff" );
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
		changeStatusbar(i18n("First letter upper case"), IDS_ACCENTS);	
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

    	slotHint();
    	setAccentBool();
	m_bCharToolbar = Prefs::showCharToolbar();
	slotAccents();
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
	allData.clear();
	if (!noCharBool) {
	QString myString=QString("khangman/%1.txt").arg(m_view->language);
	if (m_view->language =="pt_BR")
		myString=QString("khangman/pt.txt");
	QFile myFile;
	myFile.setName(locate("data",myString));
	if (!myFile.exists())
	{
		QString mString=i18n("File $KDEDIR/share/apps/khangman/%1.txt not found;\n"
					"check your installation.").arg(m_view->language);
		KMessageBox::sorry( this, mString,
					i18n("Error") );
		exit(1);
	}
	update();
	//we open the file and store info into the stream...
	QFile openFileStream(myFile.name());
	openFileStream.open(IO_ReadOnly);
	QTextStream readFileStr(&openFileStream);
	readFileStr.setEncoding(QTextStream::UnicodeUTF8);
	//allData contains all the words from the file
	allData = QStringList::split("\n", readFileStr.read(), true);
	openFileStream.close();
	for (int i=0; i<(int) allData.count(); i++)
			secondToolbar->insertButton (charIcon(allData[i].at(0)), i, SIGNAL( clicked() ), this, SLOT( slotPasteChar()), true,  i18n("Inserts the character %1").arg(allData[i]), i+1 );
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

void KHangMan::slotPasteChar()
{
	KToolBarButton *charBut = (KToolBarButton* ) sender();
	m_view->charWrite->setText(allData[charBut->id()]);
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
	if (m_view->levelFile == "world_capitals.txt" || m_view->levelFile == "departements.txt")
		changeStatusbar(i18n("First letter upper case"), IDS_ACCENTS);	
	loadLangToolBar();
	newGame();
}

void KHangMan::slotHint()
{
	if (m_view->kvtmlBool)  {
		changeStatusbar(i18n("Hints possible"), IDS_HINT);
	}
        m_view->hintBool=Prefs::hint();
	if ((m_view->kvtmlBool) && (m_view->hintBool)) 
		changeStatusbar(i18n("Hint on right-click"), IDS_HINT);	
	if (!Prefs::hint())
		changeStatusbar(i18n("Hints possible"), IDS_HINT);
}

void KHangMan::enableHint(bool m_bool)
{
	if (m_bool) {
		m_view->kvtmlBool = true;
		slotHint();
	}
	else
	{
		m_view->kvtmlBool = false;
		changeStatusbar("", IDS_HINT);
	}
}

void KHangMan::setAccentBool()
{
	if (m_view->language=="es" || m_view->language =="pt" || m_view->language == "ca" || m_view->language == "pt_BR") 
    		m_view->m_accent = true;	
    	else 
		m_view->m_accent = false;
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
	//the program scans in khangman/data/ to see what languages data is found
	QStringList mdirs = KGlobal::dirs()->findDirs("data", "khangman/data/");
	if (mdirs.isEmpty()) return;
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
			config->writeEntry(tmp, QDate::currentDate().toString());
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
	KConfigDialog* dialog = new KConfigDialog( this, "settings",  Prefs::self() );
	normal *mNormal =  new normal( 0, "Normal Settings" ); 
	dialog->addPage(mNormal, i18n("Normal Settings"), "configure");
	mNormal->kcfg_Transparent->setEnabled( modeAct->currentItem() != 0);
        mNormal->kcfg_Hint->setEnabled( m_view->kvtmlBool);
	mNormal->kcfg_AccentedLetters->setEnabled(m_view->m_accent);
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

QString KHangMan::charIcon(const QChar & c)
{
  ///Create a name and path for the icon
  QString s = locateLocal("icon", "char" + QString::number(c.unicode()) + ".png");
  
  ///No need to redraw if it already exists
 // if (KStandardDirs::exists(s))
   // return s;
  
  QRect r(4, 4, 120, 120);

  ///A font to draw the character with
  QFont font;
  if (m_view->language=="cs")
  	font.setFamily( "Arial" );
  else
   	font.setFamily( "URW Bookman" );
  font.setPixelSize(120);
  font.setWeight(QFont::DemiBold);
  
  ///Create the pixmap        
  QPixmap pm(128, 128);
  pm.fill(Qt::white);
  QPainter p(&pm);
  p.setFont(font);
  p.setPen(Qt::black);
  p.drawText(r, Qt::AlignCenter, (QString) c);
  
  ///Create transparency mask
  QBitmap bm(128, 128);
  bm.fill(Qt::color0);
  QPainter b(&bm);
  b.setFont(font);
  b.setPen(Qt::color1);
  b.drawText(r, Qt::AlignCenter, (QString) c);
  
  ///Mask the pixmap
  pm.setMask(bm); 
  
  ///Save the icon to disk
  pm.save(s, "PNG");
  
  return s;
}

#include "khangman.moc"
