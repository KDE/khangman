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

//class KToggleToolBarAction;
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
 * @version 1.1
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

    KToolBar *secondToolbar;
    KAction *toolAct;

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

    bool m_bCharToolbar;
    ///if the data file is a kvtml file thus has hints
    bool kvtmlBool;

protected:
    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);

    // watch changes in fullscreen state
    virtual bool event(QEvent *);
    
   //  virtual void mousePressEvent( QMouseEvent *mouse );

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
    void setSelectedLanguage(QString);
    ///Read settings from config file khangmanrc or set defaults if none
    void loadSettings();
    ///Switch to another language using the Languages menu
    void changeLanguage(int newLanguage);
    void slotLanguage();
    void setLanguage(int lang);
    ///When Transparent Pictures is checked/unchecked in Settings menu, go to this slot
    void slotTransparent();

    void slotSofter();

    void slotToggleFullScreen();

    void loadLangToolBar();

    void slotPasteAtilde();
    void slotPasteAgrave();
    void slotPasteAacute();
    void slotPasteAumlaut();
    void slotPasteAwithe();
    void slotPasteAcircle();
    void slotPasteCcedil();
    void slotPasteEacute();
    void slotPasteEgrave();
    void slotPasteEcirc();
    void slotPasteIgrave();
    void slotPasteIacute();
    void slotPasteNtilde();
    void slotPasteOgrave();
    void slotPasteOacute();
    void slotPasteOcirc();
    void slotPasteOcross();
    void slotPasteOtilde();
    void slotPasteOumlaut();
    void slotPasteUumlaut();
    void slotPasteUacute();
    void slotPasteSzlig();
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
    void updateFullScreenState();

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

    bool m_bFullScreen;
    ///Number of the selected language
    int selectedLanguage;
    ///Language codes of available languages
    QStringList m_languages;
    ///Translated names of languages
    QStringList m_languageNames;
    ///Translated and sorted names of languages
    QStringList m_sortedNames;
};

#endif // _KHANGMAN_H_
