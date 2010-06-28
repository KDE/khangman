/*
 * Copyright 2001-2010 Anne-Marie Mahfouf <annma@kde.org>

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

#include "khangmanview.h"

#include <KMessageBox>
#include <KPassivePopup>
#include <KPushButton>
#include <KToggleAction>
#include <KVBox>

#include <Phonon/MediaObject>

#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPoint>
#include <QRegExp>
#include <QTimer>
#include <QToolTip>
#include <QWidget>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QRegExpValidator>
//project headers
#include "prefs.h"
#include "khangman.h"
#include "khmtheme.h"
#include "langutils.h"


KHangManView::KHangManView(KHangMan*parent) 
        : QWidget(parent), khangman(parent), winCount(0), lossCount(0), m_winner(false), 
	  m_loser(false), m_bgfill(0), m_numMissedLetters(0), m_posFirstSpace(-1), m_posSecondSpace(-1), 
          m_randomInt(-1), m_hintExists(true), m_accentedLetters(true), m_doc(0), m_theme(0), m_player(0)
{
    setAttribute(Qt::WA_StaticContents);

    // The widget for entering letters.
    m_letterInput = new KLineEdit( this );
    m_letterInput->setObjectName("charWrite" );
    QRegExp regExp("[A-Za-z\\x0080-\\x00FF\\x0100-\\x017E\\x0370-\\x03FF\\x0400-\\x04FF]");
    m_letterInput->setValidator(new QRegExpValidator(regExp, this));
    QSizePolicy policy( (QSizePolicy::Policy) 1, (QSizePolicy::Policy) 0 );
    policy.setHorizontalStretch( 0 );
    policy.setVerticalStretch( 0 );
    policy.setHeightForWidth( m_letterInput->sizePolicy().hasHeightForWidth() );
    m_letterInput->setSizePolicy( policy );
    m_letterInput->setMaxLength( 1 );
    m_letterInput->setAlignment( Qt::AlignHCenter  );

    // Press this button to enter a letter (or press enter)
    m_guessButton = new KPushButton( this);
    m_guessButton->setObjectName( "guessButton" );
    m_guessButton->setSizePolicy( policy );
    m_guessButton->setCursor(Qt::PointingHandCursor);
    m_guessButton->setText( i18n( "G&uess" ) );

    m_playAgainButton = new KPushButton(this);
    m_playAgainButton->setCursor(Qt::PointingHandCursor);
    m_playAgainButton->setText( i18n( "&Play again" ) );
    
    m_playAgainButton->setStyleSheet( QString("QPushButton { border: 2px solid black; border-radius: 15px; background-color: #969696; padding: 6px; }  QPushButton:hover { background-color: #787878 } "));

    setMinimumSize( QSize( 660, 370 ) );

    connect( m_letterInput, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
    connect( m_guessButton, SIGNAL( clicked() ), this, SLOT( slotTry() ));
    connect( m_playAgainButton, SIGNAL( clicked() ), this, SLOT( newGame() ));
    connect( this, SIGNAL(signalChangeStatusbar(const QString&, int)), khangman, SLOT(changeStatusbar(const QString&, int)));
    m_renderer = new QSvgRenderer();

    // not the best thing to do, but at least avoid no theme set
    //setTheme(KHMThemeFactory::instance()->buildTheme(0));
}


KHangManView::~KHangManView()
{
    delete m_player;
    delete m_renderer;
    delete m_theme;
}


// Handle a guess of the letter in sChar.
//
void KHangManView::replaceLetters(const QString& sChar)
{
    int   index = 0;
    bool  b_end = false;
    // Replace letter in the word
    if (Prefs::oneLetter()) {
    // We just replace the next instance.
        for (int count=0; count < m_word.count(sChar); ++count) {
            index = m_word.indexOf(sChar, index);
            if (goodWord.at(2*index)=='_') {
                goodWord.replace((2*index), 1, sChar);

                if (count == m_word.count(sChar)-1)
                b_end = true;
                break;
            }
            else {
                ++index;
            }

            if (count == m_word.count(sChar)-1) {
                b_end = true;
            }
        }
    }
    else {
        for (int count=0; count < m_word.count(sChar); ++count) {
            //searching for letter location
            index = m_word.indexOf(sChar, index);

            //we replace it...
            goodWord.replace((2*index), 1,sChar);
            ++index;
        }
    }

    if (m_accentedLetters && !Prefs::accentedLetters()) {
        if (sChar=="i") replaceLetters(QString::fromUtf8("í"));
        if (sChar=="a") replaceLetters(QString::fromUtf8("à"));
        if (sChar=="a") replaceLetters(QString::fromUtf8("á"));
        if (sChar=="a") replaceLetters(QString::fromUtf8("ã"));
        if (sChar=="u") replaceLetters(QString::fromUtf8("ü"));
        if (sChar=="o") replaceLetters(QString::fromUtf8("ò"));
        if (sChar=="o") replaceLetters(QString::fromUtf8("ó"));
        if (sChar=="o") replaceLetters(QString::fromUtf8("õ"));
        if (sChar=="e") replaceLetters(QString::fromUtf8("è"));
        if (sChar=="e") replaceLetters(QString::fromUtf8("é"));
        if (sChar=="u") replaceLetters(QString::fromUtf8("ù"));
    }
    if (!Prefs::oneLetter()) {
        m_guessedLetters << sChar; //appends the list only if not in One Letter only mode...
    }

    if (m_word.count(sChar) == 1) {
        m_guessedLetters << sChar; //append if only one instance
    }

    if (Prefs::oneLetter() && b_end) {
        m_guessedLetters << sChar;
    }
}

void KHangManView::play(const QString& soundFile)
{
    if (soundFile.isEmpty()) {
        return;
    }

    if (!m_player) {
        m_player = Phonon::createPlayer(Phonon::GameCategory);
    }
    else {
        m_player->stop();
    }

    m_player->setCurrentSource(soundFile);
    m_player->play();
}

QString KHangManView::stripAccents(const QString & original)
{
    QString noAccents;
    QString decomposed = original.normalized(QString::NormalizationForm_D);
    for (int i = 0; i < decomposed.length(); ++i) {
        if ( decomposed[i].category() != QChar::Mark_NonSpacing ) {
            noAccents.append(decomposed[i]);
        }
    }
    return noAccents;
}

bool KHangManView::containsChar(const QString &sChar)
{
    return m_word.contains(sChar) || stripAccents(m_word).contains(sChar);
}

// ----------------------------------------------------------------

// FIXME: Move this function somewhere logical


void KHangManView::setTheme(KHMTheme *theme)
{
    // we don't allow null themes
    if (!theme) {
        return;
    }

    QString svgpath = KStandardDirs::locate("data", QString("khangman/pics/%1/%2").arg(theme->name(), theme->svgFileName()));
    // we don't allow themes with no svg installed

    if (!QFile::exists(svgpath) || theme->svgFileName().isEmpty()) {
        kDebug() << "SVG file doesn't exists";
        return;
    }

    delete m_theme;
    m_theme = theme;

    m_renderer->load(svgpath);


    m_backgroundCache = QPixmap();
    int r1, g1, b1;
    m_theme->guessButtonColor().getRgb(&r1, &g1, &b1);
    int r2, g2, b2;
    m_theme->guessButtonTextColor().getRgb(&r2, &g2, &b2);
    int r3, g3, b3;
    m_theme->guessButtonHoverColor().getRgb(&r3, &g3, &b3);
    m_guessButton->setStyleSheet(QString("QPushButton{border-style: solid; background-color: rgb(%1, %2, %3); color: rgb(%4, %5, %6) ; padding: 3px; border-bottom-right-radius:10; border-radius: 15px; border-width: 3px;} QPushButton:hover{ background-color: rgb(%7, %8, %9)}").arg(r1).arg(g1).arg(b1).arg(r2).arg(g2).arg(b2).arg(r3).arg(g3).arg(b3));
    m_theme->letterInputTextColor().getRgb(&r1, &g1, &b1);
    m_letterInput->setStyleSheet(QString("QLineEdit{border-style: solid; background-color: white; color: rgb(%1, %2, %3) ; border-bottom-right-radius:10; border-radius: 15px; border-width: 3px; padding: 3px; border-color: rgb(%1, %2, %3)}").arg(r1).arg(g1).arg(b1));
    m_letterInput->setFocus();
    m_guessButton->setFont(QFont("Dustismo Roman", height()/22));
    QFont charWrite_font( m_letterInput->font() );
    charWrite_font.setPointSize( height()/18 );
    m_letterInput->setFont(charWrite_font);
    update();
}

// ----------------------------------------------------------------
//                           Painting


void KHangManView::paintEvent( QPaintEvent * e )
{
    // Repaint the contents of the khangman view
    QPainter p(this);
    paintHangman(p, e->rect());
    paintWord(p, e->rect());
    paintMisses(p, e->rect());
    paintHint(p, e->rect());
    if(m_loser || m_winner) {
        paintGameOver(p, e->rect());
    }
}


void KHangManView::paintHangman(QPainter &p, const QRect& rect)
{

    // Draw the background
    if (m_backgroundCache.size() != size()) {
        m_backgroundCache = QPixmap(size());
        QPainter aux(&m_backgroundCache);
        m_renderer->render(&aux, "background");
    }
    p.drawPixmap(rect.topLeft(), m_backgroundCache, rect);
    // Draw the animated hanged K
    QRect myRect = m_theme->kRect(size());
    if (!myRect.intersects(rect)) {
        return;
    }

    m_renderer->render(&p, QString("ani%2").arg(m_numMissedLetters), myRect);
}


void KHangManView::paintWord(QPainter &p, const QRect& rect)
{
    QRect myRect = m_theme->wordRect(size());
    if (!myRect.intersects(rect)) {
        return;
    }

    p.setPen(m_theme->letterColor());

    QFont tFont = LangUtils::fontForLanguage(Prefs::selectedLanguage());

    // FIXME: This has to be scaled depending of the dpi
    tFont.setPixelSize( 28 );

    p.setFont(tFont);
    // Set first letter as upper case for German
    goodWord.replace(0, 1, LangUtils::capitalize(goodWord.at(0), Prefs::selectedLanguage()));
    p.drawText(myRect, Qt::AlignCenter|Qt::AlignCenter, goodWord);
}


void KHangManView::paintMisses(QPainter &p, const QRect& )
{
    // Get the color for the letters.
    QColor letterColor = m_theme->letterColor();

    // Draw the missed letters
    QFont tFont = LangUtils::fontForLanguage(Prefs::selectedLanguage());
    tFont.setPixelSize( 28 );
    p.setPen( letterColor );
    p.setFont(tFont);

    QFontMetrics  fm(tFont);
    QRect         fmRect(fm.boundingRect(m_missedLetters));
    QRect         myRect = QRect(width() - fmRect.width(), 15,
                      fmRect.width(), fm.height());
    p.drawText(myRect, Qt::AlignLeft, m_missedLetters);

    // Draw the "Misses" word
    QString  misses = i18n("Misses");
    QFont  mFont = QFont("Domestic Manners");
    mFont.setPointSize(30);
    p.setFont(mFont);

    QFontMetrics    fm2(mFont);
    QRect           fmRect2(fm2.boundingRect(misses));
    QRect           myRect2(width()- fmRect.width() - fmRect2.width() - 15,
                            15 - fm2.height() + fm.height(),
                            fmRect2.width(), fm2.height());
    p.setPen( letterColor );
    p.drawText(myRect2, Qt::AlignLeft|Qt::AlignCenter, misses);
}


void KHangManView::paintHint(QPainter &p, const QRect &rect)
{
    if(!Prefs::hint()) {
        return;
    }
    
    QRect myRect = m_theme->hintRect(size());
    if(!myRect.intersects(rect)) {
        return;
    }

/* Debug */  //p.fillRect(myRect, QBrush(Qt::cyan) );

    QColor letterColor = m_theme->letterColor();
    p.setPen(letterColor);

    QString hint=i18n("Hint");
    QFont hFont = QFont("Domestic Manners");
    hFont.setPointSize(14);
    QFontMetrics fm1(hFont);
    QRect fm1Rect(fm1.boundingRect(hint));
    
    QFont tFont = LangUtils::fontForLanguage(Prefs::selectedLanguage());
    tFont.setPixelSize(14);
    QFontMetrics fm2(tFont);
    QRect fm2Rect(fm2.boundingRect(m_hint));

    QRect myRect1 = QRect(myRect.x(), myRect.y(), myRect.width(), fm1Rect.height());

    QRect myRect2 = QRect(myRect.x(), myRect.y()+fm1Rect.height(), myRect.width(), myRect.height()-fm1Rect.height());

    p.setFont(hFont);
    p.drawText(myRect1, Qt::AlignHCenter, hint);

    p.setFont(tFont);
    p.drawText(myRect2, Qt::AlignHCenter | Qt::TextWordWrap, m_hint);
}


