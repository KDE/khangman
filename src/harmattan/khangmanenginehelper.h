/******************************************************************************
 * This file is part of the KHangMan project
 * Copyright (c) 2012 Laszlo Papp <lpapp@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef KHANGMAN_ENGINE_HELPER_H
#define KHANGMAN_ENGINE_HELPER_H

#include "khangmanengine.h"

class KHangManEngineHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int hintHideTime READ hintHideTime WRITE setHintHideTime NOTIFY hintHideTimeChanged )
    Q_PROPERTY( int resolveTime READ resolveTime WRITE setResolveTime NOTIFY resolveTimeChanged )
    Q_PROPERTY( bool sound READ isSound WRITE setSound NOTIFY soundToggled )
    Q_PROPERTY( QString levelFile READ levelFile WRITE setLevelFile NOTIFY levelFileChanged )
    Q_PROPERTY( QString selectedLanguage READ selectedLanguage WRITE setSelectedLanguage NOTIFY selectedLanguageChanged )

	public:
        explicit KHangManEngineHelper(KHangManEngine *khangmanEngine, QObject* parent = 0);
        ~KHangManEngineHelper();

        Q_INVOKABLE QStringList currentWordLetters() const;

        Q_INVOKABLE QStringList languageNames() const;

        // These accessor and mutator methods are not needed once the
        // kconfig_compiler can generate Q_INVOKABLE methods, slots or/and
        // properties

        int hintHideTime();
        void setHintHideTime(int hintHideTime);

        int resolveTime();
        void setResolveTime(int resolveTime);

        bool isSound();
        void setSound(bool sound);

        QString levelFile();
        void setLevelFile(const QString& levelFile);

        QString selectedLanguage();
        void setSelectedLanguage(const QString& selectedLanguage);

        void setLevel(int levelId);

        Q_INVOKABLE QStringList alphabet() const;

    public Q_SLOTS:
        void saveSettings();

    Q_SIGNALS:
        void hintHideTimeChanged();
        void resolveTimeChanged();
        void soundToggled();
        void levelFileChanged();
        void selectedLanguageChanged();

	private:
        KHangManEngine *m_khangmanEngine;
};

#endif // KHANGMAN_ENGINE_HELPER_H
