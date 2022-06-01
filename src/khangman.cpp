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

    //find the themes
    m_themeFactory.addTheme(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("khangman/themes/standardthemes.xml")));
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
        Q_EMIT currentLanguageChanged();
    }
}

void KHangMan::setCurrentTheme(int index)
{
    const KHMTheme *theme = m_themeFactory.getTheme(index);
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
    // If there are no words, there's nothing we can do, trying will crash
    if (m_randomList.isEmpty()) {
        return;
    }

    // Pick words from m_randomList sequentially since it is already shuffled.
    int whichWord = m_randomInt++ % m_randomList.count();
    m_originalWord = m_randomList[whichWord].first;
    m_originalWord = m_originalWord.toUpper();
    m_hint = m_randomList[whichWord].second;

    Q_EMIT currentHintChanged();

    m_currentWord.clear();

    // Find dashes, spaces, middot and apostrophe
    const QString search = QStringLiteral("- ·'");
    for (const QChar &c : m_originalWord) {
        if (search.contains(c)) {
            m_currentWord.append(c);
        } else {
            m_currentWord.append(QLatin1Char('_'));
        }
    }

    Q_EMIT currentWordChanged();
}

void KHangMan::slotSetWordsSequence()
{
    //Current level file
    KEduVocDocument doc(this);

    ///@todo open returns KEduVocDocument::ErrorCode
    doc.open(QUrl::fromLocalFile(Prefs::levelFile()), KEduVocDocument::FileIgnoreLock);

    //how many words in the file
    QList<KEduVocExpression *> words = doc.lesson()->entries(KEduVocLesson::Recursive);

    //get the words+hints
    m_randomList.clear();
    for (KEduVocExpression* entry : words) {
        QString hint = entry->translation(0)->comment();
        if (hint.isEmpty() && doc.identifierCount() > 0) {
            // if there is no comment or it's empty, use the first translation if there is one
            hint = entry->translation(1)->text();
        }
        QString text = entry->translation(0)->text();
        if (!text.isEmpty()) {
            m_randomList.append(qMakePair(text, hint));
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
    const KHMTheme *theme = m_themeFactory.getTheme(currentTheme());
    if (theme) {
        return QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("themes/") + theme->svgFileName());
    }
    return QString();
}

QColor KHangMan::currentThemeLetterColor()
{
    const KHMTheme *theme = m_themeFactory.getTheme(currentTheme());
    if (theme) {
        return theme->letterColor();
    }

    // Default to white letters
    return QColor("white");
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
    for (char c = 'A'; c <= 'Z'; ++c) {
        letterList.append(QChar(c));
    }

    letterList.append(m_specialCharacters);

    return letterList;
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

void KHangMan::show()
{
    loadLevels();
    // kvtml files have been found

    if (m_themeFactory.getQty() == 0) { // themes not present
        KMessageBox::sorry(this, i18n("No theme files found."), i18n("Error"));
        exit(EXIT_FAILURE);
    }

    QMainWindow::show();
    // add the qml view as the main widget
    QString location = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("qml/main.qml"));
    QUrl url = QUrl::fromLocalFile(location);
    m_view->setSource(url);
}

void KHangMan::loadLevels()
{
    //build the Level combobox menu dynamically depending of the data for each language
    m_titleLevels.clear();
    QStringList levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());

    if (levelFilenames.isEmpty()) {
        Prefs::setSelectedLanguage(QStringLiteral("en"));
        Prefs::self()->save();
        levelFilenames = SharedKvtmlFiles::fileNames(Prefs::selectedLanguage());
        if (levelFilenames.isEmpty()) {
            KMessageBox::sorry(this, i18n("No kvtml files found."), i18n("Error"));
            exit(EXIT_FAILURE);
        }
    }

    const QStringList titles = SharedKvtmlFiles::titles(Prefs::selectedLanguage());
    Q_ASSERT(levelFilenames.count() == titles.count());
    for(int i = 0; i < levelFilenames.count(); ++i) {
        m_titleLevels.insert(titles.at(i), levelFilenames.at(i));
    }
    Q_EMIT categoriesChanged();

    int level = Prefs::currentLevel();
    if (level >= m_titleLevels.count() || !levelFilenames.contains(Prefs::levelFile())) {
        level = 0;
    }
    setCurrentCategory(level);

    loadLanguageSpecialCharacters();
}

void KHangMan::slotDownloadNewStuff()
{
    KNS3::QtQuickDialogWrapper *dialog = new KNS3::QtQuickDialogWrapper(QStringLiteral("khangman.knsrc"), this);
    dialog->open();

    connect(dialog, &KNS3::QtQuickDialogWrapper::closed, this, [this, dialog] {
        const QList<KNSCore::EntryInternal> entries = dialog->changedEntries();
        if ( !entries.isEmpty() ){
            SharedKvtmlFiles::sortDownloadedFiles();
            //look for languages dirs installed
            scanLanguages();
            //refresh Languages menu
            setCurrentLanguage(m_languages.indexOf(Prefs::selectedLanguage()));
        }

        dialog->deleteLater();
    });
}

void KHangMan::loadLanguageSpecialCharacters()
{
    QString lang = Prefs::selectedLanguage();
    if (lang.isEmpty())
        return;

    m_specialCharacters.clear();
    if (!LangUtils::hasSpecialChars(lang)) {
        return;
    }

    QString langFileName=QStringLiteral("khangman/%1.txt").arg(lang);
    QFile langFile;
    langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));

    // Let's look in local KDEHOME dir then KNS installs each .txt
    // in kvtml/<lang> as it installs everything at the same place
    if (!langFile.exists()) {
        langFileName = QStringLiteral("apps/kvtml/%1/%1.txt").arg(lang);
        langFile.setFileName(QStandardPaths::locate(QStandardPaths::GenericDataLocation, langFileName));
        if (!langFile.exists()) {
            return;
        }
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


// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

