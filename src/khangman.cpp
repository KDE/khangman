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

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QPointer>

#include <KLocalizedContext>
#include <KLocalizedString>
#include <KMessageBox>
#include <KRandom>
#include <KNS3/QtQuickDialogWrapper>
#include <KHelpMenu>

#include <KEduVocDocument>
#include <keduvocexpression.h>
#include <sharedkvtmlfiles.h>

#include "langutils.h"
#include "prefs.h"

KHangMan::KHangMan()
        : QMainWindow(),
          m_currentCategory(0),
          m_currentLanguage(0),
          m_winCount(0),
          m_lossCount(0),
          m_randomInt(0),
          m_scoreMultiplyingFactor(1),
          m_netScore(0),
          m_doc(nullptr),
          m_helpMenu(new KHelpMenu(NULL))
{
    setObjectName(QStringLiteral("KHangMan"));

    m_view = new QQuickWidget(this);
    m_view->rootContext()->setContextProperty(QStringLiteral("khangman"), this);

    // prepare i18n
    auto context = new KLocalizedContext(this);
    m_view->engine()->rootContext()->setContextObject(context);

    KConfigGroup windowConfig = config(QStringLiteral("Window"));
    if (windowConfig.hasKey("geometry")) {
        setGeometry(windowConfig.readEntry<QRect>("geometry", QRect()));
        setWindowState(Qt::WindowState(windowConfig.readEntry("windowState").toInt()));
    }

    setMinimumSize(800, 600);

    m_view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    setCentralWidget(m_view);
    scanLanguages();
    setLevel();

    //find the themes
    m_themeFactory.addTheme(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("khangman/themes/standardthemes.xml")));

    loadLanguageSpecialCharacters();
}

KHangMan::~KHangMan()
{
    KConfigGroup windowConfig = config(QStringLiteral("Window"));
    windowConfig.writeEntry("geometry", geometry());
    windowConfig.writeEntry("windowState", int(windowState()));

    delete m_view;
    m_view = NULL;
}

void KHangMan::showAboutKHangMan()
{
    m_helpMenu->aboutApplication();
}

void KHangMan::showAboutKDE()
{
    m_helpMenu->aboutKDE();
}

void KHangMan::showHandbook()
{
    m_helpMenu->appHelpActivated();
}

KConfigGroup KHangMan::config(const QString &group)
{
    return KConfigGroup(KSharedConfig::openConfig(qApp->applicationName() + QStringLiteral("rc")), group);
}

// ----------------------------------------------------------------
//                               Slots
void KHangMan::setCurrentCategory(int index)
{
    QMap<QString, QString>::const_iterator currentLevel = m_titleLevels.constBegin() + index;
    Prefs::setCurrentLevel(index);
    Prefs::setLevelFile(currentLevel.value());
    Prefs::self()->save();
    m_currentCategory = index;
    Q_EMIT currentCategoryChanged();
}

void KHangMan::setCurrentLanguage(int index)
{
    if (index >= 0 && index < m_languages.size()) {
        Prefs::setSelectedLanguage(m_languages[index]);
        m_currentLanguage = index;
        Prefs::self()->save();
        loadLevels();
        loadLanguageSpecialCharacters();
        setLevel();
        Q_EMIT currentLanguageChanged();
    }
}

void KHangMan::setCurrentTheme(int index)
{
    KHMTheme *theme = m_themeFactory.buildTheme(index);
    Prefs::setTheme(theme->name());
    Prefs::self()->save();
    Q_EMIT currentThemeChanged();
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
    }

    // Detects if file is a kvtml file so that it's a hint enable file
    slotSetWordsSequence();
}

void KHangMan::nextWord()
{
    if (m_randomList.count() == 0) {
        m_originalWord = m_randomList[0].first;
        m_hint = m_randomList[0].second;
        return;
    } else {
        m_originalWord = m_randomList[m_randomInt%m_randomList.count()].first;
        m_originalWord = m_originalWord.toUpper();
        m_hint = m_randomList[m_randomInt%m_randomList.count()].second;

        Q_EMIT currentHintChanged();
    }

    if (m_originalWord.isEmpty()) {
        ++m_randomInt;
        nextWord();
    }

    m_currentWord.clear();

    int originalWordSize = m_originalWord.size();

    while (m_currentWord.size() < originalWordSize)
        m_currentWord.append(QLatin1Char('_'));

    // Find dashes, spaces, middot and apostrophe
    const QString search = QStringLiteral("- ·'");
    for (const QChar &key : search) {
        int pos = m_originalWord.indexOf( key );
        while (pos > 0) {
            m_currentWord.replace(pos, 1, key);
            pos = m_originalWord.indexOf( key );
        }
    }

    Q_EMIT currentWordChanged();

    ++m_randomInt;
}

