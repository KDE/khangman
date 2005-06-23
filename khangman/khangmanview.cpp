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
#include <qpainter.h>
#include <qpoint.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtooltip.h>
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
    charWrite = new KLineEdit( this, "charWrite" );
    charWrite->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, charWrite->sizePolicy().hasHeightForWidth() ) );
    charWrite->setMaxLength( 1 );
    charWrite->setAlignment( int( QLineEdit::AlignHCenter ) );

    // Press this button to enter a letter (or press enter)
    guessButton = new KPushButton( this, "guessButton" );
    guessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, guessButton->sizePolicy().hasHeightForWidth() ) );
    guessButton->setAutoMask( TRUE );
    guessButton->setFlat( TRUE );
    guessButton->setText( i18n( "G&uess" ) );
    
    // Get background from config file - default is sea
    loadAnimation();
    
    setMinimumSize( QSize( 700, 535) );
    slotSetPixmap( bcgdPicture);

    // Some misc initializations.
    missedChar=0;
    m_lastWordNumber = -1;
    m_accent = true;
    hintBool = true;//assume tip exists

    connect( charWrite, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
    connect( guessButton, SIGNAL( clicked() ), this, SLOT( slotTry() ));
}


KHangManView::~KHangManView()
{
}


// Handle a guess of the letter in sChar.
//

