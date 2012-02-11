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

#include "khangmanengine.h"

#include "prefs.h"

KHangManEngine::KHangManEngine()
{
}

KHangManEngine::~KHangManEngine()
{
}

void KHangManEngine::setLanguages()
{
    m_languages.clear();
    m_languageNames.clear();

    //the program scans in khangman/data/ to see what languages data is found
    m_languages = SharedKvtmlFiles::languages();
    if (m_languages.isEmpty()) {
        // qApp->closeAllWindows();
    }   
    //find duplicated entries in KDEDIR and KDEHOME

    // Write the present languages in config so they cannot be downloaded.
    // FIXME: use pre-seeding here
    KConfigGroup cg( KGlobal::config() ,"KNewStuff2");
    for (int i=0;  i<m_languages.count(); ++i) {
        //QString tmp = cg.readEntry(m_languages[i]);
       // if (!tmp)
        cg.writeEntry(m_languages[i], QDate::currentDate().toString(Qt::ISODate));
    }   
    cg.config()->sync();
    
    // We look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.

    KConfig entry(KStandardDirs::locate("locale", "all_languages"));
    for (int i = 0; i < m_languages.size(); ++i) {
        KConfigGroup group = entry.group(m_languages[i]);
        QString languageName = group.readEntry("Name");
        if (languageName.isEmpty()) {
            languageName = i18nc("@item:inlistbox no language for that locale","None");
        }   
        m_languageNames.append(languageName);
    }   
}

bool KhangManEngine::hasSpecialChars(const QString& lang)
{
    if (lang == QLatin1String("en")
            || lang == QLatin1String("en_GB")
            || lang == QLatin1String("it")
            || lang == QLatin1String("nl")
            || lang == QLatin1String("ru")
            || lang == QLatin1String("bg")
            || lang == QLatin1String("uk")
            || lang == QLatin1String("el")
            || lang == QLatin1String("ro"))
    {   
        return false;
    }   
    return true;
}

bool KHangManEngine::hasAccentedLetters(const QString& lang)
{
    if (lang == QLatin1String("es")
            || lang == QLatin1String("ca")
            || lang == QLatin1String("pt")
            || lang == QLatin1String("pt_BR"))
    {   
        return true;
    }   
    return false;
}

void KHangManEngine::loadSettings()
{
    // Language //TODO is selectedLanguage necessary??? only used here
    if (m_languages.isEmpty()){
        qApp->closeAllWindows();
    }   
    selectedLanguage = Prefs::selectedLanguage();
    if (!m_languages.contains(selectedLanguage)) {
        selectedLanguage = "en";
    }   
    int index = m_languages.indexOf(Prefs::selectedLanguage());
    if (index < 0) {
        // if the selected language is not available, use the first available one
        index = 0;
    }
}

void KHangManEngine::setLevel()
{
    m_currentLevel = Prefs::currentLevel();
    if (m_currentLevel > m_titleLevels.count()) {
        m_currentLevel = 0;
    }   
}

void KHangManEngine::slotSetWordsSequence()
{
    delete m_doc;
    m_doc = new KEduVocDocument(this);
    ///@todo open returns KEduVocDocument::ErrorCode
    m_doc->open(Prefs::levelFile());

    //how many words in the file
    int wordCount = m_doc->lesson()->entryCount(KEduVocLesson::Recursive);

    //get the words+hints
    KRandomSequence randomSequence;
    m_randomList.clear();
    for (int j = 0; j < wordCount; ++j) {
        QString hint = m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(0)->comment();
        if (hint.isEmpty() && m_doc->identifierCount() > 0) {
            // if there is no comment or it's empty, use the first translation if
            // there is one
            hint = m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(1)->text();
        }   
        if (!m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(0)->text().isEmpty()) {
            m_randomList.append(qMakePair(m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(0)->text(), hint));  
        }   
    }   
    //shuffle the list
    randomSequence.randomize(m_randomList);
}

QString KHangManEngine::stripAccents(const QString &original)
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

bool KHangManEngine::containsChar(const QString &sChar)
{
    return m_word.contains(sChar) || stripAccents(m_word).contains(sChar);
}

    // Check if the data files are installed in the correct dir.
    QFile    myFile;
    myFile.setFileName(Prefs::levelFile());

    if (!myFile.exists()) {
        QString  mString = i18n("File $KDEDIR/share/apps/kvtml/%1/%2 not found.\n"
                                "Please check your installation.",
                                 Prefs::selectedLanguage(),
                                 Prefs::levelFile());
        KDebug << mString;
    }   

    // Detects if file is a kvtml file so that it's a hint enable file
    slotSetWordsSequence();
}

void KHangManEngine::newWord()
{
    m_word = m_randomList[m_randomInt%m_randomList.count()].first;
    m_word = m_word.toLower();
    m_hint = m_randomList[m_randomInt%m_randomList.count()].second;

    if (m_word.isEmpty()) {
        ++m_randomInt;
        game();
    }

    if (m_hint.isEmpty()) {
        m_hintExists = false;   // Hint does not exist.
        //TODO refresh hint action
    }
    else {
        m_hintExists = true;
        //TODO refresh hint action
    }
    khangman->hintAct->setEnabled(m_hintExists);

    // Display the number of letters to guess with _
    for (int i = 0; i < m_word.length(); i++) {
        goodWord.append("_ ");
    }

    // Remove the last trailing space.
    goodWord.remove(goodWord.length()-1);

    // If needed, display white space or - if in word or semi dot.

    // 1. Find dashes.
    int posOfFirstDash = m_word.indexOf( "-" );

    if (posOfFirstDash>0) {
        goodWord.replace(2*posOfFirstDash, 1, "-");
        int posOfSecondDash = m_word.indexOf( "-", posOfFirstDash+1);
        if (posOfSecondDash>0) {
            goodWord.replace(2*posOfSecondDash, 3, "-");
        }
        if (posOfSecondDash>1) {
            goodWord.append("_");
        }
    }

    // 2. Find white space.
    m_posFirstSpace = m_word.indexOf( " " );

    if (m_posFirstSpace > 0) {
        goodWord.replace(2*m_posFirstSpace, 1, " ");
        m_posSecondSpace = m_word.indexOf( " ", m_posFirstSpace+1);
        if (m_posSecondSpace > 0)
            goodWord.replace(2*m_posSecondSpace, m_posFirstSpace+1, " ");
    }

    // 3. Find ·
    int posOfDot = m_word.indexOf( QString::fromUtf8("·") );

    if (posOfDot>0) {
        goodWord.replace(2*posOfDot, 1, QString::fromUtf8("·") );
    }

    // 4. Find '
    int posOfApos = m_word.indexOf( "'" );

    if (posOfApos>0) {
        goodWord.replace(2*posOfApos, 1, "'");
    }
}


#include "khangmanengine.moc"