void KHangMan::slotSetWordsSequence()
{
    delete m_doc;

    m_doc = new KEduVocDocument(this);
    ///@todo open returns KEduVocDocument::ErrorCode
    m_doc->open (QUrl::fromLocalFile (Prefs::levelFile()), KEduVocDocument::FileIgnoreLock);

    //how many words in the file
    int wordCount = m_doc->lesson()->entryCount(KEduVocLesson::Recursive);

    //get the words+hints
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
    KRandom::shuffle(m_randomList);
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
    Q_EMIT currentWordChanged();
}

bool KHangMan::isResolved() const
{
    return m_currentWord == m_originalWord;
}

void KHangMan::revealCurrentWord()
{
    m_currentWord = m_originalWord;
    Q_EMIT currentWordChanged();
}

void KHangMan::calculateNetScore()
{
    m_netScore = ( m_winCount - m_lossCount ) * m_scoreMultiplyingFactor;
    Q_EMIT netScoreChanged();
}

bool KHangMan::containsChar(const QString &sChar)
{
    return m_originalWord.contains(sChar) || stripAccents(m_originalWord).contains(sChar);
}

int KHangMan::resolveTime()
{
    return Prefs::resolveTime();
}

void KHangMan::setResolveTime(int resolveTime)
{
    Prefs::setResolveTime(resolveTime);
    Q_EMIT resolveTimeChanged();
}

bool KHangMan::soundEnabled()
{
    return Prefs::sound();
}

void KHangMan::setSoundEnabled(bool sound)
{
    Prefs::setSound(sound);
    Q_EMIT soundEnabledChanged();
}

QStringList KHangMan::languages()
{
    return m_languageNames;
}

int KHangMan::winCount() const
{
    return m_winCount;
}

void KHangMan::setWinCount(int count)
{
    m_winCount = count;
    calculateNetScore();
    Q_EMIT winCountChanged();
}

int KHangMan::lossCount() const
{
    return m_lossCount;
}

void KHangMan::setLossCount(int count)
{
    m_lossCount = count;
    calculateNetScore();
    Q_EMIT lossCountChanged();
}

int KHangMan::scoreMultiplyingFactor() const
{
    return m_scoreMultiplyingFactor;
}

void KHangMan::setScoreMultiplyingFactor( int factor )
{
    m_scoreMultiplyingFactor = factor;
    calculateNetScore();
    Q_EMIT scoreMultiplyingFactorChanged();
}

int KHangMan::netScore() const
{
    return m_netScore;
}

int KHangMan::currentLanguage()
{
    return m_currentLanguage;
}

QStringList KHangMan::themes()
{
    return m_themeFactory.themeList();
}

int KHangMan::currentTheme()
{
    QStringList themes = m_themeFactory.getNames();
    return themes.indexOf(Prefs::theme());
}

QString KHangMan::backgroundUrl()
{
    QStringList themes = m_themeFactory.getNames();
    int index = themes.indexOf(Prefs::theme());
    KHMTheme *theme = m_themeFactory.buildTheme(index);
    if (theme) {
        QString filename = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("themes/") + theme->svgFileName());
        return filename;
    }
    return QString();
}

QColor KHangMan::currentThemeLetterColor()
{
    // Default to white letters
    QColor color = "white";
    QStringList themes = m_themeFactory.getNames();
    int index = themes.indexOf(Prefs::theme());
    KHMTheme *theme = m_themeFactory.buildTheme(index);
    if (theme) {
        color = theme->letterColor();
    }

    return color;
}

QStringList KHangMan::categories()
{
    return m_titleLevels.keys();
}

int KHangMan::currentCategory()
{
    return m_currentCategory;
}

QStringList KHangMan::currentWord() const
{
    QStringList currentWordLetters;

    for (const QChar& currentWordLetter : std::as_const(m_currentWord))
    {
        currentWordLetters.append(currentWordLetter);
    }

    return currentWordLetters;
}

QString KHangMan::getCurrentHint() const
{
    return m_hint;
}

QQmlEngine* KHangMan::getEngine()
{
    return m_view->engine();
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
    letterList.append(m_specialCharacters);

    return letterList;
}

