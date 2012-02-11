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

#include <QtCore/QObject>

/** @brief game api
 * @author Anne-Marie Mahfouf <annma@kde.org>
 */
class KHANGMAN_ENGINE_EXPORT KHangManEngine : public QObject
{
    Q_OBJECT
    public:
        /** Default constructor */
        KHangManEngine();

        /** Default destructor */
        ~KHangManEngine();

        Q_INVOKABLE static bool hasSpecialChars(const QString& lang);
        Q_INVOKABLE static bool hasAccentedLetters(const QString& lang);

        /// Return true if the word contains the char in the QString.
        Q_INVOKABLE bool containsChar(const QString &original); 

        /** Strip the accents off given string
         * @params original string to strip accents off of
         * @returns string without accents
         */
        Q_INVOKABLE QString stripAccents(const QString & original);


    public Q_SLOTS:
        ///Method to set the current language into the Statusbar and to pass it to KHangManView
        void setLanguages();
        void loadSettings();

    private Q_SLOTS:
        void setLevel();
};

#endif

