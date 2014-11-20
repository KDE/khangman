/***************************************************************************
 *   Copyright 2001-2009 Anne-Marie Mahfouf <annma@kde.org>                *
 *   Copyright 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>          *
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
//#include "khangmanengine.h"
//#include "khangmanenginehelper.h"

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
#include <QQuickWidget>
#include <QQmlContext>

#include <Phonon/MediaObject>

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
#include <keduvocdocument.h>
#include <keduvocexpression.h>
#include <KDeclarative/KDeclarative>
#include <KRandomSequence>

#include <KNS3/DownloadDialog>
#include <sharedkvtmlfiles.h>

KHangMan::KHangMan()
        : KXmlGuiWindow(), m_currentLevel(-1),
          m_recent(0),
          m_player(0),
          m_doc(0)
{
    setObjectName(QLatin1String("KHangMan"));

    m_view = new QQuickWidget(this);
    m_view->rootContext()->setContextProperty("khangman", this);

    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_view->engine());
    kdeclarative.setupBindings();

    m_view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    setCentralWidget(m_view);
    setLanguages();
    //setupStatusbar();

    //load the standard set of themes
    m_khmfactory.addTheme(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "khangman/themes/standardthemes.xml"));

    setupActions();

    // Toolbar for special characters
    //m_specialCharToolbar = toolBar("specialCharToolBar");
    //addToolBar ( Qt::BottomToolBarArea, m_specialCharToolbar);

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
    //m_hintAct->setEnabled( m_view->hintExists() );
    connect(m_hintAct, SIGNAL(triggered(bool)), this, SLOT(slotSetHint(bool)));
    // Game->Get Words in New Language
    QAction *newStuffAct  = new QAction(i18n("&Get Words in New Language..."), this);
    actionCollection()->addAction("downloadnewstuff", newStuffAct );
    newStuffAct->setIcon(QIcon::fromTheme("get-hot-new-stuff"));
    //set default shortcut key combination for GetHotNewStuff
    actionCollection()->setDefaultShortcut(newStuffAct, QKeySequence(Qt::CTRL+Qt::Key_G));
    connect(newStuffAct, SIGNAL(triggered(bool)), this, SLOT(slotDownloadNewStuff()));

    //connect the quit action with close()
    KStandardAction::quit(this, SLOT(close()), actionCollection());

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
    connect(m_modeAction, SIGNAL(triggered(int)), this, SLOT(slotChangeTheme(int)));
    //m_modeAction->setItems(KHMThemeFactory::instance()->themeList());
    m_modeAction->setItems(m_khmfactory.themeList());
    m_modeAction->setCurrentItem(Prefs::mode());
    m_modeAction->setToolTip(i18n( "Choose the look and feel" ));
    m_modeAction->setWhatsThis(i18n( "Choose the look and feel" ));

    m_config = KSharedConfig::openConfig();

    m_recent=KStandardAction::openRecent(this, SLOT(slotOpenRecent(QUrl)), this);
    m_recent->setWhatsThis(i18n("Quick access to some of the files that you have recently opened."));
    actionCollection()->addAction(m_recent->objectName(), m_recent);
    m_recent->loadEntries(KConfigGroup(m_config, "KHangManRecent"));
    setupGUI();
}

// Set up the status bar with 4 different fields.
void KHangMan::setupStatusbar()
{
    // set up the status bar
    //m_levelLabel = new QLabel(this);
    //statusBar()->addPermanentWidget(m_levelLabel);
    //m_accentsLabel = new QLabel(this);
    //statusBar()->addPermanentWidget(m_accentsLabel);
    //m_winsLabel = new QLabel(this);
    //statusBar()->addPermanentWidget(m_winsLabel);
    //m_lossesLabel = new QLabel(this);
    //statusBar()->addPermanentWidget(m_lossesLabel);
    //statusBar()->insertPermanentItem("   ", IDS_LEVEL,   0);
    //statusBar()->insertPermanentItem("   ", IDS_ACCENTS, 0);
    //statusBar()->insertItem("   ", IDS_WINS,    1);
    //statusBar()->insertItem("   ", IDS_LOSSES,  1);
}


// ----------------------------------------------------------------
//                               Slots


bool KHangMan::queryClose()
{
    Prefs::setShowCharToolbar( m_specialCharToolbar->isVisible() );
    m_recent->saveEntries(KConfigGroup(m_config, "KHangManRecent"));
    Prefs::self()->save();
    return KMainWindow::queryClose();
}

void KHangMan::slotChangeLevel(int index)
{
    QMap<QString, QString>::const_iterator currentLevel = m_titleLevels.constBegin() + index;
    //m_levelLabel->setText(currentLevel.key());
    Prefs::setCurrentLevel(index);
    Prefs::setLevelFile(currentLevel.value());
    Prefs::self()->save();
    //m_view->readFile();
    //m_view->newGame();
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
    //m_view->newGame();
}

void KHangMan::slotChangeTheme(int index)
{
    Prefs::setMode(index);
    Prefs::self()->save();
    //m_view->setTheme(KHMThemeFactory::instance()->buildTheme(index));
    //m_view->setTheme(m_khmfactory.buildTheme(index));
}

void KHangMan::newGame (bool loss)
{
    m_loser=false;
    m_winner=false;
    m_bgfill=0;

    if (loss)
        m_lossCount++;

    //reset Hint action
    setHintEnabled (m_hintExists);

    readFile();

    setGameCount();
    /*if (Prefs::sound()) {
        QString soundFile = QStandardPaths::locate (QStandardPaths::GenericDataLocation, "khangman/sounds/new_game.ogg");
        qDebug () << "soundFile: " << soundFile;
        //play (soundFile);
    }*/

    reset();
    ++m_randomInt;
    game();
}

void KHangMan::game()
{
    qDebug() << "language " << Prefs::selectedLanguage();
    qDebug() << "level "    << Prefs::levelFile();

    m_word = m_randomList[m_randomInt % m_randomList.count()].first;
    m_word = m_word.toLower();
    m_hint = m_randomList[m_randomInt % m_randomList.count()].second;

    if (m_word.isEmpty()) {
        ++m_randomInt;
        game();
    }

    if (m_hint.isEmpty()) {
        m_hintExists = false;   // Hint does not exist.
    }
    else {
        m_hintExists = true;
    }
    setHintEnabled (m_hintExists);

    // Display the number of letters to guess with _
    for (int i = 0; i < m_word.length(); i++) {
        m_goodWord.append ("_ ");
    }

    // Remove the last trailing space.
    m_goodWord.remove (m_goodWord.length() - 1);

    // If needed, display white space or - if in word or semi dot.

    // 1. Find dashes.
    int posOfFirstDash = m_word.indexOf ( "-" );

    if (posOfFirstDash > 0) {
        m_goodWord.replace (2 * posOfFirstDash, 1, "-");
        int posOfSecondDash = m_word.indexOf ( "-", posOfFirstDash + 1);
        if (posOfSecondDash > 0) {
            m_goodWord.replace (2 * posOfSecondDash, 3, "-");
        }
        if (posOfSecondDash > 1) {
            m_goodWord.append("_");
        }
    }

    // 2. Find white space.
    m_posFirstSpace = m_word.indexOf( " " );

    if (m_posFirstSpace > 0) {
        m_goodWord.replace (2 * m_posFirstSpace, 1, " ");
        m_posSecondSpace = m_word.indexOf ( " ", m_posFirstSpace + 1);
        if (m_posSecondSpace > 0)
            m_goodWord.replace (2 * m_posSecondSpace, m_posFirstSpace + 1, " ");
    }

    // 3. Find ·
    int posOfDot = m_word.indexOf (QString::fromUtf8("·"));

    if (posOfDot > 0) {
        m_goodWord.replace (2 * posOfDot, 1, QString::fromUtf8("·"));
    }

    // 4. Find '
    int posOfApos = m_word.indexOf ("'");

    if (posOfApos > 0) {
        m_goodWord.replace (2 * posOfApos, 1, "'");
    }
}

void KHangMan::nextWord()
{
    if (m_randomList.count() == 0) {
        qDebug() << " m_randomList.count() = 0";
        //m_originalWord = m_randomList[0].first;
        //m_hint = m_randomList[0].second;
        return;
    } else {
        m_originalWord = m_randomList[m_randomInt%m_randomList.count()].first;
        m_originalWord = m_originalWord.toUpper();
        m_hint = m_randomList[m_randomInt%m_randomList.count()].second;
    }
    
    if (m_originalWord.isEmpty()) {
        ++m_randomInt;
        nextWord();
    }
    
    m_currentWord.clear();
    
    int originalWordSize = m_originalWord.size();
    
    while (m_currentWord.size() < originalWordSize)
        m_currentWord.append("_");
    
    ++m_randomInt;
}

void KHangMan::reset()
{
    m_goodWord = "";
    m_word   = "";

    m_guessedLetters.clear();
    m_numMissedLetters = 0;
    m_missedLetters    = "_ _ _ _ _ _ _ _ _ _  ";
}

void KHangMan::readFile()
{
    // Check if the data files are installed in the correct dir.
    QFile myFile;
    myFile.setFileName(Prefs::levelFile());

    if (!myFile.exists()) {
        QString  mString = i18n("File $KDEDIR/share/apps/kvtml/%1/%2 not found.\n"
        "Please check your installation.",
        Prefs::selectedLanguage(),
                                Prefs::levelFile());
        KMessageBox::sorry (this, mString, i18n("Error"));
        //qApp->quit();
    }

    // Detects if file is a kvtml file so that it's a hint enable file
    slotSetWordsSequence();
}

void KHangMan::slotSetWordsSequence()
{
    qDebug() << "in read kvtml file ";

    delete m_doc;

    m_doc = new KEduVocDocument(this);
    ///@todo open returns KEduVocDocument::ErrorCode
    m_doc->open (QUrl::fromLocalFile (Prefs::levelFile()), KEduVocDocument::FileIgnoreLock);

    //how many words in the file
    int wordCount = m_doc->lesson()->entryCount(KEduVocLesson::Recursive);

    qDebug() << "slotSetWordsSequence called, wordCount is " << wordCount;

    //get the words+hints
    KRandomSequence randomSequence;
    m_randomList.clear();
    for (int j = 0; j < wordCount; ++j) {
        QString hint = m_doc->lesson()->entries (KEduVocLesson::Recursive).at(j)->translation(0)->comment();
        if (hint.isEmpty() && m_doc->identifierCount() > 0) {
            // if there is no comment or it's empty, use the first translation if there is one
            hint = m_doc->lesson()->entries(KEduVocLesson::Recursive).at(j)->translation(1)->text();
        }
        if (!m_doc->lesson()->entries(KEduVocLesson::Recursive).at(j)->translation(0)->text().isEmpty()) {
            m_randomList.append(qMakePair(m_doc->lesson()->entries(KEduVocLesson::Recursive).at(j)->translation(0)->text(), hint));
        }
    }
    //shuffle the list
    randomSequence.randomize(m_randomList);
}

QString KHangMan::stripAccents(const QString & original)
{
    QString noAccents;
    QString decomposed = original.normalized(QString::NormalizationForm_D);
    for (int i = 0; i < decomposed.length(); ++i) {
        if ( decomposed[i].category() != QChar::Mark_NonSpacing ) {
            noAccents.append(decomposed[i]);
        }
    }
    return noAccents;
}

void KHangMan::replaceLetters(const QString& charString)
{
    QChar ch = charString.at(0);
    bool oneLetter = Prefs::oneLetter();

    for (int i = 0; i < m_originalWord.size(); ++i) {
        if (m_originalWord.at(i) == ch) {
            m_currentWord[i] = ch;

            if (oneLetter)
                break;
        }
    }
}

