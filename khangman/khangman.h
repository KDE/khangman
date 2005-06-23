/***************************************************************************
 *   Copyright (C) 2001-2005 Anne-Marie Mahfouf <annma@kde.org> *
 *   annemarie.mahfouf@free.fr   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

#include "khangmanview.h"

class KSelectAction;
class KToggleAction;
class KHNewStuff;
class advanced;


/**
 * @short Application Main Window
 * @author Anne-Marie Mahfouf <annemarie.mahfouf@free.fr>
 * @version 0.1
 */
class KHangMan : public KMainWindow
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
    
    /**
    Update the text in the Statusbar
    @param text the text that will be written in the statusbar
    @param id the label in which the text will be written
    */
    void changeStatusbar(const QString& text, int id);
    ///Action that sets up the Language menu

    KSelectAction *m_languageAction;
    ///Method to set the current language into the Statusbar and to pass it to KHangManView
    void setLanguages();
    ///Translated names of languages
    QStringList m_languageNames;
    ///Language codes of available languages
    QStringList m_languages;

    ///Instance of an advanced page of the config dialog
    advanced *mAdvanced;
    ///Display the correct messages in the statusbar
    void setMessages();

 private:
    ///Create the actions
    void setupActions();
    ///Create the Statusbar
    void setupStatusbar();
    ///Selected language
    QString selectedLanguage;
    ///Translated and sorted names of languages
    QStringList m_sortedNames;
    ///the different data files in each language dir
    QStringList levels;
    ///At start, restore settings from config file and apply them
    void loadSettings();
    ///Set the level and make sure it exists
    void setLevel();
    ///Current level ID
    uint currentLevel;
    ///hold the current level
    QString levelString;
    ///Scan the files in the selected language dir to set the levels
    void loadLevels();
    ///Set a bool variable to true if the language allowa accented letters to be displayed with corresponding letter
    void setAccent();

    ///Create a png image with the argument (special character) and return the path to this png image
    QString charIcon(const QChar &);

public slots:
    ///When the langugae is changed in the Language menu
    void slotChangeLanguage(int);
    
protected slots:
    ///Called when the user changes the level
    void slotChangeLevel();
    /// Called when the mode is changed
    void slotChangeMode();
    ///In Settings menu, Configure KHangMan... menu item
    void optionsPreferences();
    ///access the KNewStuff class to install new data
    void slotDownloadNewStuff();
    ///update settings after Settings->Configure KHangMan dialog is closed
    void updateSettings();
    ///Populate the second toolbar with the correct buttons if the current language has special characters
    void loadLangToolBar();
    ///When a button is clicked on the toolbar, the corresponding character is written in the lineedit  
    void slotPasteChar();
    ///Quit the application and save special toolbar settings
    void slotQuit();

private:

    // Some important members: the view and newStuff.
    KHangManView   *m_view;
    KHNewStuff     *m_newStuff;

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
    
};

#endif // _KHANGMAN_H_
