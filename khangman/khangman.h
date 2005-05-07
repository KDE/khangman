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
    
protected:
    ///Main view
    KHangManView *m_view;
    ///Action in the Game menu to start a new word
    KAction *newAct;
    ///Actions for the level combobox and the mode combobox
    KSelectAction *levelAct, *modeAct;
    ///Populate the Languages menu
    QPopupMenu *langPopup;


    void setupActions();

    void setupStatusbar();

    ///Selected language
    QString selectedLanguage;

    ///Translated and sorted names of languages
    QStringList m_sortedNames;

    ///the different data files in each language dir
    QStringList levels;

    void loadSettings();

    void setLevel();
    ///Current level ID
    uint currentLevel;
    ///hold the current level
    QString levelString;

    void loadLevels();

    
private:
    ///Create a KNewStuff instance
    KHNewStuff *mNewStuff;
    
    QString charIcon(const QChar &);
    ///true if the language has no special char as en, it and nl
    bool noCharBool;
    ///Create the Special Characters Toolbar
    KToolBar *secondToolbar;
    ///true if the Special Characters toolbar is checked to be shown
    bool m_bCharToolbar;
    ///Read the special chars from the .txt file and put it there
    QStringList allData;
    
public slots:

    void slotChangeLanguage(int);
    
protected slots:

    void slotChangeLevel();

    
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
};

#endif // _KHANGMAN_H_