bool KHangMan::isResolved() const
{
    return m_currentWord == m_originalWord;
}

bool KHangMan::containsChar(const QString &sChar)
{
    return m_originalWord.contains(sChar) || stripAccents(m_originalWord).contains(sChar);
}

void KHangMan::setGameCount()
{
    emit signalSetWins(m_winCount);
    emit signalSetLosses(m_lossCount);
}

int KHangMan::hintHideTime()
{
    return Prefs::hintHideTime();
}

void KHangMan::setHintHideTime(int hintHideTime)
{
    Prefs::setHintHideTime(hintHideTime);
    emit hintHideTimeChanged();
}

int KHangMan::resolveTime()
{
    return Prefs::resolveTime();
}

void KHangMan::setResolveTime(int resolveTime)
{
    Prefs::setResolveTime(resolveTime);
    emit resolveTimeChanged();
}

bool KHangMan::isSound()
{
    return Prefs::sound();
}

void KHangMan::setSound(bool sound)
{
    Prefs::setSound(sound);
    emit soundToggled();
}

QString KHangMan::levelFile()
{
    return Prefs::levelFile();
}

void KHangMan::setLevelFile(const QString& levelFile)
{
    Prefs::setLevelFile(levelFile);
    emit levelFileChanged();
}

QString KHangMan::selectedLanguage()
{
    QStringList languageNames = SharedKvtmlFiles::languages();
    if (languageNames.isEmpty()) {
        QApplication::instance()->quit();
    }

    KConfig entry(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + "all_languages"));
    KConfigGroup group = entry.group(Prefs::selectedLanguage());
    QString selectedLanguage = group.readEntry("Name");

    if (!languageNames.contains(selectedLanguage)) {
        selectedLanguage = "English";
    }

    return selectedLanguage;
}

void KHangMan::setSelectedLanguage(const QString& selectedLanguage)
{
    Prefs::setSelectedLanguage(selectedLanguage);
    emit selectedLanguageChanged();
}

int KHangMan::currentLevel() const
{
    return Prefs::currentLevel();
}

QStringList KHangMan::categoryList() const
{
    return m_titleLevels.keys();
}

QString KHangMan::currentWord() const
{
    return m_currentWord;
}

void KHangMan::selectCurrentLevel(int index)
{
    Prefs::setCurrentLevel(index);
}

void KHangMan::selectLevelFile(int index)
{
    Prefs::setLevelFile(m_titleLevels.values().at(index));
}

void KHangMan::saveSettings()
{
    Prefs::self()->save();
}

QStringList KHangMan::currentWordLetters() const
{
    QStringList currentWordLetters;

    QString currentWord = this->currentWord();

    foreach (const QChar& currentWordLetter, currentWord)
    {
        currentWordLetters.append(currentWordLetter);
    }

    return currentWordLetters;
}

QStringList KHangMan::alphabet() const
{
    QStringList letterList;
    char c = 'A';

    while( c != 'Z') {
        letterList.append(QChar(c));
        ++c;
    }

    letterList.append(QChar(c));

    return letterList;
}

QStringList KHangMan::languageNames() const
{
    QStringList languageCodes = SharedKvtmlFiles::languages();
    if (languageCodes.isEmpty()) {
        QApplication::instance()->quit();
    }

    QStringList languageNames;

    // Get the language names from the language codes
    // Look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.

    KConfig entry(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + "all_languages"));

    foreach (const QString& languageCode, languageCodes)
    {
        KConfigGroup group = entry.group(languageCode);

        QString languageName = group.readEntry("Name");
        if (languageName.isEmpty())
        {
            languageName = i18nc("@item:inlistbox no language for that locale", "None");
        }

        languageNames.append(languageName);
    }

    return languageNames;
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

    for (int i = 0; i < m_languages.size(); ++i) {
        QLocale locale(m_languages[i]);
        QString languageName = locale.nativeLanguageName();
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
    /*if (Prefs::showCharToolbar()) {
        m_specialCharToolbar->show();
    }
    else {
        m_specialCharToolbar->hide();
    }*/
    setMessages();
}

