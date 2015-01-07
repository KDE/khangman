/***************************************************************************
 *   Copyright 2001-2009 Anne-Marie Mahfouf <annma@kde.org>                *
 *   Copyright 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>          *
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


#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#include <KSharedConfig>

#include <QMainWindow>

#include "khmthemefactory.h"

class KEduVocDocument;
class QQmlEngine;
class QQuickWidget;
class KHelpMenu;

/**
 * @short KHangMan Main Window
 * @author Anne-Marie Mahfouf <annemarie.mahfouf@free.fr>
 * @version 3.0
 */
class KHangMan : public QMainWindow
{
    Q_OBJECT

    Q_PROPERTY( int resolveTime READ resolveTime WRITE setResolveTime NOTIFY resolveTimeChanged )
    Q_PROPERTY( bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged )
    Q_PROPERTY( int currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged )
    Q_PROPERTY( int currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged )
    Q_PROPERTY( QStringList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY( QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY( QStringList currentWord READ currentWord NOTIFY currentWordChanged)
    Q_PROPERTY( QString currentHint READ getCurrentHint NOTIFY currentHintChanged)
    Q_PROPERTY( QStringList alphabet READ alphabet NOTIFY currentLanguageChanged)

public:
    /**
    * Default Constructor
    */
    KHangMan();

    /**
    * Default Destructor
    */
    virtual ~KHangMan();

    // These accessor and mutator methods are not needed once the
    // kconfig_compiler can generate Q_INVOKABLE methods, slots or/and
    // properties
    int resolveTime();
    void setResolveTime(int resolveTime);

    /** Getter and Setter for soundEnabled property */
    bool soundEnabled();
    void setSoundEnabled(bool sound);

    int currentCategory();

    int currentLanguage();

    QStringList languages();

    QStringList categories();

    //Display the mainwindow only when kvtml files are present, else show an error message and quit.
    void show();

    // get m_view->engine()
    QQmlEngine* getEngine();

    /** Get the current categories available */
    Q_INVOKABLE QStringList categoryList() const;

    Q_INVOKABLE QStringList currentWord() const;

    /** Get the current hint */
    Q_INVOKABLE QString getCurrentHint() const;

    Q_INVOKABLE QStringList alphabet() const;

    Q_INVOKABLE QStringList languageNames() const;

    /** Return true if the word contains the char in the QString */
    Q_INVOKABLE bool containsChar(const QString &original);

    /** Return true if the answer and the current word match */
    Q_INVOKABLE bool isResolved() const;

public slots:
    ///When the language is changed in the Language menu
    void setCurrentLanguage(int index);
    ///When the category is changed in the Category menu
    void  setCurrentCategory(int index);
    ///access the KNewStuff class to install new data
    void slotDownloadNewStuff();

    ///Load kvtml file and get a word and its tip in random
    void readFile();

    /** Generate a new word */
    void nextWord();

    /** Handle the guessed letter */
    void replaceLetters(const QString& charString);

    void showAboutKHangMan();
    void showAboutKDE();
    void showHandbook();

signals:

    void signalSetWins(int wins);
    void signalSetLosses(int losses);
    void resolveTimeChanged();
    void soundEnabledChanged();
    void currentLanguageChanged();
    void currentCategoryChanged();
    void currentWordChanged();
    void currentHintChanged();
    void languagesChanged();
    void categoriesChanged();

private slots:
    // Slots for when the user changes level, setting, etc.
    void  slotChangeTheme(int);

private:
    KConfigGroup config(const QString &group);

    /** Strip the accents off given string
     * @params original string to strip accents off of
     * @returns string without accents
     */
    QString stripAccents(const QString & original);

    ///Set the level and make sure it exists
    void setLevel();
    ///Scan the files in the selected language dir to set the levels, returns true if succeeds
    bool loadLevels();
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

    // Some important members: the view and newStuff.
    QQuickWidget   *m_view;

    // Contains all the words that are read from the data file.
    QStringList m_specialCharacters;

    //Theme manager
    KHMThemeFactory m_khmfactory;

    //Config group
    KSharedConfig::Ptr m_config;

    int m_winCount;
    int m_lossCount;

    //The index to the random sequence
    int m_randomInt;

    //The random sequence of words of the current level file
    QList<QPair<QString, QString> > m_randomList;

    //Current level file
    KEduVocDocument *m_doc;

    /** The word to be guessed */
    QString m_originalWord;

    /** The hidden word that is filled in during the game. */
    QString m_currentWord;

    //Current hint
    QString m_hint;

    /** help menu for displaying about box */
    KHelpMenu *m_helpMenu;
};

#endif // _KHANGMAN_H_

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:
