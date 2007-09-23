/***************************************************************************
 *   Copyright 2001-2007 Anne-Marie Mahfouf <annma@kde.org>                *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/


#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#include <kxmlguiwindow.h>

#include "khangmanview.h"
#include "ui_generalui.h"
#include "ui_languageui.h"
//#include "timerui.h"

class KSelectAction;
class KToggleAction;


// Id tags for the status bar.
const int IDS_LANG     = 100;
const int IDS_LEVEL    = 101;
const int IDS_ACCENTS  = 102;
const int IDS_WINS     = 104;
const int IDS_LOSSES   = 105;


/**
 * @short Application Main Window
 * @author Anne-Marie Mahfouf <annemarie.mahfouf@free.fr>
 * @version 0.1
 */

class KHangMan : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
    * Default Constructor
    */
    KHangMan();

    /**
    * Default Destructor
    */
    virtual ~KHangMan();

    ///Action that sets up the Language menu
    KSelectAction *m_languageAction;

    KToggleAction *hintAct;
    ///Method to set the current language into the Statusbar and to pass it to KHangManView
    void setLanguages();

    ///Display the correct messages in the statusbar
    void setMessages();

public slots:
    ///When the langugae is changed in the Language menu
    void slotChangeLanguage(int);

private slots:
    // Slots for when the user changes level, setting, etc.
    void  slotChangeLevel(int);
    void  slotChangeMode(int);

    ///In Settings menu, Configure KHangMan... menu item
    void optionsPreferences();
    ///access the KNewStuff class to install new data
    void slotDownloadNewStuff();
    ///update settings after Settings->Configure KHangMan dialog is closed
    void updateSettings();
    ///When a button is clicked on the toolbar, the corresponding character is written in the lineedit
    void slotPasteChar();

    ///Quit the application and save special toolbar settings
    void slotQuit();
    ///if you want to play with a new word
    void slotNewGame();
    ///when tooltip is clicked, hint disappears and hintAct comes back to "Show Hint"
    void slotChangeHintAction();
    /**
    Update the text in the Statusbar
    @param text the text that will be written in the statusbar
    @param id the label in which the text will be written
    */
    void changeStatusbar(const QString& text, int id);

private:
    ///Create the actions
    void setupActions();
    ///Create the Statusbar
    void setupStatusbar();

    ///At start, restore settings from config file and apply them
    void loadSettings();
    ///Set the level and make sure it exists
    void setLevel();
    ///Scan the files in the selected language dir to set the levels
    void loadLevels();
    ///Set a bool variable to true if the language allowa accented letters to be displayed with corresponding letter
    void setAccent();

    // Populate the second toolbar with the correct buttons if the
    // current language has special characters.
    void loadLangToolBar();

    ///Create a pixmap with the argument (special character) and return the QIcon containing the pixmap
    QIcon charIcon(const QChar &) const;

    ///Selected language
    QString selectedLanguage;
    ///the different data titles and files in the current language dir
    QMap<QString, QString> m_titleLevels;

    ///Current level ID
    int m_currentLevel;

    // language information
    QStringList m_languages;
    QStringList m_languageNames;

    // Some important members: the view and newStuff.
    KHangManView   *m_view;

    // Actions in the Game menu
    KSelectAction  *m_levelAction;
    KSelectAction  *m_modeAction;

    ///Create the Special Characters Toolbar
    KToolBar *secondToolbar;

    // True if the language has no special chars, such as en, it and nl.
    // FIXME: Reverse the bool.  Negated bools are difficult to read.
    bool            m_noSpecialChars;

    // Contains all the words that are read from the data file.
    QStringList     m_allData;

    // Settings.
    Ui::generalui ui_general;
    Ui::languageui ui_language;
    //Ui::timerui ui_timer;
};

#endif // _KHANGMAN_H_

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

