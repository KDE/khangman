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
    void fileNew();
    void slotQuit();

    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void newToolbarConfig();

    void changeStatusbar(const QString& text, int id);
    void changeCaption(const QString& text);
    void slotLevel();
    void slotMode();
    void isLevel();
    void isMode();
    void setSelectedLanguage(QString);

    void readSettings();
    void writeSettings();

    void changeLanguage(int newLanguage);
    void slotLanguage();
    void setLanguage(int lang);
    void slotTransparent();

private:
    void setupAccel();
    void setupActions();
    void setupLangMenu();

private:
    KHangManView *m_view;

    KAction *newAct;
    ///Action in the Settings menu to enable/disable transparency pictures
    KToggleAction *transAct;
    KSelectAction *langAct, *levelAct, *modeAct;
    QPopupMenu *langPopup;

    int selectedLanguage;        // Number of selected language
    QStringList m_languages;     // Language codes of available languages
    QStringList m_languageNames; // Translated names of languages
    QStringList m_sortedNames;   // Translated and sorted names of languages
};

#endif // _KHANGMAN_H_
