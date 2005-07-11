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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <kapplication.h>
#include <kaudioplayer.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <qimage.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qwidget.h>

//project headers
#include "prefs.h"
#include "khangman.h"
#include "khangmanview.h"


KHangManView::KHangManView(KHangMan*parent, const char *name)
    : QWidget(parent, name, WStaticContents | WNoAutoErase)
{
    khangman = parent;
    
    // The widget for entering letters.
    m_letterInput = new KLineEdit( this, "charWrite" );
    m_letterInput->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType) 1, 
					       (QSizePolicy::SizeType) 0, 
					       0, 0, 
					       m_letterInput->sizePolicy().hasHeightForWidth() ) );
    m_letterInput->setMaxLength( 1 );
    m_letterInput->setAlignment( int( QLineEdit::AlignHCenter ) );

    // Press this button to enter a letter (or press enter)
    m_guessButton = new KPushButton( this, "guessButton" );
    m_guessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType) 1, 
					       (QSizePolicy::SizeType) 0,
					       0, 0, 
					       m_guessButton->sizePolicy().hasHeightForWidth() ) );
    m_guessButton->setAutoMask( TRUE );
    m_guessButton->setFlat( TRUE );
    m_guessButton->setText( i18n( "G&uess" ) );
    
    // Get background from config file - default is sea
    loadAnimation();
    
    setMinimumSize( QSize( 700, 535) );
    slotSetPixmap( m_originalBackground);

    // Some misc initializations.
    missedChar=0;
    m_lastWordNumber = -1;
    m_accent = true;
    hintBool = true;//assume tip exists

    connect( m_letterInput, SIGNAL( returnPressed() ),
	     this,          SLOT( slotTry() ) );
    connect( m_guessButton, SIGNAL( clicked() ),
	     this,          SLOT( slotTry() ));
}


KHangManView::~KHangManView()
{
}


// Handle a guess of the letter in sChar.
//