void KHangManView::paintGameOver(QPainter &p, const QRect &rect)
{
    if (!Prefs::enableAnimations()) {
        m_bgfill=100;
    }
    
    QString title=m_loser?i18n("You lost. The word was \"%1\".", m_word):i18n("Congratulations! You won!");
    p.fillRect(QRect(rect.x(), rect.y(), rect.width(), (int)(rect.height()/100.0*m_bgfill)), QBrush(QColor(0,0,0,70)));
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::black, 2);
    p.setPen(pen);
    QBrush brush(QColor(133,133,133,180));
    p.setBrush(brush);
    QRect rectangle=QRect(width()/10, height()*2/10, width()-width()*2/10, height()-height()*4/10);
    p.drawRoundRect(rectangle);
    QFont tFont("Domestic Manners");
    tFont.setPixelSize( (int)   ((width()*42)    /   (26*title.length()))   );
    p.setPen(Qt::black);
    p.setFont(tFont);
    p.drawText(rectangle, Qt::AlignCenter, title);

    if (m_bgfill<100) {
        m_bgfill+=5;
        QTimer::singleShot(10, this, SLOT(update()));
	return;
    }
    
    m_playAgainButton->setFocus();
    m_playAgainButton->setDefault(true);
    m_playAgainButton->move(width()/2 - m_playAgainButton->width()/2 , height()/2 + m_playAgainButton->height()/2);
    m_playAgainButton->setVisible(true);
}


