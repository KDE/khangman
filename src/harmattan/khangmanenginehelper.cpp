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

#include <libkdeedu/sharedkvtmlfiles.h>

#include <KDE/KStandardDirs>
#include <KDE/KLocale>

#include <QtGui/QApplication>

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

QStringList KHangManEngineHelper::currentWordLetters() const
{
    QStringList currentWordLetters;

    QString currentWord = m_khangmanEngine->currentWord();

    foreach (const QChar& currentWordLetter, currentWord)
    {
        currentWordLetters.append(currentWordLetter);
    }

    return currentWordLetters;
}

QStringList KHangManEngineHelper::languageNames() const
{
    QStringList languageCodes = SharedKvtmlFiles::languages();
    if (languageCodes.isEmpty()) {
        QApplication::instance()->quit();
    }

    QStringList languageNames;

    // Get the language names from the language codes
    // Look in $KDEDIR/share/locale/all_languages from
    // kdelibs/kdecore/all_languages to find the name of the country
    // corresponding to the code and the language the user set.

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

bool KHangManEngineHelper::isSound()
{
    return Prefs::sound();
}

void KHangManEngineHelper::setSound(bool sound)
{
    Prefs::setSound(sound);
    emit soundToggled();
}

QString KHangManEngineHelper::defaultVocabulary()
{
    // TODO: Fix it with the proper accessor for sure
    // return Prefs::defaultVocabulary();
    return QString();
}

void KHangManEngineHelper::setDefaultVocabulary(const QString& defaultVocabulary)
{
    // TODO: Fix it with the proper mutator for sure
    // Prefs::setDefaultVocabulary(defaultVocabulary);
    emit defaultVocabularyChanged();
}

QString KHangManEngineHelper::selectedLanguage()
{
    QStringList languageNames = SharedKvtmlFiles::languages();
    if (languageNames.isEmpty()) {
        QApplication::instance()->quit();
    }

    QString selectedLanguage = Prefs::selectedLanguage();
    if (!languageNames.contains(selectedLanguage)) {
        selectedLanguage = "en";
    }

    return selectedLanguage;
}

void KHangManEngineHelper::setSelectedLanguage(const QString& selectedLanguage)
{
    Prefs::setSelectedLanguage(selectedLanguage);
    emit selectedLanguageChanged();
}

QStringList KHangManEngineHelper::alphabet() const
{
    QStringList letterList;
    char c = 'A';

    while( c != 'Z') {
        letterList.append(QChar(c));
        ++c;
    }

    letterList.append(QChar(c));

    return letterList;
}

void KHangManEngineHelper::saveSettings()
{
    Prefs::self()->writeConfig();
}

#include "khangmanenginehelper.moc"