void KHangManView::replaceLetters(const QString& sChar)
{
    //replace letter in the word
    int index=0;
    bool b_end = false;
    kdDebug() << "word " << word << endl;

    if (Prefs::oneLetter()) {
	//we just replace the next instance
        for (int count=0; count <word.contains(sChar); count++) {
            index = word.find(sChar,index);
            if (goodWord.at(2*index)=='_') {
                goodWord.replace((2*index), 1,sChar);
                kdDebug() << "goodword " << goodWord << endl;
                if (count == word.contains(sChar)-1)
                        b_end = true;
                break;
            }
        else index ++;
        if (count == word.contains(sChar)-1)
            b_end = true;
        }
    }
    else {
        for (int count=0; count <word.contains(sChar); count++) {
            //searching for letter location
            index = word.find(sChar,index);
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
    if (word.contains(sChar)==1) 
        m_guessedLetters << sChar; //append if only one instance
    if (Prefs::oneLetter() && b_end) 
        m_guessedLetters << sChar; 
}


bool KHangManView::containsChar(const QString &sChar)
{
    bool b = false;
    if (m_accent && !Prefs::accentedLetters()) {
        if (sChar=="i") b = word.contains(QString::fromUtf8("í"));
        if (sChar=="a")	b = word.contains(QString::fromUtf8("à")) || word.contains(QString::fromUtf8("á")) || word.contains(QString::fromUtf8("ã"));
        if (sChar=="u") b = word.contains(QString::fromUtf8("ü")) || word.contains(QString::fromUtf8("ù"));
        if (sChar=="o") b = word.contains(QString::fromUtf8("ò")) || word.contains(QString::fromUtf8("ó")) || word.contains(QString::fromUtf8("õ"));
        if (sChar=="e") b = word.contains(QString::fromUtf8("è")) || word.contains(QString::fromUtf8("é"));
    }
    return (b || word.contains(sChar));
}

void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
    if (mouse->button() == RightButton && hintBool && Prefs::hint())
    {
        KPassivePopup *myPopup = new KPassivePopup( charWrite);
        myPopup->setView(i18n("Hint"), tip );
        myPopup->setPalette(QToolTip::palette());
        myPopup->setTimeout(4000); //show for 4 seconds
        int x=0, y=0;
        QPoint abspos = mapToGlobal( QPoint( 0, 0 ) );
        if (Prefs::mode() ==0)  {
            x = abspos.x() + width()*300/700;
            y = abspos.y() + height()*510/535;
        }
        else  {
            x = abspos.x() + width()*254/700;
            y = abspos.y() + height()*405/535;
        }
        myPopup->move(x, y);
        myPopup->show();
        //maybe it's better to popup where the mouse clicks, in that case kill the popup before new click
        //myPopup->move(mouse->pos());
    }
    //update();
}

void KHangManView::setTheme()
{
    loadAnimation();
    slotSetPixmap(bcgdPicture); 
    update();
}


// ----------------------------------------------------------------
//                           Painting


void KHangManView::paintEvent( QPaintEvent * )
{
    QPixmap buf(width(), height());
    QPainter p(&buf);
    p.drawPixmap(0, 0, bg);
    paintHangman(p);
    paintWord(p);
    paintMisses(p);
    bitBlt(this, 0, 0, &buf);
}

void KHangManView::paintHangman(QPainter &p)
{
    //draw the animated hanged K
    if (Prefs::mode() ==0) 
        p.drawPixmap(QRect(0,0, width()*630/700, height()*285/535), px[missedChar]);
    else
        p.drawPixmap(QRect(width()*68/700, height()*170/535, width()*259/700, height()*228/535), px[missedChar]);
}

void KHangManView::paintWord(QPainter &p)
{
    QRect myRect;
    if (Prefs::mode() ==0)   //sea
        myRect = QRect(0, height()-height()*126/535, width()*417/700, height()*126/535);
    else  
        myRect = QRect(0, height()-height()*126/535, width()*327/700, height()*126/535);
    QFont tFont;
    if (Prefs::mode() ==0)   //sea
        p.setPen( QColor(148, 156, 167));
    else  
        p.setPen( QColor(87, 0, 0));
    if (Prefs::selectedLanguage() =="tg")  {
        tFont.setFamily( "URW Bookman" );
    }
    else
        tFont.setFamily( "Arial" );
    tFont.setPixelSize( 28 ); //this has to be scaled depending of the dpi
    p.setFont(tFont);
    p.drawText(myRect, AlignCenter|AlignCenter, goodWord);
}


void KHangManView::paintMisses(QPainter &pi)
{
    //draw the Missed letters
    if (Prefs::mode() ==0)   //sea
        pi.setPen( QColor(148, 156, 167));
    else
        pi.setPen( QColor(87, 0, 0));
    QFont tFont;
    if (Prefs::selectedLanguage() =="tg")  {
        tFont.setFamily( "URW Bookman" );
    }
    else
        tFont.setFamily( "Arial" );
    tFont.setPixelSize( 28 );
    QFontMetrics fm(tFont);
    QRect fmRect(fm.boundingRect(missedL));
    QRect myRect = QRect(width() - fmRect.width(), 15, fmRect.width(), fm.height());
    pi.setFont(tFont);
    pi.drawText(myRect, AlignLeft, missedL);

    // draw the Misses word
    QString misses = i18n("Misses");
    QFont f = QFont("Domestic Manners");
    f.setPointSize(30);

    QFontMetrics fm2(f);
    QRect fmRect2(fm2.boundingRect(misses));

    QRect myRect2(width()- fmRect.width() - fmRect2.width() - 15, 15 - fm2.height() + fm.height(), fmRect2.width(), fm2.height());
    pi.setFont(f);
    if (Prefs::mode() ==0)//sea
        pi.setPen( QColor(148, 156, 167));
    else
        pi.setPen( QColor(87, 0, 0));
    pi.drawText(myRect2, AlignLeft|AlignCenter, misses);
}


void KHangManView::resizeEvent(QResizeEvent *)
{
    if(!bcgdPicture.isNull())
        slotSetPixmap(bcgdPicture);
    
    charWrite->setMinimumSize( QSize( height()/18, 0 ) );
    QFont charWrite_font(  charWrite->font() );
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Arial" );
    charWrite->setFont( charWrite_font ); 
    guessButton->setFont(QFont("Dustimo Roman", height()/22));
    charWrite->setGeometry(width()-2*height()/12, height()-2*height()/16, 
			   height()/10, height()/10);
    guessButton->setGeometry(width() -2*height()/12-guessButton->width()-5, 
			     height()-2*height()/16, 
			     guessButton->width(), height()/10);
}


// ----------------------------------------------------------------
//                             Slots


void KHangManView::slotSetPixmap(QPixmap& bgPix)
{
    QImage img = bgPix.convertToImage();
    bg.resize(size());
    bg.convertFromImage(img.smoothScale( width(), height()));
}


void KHangManView::slotTry()
{
    QString sChar = charWrite->text();
    kdDebug() << "sChar as entered: " << sChar << endl;

    // If German, make upper case, otherwise make lower case.
    if (Prefs::upperCase() && Prefs::selectedLanguage() =="de")
        sChar = sChar.upper();
    else
        sChar = sChar.lower();

    // If the char is not a letter, empty the input and return.
    if (!sChar.at(0).isLetter()) {
	charWrite->setText("");
	return;
    }

    // Handle the guess.
    if (!m_guessedLetters.contains(sChar)) {
	// The letter is not already guessed.
	
	if (containsChar(sChar)) {
	    replaceLetters(sChar);
	    stripWord = goodWord;//need that because of the white spaces
	    sword=word;
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
		// pixImage->setPixmap(px[10]);
		//TODO find a better way to finish
		//
		if (Prefs::sound()) {
		    QString soundFile = locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
		    if (soundFile != 0) 
			KAudioPlayer::play(soundFile);
		}

		if (Prefs::wonDialog()) {
		    // TODO: KPassivePopup to say you have won
		    QTimer::singleShot( 3*1000, this, SLOT(slotNewGame()) );
		}
		else if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?")) == 3)
		    slotNewGame();
		else
		    kapp->quit();
	    }
	}
	else {
	    // The char is missed.

	    m_guessedLetters << sChar;	
	    missedL=missedL.replace((2*missedChar), 1, sChar);

	    missedChar++;
	    update();
	    if (missedChar >= 10) //you are hanged!
		{
		    //TODO sequence to finish when hanged
		    QStringList charList=QStringList::split("",word);
		    QString theWord=charList.join(" ");
		    goodWord = theWord;
		    //usability: find another way to start a new game
		    QString newGameString = i18n("You lost. Do you want to play again?");
		    if (KMessageBox::questionYesNo(this, newGameString) == 3)
			slotNewGame();
		    else
			kapp->quit();
		}
	}
    }
    else {
	// The letter is already guessed.

	// Show a popup that says as much.
	// FIXME: usability: highlight it in Missed if it is there
    QPoint point;
	KPassivePopup *popup = new KPassivePopup( KPassivePopup::Balloon, this, "popup" );
	popup->setAutoDelete( true );
	popup->setTimeout( 1000 );
	popup->setView(i18n("This letter has already been guessed.") );

	int x =0, y = 0;
	if (missedL.contains(sChar)>0) { //TODO popup should be better placed
	    QPoint abspos = popup->pos();
	    x = abspos.x() + width()*400/700;
	    y = abspos.y() + height()*50/535;
	    point = QPoint(x, y);
	    //put the letter in red for 1 second
	    QTimer *timer = new QTimer( this);
	    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
	    timer->start( 1000, TRUE ); // 1 second single-shot timer
	    //TODO should highlight the repeated letter in red
	    //disable any possible entry
	    charWrite->setEnabled(false);
	}

	if (goodWord.contains(sChar) > 0) {
	    QPoint abspos = popup->pos();
	    if (Prefs::mode() == 0)  {
		x = abspos.x() + width()*250/700;
		y = abspos.y() + height()*485/535;
		point = QPoint(x, y);
	    }
	    else  {
		x = abspos.x() + width()*200/700;
		y = abspos.y() + height()*485/535;
		point = QPoint(x, y);
	    }
	    QTimer *timer = new QTimer( this);
	    connect( timer, SIGNAL(timeout()), this, SLOT(timerWordDone()) );
	    timer->start( 1000, TRUE ); // 1 second single-shot timer
	    //TODO should highlight the repeated letter in red
	    //disable any possible entry
	    charWrite->setEnabled(false);	
	}
	popup->show(mapToGlobal(point));
    }

    // Reset the entry field after guess.
    charWrite->setText("");
}


void KHangManView::timerDone()
{
    charWrite->setEnabled(true);
    charWrite->setFocus();
}

void KHangManView::timerWordDone()
{
    charWrite->setEnabled(true);
    charWrite->setFocus();
}

void KHangManView::slotNewGame()
{
    if (Prefs::sound()) {
	QString soundFile = locate("data", "khangman/sounds/new_game.ogg");
	if (soundFile != 0) 
	    KAudioPlayer::play(soundFile);
    }
    reset();

    //distinction between upper and lower case letters
    //if (Prefs::levelFile() == "world_capitals.kvtml" || Prefs::levelFile() == "departements.kvtml")
     //   upperBool = true;
    //else upperBool = false;

    game();
    charWrite->setFocus();
}


void KHangManView::reset()
{
    goodWord="";
    word="";
    charWrite->setText("");
    missedChar=0;
    m_guessedLetters.clear();
    missedL = "_ _ _ _ _ _ _ _ _ _  ";
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
    kdDebug() << word << endl;
    //display the number of letters to guess with _
    for(unsigned int i = 0; i < word.length(); i++)
    {
        goodWord.append("_");
        goodWord.append(" ");
    }        
    goodWord.remove(goodWord.length()-1);
    
    kdDebug() << goodWord << endl;
    stripWord=goodWord;
    //if needed, display white space or - if in word or semi dot
    f = word.find( "-" );
    if (f>0)
    {
        g=0;
        goodWord.replace(2*f, 1, "-");
        g = word.find( "-", f+1);
        if (g>0) 
                goodWord.replace(2*g, 3, "-");
        if (g>1)
                goodWord.append("_");
    }
    c = word.find( " " );
    if (c>0) //find another white space
    {
        d=0;
        goodWord.replace(2*c, 1, " ");
        d = word.find( " ", c+1);
        if (d>0)  goodWord.replace(2*d, c+1, " ");
    }
    int e = word.find( QString::fromUtf8("·") );
    if (e>0) goodWord.replace(2*e, 1, QString::fromUtf8("·") );
    int h = word.find( "'" );
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

    word = verbs[wordNumber].originalText();
    tip  = verbs[wordNumber].translatedText(); 

    if (word.isEmpty()) 
        readFile();

    if (tip.isEmpty()) {
        Prefs::setHint(false);//hint can't be enabled
        Prefs::writeConfig();
        hintBool = false;//hint does not exist
        khangman ->changeStatusbar("", 103);
    }
    else {
        hintBool = true;
        khangman ->setMessages();
    }

    if (Prefs::upperCase() && Prefs::selectedLanguage() =="de")
    {
        word = word.upper();// only for German currently
        //replace ß with SS in German 
        if (word.contains(QString::fromUtf8("ß"))) {
            int index = word.find(QString::fromUtf8("ß"),0);
            word.replace(index,1, "S");
            //TODO add a S here
        }
    }
}

