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
#include <KXmlGuiWindow>
#include <QLineEdit>

#include <keduvocdocument.h>

#include <QQuickWidget>
#include <QPushButton>

#include "ui_generalui.h"
#include "ui_languageui.h"
#include "khmthemefactory.h"
//#include "khangmanengine.h"
//#include "khangmanenginehelper.h"

class KSelectAction;
class KToggleAction;
class KRecentFilesAction;

namespace Phonon {
    class MediaObject;
}

class QLabel;

/**
 * @short Application Main Window
 * @author Anne-Marie Mahfouf <annemarie.mahfouf@free.fr>
 * @version 0.1
 */

class KHangMan : public KXmlGuiWindow
{
    Q_OBJECT

    Q_PROPERTY( int hintHideTime READ hintHideTime WRITE setHintHideTime NOTIFY hintHideTimeChanged )
    Q_PROPERTY( int resolveTime READ resolveTime WRITE setResolveTime NOTIFY resolveTimeChanged )
    Q_PROPERTY( bool sound READ isSound WRITE setSound NOTIFY soundToggled )
    Q_PROPERTY( QString levelFile READ levelFile WRITE setLevelFile NOTIFY levelFileChanged )
    Q_PROPERTY( QString selectedLanguage READ selectedLanguage WRITE setSelectedLanguage NOTIFY selectedLanguageChanged )

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

    int hintHideTime();
    void setHintHideTime(int hintHideTime);

    int resolveTime();
    void setResolveTime(int resolveTime);

    bool isSound();
    void setSound(bool sound);

    QString levelFile();
    void setLevelFile(const QString& levelFile);

    QString selectedLanguage();
    void setSelectedLanguage(const QString& selectedLanguage);

    /// Set whether the hint action is enabled or disabled
    void setHintEnabled(bool enable);

    //Display the mainwindow only when kvtml files are present, else show an error message and quit.
    void show();

    /** The hidden word that is filled in during the game. */
    QString m_currentWord;

    /** Get the index of the desired level in the list */
    Q_INVOKABLE int currentLevel() const;

    /** Get the current categories available */
    Q_INVOKABLE QStringList categoryList() const;

    Q_INVOKABLE QStringList currentWordLetters() const;

    /** Get the current word */
    Q_INVOKABLE QString currentWord() const;

    Q_INVOKABLE QStringList alphabet() const;

    Q_INVOKABLE QStringList languageNames() const;

    /** Return true if the word contains the char in the QString */
    Q_INVOKABLE bool containsChar(const QString &original);

    /** Return true if the answer and the current word match */
    Q_INVOKABLE bool isResolved() const;

public slots:
    ///When the language is changed in the Language menu
    void slotChangeLanguage(int);
    ///When the hint is set or unset
    void slotSetHint(bool);
    void slotSetWins(int);
    void slotSetLosses(int);
    ///Load kvtml file and get a word and its tip in random
    void readFile();
    ///if you want to play with a new word
    // @param loss if the previous game should be counted as a loss
    void newGame(bool loss = false);

public Q_SLOTS:

    /** Select the index of the desired level in the list as the current active one */
    void selectCurrentLevel (int index);

    /** Select the desired level file in the list as the current active one */
    void selectLevelFile(int index);

    void saveSettings();

    /** Generate a new word */
    void nextWord();

    /** Handle the guessed letter */
    void replaceLetters(const QString& charString);

signals:

    void signalSetWins(int wins);
    void signalSetLosses(int losses);

Q_SIGNALS:
    void hintHideTimeChanged();
    void resolveTimeChanged();
    void soundToggled();
    void levelFileChanged();
    void selectedLanguageChanged();

private slots:
    // Slots for when the user changes level, setting, etc.
    void  slotChangeLevel(int);
    void  slotChangeTheme(int);

    ///In Settings menu, Configure KHangMan... menu item
    void optionsPreferences();
    ///access the KNewStuff class to install new data
    void slotDownloadNewStuff();
    ///update settings after Settings->Configure KHangMan dialog is closed
    void updateSettings();
    ///When a button is clicked on the toolbar, the corresponding character is written in the lineedit
    void slotPasteChar();

