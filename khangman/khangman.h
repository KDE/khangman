/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <kapplication.h>
#include <kmainwindow.h>

#include "khangmanview.h"

class KSelectAction;
class KToggleAction;
class KToggleToolBarAction;
class QPopupMenu;
class KToolBar;
class KHNewStuff;

/**
 * This class serves as the main window for KHangMan.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Anne-Marie Mahfouf <annma@kde.org>
 * @version 1.3
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
    ///Current level ID
    uint currentLevel;
    ///Create the Special Characters Toolbar
    KToolBar *secondToolbar;
    ///Language codes of available languages
    QStringList m_languages;
     ///Translated and sorted names of languages
    QStringList m_sortedNames;
    ///Method to set the current language into the Statusbar and to pass it to KHangManView
    void setLanguages();
    ///Build the Level combobox menu dynamically depending of the data for each language
    void loadDataFiles();
    ///Action for building the Languages menu
    KSelectAction *langAct;
    
    QStringList allData;
    ///Build the Languages menu
    void setupLangMenu();   
    /**
    Update the text in the Statusbar
    @param text the text that will be written in the statusbar
    @param id the label in which the text will be written
    */
    void changeStatusbar(const QString& text, int id);

private:
    ///hold the current level
    QString levelString;
    ///hold the current mode
    QString modeString;
    ///the language used in KDE for the user
    QString userLanguage;
    ///the different data files in each language dir
    QStringList levels;
    ///true if the Special Characters toolbar is checked to be shown
    bool m_bCharToolbar;
    ///true if the language has no special char as en, it and nl
    bool noCharBool;
    
    QString charIcon(const QChar &);

private slots:
    ///Start a new game i.e. repaint and set a new word
    void newGame();
    ///In Settings menu, Configure KHangMan... menu item
    void optionsPreferences();
    /**Update the text in the caption in the main window
    @param text the new text that will be written as caption in the window
    */
    void changeCaption(const QString& text);
    ///this slot is called when the user changes level with the level combobox in the toolbar
    void changeLevel();
    ///This slot is called when the user changes background mode with the mode combobox in the toolbar
    void changeMode();
    /**
      *When config is read, set the level KComboBox to the right level
      *and update the statusbar
      */
    void setLevel_WindowState();
     /**
      *When config is read, set the mode KComboBox to the right background
      *and call the corresponding slot in the main view to set the background
      */
    void setMode_WindowState();
    ///Read settings from config file khangmanrc or set defaults if none
    void loadSettings();
    /**Switch to another language using the Languages menu
    @param newLanguage the index of the new selected language in the menu
    */
    void changeLanguage(int newLanguage);
    ///Convenient slot to call changeLanguage slot
    void slotLanguage();
    /**Set the current language in the view and update the statusbar
    @param lang the code of the currently selected language
    */
    void setLanguage(QString lang);
    ///When Transparent Pictures is checked/unchecked in Settings menu, go to this slot
    void slotTransparent();
    /**
    Slot to toggle the full screen mode	
    @param set the state of the Full Screen Mode ToggleMenu, true if toggled
    */
    void slotSetFullScreen( bool set);
    ///Populate the second toolbar with the correct buttons if the current language has special characters
    void loadLangToolBar();
    ///When a button is clicked on the toolbar, the corresponding character is written in the lineedit  
    void slotPasteChar();
    ///Write some config settings and close the main window
    void slotClose();
    ///Check if the language has special characters and load the second toolbar if so
    void slotAccents();
    ///Whether Enabled Hint  is checked or not
    void slotHint();
    ///if the data file is a kvtml one then Enable Hint must be enabled
    void enableHint(bool);
    ///access the KNewStuff class to install new data
    void downloadNewStuff();
    ///update settings after Settings->Configure KHangMan dialog is closed
    void updateSettings();

  protected:
    ///Set up the actions for the menus
    void setupActions();
    ///Set a bool to true for languages that allow Typing Accented Letters
    void setAccentBool();
    
private:
    ///Main view
    KHangManView *m_view;
    ///Action in the Game menu to start a new word
    KAction *newAct;
    ///Actions for the level combobox and the mode combobox
    KSelectAction *levelAct, *modeAct;
    ///Populate the Languages menu
    QPopupMenu *langPopup;
    ///Full-Screen mode action
    KToggleAction* m_pFullScreen;
    ///Selected language
    QString selectedLanguage;
    ///Translated names of languages
    QStringList m_languageNames;
    ///Create a KNewStuff instance
    KHNewStuff *mNewStuff;
    
};

#endif // _KHANGMAN_H_