void KHangManView::resizeEvent(QResizeEvent *)
{
    m_letterInput->setMinimumSize( QSize( height()/18, 0 ) );

    QFont charWrite_font( m_letterInput->font() );
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Sans Serif" );

    m_letterInput->setFont( charWrite_font );
    m_letterInput->setGeometry(width()-2*height()/12, height()-2*height()/16,
                               height()/9, height()/9);
    m_guessButton->setFont(QFont("Dustismo Roman", height()/22));
    m_guessButton->setGeometry(width() - 2*height()/12
                               - m_guessButton->sizeHint().width()-7,
                               height() - 2*height()/16,
                               m_guessButton->sizeHint().width(), height()/9);

    m_playAgainButton->setFont(QFont("Dustismo Roman", height()/32));


    m_playAgainButton->setGeometry( width()/2 - m_playAgainButton->width()/2 , 
                                    height()/2 + m_playAgainButton->height()/2,
				    m_playAgainButton->sizeHint().width(), height()/9 );
}

// ----------------------------------------------------------------
//                             Slots

void KHangManView::slotTry()
{
    QString guess = m_letterInput->text().toLower();
    
    if (guess.isEmpty()) {
        m_letterInput->setFocus();
        return;
    }
    kDebug() << "guess as entered: " << guess;

    // Handle the guess.
    if (!m_guessedLetters.contains(guess)) {
        // The letter is not already guessed.

        if (containsChar(guess)) {
            replaceLetters(guess);

            // This is needed because of the white spaces.
            QString  stripWord = goodWord;
            QString  sword     = m_word;
            if (m_posSecondSpace > 0)  {
                stripWord.replace(2*m_posFirstSpace,   1, "");
                stripWord.replace(2*m_posFirstSpace-1, 1, "");

                stripWord.replace(2*(m_posSecondSpace-1),   1, "");
                stripWord.replace(2*(m_posSecondSpace-1)-1, 1, "");
            }

            QStringList  rightChars =  stripWord.split(' ');
            QString      rightWord  = rightChars.join("");
            update();
            sword.remove(QRegExp(" "));

            // If the user made it...
            if (rightWord.trimmed().toLower() == sword.trimmed().toLower()) {
                if (Prefs::sound()) {
                    QString soundFile = KStandardDirs::locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
                    play(soundFile);
                }
                m_winner=true;
                ++winCount;
                setGameCount();
                m_letterInput->setEnabled(false);
                m_guessButton->setEnabled(false);
            }
        }
        else {
            // The char is missed.

            m_guessedLetters << guess;
            m_missedLetters = m_missedLetters.replace((2 * m_numMissedLetters), 1, guess);

            ++m_numMissedLetters;
            update();
            // Check if we have reached the limit of wrong guesses.
            if (m_numMissedLetters >= MAXWRONGGUESSES) {
                m_loser=true;
                ++lossCount;
                setGameCount();
                m_letterInput->setEnabled(false);
                m_guessButton->setEnabled(false);
            }
        }
    }
    else {
        // The letter is already guessed.

        // Show a popup that says as much.
        QPoint point; 
        KPassivePopup *popup = new KPassivePopup( this );
        popup->setPopupStyle( KPassivePopup::Balloon );
        popup->setAutoDelete( true );
        popup->setTimeout( Prefs::missedTimer()*1000 );
        popup->setView(i18n("This letter has already been guessed.") );

        int  x = 0;
        int  y = 0;
        if (m_missedLetters.contains(guess) > 0) {
            // FIXME: popup should be better placed.

            QPoint abspos = popup->pos();
            x = abspos.x() + width()*400/700;
            y = abspos.y() + height()*50/535;
            point = QPoint(x, y);

            // Create a 1 second single-shot timer, and reenable user
            // input after this time.
            QTimer::singleShot( Prefs::missedTimer() * 1000,
                                this, SLOT(enableUserInput()) );

            // Disable any possible entry
            m_letterInput->setEnabled(false);
        }

        if (goodWord.contains(guess) > 0) {
            point = m_theme->goodWordPos(size(), popup->pos());
            
            QTimer::singleShot( Prefs::missedTimer() * 1000,
                                this, SLOT(enableUserInput()) );

            // Disable any possible entry
            m_letterInput->setEnabled(false);
        }

        popup->show(mapToGlobal(point));
    }

    // Reset the entry field after guess.
    m_letterInput->clear();
    m_letterInput->setFocus();
}


