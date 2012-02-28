/***************************************************************************
 *   Copyright 2001-2009 Anne-Marie Mahfouf <annma@kde.org>                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef KHANGMANGAME_H
#define KHANGMANGAME_H

#include "khangman_engine_export.h"

#include <KDE/KRandomSequence>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QStringList>

class KEduVocDocument;

/** @brief game engine api
 * @author Anne-Marie Mahfouf <annma@kde.org>
 * @author Laszlo Papp <lpapp@kde.org>
 */
class KHANGMAN_ENGINE_EXPORT KHangManEngine : public QObject
{
    Q_OBJECT
    public:
        /** Default constructor */
        KHangManEngine();

        /** Default destructor */
        ~KHangManEngine();

        Q_INVOKABLE static bool hasSpecialChars(const QString& languageCode);
        Q_INVOKABLE static bool hasAccentedLetters(const QString& languageCode);

        /** Return true if the word contains the char in the QString */
        Q_INVOKABLE bool containsChar(const QString &original); 

        /** Strip the accents off given string
         * @params original string to strip accents off of
         * @returns string without accents
         */
        Q_INVOKABLE QString stripAccents(const QString & original);

        /** Get the hint for the current word */
        Q_INVOKABLE QString hint() const;

        /** Get the current answer */
        Q_INVOKABLE QString word() const;

    public Q_SLOTS:
        /** Generate a new word */
        void newWord();

        /** Method to set the current language into the Statusbar and to pass it to KHangManView */
        void readFile();

    private:
        /** Scan the files in the selected language dir to set the levels */
        void loadLevels();

        /** the different data titles and files in the current language dir */
        QMap<QString, QString> m_titleLevels;

        /** Random sequence used to scramble the letters */
        KRandomSequence m_random;

        /** The index to the random sequence */
        int m_randomInt;

        /** The random sequence of words of the current level file */
        QList<QPair<QString, QString> > m_randomList;

        /** The current anagram's hint */
        QString m_hint;

        /** The word to be guessed */
        QString m_originalWord;

        /** Which index the current filename is in m_fileList */
        int m_index;

        /** The list of vocabulary files */
        QStringList m_fileList;

        /** The list of words that have been answered */
        QStringList m_answeredWords;

        /** Current level file */
        KEduVocDocument *m_doc;
};

#endif

