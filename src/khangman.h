 // SPDX-FileCopyrightText: 2001-2009 Anne-Marie Mahfouf <annma@kde.org>
 // SPDX-FileCopyrightText: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
 // SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KSharedConfig>
#include <KNSCore/Entry>
#include <QtQml>

#include "khmthemefactory.h"

/**
 * @short KHangMan Main Window
 * @author Anne-Marie Mahfouf <annemarie.mahfouf@free.fr>
 * @version 3.0
 */
class KHangMan : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY( int resolveTime READ resolveTime WRITE setResolveTime NOTIFY resolveTimeChanged )
    Q_PROPERTY( bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged )

    Q_PROPERTY( int currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged )
    Q_PROPERTY( QStringList languages READ languages NOTIFY languagesChanged)

    Q_PROPERTY( int currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged )
    Q_PROPERTY( QStringList categories READ categories NOTIFY categoriesChanged)

    Q_PROPERTY( int currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged )
    Q_PROPERTY( QStringList themes READ themes NOTIFY themesChanged)
    Q_PROPERTY( QUrl backgroundUrl READ backgroundUrl NOTIFY currentThemeChanged)
    Q_PROPERTY( QColor letterColor READ currentThemeLetterColor NOTIFY currentThemeChanged)

    Q_PROPERTY( QStringList currentWord READ currentWord NOTIFY currentWordChanged)
    Q_PROPERTY( QString currentHint READ getCurrentHint NOTIFY currentHintChanged)
    Q_PROPERTY( QStringList alphabet READ alphabet NOTIFY currentLanguageChanged)

    Q_PROPERTY( int winCount READ winCount WRITE setWinCount NOTIFY winCountChanged )
    Q_PROPERTY( int lossCount READ lossCount WRITE setLossCount NOTIFY lossCountChanged )

    Q_PROPERTY( int scoreMultiplyingFactor READ scoreMultiplyingFactor WRITE setScoreMultiplyingFactor NOTIFY scoreMultiplyingFactorChanged )

    Q_PROPERTY( int netScore READ netScore NOTIFY netScoreChanged )

public:
    /**
    * Default Constructor
    */
    KHangMan();

    // These accessor and mutator methods are not needed once the
    // kconfig_compiler can generate Q_INVOKABLE methods, slots or/and
    // properties
    int resolveTime();
    void setResolveTime(int resolveTime);

    /** Getter and Setter for soundEnabled property */
    bool soundEnabled() const;
    void setSoundEnabled(bool sound);

    int currentCategory() const;
    QStringList categories() const;

    int currentLanguage() const;
    QStringList languages() const;

    int winCount() const;
    int lossCount() const;

    int scoreMultiplyingFactor() const;
    int netScore() const;

    int currentTheme() const;
    QStringList themes() const;
    QUrl backgroundUrl() const;
    QColor currentThemeLetterColor() const;

    /** Calculate the net score */
    void calculateNetScore();

    Q_INVOKABLE QStringList currentWord() const;

    /** Get the current hint */
    Q_INVOKABLE QString getCurrentHint() const;

    Q_INVOKABLE QStringList alphabet() const;

    /** Return true if the word contains the char in the QString */
    Q_INVOKABLE bool containsChar(const QString &original) const;

    /** Return true if the answer and the current word match */
    Q_INVOKABLE bool isResolved() const;

    /** Reveals the solution to the current puzzle */
    Q_INVOKABLE void revealCurrentWord();

public Q_SLOTS:
    ///When the language is changed in the Language menu
    void setCurrentLanguage(int index);

    ///When the category is changed in the Category menu
    void  setCurrentCategory(int index);

    /** Set the current theme */
    void setCurrentTheme(int index);

    ///access the KNewStuff class to install new data
    void slotDownloadNewStuff(KNSCore::Entry *entry);

    ///Load kvtml file and get a word and its tip in random
    void readFile();

    /** Generate a new word */
    void nextWord();

    /** Sets the count for number of words correctly guessed */
    void setWinCount(int count);

    /** Sets the count for number of words wrongly guessed */
    void setLossCount(int count);

    /** Sets the score multiplying factor */
    void setScoreMultiplyingFactor( int factor );

    /** Handle the guessed letter */
    void replaceLetters(const QString& charString);

    void showHandbook() const;

Q_SIGNALS:

    void resolveTimeChanged();
    void soundEnabledChanged();
    void currentLanguageChanged();
    void currentCategoryChanged();
    void currentWordChanged();
    void currentHintChanged();
    void languagesChanged();
    void categoriesChanged();
    void themesChanged();
    void currentThemeChanged();
    void winCountChanged();
    void lossCountChanged();
    void scoreMultiplyingFactorChanged();
    void netScoreChanged();
    void errorOccured(const QString &text);

private:
    KConfigGroup config(const QString &group);

    ///Scan the files in the selected language dir to set the levels
    void loadLevels();
    // Populate m_specialCharacters if the
    // current language has special characters.
    void loadLanguageSpecialCharacters();

    /** Scan the languages found on disk and load the current
     * language from settings with en as default.
     */
    void scanLanguages();

    //shuffle words+hints
    void slotSetWordsSequence();

    ///the different data titles and files in the current language dir
    QMap<QString, QString> m_titleLevels;

    int m_currentCategory;
    int m_currentLanguage;

    // language information
    QStringList m_languages;
    QStringList m_languageNames;

    // Contains all the words that are read from the data file.
    QStringList m_specialCharacters;

    //Theme manager
    KHMThemeFactory m_themeFactory;

    //Config group
    KSharedConfig::Ptr m_config;

    int m_winCount;
    int m_lossCount;

    //The index to the random sequence
    int m_randomInt;

    // Multiplying factor for scores
    int m_scoreMultiplyingFactor;

    int m_netScore;

    //The random sequence of words of the current level file
    QList<QPair<QString, QString> > m_randomList;

    /** The word to be guessed */
    QString m_originalWord;

    /** The hidden word that is filled in during the game. */
    QString m_currentWord;

    //Current hint
    QString m_hint;
};

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:
