/*
 * Copyright 2001-2009 Anne-Marie Mahfouf <annma@kde.org>

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef KHANGMANVIEW_H
#define KHANGMANVIEW_H


class QPushButton;
class KHangMan;
namespace Phonon {
    class MediaObject;
}
class QRect;
class QSvgRenderer;
class KHMTheme;

#include <QImage>
#include <QPixmap>

#include <KLineEdit>

#include <krandomsequence.h>
#include <keduvocdocument.h>
#include <keduvocexpression.h>

#define MAXWRONGGUESSES  10


/**
 * This is the main view class for KHangMan.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 */

class KHangManView : public QWidget
{
    Q_OBJECT
public:

    KHangManView(KHangMan *parent=0);

    virtual ~KHangManView();

    ///parent instance
    KHangMan *khangman;
    ///set the chosen theme
    void setTheme(KHMTheme *theme);

    bool  hintExists() const          { return m_hintExists;            }

    bool  accentedLetters() const     { return m_accentedLetters;       }
    void  setAccentedLetters( bool _accentedLetters )
                                      { m_accentedLetters = _accentedLetters; }

    /// Enter a letter into the input widget.
    void  enterLetter(QString letter) { m_letterInput->setText(letter); }

public slots:

    ///Load kvtml file and get a word and its tip in random
    void readFile();
    ///if you want to play with a new word
    // @param loss if the previous game should be counted as a loss
    void newGame(bool loss = false);

signals:

    void signalSetWins(int wins);
    void signalSetLosses(int losses);

 protected:

    // Events
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );


private slots:

    /// After you click on Guess button or hit Enter when guessing a
    /// new letter, see if the letter is in the word or not
    void slotTry();

    /// Reenable user input.  This is used as a target for timers when
    /// the user has made a guess that was already made earlier, and a
    /// popup informing about this is closed.
    void enableUserInput();

 private:
    // Painting

    ///Paint the animated hanged K sequence
    void paintHangman(QPainter &p, const QRect& rect);
    void paintWord(QPainter &p, const QRect& rect);
    void paintMisses(QPainter &p, const QRect& rect);
    void paintHint(QPainter &p, const QRect& rect);
    void paintGameOver(QPainter &p, const QRect& rect);

    ///Reset everything to start a new game, missed letters is empty
    void reset();
    ///shuffle words+hints
    void slotSetWordsSequence();
    ///Play a game: look for a word to be guessed and load its tip
    void game();

    /// Return true if the word contains the char in the QString.
    bool  containsChar(const QString &);

    void  replaceLetters(const QString &);

    void play(const QString& soundFile);
    ///Display the win/loss count in the statusbar
    void setGameCount();

    /** Strip the accents off given string
     * @params original string to strip accents off of
     * @returns string without accents
     */
    QString stripAccents(const QString & original);

    bool m_showhint;
    bool m_winner;
    bool m_loser;
    int m_bgfill;

    int m_winCount;
    int m_lossCount;

    // The basic data ----------------

    // FIXME: Rewrite the whole handling of this so that goodWord,
    //        guessedLetters, and missedLetters all lack spaces.
    //        These spaces will then be added at draw time.

    /// The word to be guessed.
    QString          m_word;
    /// goodWord is the hidden word that is filled in during the game.
    /// Initialized to "_ " * (number of letters in the word).
    QString m_goodWord;

    /// Contains all letters already guessed.
    QStringList      m_guessedLetters;

    // Stores the missed letters that are shown on the screen.
    // Initialiazed to "_ " * MAXWRONGGUESSES.
    QString          m_missedLetters;

    /// How many times you missed.
    /// When this reaches MAXWRONGGUESSES, you are hanged.
    int              m_numMissedLetters;

    /// These two are the positions of the first and second spaces in the word.
    int       m_posFirstSpace;
    int       m_posSecondSpace;

    // Misc data  ----------------

    ///The index to the random sequence
    int m_randomInt;

    ///The random sequence of words of the current level file
    QList<QPair<QString, QString> > m_randomList;

    /// true if a hint exists
    bool             m_hintExists;

    ///Current hint
    QString          m_hint;

    /// true if the language contains accented letters.
    /// This is true for, among others, es, ca, pt or pt_BR
    bool             m_accentedLetters;


    // Graphics  ----------------
    QSvgRenderer *m_renderer;
    QPixmap       m_backgroundCache;

    // Widgets ----------------

    /// The widget where the user enters the letter. Upper case is
    /// transformed into lower case, except for german(!).
    KLineEdit       *m_letterInput;

    /// After you entered a letter in the line edit click this button
    /// to see if the letter is in the word or not.
    QPushButton     *m_guessButton, *m_playAgainButton;

    ///Current level file
    KEduVocDocument *m_doc;

    ///Current theme
    KHMTheme *m_theme;

    Phonon::MediaObject *m_player;
};

#endif // KHANGMANVIEW_H

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

