/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>
 */

#ifndef _KHANGMANPREF_H_
#define _KHANGMANPREF_H_

#include <kdialogbase.h>
#include <qstring.h>
#include <qstringlist.h>

#include "pref1ui.h"
#include "pref2ui.h"

class KHangManPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KHangManPreferences();

	QString levelString, modeString;
	bool transparent;
	int langNum;
	bool cancelBool;
	bool levelChanged;
	bool langChanged;
private:
    pref1ui *m_pageOne;
    pref2ui *m_pageTwo;

    int defaultLang;
    QStringList languageNames;

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
	void slotTransparent(bool);
	void slotChanged();
	void slotLang(const QString &);

signals:
	void aClicked();
};

#endif // _KHANGMANPREF_H_