void KHangManView::enableUserInput()
{
    m_letterInput->setEnabled(true);
    m_letterInput->setFocus();
}


void KHangManView::newGame()
{
    m_loser=false;
    m_winner=false;
    m_bgfill=0;

    m_letterInput->setEnabled(true);
    m_guessButton->setEnabled(true);
    m_playAgainButton->setVisible(false);

    //reset Hint action
    khangman->hintAct->setChecked(Prefs::hint());
    khangman->hintAct->setEnabled(m_hintExists);

    setGameCount();
    if (Prefs::sound()) {
        QString soundFile = KStandardDirs::locate("data", "khangman/sounds/new_game.ogg");
        play(soundFile);
    }

    reset();
    ++m_randomInt;
    game();

    update();
    m_letterInput->setFocus();
}

void KHangManView::readFile()
{
    // Check if the data files are installed in the correct dir.
    QFile    myFile;
    myFile.setFileName(Prefs::levelFile());

    if (!myFile.exists()) {
        QString  mString = i18n("File $KDEDIR/share/apps/kvtml/%1/%2 not found.\n"
                                "Please check your installation.",
                                 Prefs::selectedLanguage(),
                                 Prefs::levelFile());
        KMessageBox::sorry( this, mString, i18n("Error") );
        //qApp->quit();
    }

    // Detects if file is a kvtml file so that it's a hint enable file
    slotSetWordsSequence();
}

