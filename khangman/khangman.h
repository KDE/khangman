/*
 * Copyright (C) 2003 Anne-Marie Mahfouf,,, <annma@kde.org>
 */

#ifndef _KHANGMAN_H_
#define _KHANGMAN_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qpixmap.h>
#include <qstring.h>

#include <kapplication.h>
#include <kmainwindow.h>

#include "khangmanview.h"

class KToggleAction;
class KComboBox;


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

	KComboBox *combo;
	KComboBox *comboMode;
	QString levelString, modeString;

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

    void optionsShowToolbar();
    void optionsShowStatusbar();
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void optionsPreferences();
    void newToolbarConfig();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);
	void slotLevel(int);
	void slotMode(int index);
	void isLevel();
	void isMode();

	void readSettings();
	void writeSettings();

private:
    void setupAccel();
    void setupActions();

private:
    KHangManView *m_view;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
	KAction *newAct;
};

#endif // _KHANGMAN_H_