    ///Quit the application and save special toolbar settings
    bool queryClose();
    ///if you want to play with a new word
    void slotNewGame();
    ///open a local KVTML file
    void slotFileOpen();
    ///open a user's recent file
    void slotOpenRecent(const QUrl &);

private:
    ///Create the actions
    void setupActions();
    ///Create the Statusbar
    void setupStatusbar();

    ///At start, restore settings from config file and apply them
    void loadSettings();
    ///Set the level and make sure it exists
    void setLevel();
    ///Scan the files in the selected language dir to set the levels, returns true if succeeds
    bool loadLevels();
    ///Set a bool variable to true if the language allowa accented letters to be displayed with corresponding letter
    void setAccent();
    ///Loads a file in URL, or displays file open dialog if argument is empty
    void loadFile(const QUrl &);
    // Populate the second toolbar with the correct buttons if the
    // current language has special characters.
    void loadLangToolBar();

    ///Method to set the current language into the Statusbar and to pass it to KHangManView
    void setLanguages();

    ///Display the correct messages in the statusbar
    void setMessages();

    ///Create a pixmap with the argument (special character) and return the QIcon containing the pixmap
    QIcon charIcon(const QChar &) const;

    ///the different data titles and files in the current language dir
    QMap<QString, QString> m_titleLevels;

    ///Current level ID
    int m_currentLevel;

    // language information
    QStringList m_languages;
    QStringList m_languageNames;

    // Some important members: the view and newStuff.
    QQuickWidget   *m_view;

    // Actions in the Game menu
    KSelectAction  *m_levelAction;
    KSelectAction  *m_modeAction;
    //Action that sets up the Language menu
    KSelectAction *m_languageAction;
    KToggleAction *m_hintAct;

    ///Create the Special Characters Toolbar
    KToolBar *m_specialCharToolbar;

    // True if the language has no special chars, such as en, it and nl.
    bool m_specialChars;

    // Contains all the words that are read from the data file.
    QStringList m_allData;

    // Settings.
    Ui::generalui ui_general;
    Ui::languageui ui_language;

    //Theme manager
    KHMThemeFactory m_khmfactory;

    //Config group
    KSharedConfig::Ptr m_config;
    //Recent files element
    KRecentFilesAction * m_recent;

    QLabel *m_levelLabel;
    QLabel *m_accentsLabel;
    QLabel *m_winsLabel;
    QLabel *m_lossesLabel;

    bool m_winner;
    bool m_loser;
    int m_bgfill;

    int m_winCount;
    int m_lossCount;

    // true if a hint exists
    bool m_hintExists;

    //The index to the random sequence
    int m_randomInt;

    void setGameCount();

    //Play a game: look for a word to be guessed and load its tip
    void game();

    void play(const QString& soundFile);

    Phonon::MediaObject *m_player;

    // The word to be guessed.
    QString m_word;
    // goodWord is the hidden word that is filled in during the game.
    // Initialized to "_ " * (number of letters in the word).
    QString m_goodWord;

    //Current hint
    QString m_hint;

    // These two are the positions of the first and second spaces in the word.
    int m_posFirstSpace;
    int m_posSecondSpace;

    //The random sequence of words of the current level file
    QList<QPair<QString, QString> > m_randomList;

    //shuffle words+hints
    void slotSetWordsSequence();

    //Current level file
    KEduVocDocument *m_doc;

    //Reset everything to start a new game, missed letters is empty
    void reset();

    // Contains all letters already guessed.
    QStringList m_guessedLetters;

    // Stores the missed letters that are shown on the screen.
    // Initialiazed to "_ " * MAXWRONGGUESSES.
    QString m_missedLetters;

    // How many times you missed.
    // When this reaches MAXWRONGGUESSES, you are hanged.
    int m_numMissedLetters;

    /** Strip the accents off given string
     * @params original string to strip accents off of
     * @returns string without accents
     */
    QString stripAccents(const QString & original);

    /** The word to be guessed */
    QString m_originalWord;
};

#endif // _KHANGMAN_H_

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

