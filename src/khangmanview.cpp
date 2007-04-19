/*
 * Copyright (C) 2001-2006 Anne-Marie Mahfouf <annma@kde.org>

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

#include "khangmanview.h"

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kvbox.h>

#include <phonon/audioplayer.h>

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
    : QWidget(parent /*WStaticContents | WNoAutoErase*/)
{
    setAttribute(Qt::WA_StaticContents);
    khangman = parent;
    

    // The widget for entering letters.
    m_letterInput = new KLineEdit( this );
    m_letterInput->setObjectName("charWrite" );
    QRegExp regExp("[A-Za-z\\x0080-\\x00FF\\x0100-\\x017E\\x0400-\\x04FF]");
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
    //m_guessButton->setAutoMask( true );
    m_guessButton->setFlat( true );
    m_guessButton->setText( i18n( "G&uess" ) );

    setMinimumSize( QSize( 700, 535 ) );

    // Some misc initializations.
    m_posFirstSpace    = -1;
    m_posSecondSpace   = -1;
    m_numMissedLetters = 0;
    m_accentedLetters  = true;
    m_hintExists       = true;	// Assume hint exists
    m_doc              = 0;
    m_theme            = 0; // essential
    m_player           = 0;
    randomInt          = -1;
    connect( m_letterInput, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
    connect( m_guessButton, SIGNAL( clicked() ), this, SLOT( slotTry() ));
    
    m_renderer = new QSvgRenderer();

    // not the best thing to do, but at least avoid no theme set
    setTheme(KHMThemeFactory::instance()->buildTheme(0));
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

    kDebug() << "word " << m_word << endl;

    // Replace letter in the word
    if (Prefs::oneLetter()) {
    // We just replace the next instance.
        for (int count=0; count < m_word.count(sChar); count++) {

            index = m_word.indexOf(sChar, index);
            if (goodWord.at(2*index)=='_') {
                goodWord.replace((2*index), 1, sChar);

                kDebug() << "goodword " << goodWord << endl;
                if (count == m_word.count(sChar)-1)
                b_end = true;
                break;
            }
            else
                index++;

            if (count == m_word.count(sChar)-1)
            b_end = true;
        }
    }
    else {
        for (int count=0; count < m_word.count(sChar); count++) {
            //searching for letter location
            index = m_word.indexOf(sChar, index);

            //we replace it...
            goodWord.replace((2*index), 1,sChar);
            index++;
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
    if (!Prefs::oneLetter())
        m_guessedLetters << sChar; //appends the list only if not in One Letter only mode...

    if (m_word.count(sChar) == 1)
        m_guessedLetters << sChar; //append if only one instance

    if (Prefs::oneLetter() && b_end)
        m_guessedLetters << sChar;
}

void KHangManView::play(const QString& soundFile)
{
    if (soundFile.isEmpty())
        return;

    if (!m_player)
    {
        m_player = new Phonon::AudioPlayer(Phonon::GameCategory);
    }
    m_player->stop();
    m_player->play(soundFile);
}


bool KHangManView::containsChar(const QString &sChar)
{
    bool b_isInWord = false;
    if (m_accentedLetters && !Prefs::accentedLetters()) {
        if (sChar=="i")
            b_isInWord = m_word.contains(QString::fromUtf8("í"));

        if (sChar=="a")
            b_isInWord = m_word.contains(QString::fromUtf8("à"))
                || m_word.contains(QString::fromUtf8("á"))
                || m_word.contains(QString::fromUtf8("ã"));

        if (sChar=="u")
            b_isInWord = m_word.contains(QString::fromUtf8("ü"))
                || m_word.contains(QString::fromUtf8("ù"));

        if (sChar=="o")
            b_isInWord = m_word.contains(QString::fromUtf8("ò"))
                || m_word.contains(QString::fromUtf8("ó"))
                || m_word.contains(QString::fromUtf8("õ"));

        if (sChar=="e")
            b_isInWord = m_word.contains(QString::fromUtf8("è"))
                || m_word.contains(QString::fromUtf8("é"));
    }

    return (b_isInWord || m_word.contains(sChar));
}


// ----------------------------------------------------------------
//                           Event handlers


void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
    if (mouse->button() == Qt::RightButton && m_hintExists && Prefs::hint()) {

        KPassivePopup *myPopup = new KPassivePopup( m_letterInput);
        myPopup->setView(i18n("Hint"), m_hint );
        myPopup->setPalette(QToolTip::palette());
        myPopup->setTimeout(Prefs::hintTimer()*1000); //show for 4 seconds as default
        int x=0, y=0;

        QPoint abspos = mapToGlobal( QPoint( 0, 0 ) );
        x = abspos.x() + width()*70/700;
        y = abspos.y() + height()*150/535;

        QPoint  point = QPoint(x, y);
        myPopup->show(mapToGlobal(point));

        // Maybe it's better to popup where the mouse clicks, in that
        // case kill the popup before new click
        //myPopup->move(mouse->pos());
    }

    //update();
}


// ----------------------------------------------------------------

// FIXME: Move this function somewhere logical


void KHangManView::setTheme(KHMTheme *theme)
{
    // we don't allow null themes
    if (!theme)
        return;

    QString svgpath = KStandardDirs::locate("data", QString("khangman/pics/%1/%2").arg(theme->name(), theme->svgFileName()));
    // we don't allow themes with no svg installed
    if (!QFile::exists(svgpath))
        return;

    delete m_theme;
    m_theme = theme;

    m_renderer->load(svgpath);

    m_backgroundCache = QPixmap();
    m_guessButton->setPalette(m_theme->palette(KHMTheme::GuessButtonPalette));
    m_letterInput->setPalette(m_theme->palette(KHMTheme::LetterInputPalette));
    m_letterInput->setFocus();
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
}


void KHangManView::paintHangman(QPainter &p, const QRect& rect)
{
    QRect drawRect;
    // Draw the background
    if (m_backgroundCache.size() != size()) {
        m_backgroundCache = QPixmap(size());
        QPainter aux(&m_backgroundCache);
        m_renderer->render(&aux, "background");
        // Draw the animated hanged K
        // drawRect = QRect(0, 0, width()*630/700, height()*285/535);
        // m_renderer->render(&aux, QString("ani%2").arg(m_numMissedLetters), drawRect);
    }
    p.drawPixmap(rect.topLeft(), m_backgroundCache, rect);
}


void KHangManView::paintWord(QPainter &p, const QRect& rect)
{
    QRect myRect = m_theme->wordRect(size());
    if (!myRect.intersects(rect))
        return;

    p.setPen(m_theme->fontColor());

    QFont tFont = LangUtils::fontForLanguage(Prefs::selectedLanguage());

    // FIXME: This has to be scaled depending of the dpi
    tFont.setPixelSize( 28 );

    p.setFont(tFont);
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

    QFontMetrics  fm2(mFont);
    QRect         fmRect2(fm2.boundingRect(misses));
    QRect         myRect2(width()- fmRect.width() - fmRect2.width() - 15,
                      15 - fm2.height() + fm.height(),
                      fmRect2.width(), fm2.height());
    p.setPen( letterColor );
    p.drawText(myRect2, Qt::AlignLeft|Qt::AlignCenter, misses);
}


void KHangManView::resizeEvent(QResizeEvent *)
{
    m_letterInput->setMinimumSize( QSize( height()/18, 0 ) );

    QFont charWrite_font( m_letterInput->font() );
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Sans Serif" );

    m_letterInput->setFont( charWrite_font );
    m_letterInput->setGeometry(width()-2*height()/12, height()-2*height()/16,
                               height()/10, height()/10);
    m_guessButton->setFont(QFont("Dustimo Roman", height()/22));
    m_guessButton->setGeometry(width() - 2*height()/12
                               - m_guessButton->sizeHint().width()-5,
                               height() - 2*height()/16,
                               m_guessButton->sizeHint().width(), height()/10);
}


// ----------------------------------------------------------------
//                             Slots

void KHangManView::slotTry()
{
    QString guess = m_letterInput->text();
    kDebug() << "guess as entered: " << guess << endl;

    guess = LangUtils::capitalize(guess, Prefs::selectedLanguage(), Prefs::upperCase());

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

        QStringList  rightChars =  stripWord.split(" ");
        QString      rightWord  = rightChars.join("");
        update();
        sword.remove(QRegExp(" "));
        
        // If the user made it...
        if (rightWord.trimmed().toLower() == sword.trimmed().toLower()) {

            // We reset everything...
            //TODO find a better way to finish
            //
            if (Prefs::sound()) {
                QString soundFile = KStandardDirs::locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
                play(soundFile);
            }

            if (Prefs::wonDialog()) {
                // TODO: hide Hint KPassivePopup if any
                QPoint point;
                KPassivePopup *popup = new KPassivePopup( this);
                popup->setAutoDelete( true );
                popup->setTimeout( 4*1000 );
                popup->setView(i18n("Congratulations,\nyou won!") );

                int x =0, y = 0;
                QPoint abspos = popup->pos();
                x = abspos.x() + width()*50/700;
                y = abspos.y() + height()*20/535;
                point = QPoint(x, y);
                popup->show(mapToGlobal(point));
                QTimer::singleShot( 4*1000, this, SLOT(slotNewGame()) );
            }
            else if (KMessageBox::questionYesNo(this,
                                        i18n("Congratulations! You won! Do you want to play again?"),
                                        QString(),KGuiItem(i18n("Play Again")), KGuiItem(i18n("Do Not Play"))) == 3)
                slotNewGame();
            else
                qApp->quit();
        }
    }
    else {
        // The char is missed.

        m_guessedLetters << guess;
        m_missedLetters = m_missedLetters.replace((2 * m_numMissedLetters), 1, guess);

        m_numMissedLetters++;
        update();

        // Check if we have reached the limit of wrong guesses.
        if (m_numMissedLetters >= MAXWRONGGUESSES) {

            //TODO sequence to finish when hanged
            QStringList charList=m_word.split(" ");
            QString theWord=charList.join(" ");
            goodWord = theWord;
            //usability: find another way to start a new game
            QString newGameString = i18n("You lost. Do you want to play again?");
            if (Prefs::wonDialog()) {
                // TODO: hide Hint KPassivePopup if any
                QPoint point;
                KPassivePopup *popup = new KPassivePopup( this);
                popup->setAutoDelete( true );
                popup->setTimeout( 4*1000 );

                KVBox *vb = new KVBox( popup );

                QLabel *popLabel = new QLabel( vb);
                popLabel->setFont(QFont("Sans Serif", 14, QFont::Normal));
                popLabel->setText(i18n("<qt>You lost!\nThe word was\n<b>%1</b></qt>", m_word));
                popup->setView( vb );

                QPoint abspos = popup->pos();
                int  x = abspos.x() + width()  * 50 / 700;
                int  y = abspos.y() + height() * 20 / 535;
                popup->show(mapToGlobal(QPoint(x, y)));

                QTimer::singleShot( 4 * 1000, this, SLOT(slotNewGame()) );
            }
            else if (KMessageBox::questionYesNo(this, newGameString, QString(),
                                                KGuiItem(i18n("Play Again")), KGuiItem(i18n("Do Not Play"))) == 3)
                slotNewGame();
            else
                qApp->quit();
        }
    }
    }
    else {
        // The letter is already guessed.

        // Show a popup that says as much.
        QPoint point;
        KPassivePopup *popup = new KPassivePopup( this );
        popup->setPopupStyle( KPassivePopup::Boxed );
        popup->setAutoDelete( true );
        popup->setTimeout( 1000 );
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
    m_letterInput->setText("");
}


void KHangManView::enableUserInput()
{
    m_letterInput->setEnabled(true);
    m_letterInput->setFocus();
}


void KHangManView::slotNewGame()
{
    if (Prefs::sound()) {
        QString soundFile = KStandardDirs::locate("data", "khangman/sounds/new_game.ogg");
        play(soundFile);
    }
    
    reset();
    randomInt++;
    game();

    update();
    m_letterInput->setFocus();
}

void KHangManView::readFile()
{
    // Check if the data files are installed in the correct dir.
    QString  myString = QString("khangman/data/%1/%2")
                          .arg(Prefs::selectedLanguage())
                          .arg(Prefs::levelFile());
    QFile    myFile;
    myFile.setFileName(KStandardDirs::locate("data", myString));

    if (!myFile.exists()) {
        QString  mString = i18n("File $KDEDIR/share/apps/khangman/data/%1/%2 not found!\n"
                                "Check your installation, please!",
                                 Prefs::selectedLanguage(),
                                 Prefs::levelFile());
        KMessageBox::sorry( this, mString, i18n("Error") );
        qApp->quit();
    }

    // We open the file and store info into the stream...
    QFile  openFileStream(myFile.fileName());
    openFileStream.open(QIODevice::ReadOnly);
    QTextStream readFileStr(&openFileStream);
    readFileStr.setCodec("UTF-8");

    // Alldata contains all the words from the file
    QStringList allData = readFileStr.readAll().split("\n");
    openFileStream.close();

    // Detects if file is a kvtml file so that it's a hint enable file
    if (allData.first() == "<?xml version=\"1.0\"?>") {
        slotSetWordsSequence();
    }
    else {
        //TODO abort if not a kvtml file maybe
        kDebug() << "Not a kvtml file!" << endl;
    }
}

void KHangManView::reset()
{
    goodWord = "";
    m_word   = "";

    m_guessedLetters.clear();
    m_numMissedLetters = 0;
    m_missedLetters    = "_ _ _ _ _ _ _ _ _ _  ";

    // Clear the input field.
    m_letterInput->setText("");
}


void KHangManView::game()
{
    kDebug() << "language " << Prefs::selectedLanguage() << endl;
    kDebug() << "level "    << Prefs::levelFile()        << endl;

    m_word = m_randomList[randomInt%NumberOfWords].first;
    m_hint = m_randomList[randomInt%NumberOfWords].second;

    if (m_word.isEmpty()){
        randomInt++;
	game();
	}

    if (m_hint.isEmpty()) {
        m_hintExists = false;	// Hint does not exist.
        // FIXME: Make this a signal instead.
        khangman->changeStatusbar("", 103);
    }
    else {
        m_hintExists = true;
        khangman->setMessages();
    }

    if (Prefs::upperCase() && Prefs::selectedLanguage() =="de")
    {
        m_word = m_word.toUpper();// only for German currently

        // Replace ß with SS in German
        if (m_word.contains(QString::fromUtf8("ß"))) {
            int index = m_word.indexOf(QString::fromUtf8("ß"),0);
            m_word.replace(index,1, "S");
            //TODO add a S here
        }
    }

    kDebug() << m_word << endl;

    // Display the number of letters to guess with _
    for (int i = 0; i < m_word.length(); i++)
        goodWord.append("_ ");

    // Remove the last trailing space.
    goodWord.remove(goodWord.length()-1);

    kDebug() << goodWord << endl;

    // If needed, display white space or - if in word or semi dot.

    // 1. Find dashes.
    int posOfFirstDash = m_word.indexOf( "-" );
    if (posOfFirstDash>0) {
        goodWord.replace(2*posOfFirstDash, 1, "-");

        int posOfSecondDash = m_word.indexOf( "-", posOfFirstDash+1);
        if (posOfSecondDash>0)
            goodWord.replace(2*posOfSecondDash, 3, "-");
        if (posOfSecondDash>1)
            goodWord.append("_");
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
    if (posOfDot>0)
        goodWord.replace(2*posOfDot, 1, QString::fromUtf8("·") );

    // 4. Find '
    int posOfApos = m_word.indexOf( "'" );
    if (posOfApos>0)
        goodWord.replace(2*posOfApos, 1, "'");
}


void KHangManView::slotSetWordsSequence()
{
    kDebug() << "in read kvtml file " << endl;

    if (m_doc != 0)
    {
        delete m_doc;
        m_doc = 0;
    }

    QString myString=QString("khangman/data/%1/%2").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
    myString= KStandardDirs::locate("data", myString);

    m_doc = new KEduVocDocument(this);
    m_doc->open(myString);

    //how many words in the file
    NumberOfWords = m_doc->entryCount() /*verbs.count()*/;

    //get the words+hints
    KRandomSequence randomSequence;
    m_randomList.clear();
    for (int j = 0; j < NumberOfWords; j++) 
        m_randomList.append(qMakePair(m_doc->entry(j)->original(), m_doc->entry(j)->translation(1)));
    //shuffle the list
    randomSequence.randomize(m_randomList);
}

#include "khangmanview.moc"

