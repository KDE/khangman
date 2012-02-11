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

#include "khangmanenginehelper.h"

#include <prefs.h>

#include <sharedkvtmlfiles.h>

#include <KDE/KStandardDirs>
#include <KDE/KLocale>

KHangManEngineHelper::KHangManEngineHelper(KHangManEngine* khangmanEngine, QObject* parent)
    : QObject(parent)
    , m_khangmanEngine(khangmanEngine)
    , m_insertCounter(0)
{
}

KHangManEngineHelper::~KHangManEngineHelper()
{
    delete m_khangmanEngine;
}

QStringList KHangManEngineHelper::languageNames() const
{
    QStringList languageCodes = SharedKvtmlFiles::languages();
    QStringList languageNames;

    // Get the language names from the language codes

    KConfig entry(KStandardDirs::locate("locale", "all_languages"));

    foreach (const QString& languageCode, languageCodes)
    {
        KConfigGroup group = entry.group(languageCode);

        QString languageName = group.readEntry("Name");
        if (languageName.isEmpty())
        {
            languageName = i18nc("@item:inlistbox no language for that locale", "None");
        }

        languageNames.append(languageName);
    }

    return languageNames;
}

bool KHangManEngineHelper::compareWords() const
{
    return m_currentOriginalWord.join("") == m_khangmanEngine->word();
}

int KHangManEngineHelper::hintHideTime()
{
    return Prefs::hintHideTime();
}

void KHangManEngineHelper::setHintHideTime(int hintHideTime)
{
    Prefs::setHintHideTime(hintHideTime);
    emit hintHideTimeChanged();
}

int KHangManEngineHelper::resolveTime()
{
    return Prefs::resolveTime();
}

void KHangManEngineHelper::setResolveTime(int resolveTime)
{
    Prefs::setResolveTime(resolveTime);
    emit resolveTimeChanged();
}

bool KHangManEngineHelper::useSounds()
{
    return Prefs::useSounds();
}

void KHangManEngineHelper::setUseSounds(bool useSounds)
{
    Prefs::setUseSounds(useSounds);
    emit useSoundsToggled();
}

QString KHangManEngineHelper::defaultVocabulary()
{
    return Prefs::defaultVocabulary();
}

void KHangManEngineHelper::setDefaultVocabulary(const QString& defaultVocabulary)
{
    Prefs::setDefaultVocabulary(defaultVocabulary);
    emit defaultVocabularyChanged();
}

QString KHangManEngineHelper::dataLanguage()
{
    return Prefs::dataLanguage();
}

void KHangManEngineHelper::setDataLanguage(const QString& dataLanguage)
{
    Prefs::setDataLanguage(dataLanguage);
    emit dataLanguageChanged();
}

void KHangManEngineHelper::saveSettings()
{
    Prefs::self()->writeConfig();
}

#include "khangmanenginehelper.moc"
