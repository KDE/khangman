/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */

#ifndef _KHANGMANPREF_H_
#define _KHANGMANPREF_H_

#include <kdialogbase.h>
#include <qframe.h>
#include <qstring.h>

#include "pref1ui.h"
#include "pref2ui.h"

class KHangManPrefPageOne;
class KHangManPrefPageTwo;

class KHangManPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KHangManPreferences();

	QString levelString, modeString;
	int langNum;
	bool cancelBool;
	bool levelChanged;
	bool langChanged;

private:
    KHangManPrefPageOne *m_pageOne;
    KHangManPrefPageTwo *m_pageTwo;

    bool configChanged;

private slots:
	void readConfig();
	void writeConfig();
	void slotOk();
	void slotApply();
	void slotCancel();
	void slotDefault();
	void slotSet();
	void slotMode(int);
	void slotLevel(int);
	void slotChanged();
	void slotLang(int);

signals:
	void aClicked();
};

class KHangManPrefPageOne : public pref1ui
{
    Q_OBJECT
public:
        KHangManPrefPageOne(QWidget *parent = 0);
};

class KHangManPrefPageTwo : public pref2ui
{
    Q_OBJECT
public:
    KHangManPrefPageTwo(QWidget *parent = 0);
};

#endif // _KHANGMANPREF_H_
