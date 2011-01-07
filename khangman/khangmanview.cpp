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

#include <kapplication.h>
#include <kaudioplayer.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <tqimage.h>
#include <tqlabel.h>
#include <tqpainter.h>
#include <tqpoint.h>
#include <tqregexp.h>
#include <tqtimer.h>
#include <tqtooltip.h>
#include <tqvbox.h>
#include <tqwidget.h>

//project headers
#include "prefs.h"
#include "khangman.h"
#include "khangmanview.h"


KHangManView::KHangManView(KHangMan*parent, const char *name)
    : TQWidget(parent, name, WStaticContents | WNoAutoErase)
{
    khangman = parent;
    
    // The widget for entering letters.
    m_letterInput = new KLineEdit( this, "charWrite" );
    m_letterInput->setSizePolicy( TQSizePolicy( (TQSizePolicy::SizeType) 1, 
					       (TQSizePolicy::SizeType) 0, 
					       0, 0, 
					       m_letterInput->sizePolicy().hasHeightForWidth() ) );
    m_letterInput->setMaxLength( 1 );
    m_letterInput->setAlignment( int( TQLineEdit::AlignHCenter ) );

    // Press this button to enter a letter (or press enter)
    m_guessButton = new KPushButton( this, "guessButton" );
    m_guessButton->setSizePolicy( TQSizePolicy( (TQSizePolicy::SizeType) 1, 
					       (TQSizePolicy::SizeType) 0,
					       0, 0, 
					       m_guessButton->sizePolicy().hasHeightForWidth() ) );
    m_guessButton->setAutoMask( true );
    m_guessButton->setFlat( true );
    m_guessButton->setText( i18n( "G&uess" ) );
    
    // Get background from config file - default is sea
    loadAnimation();
    
    setMinimumSize( TQSize( 660, 370 ) );
    setBackground( m_originalBackground );

    // Some misc initializations.
    m_numMissedLetters = 0;
    m_lastWordNumber   = -1;
    m_accentedLetters  = true;
    m_hintExists       = true;	// Assume hint exists

    connect( m_letterInput, TQT_SIGNAL( returnPressed() ),
	     this,          TQT_SLOT( slotTry() ) );
    connect( m_guessButton, TQT_SIGNAL( clicked() ),
	     this,          TQT_SLOT( slotTry() ));
}


KHangManView::~KHangManView()
{
}


// Handle a guess of the letter in sChar.
//

void KHangManView::replaceLetters(const TQString& sChar)
{
    int   index = 0;
    bool  b_end = false;

    kdDebug() << "word " << m_word << endl;

    // Replace letter in the word
    if (Prefs::oneLetter()) {
	// We just replace the next instance.
        for (int count=0; count < m_word.contains(sChar, false); count++) {

            index = m_word.find(sChar, index, false);
            if (m_goodWord.at(2*index)=='_') {
		m_goodWord.replace((2*index), 1, m_word.at(index));

                kdDebug() << "m_goodword " << m_goodWord << endl;
                if (count == m_word.contains(sChar, false)-1)
		    b_end = true;
                break;
            }
	    else
		index++;

	    if (count == m_word.contains(sChar, false)-1)
		b_end = true;
        }
    }
    else {
        for (int count=0; count < m_word.contains(sChar, false); count++) {
            //searching for letter location
            index = m_word.find(sChar, index, false);
            //we replace it...
            m_goodWord.replace((2*index), 1,m_word.at(index));
            index++;
	}
    }

    if (m_accentedLetters && !Prefs::accentedLetters()) {
        if (sChar=="i") replaceLetters(TQString::fromUtf8("í"));
        if (sChar=="a") replaceLetters(TQString::fromUtf8("à"));
        if (sChar=="a") replaceLetters(TQString::fromUtf8("á"));
        if (sChar=="a") replaceLetters(TQString::fromUtf8("ã"));
        if (sChar=="u") replaceLetters(TQString::fromUtf8("ü"));
        if (sChar=="o") replaceLetters(TQString::fromUtf8("ò"));
        if (sChar=="o") replaceLetters(TQString::fromUtf8("ó"));
        if (sChar=="o") replaceLetters(TQString::fromUtf8("õ"));
        if (sChar=="e") replaceLetters(TQString::fromUtf8("è"));
        if (sChar=="e") replaceLetters(TQString::fromUtf8("é"));
        if (sChar=="u") replaceLetters(TQString::fromUtf8("ù"));
    }
    if (!Prefs::oneLetter()) 
        m_guessedLetters << sChar; //appends the list only if not in One Letter only mode...

    if (m_word.contains(sChar, false) == 1) 
        m_guessedLetters << sChar; //append if only one instance

    if (Prefs::oneLetter() && b_end) 
        m_guessedLetters << sChar; 
}