void KHangMan::setLevel()
{
    m_currentLevel = Prefs::currentLevel();
    if (m_currentLevel > m_titleLevels.count()) {
        m_currentLevel = 0;
    }
    //m_view->readFile();
}

void KHangMan::show()
{
    if (loadLevels()) { // kvtml files have been found
        if (m_khmfactory.getQty() > 0) {  // themes present
            slotChangeTheme(Prefs::mode());
        } else { // themes not present
            QMessageBox::information(this, i18n("Error"), i18n("No theme files found."));
            close();
            return;
        }
        //call show() of base class KXmlGuiWindow
        KXmlGuiWindow::show();
        newGame();
    } else { // no kvtml files present
        QMessageBox::information(this, i18n("Error"), i18n("No kvtml files found."));
        close();
    }
    // add the qml view as the main widget
    QString location = QStandardPaths::locate(QStandardPaths::DataLocation, "qml/main.qml");
    QUrl url = QUrl::fromLocalFile(location);
    qDebug() << "URL passed to setSource: " << url.toString();
    m_view->setSource(url);
    //m_view->show();
    qDebug() << "added main.qml as widget";
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
    //m_levelLabel->setText(currentLevel.key());

    return true;
}


void KHangMan::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        //ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());
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

    //ui_language.kcfg_AccentedLetters->setEnabled(m_view->accentedLetters());

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
    /*QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog("khangman.knsrc", this);
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

    delete dialog;*/
}

void KHangMan::loadLangToolBar()
{
    QString lang = Prefs::selectedLanguage();
    if (lang.isEmpty())
        return;
    bool hasSpecialChars = LangUtils::hasSpecialChars(lang);

    // If the setting changed, save showToolbar setting
    /*if (m_specialCharToolbar->isVisible() && hasSpecialChars) {
        Prefs::setShowCharToolbar(true);
        Prefs::self()->save();
    }*/

    //m_specialCharToolbar->clear();

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

        /*for (int i=0; i<m_allData.count(); ++i) {
            if (!m_allData.at(i).isEmpty()) {
                QAction *act = m_specialCharToolbar->addAction(m_allData.at(i));
                act->setIcon(charIcon(m_allData.at(i).at(0)));
                // used to carry the id
                act->setData(i);
                connect(act, SIGNAL(triggered(bool)), this, SLOT(slotPasteChar()));
            }
        }*/
        //m_specialCharToolbar->show();
    } else {
        //m_specialCharToolbar->hide();
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

    //m_view->enterLetter(m_allData.at(id));
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
    //m_view->setAccentedLetters(LangUtils::hasAccentedLetters(Prefs::selectedLanguage()));
}

void KHangMan::setHintEnabled(bool enable)
{
    m_hintAct->setEnabled(enable);
}

void KHangMan::setMessages()
{
    // Tell the user about accented characters
    /*if (m_view->accentedLetters() && Prefs::accentedLetters()) {
        //m_accentsLabel->setText(i18n("Type accented letters"));
    }
    else {
        //m_accentsLabel->setText("");
    }*/
}

void KHangMan::loadFile(const QUrl &url)
{
    if ( url.isValid() )  {
        if(url.isLocalFile())
            Prefs::setLevelFile(url.toLocalFile());
        else Prefs::setLevelFile(url.path());

        //m_levelLabel->setText(url.path().section('/', -1));

        m_recent->addUrl(url);
        m_recent->saveEntries(KConfigGroup(m_config, "KHangManRecent"));
        Prefs::self()->save();

        //m_view->readFile();
        //m_view->newGame();
    }
}

void KHangMan::slotNewGame()
{
    //m_view->newGame(true);
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
    //m_winsLabel->setText(i18n("Wins: %1", wins));
}

void KHangMan::slotSetLosses(int losses)
{
    //m_lossesLabel->setText(i18n("Losses: %1", losses));
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

