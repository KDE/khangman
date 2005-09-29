/*
 * Copyright (C) 2001-2005 Anne-Marie Mahfouf <annma@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

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


class KPushButton;
class KHangMan;

#include <klineedit.h>

#include <krandomsequence.h>
#include <keduvocdata.h>


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

    KHangManView(KHangMan *parent=0, const char *name=0);

    virtual ~KHangManView();

    ///parent instance
    KHangMan *khangman;

    void setTheme();
    
    bool  hintExists() const          { return m_hintExists;            }

    bool  accentedLetters() const     { return m_accentedLetters;       }
    void  setAccentedLetters( bool _accentedLetters )
                                      { m_accentedLetters = _accentedLetters; }

    /// Enter a letter into the input widget.
    void  enterLetter(QString letter) { m_letterInput->setText(letter); }

    
signals:

     /// Use this signal to change the content of the statusbar
    void signalChangeLanguage(int);

    /// Emit this signal to say if this is a kvtml file or not (hints
    /// enabled or not).
    void signalKvtml(bool);

private:

    // FIXME: Move all these below the next "private:" and when doing
    //        that, rename them (if necessary), and regroup them into 
    //        logical groups.

    // FIXME:  Rename these into something sensible!
    //         (or better yet: remove them altogether)
    int  c;    // These two are the positions of the first and 
    int  dd;   // second spaces in the word.


 protected:

    // Events
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );

    /// Enable hints on mouse right click if Hints exist.
    virtual void mousePressEvent( QMouseEvent *mouse );


 private:

    // Painting

    /// Paint the animated hanged K sequence.
    void paintHangman(QPainter &p);
    void paintWord(QPainter &p);
    void paintMisses(QPainter &p);

    /// Reset everything to start a new game, missed letters is empty.
    void reset();
    /// Load kvtml file and get a word and its tip in random.
    void readFile();
    /// Play a game: look for a word to be guessed and load its tip.
    void game();

    /// Load the K animated sequence depending of the theme.
    void loadAnimation();

    /// Set the background pixmap to the QPixmap argument.
    void setBackground(QPixmap& );

    /// Return true if the word contains the char in the QString.
    bool  containsChar(const QString &);

    void  replaceLetters(const QString &);

public slots:
    /// If you want to play with a new word
    void slotNewGame();

private slots:

    /// After you click on Guess button or hit Enter when guessing a
    /// new letter, see if the letter is in the word or not
    void slotTry();

    /// Reenable user input.  This is used as a target for timers when
    /// the user has made a guess that was already made earlier, and a
    /// popup informing about this is closed.
    void enableUserInput();

private:

    // The basic data ----------------

    // FIXME: Rewrite the whole handling of this so that goodWord,
    //        guessedLetters, and missedLetters all lack spaces.  
    //        These spaces will then be added at draw time.

    /// The word to be guessed.
    QString          m_word;
    /// goodWord is the hidden word that is filled in during the game.
    /// Initialized to "_ " * (number of letters in the word).
    QString          m_goodWord;

    /// Contains all letters already guessed.
    QStringList      m_guessedLetters;

    // Stores the missed letters that are shown on the screen.
    // Initialiazed to "_ " * MAXWRONGGUESSES.
    QString          m_missedLetters;
    /// How many times you missed.  
    /// When this reaches MAXWRONGGUESSES, you are hanged.
    int              m_numMissedLetters;


    // Misc data  ----------------

    // Stores the number of the last word.  This is to make sure that
    // the same word is not given twice in a row.
    int              m_lastWordNumber;

    QString          m_themeName;
    KRandomSequence  m_random;

    /// true if a hint exists
    bool             m_hintExists;
    QString          m_hint;

    /// true if the language contains accented letters.  
    /// This is true for, among others, es, ca, pt or pt_BR
    bool             m_accentedLetters;
    

    // Graphics  ----------------

    // Background picture (sea or desert)
    QPixmap          m_originalBackground;   // Original image
    QPixmap          m_resizedBackground;    // Resized to fit the window.

    /// The hanged K animation sequence.
    QPixmap          m_animationPics[MAXWRONGGUESSES + 1];

    // Widgets ----------------

    /// The widget where the user enters the letter. Upper case is
    /// transformed into lower case, except for german(!).
    KLineEdit       *m_letterInput;

    /// After you entered a letter in the line edit click this button
    /// to see if the letter is in the word or not.
    KPushButton     *m_guessButton;
};

#endif // KHANGMANVIEW_H
 
