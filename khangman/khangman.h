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

#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,1,90)
#define KHANGMAN_KDE_3_2_0
#else
#endif
#endif

class KSelectAction;
class KToggleAction;
class KToggleToolBarAction;
class QPopupMenu;
class KToolBar;

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

    uint currentLevel;
    ///Create the Special Characters Toolbar
    KToolBar *secondToolbar;

private:
    ///hold the current level
    QString levelString;
    ///hold the current mode
    QString modeString;
    ///the language used in KDE for the user
    QString userLanguage;
    ///the language that is set to default: either userLanguage or en if userLanguage is not available
    int defaultLang;
    ///the different data files in each language dir
    QStringList levels;
    ///true if the Special Characters toolbar is checked to be shown
    bool m_bCharToolbar;
    ///true if the language has no special char as en, it and nl
    bool noCharBool;

private slots:
    ///Start a new game i.e. repaint and set a new word
    void newGame();
    ///Configure shortcut keys standard KDE dialog
    void optionsConfigureKeys();
    ///Configure toolbars standard KDE dialog
    void optionsConfigureToolbars();
    /**
     *this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
     *Recreate our GUI and re-apply the settings (e.g. "text under icons", etc.)
     */
    void newToolbarConfig();
    ///Update the text in the Statusbar
    void changeStatusbar(const QString& text, int id);
    ///Update the text in the caption in the main window
    void changeCaption(const QString& text);
    ///this slot is called when the user changes level with the level combobox in the toolbar
    void changeLevel();
    ///this slot is called when the user changes background mode with the mode combobox in the toolbar
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
    //void setSelectedLanguage(QString);
    ///Read settings from config file khangmanrc or set defaults if none
    void loadSettings();
    ///Switch to another language using the Languages menu
    void changeLanguage(int newLanguage);
    void slotLanguage();
    void setLanguage(QString lang);
    ///When Transparent Pictures is checked/unchecked in Settings menu, go to this slot
    void slotTransparent();

    void slotSofter();

    void slotSetFullScreen( bool );

    void loadLangToolBar();

    void slotPasteAtilde();
    void slotPasteAgrave();
    void slotPasteAacute();
    void slotPasteAumlaut();
    void slotPasteAwithe();
    void slotPasteAcircle();
    void slotPasteCcaron();
    void slotPasteCcedil();
    void slotPasteDapos();
    void slotPasteEcaron();
    void slotPasteEacute();
    void slotPasteEgrave();
    void slotPasteEcirc();
    void slotPasteIgrave();
    void slotPasteIacute();
    void slotPasteNcaron();
    void slotPasteNtilde();
    void slotPasteOgrave();
    void slotPasteOacute();
    void slotPasteOcirc();
    void slotPasteOcross();
    void slotPasteOtilde();
    void slotPasteOumlaut();
    void slotPasteRcaron();
    void slotPasteScaron();
    void slotPasteUumlaut();
    void slotPasteUacute();
    void slotPasteUdot();
    void slotPasteSzlig();
    void slotPasteYacute();
    void slotPasteZcaron();
    void slotClose();
    void slotAccents();
    ///when Enabled Hint is checked or not by the user
    void slotChooseHint();
    ///Whether Enabled Hint  is checked or not
    void slotHint();
    ///if the data file is a kvtml one then Enable Hint must be enabled
    void enableHint(bool);

  private:
    void setupAccel();
    void setupActions();
    void setupLangMenu();
    void loadDataFiles();
    void setAccentBool();
    void restoreAccentConfig();
    
private:
    ///Main view
    KHangManView *m_view;
    ///Action in the Game menu to start a new word
    KAction *newAct;
    ///Action in the Settings menu to enable/disable transparency pictures
    KToggleAction *transAct;
    KToggleAction *softAct;
    KToggleAction *hintAct;
    KToggleAction *accentsAct;
    //KToggleToolBarAction *secondAct;
    KSelectAction *langAct, *levelAct, *modeAct;
    QPopupMenu *langPopup;
    ///Full-Screen mode action
#ifdef KHANGMAN_KDE_3_2_0
    KToggleAction* m_pFullScreen;
#else
    KAction* m_pFullScreen;
#endif

    ///Selected language
    QString selectedLanguage;
    ///Language codes of available languages
    QStringList m_languages;
    ///Translated names of languages
    QStringList m_languageNames;
    ///Translated and sorted names of languages
    QStringList m_sortedNames;
};

#endif // _KHANGMAN_H_
