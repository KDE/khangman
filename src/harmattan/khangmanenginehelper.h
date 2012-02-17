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

#include <khangmangame.h>

class KHangManEngineHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int hintHideTime READ hintHideTime WRITE setHintHideTime NOTIFY hintHideTimeChanged )
    Q_PROPERTY( int resolveTime READ resolveTime WRITE setResolveTime NOTIFY resolveTimeChanged )
    Q_PROPERTY( bool sound READ isSound WRITE setSound NOTIFY soundToggled )
    Q_PROPERTY( QString defaultVocabulary READ defaultVocabulary WRITE setDefaultVocabulary NOTIFY defaultVocabularyChanged )
    Q_PROPERTY( QString selectedLanguage READ selectedLanguage WRITE setSelectedLanguage NOTIFY selectedLanguageChanged )

	public:
        explicit KHangManEngineHelper(KHangManEngine *khangmanEngine, QObject* parent = 0);
        ~KHangManEngineHelper();

        Q_INVOKABLE QStringList languageNames() const;
        Q_INVOKABLE bool compareWords() const;

        // These accessor and mutator methods are not needed once the
        // kconfig_compiler can generate Q_INVOKABLE methods, slots or/and
        // properties

        int hintHideTime();
        void setHintHideTime(int hintHideTime);

        int resolveTime();
        void setResolveTime(int resolveTime);

        bool isSound();
        void setSound(bool sound);

        QString defaultVocabulary();
        void setDefaultVocabulary(const QString& defaultVocabulary);

        QString selectedLanguage();
        void setSelectedLanguage(const QString& selectedLanguage);

        Q_INVOKABLE QStringList alphabet() const;

    public Q_SLOTS:
        void saveSettings();

    Q_SIGNALS:
        void hintHideTimeChanged();
        void resolveTimeChanged();
        void soundToggled();
        void defaultVocabularyChanged();
        void selectedLanguageChanged();

	private:
        KHangManEngine *m_khangmanEngine;
        QStringList m_currentOriginalWord;
        int m_insertCounter;
};

#endif // KHANGMAN_ENGINE_HELPER_H