QStringList KHangMan::languageNames() const
{
    QStringList languageCodes = SharedKvtmlFiles::languages();
    if (languageCodes.isEmpty()) {
        QApplication::instance()->quit();
    }

    QStringList languageNames;

    for (const QString& languageCode : std::as_const(languageCodes))
    {
        QLocale locale(languageCode);
        QString languageName = locale.nativeLanguageName();

        languageNames.append(languageName);
    }

    return languageNames;
}

// ----------------------------------------------------------------


void KHangMan::scanLanguages()
{
    m_languageNames.clear();

    //the program scans in khangman/data/ to see what languages data is found
    m_languages = SharedKvtmlFiles::languages();
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

        if (m_languages[i] == Prefs::selectedLanguage())
            m_currentLanguage = i;
    }

    Q_EMIT languagesChanged();
}

void KHangMan::setLevel()
{
    m_currentCategory = Prefs::currentLevel();
    if (m_currentCategory > m_titleLevels.count()) {
        m_currentCategory = 0;
    }
    Q_EMIT currentCategoryChanged();
}

void KHangMan::show()
{
    if (loadLevels()) { // kvtml files have been found
        if (m_themeFactory.getQty() > 0) {  // themes present
            QMainWindow::show();
            // add the qml view as the main widget
            QString location = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("qml/main.qml"));
            QUrl url = QUrl::fromLocalFile(location);
            m_view->setSource(url);
        } else { // themes not present
            QMessageBox::information(this, i18n("Error"), i18n("No theme files found."));
            exit(EXIT_FAILURE);
        }
    } else { // no kvtml files present
        QMessageBox::information(this, i18n("Error"), i18n("No kvtml files found."));
        exit(EXIT_FAILURE);
    }
}

bool KHangMan::loadLevels()
{
    //build the Level combobox menu dynamically depending of the data for each language
    m_titleLevels.clear();
    QStringList levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
    QStringList titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());

    if (levelFilenames.size() == 0) {
        Prefs::setSelectedLanguage(QStringLiteral("en"));
        Prefs::self()->save();
        levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
        titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());
    }

    if (levelFilenames.isEmpty()){
        // no kvtml files found
        return false;
    }

    Q_ASSERT(levelFilenames.count() == titles.count());
    for(int i = 0; i < levelFilenames.count(); ++i) {
        m_titleLevels.insert(titles.at(i), levelFilenames.at(i));
    }
    Q_EMIT categoriesChanged();

    if (!levelFilenames.contains(Prefs::levelFile())) {
        Prefs::setLevelFile(m_titleLevels.constBegin().value());
        Prefs::setCurrentLevel(0);
        m_currentCategory = 0;
        Q_EMIT currentCategoryChanged();
        Prefs::self()->save();
    }

    // don't run off the end of the list
    if (m_currentCategory!=-1 && m_currentCategory > m_titleLevels.count()) {
        m_currentCategory = m_titleLevels.count();
        Q_EMIT currentCategoryChanged();
    }

    setLevel();

    return true;
}

void KHangMan::slotDownloadNewStuff()
{
    KNS3::QtQuickDialogWrapper dialog(QStringLiteral("khangman.knsrc"), this);
    const QList<KNSCore::EntryInternal> entries = dialog.exec();
    if ( entries.size() > 0 ){
        SharedKvtmlFiles::sortDownloadedFiles();
        //look for languages dirs installed
        scanLanguages();
        //refresh Languages menu
        setCurrentLanguage(m_languages.indexOf(Prefs::selectedLanguage()));
    }
}

void KHangMan::loadLanguageSpecialCharacters()
{
    QString lang = Prefs::selectedLanguage();
    if (lang.isEmpty())
        return;
    bool hasSpecialChars = LangUtils::hasSpecialChars(lang);

    m_specialCharacters.clear();
    if (hasSpecialChars) {
        QString langFileName=QStringLiteral("khangman/%1.txt").arg(lang);
        QFile langFile;
        langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));

        // Let's look in local KDEHOME dir then KNS installs each .txt
        // in kvtml/<lang> as it installs everything at the same place
        if (!langFile.exists()) {
            langFileName = QStringLiteral("apps/kvtml/%1/%1.txt").arg(lang);
            langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));
        }

        if (!langFile.exists()) {
            return;
        }

        update();

        // We open the file and store info into the stream...
        QFile openFileStream(langFile.fileName());
        openFileStream.open(QIODevice::ReadOnly);
        QTextStream readFileStr(&openFileStream);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        readFileStr.setCodec("UTF-8");
#endif

        // m_specialCharacters contains all the words from the file
        // FIXME: Better name
        m_specialCharacters = readFileStr.readAll().split(QLatin1Char('\n'));
        openFileStream.close();
    }
}


// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

