/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf  <annma@kde.org>
 */

#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kapplication.h>
#include <kmainwindow.h>

#include "khangmanview.h"

class KSelectAction;
class KToggleAction;
class QPopupMenu;

/**
 * This class serves as the main window for KHangMan.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
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

private:
    QString levelString, modeString;
    //the language used in KDE for the user
    QString userLanguage;
    int defaultLang;

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

private slots:
    ///Start a new game i.e. repaint and set a new word
    void newGame();

    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void newToolbarConfig();
    ///Update the text in the Statusbar
    void changeStatusbar(const QString& text, int id);
    void changeCaption(const QString& text);
    void changeLevel();
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

private:
    void setupAccel();
    void setupActions();
    void setupLangMenu();

private:
    ///Main view
    KHangManView *m_view;
    ///Action in the Game menu to start a new word
    KAction *newAct;
    ///Action in the Settings menu to enable/disable transparency pictures
    KToggleAction *transAct;
    KSelectAction *langAct, *levelAct, *modeAct;
    QPopupMenu *langPopup;

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