void KHangManView::loadAnimation()
{
    switch (Prefs::mode())  {
        case Prefs::EnumMode::sea:
            bcgdPicture = QPixmap(locate("data","khangman/pics/sea/sea_theme.png") );
            theme = "sea";
            charWrite->setPaletteForegroundColor(QColor(83, 40, 14));
            guessButton->setPaletteBackgroundColor( QColor( 115, 64, 49) );
            guessButton->setPaletteForegroundColor( QColor( 148, 156, 167) );
            guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;
        case Prefs::EnumMode::desert:
            bcgdPicture = QPixmap(locate("data","khangman/pics/desert/desert_theme.png") );
            theme = "desert";
            charWrite->setPaletteForegroundColor(QColor(87, 0, 0));
            guessButton->setPaletteBackgroundColor( QColor( 205, 214, 90) );
            guessButton->setPaletteForegroundColor( QColor( 87, 0, 0) );
            guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
            break;
    }	

    //now we preload the pixmaps...
    px[0].load(locate("data", QString("khangman/pics/%1/animation0.png").arg(theme)));
    px[1].load(locate("data", QString("khangman/pics/%1/animation1.png").arg(theme)));
    px[2].load(locate("data", QString("khangman/pics/%1/animation2.png").arg(theme)));
    px[3].load(locate("data", QString("khangman/pics/%1/animation3.png").arg(theme)));
    px[4].load(locate("data", QString("khangman/pics/%1/animation4.png").arg(theme)));
    px[5].load(locate("data", QString("khangman/pics/%1/animation5.png").arg(theme)));
    px[6].load(locate("data", QString("khangman/pics/%1/animation6.png").arg(theme)));
    px[7].load(locate("data", QString("khangman/pics/%1/animation7.png").arg(theme)));
    px[8].load(locate("data", QString("khangman/pics/%1/animation8.png").arg(theme)));
    px[9].load(locate("data", QString("khangman/pics/%1/animation9.png").arg(theme)));
    px[10].load(locate("data", QString("khangman/pics/%1/animation10.png").arg(theme)));
}

#include "khangmanview.moc"