bool KHangManView::containsChar(const TQString &sChar)
{
    bool b = false;
    if (m_accentedLetters && !Prefs::accentedLetters()) {
        if (sChar=="i")
	    b = m_word.contains(TQString::fromUtf8("í"), false);

        if (sChar=="a")
	    b = m_word.contains(TQString::fromUtf8("à"), false)
		|| m_word.contains(TQString::fromUtf8("á"), false)
		|| m_word.contains(TQString::fromUtf8("ã"), false);

        if (sChar=="u")
	    b = m_word.contains(TQString::fromUtf8("ü"), false)
		|| m_word.contains(TQString::fromUtf8("ù"), false);

        if (sChar=="o")
	    b = m_word.contains(TQString::fromUtf8("ò"), false)
		|| m_word.contains(TQString::fromUtf8("ó"), false)
		|| m_word.contains(TQString::fromUtf8("õ"), false);

        if (sChar=="e")
	    b = m_word.contains(TQString::fromUtf8("è"), false)
		|| m_word.contains(TQString::fromUtf8("é"), false);
    }

    return (b || m_word.contains(sChar, false));
}


// ----------------------------------------------------------------
//                           Event handlers


void KHangManView::mousePressEvent(TQMouseEvent *mouse)
{
    if (mouse->button() == RightButton && m_hintExists && Prefs::hint()) {

        KPassivePopup *myPopup = new KPassivePopup( m_letterInput);
        myPopup->setView(i18n("Hint"), m_hint );
        myPopup->setPalette(TQToolTip::palette());
        myPopup->setTimeout(Prefs::hintTimer()*1000); //show for 4 seconds as default
        int x=0, y=0;

        TQPoint abspos = mapToGlobal( TQPoint( 0, 0 ) );
        x = abspos.x() + width()*70/700;
        y = abspos.y() + height()*150/535;

	TQPoint  point = TQPoint(x, y);
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
    setBackground(m_originalBackground); 
    update();
}


// ----------------------------------------------------------------
//                           Painting


void KHangManView::paintEvent( TQPaintEvent * )
{
    // This pixmap implements double buffering to remove all forms of
    // flicker in the repainting.
    TQPixmap   buf(width(), height());

    // Repaint the contents of the khangman view into the pixmap.
    TQPainter  p(&buf);
    p.drawPixmap(0, 0, m_resizedBackground);
    paintHangman(p);
    paintWord(p);
    paintMisses(p);

    // ...and finally, put the pixmap into the widget.
    bitBlt(this, 0, 0, &buf);
}


void KHangManView::paintHangman(TQPainter &p)
{
    // Draw the animated hanged K
    if (Prefs::mode() == 0)  // sea
        p.drawPixmap(TQRect(0, 0, 
			   width()*630/700, height()*285/535),
		     m_animationPics[m_numMissedLetters]);
    else
        p.drawPixmap(TQRect(width()*68/700, height()*170/535, 
			   width()*259/700, height()*228/535), 
		     m_animationPics[m_numMissedLetters]);
}


void KHangManView::paintWord(TQPainter &p)
{
    TQRect  myRect;
    if (Prefs::mode() == 0)   //sea
        myRect = TQRect(0, height()-height()*126/535,
		       width()*417/700, height()*126/535);
    else  
        myRect = TQRect(0, height()-height()*126/535, 
		       width()*327/700, height()*126/535);

    TQFont tFont;
    if (Prefs::mode() == 0)   //sea
        p.setPen( TQColor(148, 156, 167));
    else  
        p.setPen( TQColor(87, 0, 0));

    if (Prefs::selectedLanguage() =="tg")
        tFont.setFamily( "URW Bookman" );
    else
        tFont.setFamily( "Sans Serif" );

    // FIXME: This has to be scaled depending of the dpi
    tFont.setPixelSize( 28 ); 

    p.setFont(tFont);
    p.drawText(myRect, AlignCenter|AlignCenter, m_goodWord);
}


void KHangManView::paintMisses(TQPainter &p)
{
    // Get the color for the letters.
    TQColor  letterColor;
    if (Prefs::mode() == 0)   //sea
	letterColor = TQColor(148, 156, 167);
    else
	letterColor = TQColor(87, 0, 0);

    // Draw the missed letters
    TQFont tFont;
    if (Prefs::selectedLanguage() =="tg")
        tFont.setFamily( "URW Bookman" );
    else
        tFont.setFamily( "Arial" );
    tFont.setPixelSize( 28 );
    p.setPen( letterColor );
    p.setFont(tFont);

    TQFontMetrics  fm(tFont);
    TQRect         fmRect(fm.boundingRect(m_missedLetters));
    TQRect         myRect = TQRect(width() - fmRect.width(), 15, 
				 fmRect.width(), fm.height());
    p.drawText(myRect, AlignLeft, m_missedLetters);

    // Draw the "Misses" word
    TQString  misses = i18n("Misses");
    TQFont  mFont = TQFont(Prefs::missesFontFamily());
    mFont.setPointSize(30);
    p.setFont(mFont);

    TQFontMetrics  fm2(mFont);
    TQRect         fmRect2(fm2.boundingRect(misses));
    TQRect         myRect2(width()- fmRect.width() - fmRect2.width() - 15,
			  15 - fm2.height() + fm.height(),
			  fmRect2.width(), fm2.height());
    p.setPen( letterColor );
    p.drawText(myRect2, AlignLeft|AlignCenter, misses);
}


void KHangManView::resizeEvent(TQResizeEvent *)
{
    if(!m_originalBackground.isNull())
        setBackground(m_originalBackground);
    
    m_letterInput->setMinimumSize( TQSize( height()/18, 0 ) );

    TQFont charWrite_font( m_letterInput->font() );
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Sans Serif" );

    m_letterInput->setFont( charWrite_font ); 
    m_letterInput->setGeometry(width()-2*height()/12, height()-2*height()/16, 
			       height()/10, height()/10);
    m_guessButton->setFont(TQFont(Prefs::guessFontFamily(), height()/22));
    m_guessButton->setGeometry(width() - 2*height()/12 
			       - m_guessButton->width()-5, 
			       height() - 2*height()/16, 
			       m_guessButton->width(), height()/10);
}


// ----------------------------------------------------------------
//                             Slots


void KHangManView::setBackground(TQPixmap& bgPix)
{
    TQImage img = bgPix.convertToImage();
    m_resizedBackground.resize(size());
    m_resizedBackground.convertFromImage(img.smoothScale( width(), height()));
}


void KHangManView::slotTry()
{
    TQString guess = m_letterInput->text();
    kdDebug() << "guess as entered: " << guess << endl;

    // If the char is not a letter, empty the input and return.
    if (!guess.at(0).isLetter()) {
	m_letterInput->setText("");
	return;
    }

    // Handle the guess.
    if (!m_guessedLetters.contains(guess.lower())) {
	// The letter is not already guessed.
	
	if (containsChar(guess)) {
	    replaceLetters(guess);

	    // This is needed because of the white spaces.
	    TQString  stripWord = m_goodWord;
	    TQString  sword     = m_word;
	    if (dd > 0)  {
		stripWord.replace(2*c,   1, "");
		stripWord.replace(2*c-1, 1, "");

		stripWord.replace(2*(dd-1),   1, "");
		stripWord.replace(2*(dd-1)-1, 1, "");
	    }

	    TQStringList  rightChars = TQStringList::split(" ", stripWord, true);
	    TQString      rightWord  = rightChars.join("");
	    update();
	    sword.remove(TQRegExp(" "));

	    // If the user made it...
	    if (rightWord.stripWhiteSpace().lower() 
		== sword.stripWhiteSpace().lower()) { 

		// We reset everything...
		// pixImage->setPixmap(m_animationPics[10]);
		//TODO find a better way to finish
		//
		if (Prefs::sound()) {
		    TQString soundFile = locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
		    if (soundFile != 0) 
			KAudioPlayer::play(soundFile);
		}

		if (Prefs::wonDialog()) {
		    // TODO: hide Hint KPassivePopup if any
		    TQPoint point;
		    KPassivePopup *popup = new KPassivePopup( this, "popup" );
		    popup->setAutoDelete( true );
		    popup->setTimeout( 4*1000 );
		    popup->setView(i18n("Congratulations,\nyou won!") );

		    int x =0, y = 0;
		    TQPoint abspos = popup->pos();
		    x = abspos.x() + width()*50/700;
		    y = abspos.y() + height()*20/535;
		    point = TQPoint(x, y);
		    popup->show(mapToGlobal(point));
		    TQTimer::singleShot( 4*1000, this, TQT_SLOT(slotNewGame()) );
		}
		else if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?"),TQString::null,i18n("Play Again"), i18n("Do Not Play")) == 3)
		    slotNewGame();
		else
		    kapp->quit();
	    }
	}
	else {
	    // The char is missed.

	    m_guessedLetters << guess.lower();	
            m_guessedLetters << guess.upper();
	    m_missedLetters = m_missedLetters.replace((2 * m_numMissedLetters), 
						      1, guess.lower());
	    m_numMissedLetters++;
	    update();

	    // Check if we have reached the limit of wrong guesses.
	    if (m_numMissedLetters >= MAXWRONGGUESSES) {

		//TODO sequence to finish when hanged
		TQStringList charList=TQStringList::split("", m_word);
		TQString theWord=charList.join(" ");
		m_goodWord = theWord;
		//usability: find another way to start a new game
		TQString newGameString = i18n("You lost. Do you want to play again?");
		if (Prefs::wonDialog()) {
		    // TODO: hide Hint KPassivePopup if any
		    TQPoint point;
		    KPassivePopup *popup = new KPassivePopup(KPassivePopup::Boxed,  this, "popup" );
		    popup->setAutoDelete( true );
		    popup->setTimeout( 4*1000 );

		    TQVBox *vb = new TQVBox( popup );

		    TQLabel *popLabel = new TQLabel( vb);
		    popLabel->setFont(TQFont("Sans Serif", 14, TQFont::Normal));
		    popLabel->setText(i18n("<qt>You lost!\nThe word was\n<b>%1</b></qt>").arg(m_word));
		    popup->setView( vb );

		    TQPoint abspos = popup->pos();
		    int  x = abspos.x() + width()  * 50 / 700;
		    int  y = abspos.y() + height() * 20 / 535;
		    popup->show(mapToGlobal(TQPoint(x, y)));

		    TQTimer::singleShot( 4 * 1000, this, TQT_SLOT(slotNewGame()) );
		}
		else if (KMessageBox::questionYesNo(this, newGameString, TQString::null, i18n("Play Again"), i18n("Do Not Play")) == 3)
		    slotNewGame();
		else
		    kapp->quit();
	    }
	}
    }
    else {
	// The letter is already guessed.

	// Show a popup that says as much.
	TQPoint point;
	KPassivePopup *popup = new KPassivePopup(KPassivePopup::Balloon, this, "popup" );
	popup->setAutoDelete( true );
	popup->setTimeout( 1000 );
	popup->setView(i18n("This letter has already been guessed.") );

	int  x = 0;
	int  y = 0;
	if (m_missedLetters.contains(guess, false) > 0) {
	    // FIXME: popup should be better placed.

	    TQPoint abspos = popup->pos();
	    x = abspos.x() + width()*400/700;
	    y = abspos.y() + height()*50/535;
	    point = TQPoint(x, y);

	    // Create a 1 second single-shot timer, and reenable user
	    // input after this time.
	    TQTimer::singleShot( Prefs::missedTimer() * 1000, 
				this, TQT_SLOT(enableUserInput()) );

	    // Disable any possible entry
	    m_letterInput->setEnabled(false);
	}

	if (m_goodWord.contains(guess, false) > 0) {
	    TQPoint abspos = popup->pos();

	    if (Prefs::mode() == 0) {
		// sea
		x = abspos.x() + width()*250/700;
		y = abspos.y() + height()*485/535;
	    }
	    else {
		x = abspos.x() + width()*200/700;
		y = abspos.y() + height()*485/535;
	    }
	    point = TQPoint(x, y);

	    TQTimer::singleShot( Prefs::missedTimer() * 1000,
				this, TQT_SLOT(enableUserInput()) );

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
	TQString soundFile = locate("data", "khangman/sounds/new_game.ogg");
	if (soundFile != 0) 
	    KAudioPlayer::play(soundFile);
    }

    reset();
    game();
    m_letterInput->setFocus();
}


void KHangManView::reset()
{
    m_goodWord = "";
    m_word     = "";

    m_guessedLetters.clear();
    m_numMissedLetters = 0;
    m_missedLetters    = "_ _ _ _ _ _ _ _ _ _  ";

    // Clear the input field.
    m_letterInput->setText("");

    update();
}


void KHangManView::game()
{
    kdDebug() << "language " << Prefs::selectedLanguage() << endl;
    kdDebug() << "level "    << Prefs::levelFile()        << endl;

    // Check if the data files are installed in the correct dir.
    TQString  myString = TQString("khangman/data/%1/%2")
                          .arg(Prefs::selectedLanguage())
                          .arg(Prefs::levelFile());
    TQFile    myFile;
    myFile.setName(locate("data", myString));
    if (!myFile.exists()) {
	TQString  mString = i18n("File $KDEDIR/share/apps/khangman/data/%1/%2 not found!\n"
				"Check your installation, please!")
	             .arg(Prefs::selectedLanguage())
	             .arg(Prefs::levelFile());
	KMessageBox::sorry( this, mString, i18n("Error") );
	kapp->quit();
    }

    // We open the file and store info into the stream...
    TQFile  openFileStream(myFile.name());
    openFileStream.open(IO_ReadOnly);
    TQTextStream readFileStr(&openFileStream);
    readFileStr.setEncoding(TQTextStream::UnicodeUTF8);

    // Alldata contains all the words from the file
    TQStringList allData = TQStringList::split("\n", readFileStr.read(), true);
    openFileStream.close();

    // Detects if file is a kvtml file so that it's a hint enable file
    if (allData.first() == "<?xml version=\"1.0\"?>") {
        readFile();
    }
    else {
	//TODO abort if not a kvtml file maybe
        kdDebug() << "Not a kvtml file!" << endl;
    }
    kdDebug() << m_word << endl;

    // Display the number of letters to guess with _
    for (unsigned int i = 0; i < m_word.length(); i++)
        m_goodWord.append("_ ");

    // Remove the last trailing space.
    m_goodWord.remove(m_goodWord.length()-1);
    
    kdDebug() << m_goodWord << endl;

    // If needed, display white space or - if in word or semi dot.

    // 1. Find dashes.
    int f = m_word.find( "-" );
    if (f>0) {
        m_goodWord.replace(2*f, 1, "-");

        int g = m_word.find( "-", f+1);
        if (g>0) 
	    m_goodWord.replace(2*g, 3, "-");
        if (g>1)
	    m_goodWord.append("_");
    }

    // 2. Find white space.
    c = m_word.find( " " );
    if (c > 0) {
        m_goodWord.replace(2*c, 1, " ");
        dd = m_word.find( " ", c+1);
        if (dd > 0)
	    m_goodWord.replace(2*dd, c+1, " ");
    }

    // 3. Find ·
    int e = m_word.find( TQString::fromUtf8("·") );
    if (e>0)
	m_goodWord.replace(2*e, 1, TQString::fromUtf8("·") );

    // 4. Find '
    int h = m_word.find( "'" );
    if (h>0)
	m_goodWord.replace(2*h, 1, "'");
}


void KHangManView::readFile()
{
    kdDebug() << "in read kvtml file " << endl;
    TQString myString=TQString("khangman/data/%1/%2").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
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
    m_hint = verbs[wordNumber].translatedText(); 

    if (m_word.isEmpty()) 
        readFile();

    if (m_hint.isEmpty()) {
        m_hintExists = false;	// Hint does not exist.

	// FIXME: Make this a signal instead.
        khangman->changeStatusbar("", 103);
    }
    else {
        m_hintExists = true;
        khangman->setMessages();
    }
}


void KHangManView::loadAnimation()
{
    switch (Prefs::mode())  {
        case Prefs::EnumMode::sea:
            m_originalBackground = TQPixmap(locate("data", "khangman/pics/sea/sea_theme.png") );
            m_themeName = "sea";
            m_letterInput->setPaletteForegroundColor( TQColor(  83,  40,  14) );
            m_guessButton->setPaletteBackgroundColor( TQColor( 115,  64,  49) );
            m_guessButton->setPaletteForegroundColor( TQColor( 148, 156, 167) );
            m_guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;

        case Prefs::EnumMode::desert:
            m_originalBackground = TQPixmap(locate("data","khangman/pics/desert/desert_theme.png") );
            m_themeName = "desert";
            m_letterInput->setPaletteForegroundColor( TQColor(  87,   0,  0) );
            m_guessButton->setPaletteBackgroundColor( TQColor( 205, 214, 90) );
            m_guessButton->setPaletteForegroundColor( TQColor(  87,   0,  0) );
            m_guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;
    }	

    // Now we load the pixmaps...
    for (uint i = 0; i < 11; i++) {
	m_animationPics[i].load(locate( "data", 
		 	   TQString("khangman/pics/%1/animation%2.png")
			   .arg(m_themeName).arg(i) ));
    }
}

#include "khangmanview.moc"

