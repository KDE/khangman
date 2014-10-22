/***************************************************************************
 *   Copyright 2001-2009 Anne-Marie Mahfouf <annma@kde.org>                *
 *                                                                         *
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

#include "langutils.h"
#include "khangmanview.h"

#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QPainter>
#include <QStandardPaths>
#include <QStatusBar>

#include <KSelectAction>
#include <KToggleAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
#include <KToolBar>
#include <KSharedConfig>
#include <KRecentFilesAction>

#include <KNS3/DownloadDialog>
#include <sharedkvtmlfiles.h>

KHangMan::KHangMan()
        : KXmlGuiWindow(), m_currentLevel(-1),
          m_view(new KHangManView(this)),
          m_recent(0)
{
    setObjectName(QLatin1String("KHangMan"));

    setCentralWidget(m_view);
    setLanguages();
    setupStatusbar();

    //load the standard set of themes
    khm_factory.addTheme(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "khangman/pics/standardthemes.xml"));   //TODO move it to better place

    setupActions();

    // Toolbar for special characters
    m_specialCharToolbar = toolBar("specialCharToolBar");
    addToolBar ( Qt::BottomToolBarArea, m_specialCharToolbar);

    loadSettings();
    setAccent();
    loadLangToolBar();
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

    QAction * fileOpen = KStandardAction::open(this, SLOT(slotFileOpen()), actionCollection());
    fileOpen->setWhatsThis(i18n("Opens an existing vocabulary document"));
    fileOpen->setToolTip(fileOpen->whatsThis());

    m_hintAct = new KToggleAction(i18n("&Show Hint"), this);
    m_hintAct->setToolTip(i18n( "Show/Hide the hint to help guessing the word" ));
    actionCollection()->addAction("show_hint", m_hintAct );
    //hintAct->setCheckedState(KGuiItem(i18n("&Hide Hint")));
    m_hintAct->setIcon(QIcon::fromTheme("games-hint"));
    //set default shortcut key combination for displaying hints
    actionCollection()->setDefaultShortcut(m_hintAct, QKeySequence(Qt::CTRL+Qt::Key_H));
    m_hintAct->setEnabled( m_view->hintExists() );
    connect(m_hintAct, SIGNAL(triggered(bool)), this, SLOT(slotSetHint(bool)));
    // Game->Get Words in New Language
    QAction *newStuffAct  = new QAction(i18n("&Get Words in New Language..."), this);
    actionCollection()->addAction("downloadnewstuff", newStuffAct );
    newStuffAct->setIcon(QIcon::fromTheme("get-hot-new-stuff"));
    //set default shortcut key combination for GetHotNewStuff
    actionCollection()->setDefaultShortcut(newStuffAct, QKeySequence(Qt::CTRL+Qt::Key_G));
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
    m_languageAction->setToolTip(i18n( "Choose the language" ));
    m_languageAction->setWhatsThis(i18n( "Choose the language" ));

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // Mode.
    m_modeAction  = new KSelectAction(i18n("L&ook"), this);
    actionCollection()->addAction("combo_mode", m_modeAction );
    connect(m_modeAction, SIGNAL(triggered(int)), this, SLOT(slotChangeMode(int)));
    //m_modeAction->setItems(KHMThemeFactory::instance()->themeList());
    m_modeAction->setItems(khm_factory.themeList());
    m_modeAction->setCurrentItem(Prefs::mode());
    m_modeAction->setToolTip(i18n( "Choose the look and feel" ));
    m_modeAction->setWhatsThis(i18n( "Choose the look and feel" ));

    m_config = KSharedConfig::openConfig();

    m_recent=KStandardAction::openRecent(this, SLOT(slotOpenRecent(QUrl)), this);
    m_recent->setWhatsThis(i18n("You can open last opened files")); //TODO: Check the description
    actionCollection()->addAction(m_recent->objectName(), m_recent);
    m_recent->loadEntries(KConfigGroup(m_config, "KHangManRecent"));
    setupGUI();
}

// Set up the status bar with 4 different fields.
void KHangMan::setupStatusbar()
{
    // set up the status bar
    m_levelLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_levelLabel);
    m_accentsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_accentsLabel);
    m_winsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_winsLabel);
    m_lossesLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_lossesLabel);
    //statusBar()->insertPermanentItem("   ", IDS_LEVEL,   0);
    //statusBar()->insertPermanentItem("   ", IDS_ACCENTS, 0);
    //statusBar()->insertItem("   ", IDS_WINS,    1);
    //statusBar()->insertItem("   ", IDS_LOSSES,  1);
}


// ----------------------------------------------------------------
//                               Slots


void KHangMan::slotQuit()   //TODO: Isn't called when "X" pressed, only when File->Quit
{
    Prefs::setShowCharToolbar( m_specialCharToolbar->isVisible() );
    m_recent->saveEntries(KConfigGroup(m_config, "KHangManRecent"));
    Prefs::self()->save();
    qApp->closeAllWindows();
}

void KHangMan::slotChangeLevel(int index)
{
    QMap<QString, QString>::const_iterator currentLevel = m_titleLevels.constBegin() + index;
    m_levelLabel->setText(currentLevel.key());
    Prefs::setCurrentLevel(index);
    Prefs::setLevelFile(currentLevel.value());
    Prefs::self()->save();
    m_view->readFile();
    m_view->newGame();
}

void KHangMan::slotChangeLanguage(int index)
{
    //good when no in English
    qDebug() << "Change to " << m_languages[m_languageNames.indexOf(m_languageNames[index])];
    Prefs::setSelectedLanguage(m_languages[m_languageNames.indexOf(m_languageNames[index])]);
    Prefs::self()->save();
    loadLevels();
    loadLangToolBar();
    setAccent();
    setMessages();
    m_view->newGame();
}

void KHangMan::slotChangeMode(int index)
{
    Prefs::setMode(index);
    Prefs::self()->save();
    //m_view->setTheme(KHMThemeFactory::instance()->buildTheme(index));
    m_view->setTheme(khm_factory.buildTheme(index));
}


// ----------------------------------------------------------------


void KHangMan::setLanguages()
{
    m_languages.clear();
    m_languageNames.clear();

    //the program scans in khangman/data/ to see what languages data is found
    m_languages = SharedKvtmlFiles::languages();
    qDebug() << "Languages " << m_languages;
    if (m_languages.isEmpty()) {
        qApp->closeAllWindows();
    }
    //find duplicated entries in KDEDIR and KDEHOME

    // Write the present languages in config so they cannot be downloaded.
    // FIXME: use pre-seeding here
    KConfigGroup cg( KSharedConfig::openConfig() ,"KNewStuff2");
    for (int i=0;  i<m_languages.count(); ++i) {
        //QString tmp = cg.readEntry(m_languages[i]);
       // if (!tmp)
        cg.writeEntry(m_languages[i], QDate::currentDate().toString(Qt::ISODate));
    }
    cg.config()->sync();

    // We look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.

    KConfig entry(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + "all_languages"));
    for (int i = 0; i < m_languages.size(); ++i) {
        KConfigGroup group = entry.group(m_languages[i]);
        QString languageName = group.readEntry("Name");
        if (languageName.isEmpty()) {
            languageName = i18nc("@item:inlistbox no language for that locale","None");
        }
        m_languageNames.append(languageName);
    }
}


void KHangMan::loadSettings()
{
    // Language //TODO is selectedLanguage necessary??? only used here
    if (m_languages.isEmpty()){
        qApp->closeAllWindows();
    }
    // Show/hide characters toolbar
    if (Prefs::showCharToolbar()) {
        m_specialCharToolbar->show();
    }
    else {
        m_specialCharToolbar->hide();
    }
    setMessages();
}

void KHangMan::setLevel()
{
    m_currentLevel = Prefs::currentLevel();
    if (m_currentLevel > m_titleLevels.count()) {
        m_currentLevel = 0;
    }
    m_view->readFile();
}

void KHangMan::show()
{
    if (loadLevels()) { // kvtml files have been found
        // set the theme
        slotChangeMode(Prefs::mode());
        //call show() of base class KXmlGuiWindow
        KXmlGuiWindow::show();
        m_view->newGame();
    }
    
    else { // no kvtml files present
        QMessageBox *fileNotFound = new QMessageBox(this);
        fileNotFound->setText("No kvtml files found.");
        fileNotFound->exec();
        close();
    }
}



bool KHangMan::loadLevels()
{
    //build the Level combobox menu dynamically depending of the data for each language
    m_titleLevels.clear();
    QStringList levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
    QStringList titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());

    if (levelFilenames.size() == 0) {
        Prefs::setSelectedLanguage("en");
        Prefs::self()->save();
        levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
        titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());
    }

    if (levelFilenames.isEmpty()){
        // no kvtml files found
        qDebug() << "levelFilenames.isEmpty() true";
        return false;
    }
    m_languageAction->setCurrentItem(m_languages.indexOf(Prefs::selectedLanguage()));

    Q_ASSERT(levelFilenames.count() == titles.count());
    for(int i = 0; i < levelFilenames.count(); ++i) {
        m_titleLevels.insert(titles.at(i), levelFilenames.at(i));
    }

    if (!levelFilenames.contains(Prefs::levelFile())) {
        Prefs::setLevelFile(m_titleLevels.constBegin().value());
        Prefs::setCurrentLevel(0);
        m_currentLevel = 0;
        Prefs::self()->save();
    }

    // don't run off the end of the list
    if (m_currentLevel!=-1 && m_currentLevel > m_titleLevels.count()) {
        m_currentLevel = m_titleLevels.count();
    }

    // titles should be translated in the data files themselves
    m_levelAction->setItems(m_titleLevels.keys());
    m_levelAction->setCurrentItem(Prefs::currentLevel());

    setLevel();
    QMap<QString, QString>::const_iterator currentLevel = m_titleLevels.constBegin() + m_currentLevel;
    m_levelLabel->setText(currentLevel.key());
    
    return true;
}


void KHangMan::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());
        return;
    }

    //KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog( this, "settings",  Prefs::self() );
    // Add the General Settings page
    QWidget *generalSettingsDlg = new QWidget;
    ui_general.setupUi(generalSettingsDlg);

    dialog->addPage(generalSettingsDlg, i18n("General"), "khangman");

    // Add the Language Settings page
    QWidget *languageSettingsDlg = new QWidget;
    ui_language.setupUi(languageSettingsDlg);
    dialog->addPage(languageSettingsDlg, i18n("Languages"), "preferences-desktop-locale");

    ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());

    Timer *m_timer = new Timer();
    dialog->addPage(m_timer, i18n("Timers"), "chronometer");

    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(updateSettings()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    //dialog->setHelp(QString(),"khangman");
    dialog->show();
}

void KHangMan::updateSettings()
{
    //after upperCase() changed, reload new game
    setAccent();
    if (Prefs::selectedLanguage() == "de") {
        loadLangToolBar();
    }
    m_hintAct->setChecked(Prefs::hint());

    setMessages();
    //m_view->newGame();
}

void KHangMan::slotDownloadNewStuff()
{
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog("khangman.knsrc", this);
    dialog->exec();
    if (!dialog->changedEntries().isEmpty()) {
        SharedKvtmlFiles::sortDownloadedFiles();
        //look for languages dirs installed
        setLanguages();
        //refresh Languages menu
        m_languageAction->setItems(m_languageNames);
        slotChangeLanguage(m_languages.indexOf(Prefs::selectedLanguage()));
        m_languageAction->setCurrentItem(m_languages.indexOf(Prefs::selectedLanguage()));
    }

    delete dialog;
}

void KHangMan::loadLangToolBar()
{
    QString lang = Prefs::selectedLanguage();
    if (lang.isEmpty())
        return;
    bool hasSpecialChars = LangUtils::hasSpecialChars(lang);

    // If the setting changed, save showToolbar setting
    if (m_specialCharToolbar->isVisible() && hasSpecialChars) {
        Prefs::setShowCharToolbar(true);
        Prefs::self()->save();
    }

    m_specialCharToolbar->clear();

    m_allData.clear();
    if (hasSpecialChars) {
        QString langFileName=QString("khangman/%1.txt").arg(lang);
        QFile langFile;
        langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));

        // Let's look in local KDEHOME dir then KNS installs each .txt
        // in kvtml/<lang> as it installs everything at the same place
        if (!langFile.exists()) {
            langFileName = QString("kvtml/%1/%1.txt").arg(lang);
            langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));
            qDebug() << langFileName;
        }

        if (!langFile.exists()) {
            return;
        }

        update();

        // We open the file and store info into the stream...
        QFile openFileStream(langFile.fileName());
        openFileStream.open(QIODevice::ReadOnly);
        QTextStream readFileStr(&openFileStream);
        readFileStr.setCodec("UTF-8");

        // m_allData contains all the words from the file
        // FIXME: Better name
        m_allData = readFileStr.readAll().split('\n');
        openFileStream.close();

        for (int i=0; i<m_allData.count(); ++i) {
            if (!m_allData.at(i).isEmpty()) {
                QAction *act = m_specialCharToolbar->addAction(m_allData.at(i));
                act->setIcon(charIcon(m_allData.at(i).at(0)));
                // used to carry the id
                act->setData(i);
                connect(act, SIGNAL(triggered(bool)), this, SLOT(slotPasteChar()));
            }
        }
        m_specialCharToolbar->show();
    } else {
        m_specialCharToolbar->hide();
    }
}


void KHangMan::slotPasteChar()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if (!act) {
        return;
    }

    bool ok = true;
    int id = act->data().toInt(&ok);
    if (!ok || id < 0 || id >= m_allData.count()) {
        return;
    }

    m_view->enterLetter(m_allData.at(id));
}

QIcon KHangMan::charIcon(const QChar & c) const
{
    QRect r(4, 4, 120, 120);

    //A font to draw the character with
    QFont font;
    font.setFamily( "Sans Serif" );
    font.setPointSize(96);
    font.setWeight(QFont::Normal);

    //Create the pixmap
    QPixmap pm(128, 128);
    pm.fill(Qt::white);
    QPainter p(&pm);
    p.setFont(font);
    p.setPen(Qt::black);
    p.drawText(r, Qt::AlignCenter, (QString) c);
    p.end();

    //Create transparency mask
    QBitmap bm(128, 128);
    bm.fill(Qt::color0);
    QPainter b(&bm);
    b.setFont(font);
    b.setPen(Qt::color1);
    b.drawText(r, Qt::AlignCenter, (QString) c);
    b.end();

    //Mask the pixmap
    pm.setMask(bm);

    return QIcon(pm);
}

void KHangMan::setAccent()
{
    qDebug() << "in slot accent  ";
    m_view->setAccentedLetters(LangUtils::hasAccentedLetters(Prefs::selectedLanguage()));
}

void KHangMan::setHintEnabled(bool enable)
{
    m_hintAct->setEnabled(enable);
}

void KHangMan::setMessages()
{
    // Tell the user about accented characters
    if (m_view->accentedLetters() && Prefs::accentedLetters()) {
        m_accentsLabel->setText(i18n("Type accented letters"));
    }
    else {
        m_accentsLabel->setText("");
    }
}

void KHangMan::loadFile(const QUrl &url)
{
    if ( url.isValid() )  {
        if(url.isLocalFile())
            Prefs::setLevelFile(url.toLocalFile());
        else Prefs::setLevelFile(url.path());

        m_levelLabel->setText(url.path().section('/', -1));

        m_recent->addUrl(url);
        m_recent->saveEntries(KConfigGroup(m_config, "KHangManRecent"));
        Prefs::self()->save();

        m_view->readFile();
        m_view->newGame();
    }
}

void KHangMan::slotNewGame()
{
    m_view->newGame(true);
    m_hintAct->setChecked(Prefs::hint());
}

void KHangMan::slotOpenRecent(const QUrl &url)
{
    loadFile(url);
}

void KHangMan::slotFileOpen()
{
    QUrl url = QFileDialog::getOpenFileUrl(this,
                                           i18n("Open Vocabulary Document"),
                                           QString(),
                                           KEduVocDocument::pattern(KEduVocDocument::Reading));
    loadFile(url);
}

void KHangMan::slotSetWins(int wins)
{
    m_winsLabel->setText(i18n("Wins: %1", wins));
}

void KHangMan::slotSetLosses(int losses)
{
    m_lossesLabel->setText(i18n("Losses: %1", losses));
}

void KHangMan::slotSetHint(bool hint)
{
    Prefs::setHint(hint);
    Prefs::self()->save();
    m_view->update();
}

#include "khangman.moc"

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