void KHangManView::replaceLetters(const QString& sChar)
{
    int   index = 0;
    bool  b_end = false;

    kdDebug() << "word " << m_word << endl;

    // Replace letter in the word
    if (Prefs::oneLetter()) {
	// We just replace the next instance.
        for (int count=0; count < m_word.contains(sChar); count++) {

            index = m_word.find(sChar, index);
            if (goodWord.at(2*index)=='_') {
                goodWord.replace((2*index), 1, sChar);

                kdDebug() << "goodword " << goodWord << endl;
                if (count == m_word.contains(sChar)-1)
		    b_end = true;
                break;
            }
	    else
		index++;

	    if (count == m_word.contains(sChar)-1)
		b_end = true;
        }
    }
    else {
        for (int count=0; count < m_word.contains(sChar); count++) {
            //searching for letter location
            index = m_word.find(sChar, index);

            //we replace it...
            goodWord.replace((2*index), 1,sChar);
            index++;
	}
    }

    if (m_accent && !Prefs::accentedLetters()) {
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

    if (m_word.contains(sChar)==1) 
        m_guessedLetters << sChar; //append if only one instance

    if (Prefs::oneLetter() && b_end) 
        m_guessedLetters << sChar; 
}


bool KHangManView::containsChar(const QString &sChar)
{
    bool b = false;
    if (m_accent && !Prefs::accentedLetters()) {
        if (sChar=="i")
	    b = m_word.contains(QString::fromUtf8("í"));

        if (sChar=="a")
	    b = m_word.contains(QString::fromUtf8("à"))
		|| m_word.contains(QString::fromUtf8("á"))
		|| m_word.contains(QString::fromUtf8("ã"));

        if (sChar=="u")
	    b = m_word.contains(QString::fromUtf8("ü"))
		|| m_word.contains(QString::fromUtf8("ù"));

        if (sChar=="o")
	    b = m_word.contains(QString::fromUtf8("ò"))
		|| m_word.contains(QString::fromUtf8("ó"))
		|| m_word.contains(QString::fromUtf8("õ"));

        if (sChar=="e")
	    b = m_word.contains(QString::fromUtf8("è"))
		|| m_word.contains(QString::fromUtf8("é"));
    }

    return (b || m_word.contains(sChar));
}


// ----------------------------------------------------------------
//                           Event handlers


void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
    if (mouse->button() == RightButton && hintBool && Prefs::hint()) {

        KPassivePopup *myPopup = new KPassivePopup( m_letterInput);
        myPopup->setView(i18n("Hint"), tip );
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


void KHangManView::setTheme()
{
    loadAnimation();
    slotSetPixmap(m_originalBackground); 
    update();
}


// ----------------------------------------------------------------
//                           Painting


void KHangManView::paintEvent( QPaintEvent * )
{
    // This pixmap implements double buffering to remove all forms of
    // flicker in the repainting.
    QPixmap   buf(width(), height());

    // Repaint the contents of the khangman view into the pixmap.
    QPainter  p(&buf);
    p.drawPixmap(0, 0, m_resizedBackground);
    paintHangman(p);
    paintWord(p);
    paintMisses(p);

    // ...and finally, put the pixmap into the widget.
    bitBlt(this, 0, 0, &buf);
}


void KHangManView::paintHangman(QPainter &p)
{
    // Draw the animated hanged K
    if (Prefs::mode() == 0)  // sea
        p.drawPixmap(QRect(0, 0, 
			   width()*630/700, height()*285/535),
		     m_animationPics[missedChar]);
    else
        p.drawPixmap(QRect(width()*68/700, height()*170/535, 
			   width()*259/700, height()*228/535), 
		     m_animationPics[missedChar]);
}


void KHangManView::paintWord(QPainter &p)
{
    QRect  myRect;
    if (Prefs::mode() == 0)   //sea
        myRect = QRect(0, height()-height()*126/535,
		       width()*417/700, height()*126/535);
    else  
        myRect = QRect(0, height()-height()*126/535, 
		       width()*327/700, height()*126/535);

    QFont tFont;
    if (Prefs::mode() == 0)   //sea
        p.setPen( QColor(148, 156, 167));
    else  
        p.setPen( QColor(87, 0, 0));

    if (Prefs::selectedLanguage() =="tg")
        tFont.setFamily( "URW Bookman" );
    else
        tFont.setFamily( "Sans Serif" );

    // FIXME: This has to be scaled depending of the dpi
    tFont.setPixelSize( 28 ); 

    p.setFont(tFont);
    p.drawText(myRect, AlignCenter|AlignCenter, goodWord);
}


void KHangManView::paintMisses(QPainter &p)
{
    // Get the color for the letters.
    QColor  letterColor;
    if (Prefs::mode() == 0)   //sea
	letterColor = QColor(148, 156, 167);
    else
	letterColor = QColor(87, 0, 0);

    // Draw the missed letters
    QFont tFont;
    if (Prefs::selectedLanguage() =="tg")
        tFont.setFamily( "URW Bookman" );
    else
        tFont.setFamily( "Sans Serif" );
    tFont.setPixelSize( 28 );
    p.setPen( letterColor );
    p.setFont(tFont);

    QFontMetrics  fm(tFont);
    QRect         fmRect(fm.boundingRect(missedL));
    QRect         myRect = QRect(width() - fmRect.width(), 15, 
				 fmRect.width(), fm.height());
    p.drawText(myRect, AlignLeft, missedL);

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
    p.drawText(myRect2, AlignLeft|AlignCenter, misses);
}


void KHangManView::resizeEvent(QResizeEvent *)
{
    if(!m_originalBackground.isNull())
        slotSetPixmap(m_originalBackground);
    
    m_letterInput->setMinimumSize( QSize( height()/18, 0 ) );

    QFont charWrite_font( m_letterInput->font() );
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Sans Serif" );

    m_letterInput->setFont( charWrite_font ); 
    m_letterInput->setGeometry(width()-2*height()/12, height()-2*height()/16, 
			       height()/10, height()/10);
    m_guessButton->setFont(QFont("Dustimo Roman", height()/22));
    m_guessButton->setGeometry(width() - 2*height()/12 
			       - m_guessButton->width()-5, 
			       height() - 2*height()/16, 
			       m_guessButton->width(), height()/10);
}


// ----------------------------------------------------------------
//                             Slots


void KHangManView::slotSetPixmap(QPixmap& bgPix)
{
    QImage img = bgPix.convertToImage();
    m_resizedBackground.resize(size());
    m_resizedBackground.convertFromImage(img.smoothScale( width(), height()));
}


void KHangManView::slotTry()
{
    QString guess = m_letterInput->text();
    kdDebug() << "guess as entered: " << guess << endl;

    // If German, make upper case, otherwise make lower case.
    if (Prefs::upperCase() && Prefs::selectedLanguage() =="de")
        guess = guess.upper();
    else
        guess = guess.lower();

    // If the char is not a letter, empty the input and return.
    if (!guess.at(0).isLetter()) {
	m_letterInput->setText("");
	return;
    }

    // Handle the guess.
    if (!m_guessedLetters.contains(guess)) {
	// The letter is not already guessed.
	
	if (containsChar(guess)) {
	    replaceLetters(guess);
	    stripWord = goodWord;//need that because of the white spaces
	    sword = m_word;
	    if (d>0)  {
		stripWord.replace(2*c, 1, "");
		stripWord.replace(2*c-1, 1, "");
	    }
	    if (d>0)  {
		stripWord.replace(2*(d-1), 1, "");
		stripWord.replace(2*(d-1)-1, 1, "");
	    }

	    QStringList rightChars=QStringList::split(" ", stripWord, true);
	    QString rightWord= rightChars.join("");
	    update();
	    sword.remove(QRegExp(" "));

	    // If the user made it...
	    if (rightWord.stripWhiteSpace().lower() 
		== sword.stripWhiteSpace().lower()) { 

		// We reset everything...
		// pixImage->setPixmap(m_animationPics[10]);
		//TODO find a better way to finish
		//
		if (Prefs::sound()) {
		    QString soundFile = locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
		    if (soundFile != 0) 
			KAudioPlayer::play(soundFile);
		}

		if (Prefs::wonDialog()) {
		    // TODO: hide Hint KPassivePopup if any
		    QPoint point;
		    KPassivePopup *popup = new KPassivePopup( this, "popup" );
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
		else if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?")) == 3)
		    slotNewGame();
		else
		    kapp->quit();
	    }
	}
	else {
	    // The char is missed.

	    m_guessedLetters << guess;	
	    missedL = missedL.replace((2*missedChar), 1, guess);

	    missedChar++;
	    update();

	    // Check if we have reached the limit of wrong guesses.
	    if (missedChar >= MAXWRONGGUESSES) {
		//TODO sequence to finish when hanged
		QStringList charList=QStringList::split("", m_word);
		QString theWord=charList.join(" ");
		goodWord = theWord;
		//usability: find another way to start a new game
		QString newGameString = i18n("You lost. Do you want to play again?");
		if (Prefs::wonDialog()) {
		    // TODO: hide Hint KPassivePopup if any
		    QPoint point;
		    KPassivePopup *popup = new KPassivePopup( this, "popup" );
		    popup->setAutoDelete( true );
		    popup->setTimeout( 4*1000 );

		    QVBox *vb = new QVBox( popup );
		    QLabel *popLabel = new QLabel( vb);
		    popLabel->setFont(QFont("Sans Serif", 14, QFont::Normal));
		    popLabel->setText(i18n("<qt>You lost!\nThe word was\n<b>%1</b></qt>").arg(sword));
		    popup->setView( vb );

		    int x =0, y = 0;
		    QPoint abspos = popup->pos();
		    x = abspos.x() + width()*50/700;
		    y = abspos.y() + height()*20/535;
		    point = QPoint(x, y);
		    popup->show(mapToGlobal(point));
		    QTimer::singleShot( 4*1000, this, SLOT(slotNewGame()) );
		}
		else if (KMessageBox::questionYesNo(this, newGameString) == 3)
		    slotNewGame();
		else
		    kapp->quit();
	    }
	}
    }
    else {
	// The letter is already guessed.

	// Show a popup that says as much.
	QPoint point;
	KPassivePopup *popup = new KPassivePopup( KPassivePopup::Balloon, 
						  this, "popup" );
	popup->setAutoDelete( true );
	popup->setTimeout( 1000 );
	popup->setView(i18n("This letter has already been guessed.") );

	int x =0, y = 0;
	if (missedL.contains(guess)>0) { //TODO popup should be better placed
	    QPoint abspos = popup->pos();
	    x = abspos.x() + width()*400/700;
	    y = abspos.y() + height()*50/535;
	    point = QPoint(x, y);
	    QTimer *timer = new QTimer( this);
	    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
	    timer->start( Prefs::missedTimer()*1000, TRUE ); // 1 second single-shot timer
	    //disable any possible entry
	    m_letterInput->setEnabled(false);
	}

	if (goodWord.contains(guess) > 0) {
	    QPoint abspos = popup->pos();

	    if (Prefs::mode() == 0) {
		// sea
		x = abspos.x() + width()*250/700;
		y = abspos.y() + height()*485/535;
	    }
	    else {
		x = abspos.x() + width()*200/700;
		y = abspos.y() + height()*485/535;
	    }
	    point = QPoint(x, y);

	    QTimer *timer = new QTimer( this);
	    connect( timer, SIGNAL(timeout()), this, SLOT(timerWordDone()) );
	    timer->start( Prefs::missedTimer()*1000, TRUE ); // 1 second single-shot timer
	    //disable any possible entry
	    m_letterInput->setEnabled(false);	
	}

	popup->show(mapToGlobal(point));
    }

    // Reset the entry field after guess.
    m_letterInput->setText("");
}


void KHangManView::timerDone()
{
    m_letterInput->setEnabled(true);
    m_letterInput->setFocus();
}

void KHangManView::timerWordDone()
{
    m_letterInput->setEnabled(true);
    m_letterInput->setFocus();
}

void KHangManView::slotNewGame()
{
    if (Prefs::sound()) {
	QString soundFile = locate("data", "khangman/sounds/new_game.ogg");
	if (soundFile != 0) 
	    KAudioPlayer::play(soundFile);
    }
    reset();

    game();
    m_letterInput->setFocus();
}


void KHangManView::reset()
{
    goodWord = "";
    m_word   = "";

    missedChar = 0;
    missedL    = "_ _ _ _ _ _ _ _ _ _  ";
    m_guessedLetters.clear();

    // Clear the input field.
    m_letterInput->setText("");

    update();
}

void KHangManView::game()
{
    //if the data files are not installed in the correct dir
    QString myString=QString("khangman/data/%1/%2").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
    kdDebug() << "language " << Prefs::selectedLanguage() << endl;
    kdDebug() << "level " << Prefs::levelFile() << endl;
    QFile myFile;
    myFile.setName(locate("data", myString));
    if (!myFile.exists())
    {
            QString mString=i18n("File $KDEDIR/share/apps/khangman/data/%1/%2 not found!\n"
                                    "Check your installation, please!").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
            KMessageBox::sorry( this, mString,
                                i18n("Error") );
            kapp->quit();
    }
    //update();
    //we open the file and store info into the stream...
    QFile openFileStream(myFile.name());
    openFileStream.open(IO_ReadOnly);
    QTextStream readFileStr(&openFileStream);
    readFileStr.setEncoding(QTextStream::UnicodeUTF8);
    //allData contains all the words from the file
    QStringList allData = QStringList::split("\n", readFileStr.read(), true);
    openFileStream.close();
    //detects if file is a kvtml file so that it's a hint enable file
    if (allData.first() == "<?xml version=\"1.0\"?>") {
        readFile();
    }
    else {//TODO abort if not a kvtml file maybe
        kdDebug() << "Not a kvtml file!" << endl;
    }
    kdDebug() << m_word << endl;
    //display the number of letters to guess with _
    for(unsigned int i = 0; i < m_word.length(); i++)
    {
        goodWord.append("_");
        goodWord.append(" ");
    }        
    goodWord.remove(goodWord.length()-1);
    
    kdDebug() << goodWord << endl;
    stripWord=goodWord;
    //if needed, display white space or - if in word or semi dot
    f = m_word.find( "-" );
    if (f>0)
    {
        g=0;
        goodWord.replace(2*f, 1, "-");
        g = m_word.find( "-", f+1);
        if (g>0) 
                goodWord.replace(2*g, 3, "-");
        if (g>1)
                goodWord.append("_");
    }
    c = m_word.find( " " );
    if (c>0) //find another white space
    {
        d=0;
        goodWord.replace(2*c, 1, " ");
        d = m_word.find( " ", c+1);
        if (d>0)  goodWord.replace(2*d, c+1, " ");
    }
    int e = m_word.find( QString::fromUtf8("·") );
    if (e>0) goodWord.replace(2*e, 1, QString::fromUtf8("·") );
    int h = m_word.find( "'" );
    if (h>0) goodWord.replace(2*h, 1, "'");
}

void KHangManView::readFile()
{
    kdDebug() << "in read kvtml file " << endl;
    QString myString=QString("khangman/data/%1/%2").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
    myString= locate("data", myString);
    KEduVocDataItemList verbs = KEduVocData::parse(myString);

    //how many words in the file
    int NumberOfWords = verbs.count();

    //pick a number in random

    // Make sure the same word is not chosen twice in a row.
    int  wordNumber = m_random.getLong(NumberOfWords);
    if (m_lastWordNumber != -1) {
        while (wordNumber == m_lastWordNumber)
	    wordNumber = m_random.getLong(NumberOfWords);
    }
    m_lastWordNumber = wordNumber;

    m_word = verbs[wordNumber].originalText();
    tip  = verbs[wordNumber].translatedText(); 

    if (m_word.isEmpty()) 
        readFile();

    if (tip.isEmpty()) {
        Prefs::setHint(false);//hint can't be enabled
        Prefs::writeConfig();
        hintBool = false;//hint does not exist

	// FIXME: Make this a signal instead.
        khangman->changeStatusbar("", 103);
    }
    else {
        hintBool = true;
        khangman->setMessages();
    }

    if (Prefs::upperCase() && Prefs::selectedLanguage() =="de")
    {
        m_word = m_word.upper();// only for German currently

        // Replace ß with SS in German 
        if (m_word.contains(QString::fromUtf8("ß"))) {
            int index = m_word.find(QString::fromUtf8("ß"),0);
            m_word.replace(index,1, "S");
            //TODO add a S here
        }
    }
}


void KHangManView::loadAnimation()
{
    switch (Prefs::mode())  {
        case Prefs::EnumMode::sea:
            m_originalBackground = QPixmap(locate("data", "khangman/pics/sea/sea_theme.png") );
            m_themeName = "sea";
            m_letterInput->setPaletteForegroundColor( QColor(  83,  40,  14) );
            m_guessButton->setPaletteBackgroundColor( QColor( 115,  64,  49) );
            m_guessButton->setPaletteForegroundColor( QColor( 148, 156, 167) );
            m_guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;

        case Prefs::EnumMode::desert:
            m_originalBackground = QPixmap(locate("data","khangman/pics/desert/desert_theme.png") );
            m_themeName = "desert";
            m_letterInput->setPaletteForegroundColor( QColor(  87,   0,  0) );
            m_guessButton->setPaletteBackgroundColor( QColor( 205, 214, 90) );
            m_guessButton->setPaletteForegroundColor( QColor(  87,   0,  0) );
            m_guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;
    }	

    // Now we load the pixmaps...
    for (uint i = 0; i < 11; i++) {
	m_animationPics[i].load(locate( "data", 
		 	   QString("khangman/pics/%1/animation%2.png")
			   .arg(m_themeName).arg(i) ));
    }
}

#include "khangmanview.moc"