void KHangManView::reset()
{
    goodWord = "";
    m_word   = "";

    m_guessedLetters.clear();
    m_numMissedLetters = 0;
    m_missedLetters    = "_ _ _ _ _ _ _ _ _ _  ";

    // Clear the input field.
    m_letterInput->clear();
}


void KHangManView::game()
{
    kDebug() << "language " << Prefs::selectedLanguage();
    kDebug() << "level "    << Prefs::levelFile();

    m_word = m_randomList[m_randomInt%NumberOfWords].first;
    m_word = m_word.toLower();
    m_hint = m_randomList[m_randomInt%NumberOfWords].second;

    if (m_word.isEmpty()) {
        ++m_randomInt;
        game();
    }

    if (m_hint.isEmpty()) {
        m_hintExists = false;   // Hint does not exist.
        //TODO refresh hint action
    }
    else {
        m_hintExists = true;
        //TODO refresh hint action
    }
    khangman->hintAct->setEnabled(m_hintExists);

    // Display the number of letters to guess with _
    for (int i = 0; i < m_word.length(); i++) {
        goodWord.append("_ ");
    }

    // Remove the last trailing space.
    goodWord.remove(goodWord.length()-1);

    // If needed, display white space or - if in word or semi dot.

    // 1. Find dashes.
    int posOfFirstDash = m_word.indexOf( "-" );
    
    if (posOfFirstDash>0) {
        goodWord.replace(2*posOfFirstDash, 1, "-");
        int posOfSecondDash = m_word.indexOf( "-", posOfFirstDash+1);
        if (posOfSecondDash>0) {
            goodWord.replace(2*posOfSecondDash, 3, "-");
        }
        if (posOfSecondDash>1) {
            goodWord.append("_");
        }
    }

    // 2. Find white space.
    m_posFirstSpace = m_word.indexOf( " " );
    
    if (m_posFirstSpace > 0) {
        goodWord.replace(2*m_posFirstSpace, 1, " ");
        m_posSecondSpace = m_word.indexOf( " ", m_posFirstSpace+1);
        if (m_posSecondSpace > 0)
            goodWord.replace(2*m_posSecondSpace, m_posFirstSpace+1, " ");
    }

    // 3. Find ·
    int posOfDot = m_word.indexOf( QString::fromUtf8("·") );
    
    if (posOfDot>0) {
        goodWord.replace(2*posOfDot, 1, QString::fromUtf8("·") );
    }

    // 4. Find '
    int posOfApos = m_word.indexOf( "'" );
    
    if (posOfApos>0) {
        goodWord.replace(2*posOfApos, 1, "'");
    }
}


void KHangManView::slotSetWordsSequence()
{
    kDebug() << "in read kvtml file ";

    delete m_doc;

    m_doc = new KEduVocDocument(this);
    ///@todo open returns KEduVocDocument::ErrorCode
    m_doc->open(Prefs::levelFile());

    //how many words in the file
    NumberOfWords = m_doc->lesson()->entryCount(KEduVocLesson::Recursive);

    //get the words+hints
    KRandomSequence randomSequence;
    m_randomList.clear();
    for (int j = 0; j < NumberOfWords; ++j) {
        QString hint = m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(0)->comment();
        if (hint.isEmpty() && m_doc->identifierCount() > 0) {
            // if there is no comment or it's empty, use the first translation if
            // there is one
            hint = m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(1)->text();
        }

        m_randomList.append(qMakePair(m_doc->lesson()->entries(KEduVocLesson::Recursive).value(j)->translation(0)->text(), hint));    }
    //shuffle the list
    randomSequence.randomize(m_randomList);
}

void KHangManView::setGameCount()
{
    emit signalChangeStatusbar(i18n("Wins: %1", winCount), IDS_WINS);
    emit signalChangeStatusbar(i18n("Losses: %1", lossCount), IDS_LOSSES);
}

#include "khangmanview.moc"

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

