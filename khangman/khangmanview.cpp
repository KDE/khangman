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

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <qimage.h>
#include <qpainter.h>
#include <qregexp.h>
//project headers
#include "khangman.h"
#include "khangmanview.h"


KHangManView::KHangManView(KHangMan*parent, const char *name)
    : QWidget(parent)
{
    khangman = parent;
    //get background from config file - default is sea
    seaPicture = QPixmap(locate("data","khangman/pics/sea_theme.png") );

    //now we preload the pixmaps...
    px[0].load(locate("data","khangman/pics/animation0.png"));
    px[1].load(locate("data","khangman/pics/animation1.png"));
    px[2].load(locate("data","khangman/pics/animation2.png"));
    px[3].load(locate("data","khangman/pics/animation3.png"));
    px[4].load(locate("data","khangman/pics/animation4.png"));
    px[5].load(locate("data","khangman/pics/animation5.png"));
    px[6].load(locate("data","khangman/pics/animation6.png"));
    px[7].load(locate("data","khangman/pics/animation7.png"));
    px[8].load(locate("data","khangman/pics/animation8.png"));
    px[9].load(locate("data","khangman/pics/animation9.png"));
    px[10].load(locate("data","khangman/pics/animation10.png"));

    setMinimumSize( QSize( 700, 535) );
    slotSetPixmap( seaPicture);

    charWrite = new KLineEdit( this, "charWrite" );
    charWrite->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, charWrite->sizePolicy().hasHeightForWidth() ) );
    charWrite->setMaxLength( 1 );
    charWrite->setAlignment( int( QLineEdit::AlignHCenter ) );

    guessButton = new KPushButton( this, "guessButton" );
    guessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, guessButton->sizePolicy().hasHeightForWidth() ) );
    guessButton->setPaletteBackgroundColor( QColor( 115, 64, 49) );
    guessButton->setPaletteForegroundColor( QColor( 148, 156, 167) );
    guessButton->setBackgroundOrigin( KPushButton::ParentOrigin );
    guessButton->setAutoMask( TRUE );
    guessButton->setFlat( TRUE );
    guessButton->setText( i18n( "G&uess" ) );

    temp="";
    missedChar=0;
    tmp = 0;
    accent_b = false;
    m_accent = true;
    //tip="";
    missedL = "_ _ _ _ _  \n_ _ _ _ _  ";

    word = "khangman";
    goodWord = "_ _ _ _ _ _ _ _";
    connect( charWrite, SIGNAL( textChanged(const QString &) ), this, SLOT( slotValidate(const QString &) ) );
    connect( charWrite, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
    connect (guessButton, SIGNAL( clicked() ), this, SLOT( slotTry() ));
}

KHangManView::~KHangManView()
{
}

void KHangManView::replaceLetters(const QString& sChar)
{
    //replace letter in the word
    kdDebug() << "sChar " << sChar.latin1() << endl;
    int index=0;
    bool b_end = false;
    if (b_oneLetter)  {  //we just replace the next instance
        for (int count=0; count <word.contains(sChar); count++) {
            index = word.find(sChar.latin1(),index);
            if (goodWord.at(2*index)=='_') {
                goodWord.replace((2*index), 1,sChar);
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
    kdDebug() << "m_accent  " << m_accent << endl;
    kdDebug() << "accent_b  " << accent_b << endl;
    if (m_accent && !accent_b) {
        //characters must be lisible as utf-8 and file must be saved with this encoding. 
        kdDebug() << "In accent " << endl;
        if (sChar=="i") replaceLetters(QString("í").latin1());
        if (sChar=="a") replaceLetters(QString("à"));
        if (sChar=="a") replaceLetters(QString("á"));
        if (sChar=="a") replaceLetters(QString("ã"));
        if (sChar=="u") replaceLetters(QString("ü"));
        if (sChar=="o") replaceLetters(QString("ò"));
        if (sChar=="o") replaceLetters(QString("ó"));
        if (sChar=="o") replaceLetters(QString("õ"));
        if (sChar=="e") replaceLetters(QString("è"));
        if (sChar=="e") replaceLetters(QString("é"));
        if (sChar=="u") replaceLetters(QString("ù"));
    }
    if (!b_oneLetter) 
        allWords << sChar; //appends the list only if not in One Letter only mode...
    if (word.contains(sChar)==1) 
        allWords << sChar; //append if only one instance
    if (b_oneLetter && b_end) 
        allWords << sChar; 
}


bool KHangManView::containsChar(const QString &sChar)
{
    bool b = false;
    kdDebug() << "In containsChar " << endl;
    if (m_accent && !accent_b) {
        kdDebug() << "Testing " << endl;
        if (sChar=="i") b = word.contains(QString("í"))>0;//QChar('í').unicode()) > 0;
        if (sChar=="a") b = word.contains(QString("à")) > 0 || word.contains(QString("á")) > 0 || word.contains(QString("ã")) > 0;
        if (sChar=="u") b = word.contains(QString("ü")) > 0 || word.contains(QString("ù")) > 0;
        if (sChar=="o") b = word.contains(QString("ò")) > 0 || word.contains(QString("ó")) || word.contains(QString("õ")) > 0;
        if (sChar=="e") b = word.contains(QString("è")) > 0 || word.contains(QString("é")) > 0;
    }
    
    return ((word.contains(sChar) > 0) || b);
}

void KHangManView::paintEvent( QPaintEvent * )
{
    paintHangman();
    paintWord();
}

void KHangManView::paintWord()
{
    QPainter paint;
    paint.begin(paletteBackgroundPixmap());
    paint.setPen( QColor(148, 156, 167));
    paint.setFont(QFont("Arial", height()/15, QFont::Bold));
    paint.drawText(width()/50, height()-height()/10, goodWord);
    paint.end();
    bitBlt(this, 0, 0, paletteBackgroundPixmap());
}

void KHangManView::resizeEvent(QResizeEvent *)
{
    if(!seaPicture.isNull())
        slotSetPixmap(seaPicture);
    charWrite->setGeometry(width()-2*height()/12, height()-2*height()/16, height()/10, height()/10);
    charWrite->setMinimumSize( QSize( height()/18, 0 ) );
    QFont charWrite_font(  charWrite->font() );
    charWrite_font.setPointSize( height()/12 );
    charWrite_font.setFamily( "Arial Narrow" );
    charWrite->setFont( charWrite_font ); 
    guessButton->setFont(QFont("Bitstream Charter", height()/20, QFont::Bold));
    guessButton->setGeometry(width()-2*height()/12-guessButton->width()-5, height()-2*height()/16, guessButton->width(), height()/10);
    bitBlt(this, 0, 0, paletteBackgroundPixmap());
}

void KHangManView::slotSetPixmap(QPixmap& bgPix)
{
    QImage img = bgPix.convertToImage();
    QPixmap bg(size());
    bg.convertFromImage(img.smoothScale( width(), height()));
    setPaletteBackgroundPixmap(bg);
}

void KHangManView::paintHangman()
{
    kdDebug() << "In painting" << endl;
    QPainter p;
    p.begin(paletteBackgroundPixmap());//p.begin(&pm);
    p.drawPixmap(QRect(0,0, width()*630/700, height()*285/535), px[missedChar]);
    p.end();
    QPainter paint;
    paint.begin(paletteBackgroundPixmap());
    paint.setPen( QColor(148, 156, 167));
    paint.setFont(QFont("Steve", height()/14));
    QString misses = i18n("Misses");
    paint.drawText(width()/2, height()/12, misses);
    paint.setFont(QFont("Bitstream Charter", height()/13, QFont::Bold));
    paint.drawText( width()/2+width()/4, 0, 0, 0, AlignLeft|AlignTop|DontClip, missedL );
    paint.end();
    bitBlt(this, 0, 0, paletteBackgroundPixmap());
}

void KHangManView::game()
{
    /////goodWord = "k h a _ g m a _";
    //update();
}

void KHangManView::slotTry()
{
    QString sChar = charWrite->text();
//    if (language=="de")
      //  upperBool = true;
  //  if ((!upperBool))
   //     sChar = sChar.lower();
    if (sChar.at(0).isLetter()) {//if the char is a letter
        if (allWords.contains(sChar) == 0) {  //if letter not alreasy guessed
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
                    //if (language =="de") {
                        //goodWord = goodWord.replace(0,1, goodWord.left(1).upper());
                       // }
                    paintWord();;
                    sword.remove(QRegExp(" "));
                    if (rightWord.stripWhiteSpace().lower() == sword.stripWhiteSpace().lower()) {   //you made it!
                        //we reset everything...
                       // pixImage->setPixmap(px[10]);
                        /*if (Prefs::sound()) {
                            QString soundFile = locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
                            if (soundFile != 0) 
                                KAudioPlayer::play(soundFile);
                        }
                        if (Prefs::wonDialog()) {
                            QTimer::singleShot( 3*1000, this, SLOT(slotNewGame()) );
                        }
                        else {
                            if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?")) == 3)
                                slotNewGame();
                            else
                                kapp->quit();
                       }*/
                    }

                }
                else //if the char is missed...
                {
                        allWords << sChar;
                        if (missedChar<5)
                            missedL=missedL.replace(2*missedChar, 1, sChar);
                        else if(missedChar>5)
                            missedL=missedL.replace((2*missedChar)+2, 1, sChar);

                        if (missedChar==5) //we actually need to replace one underscore too much..
                        {
                                missedL=missedL.replace((2*missedChar)+1,1, "\n"+sChar+" ");
                                missedL=missedL.replace(22,2, "");
                        }
                       
                        paintHangman();
                        missedChar++;

                        if (missedChar >= 10) //you are hanged!
                        {
                                //we reset everything...
                                //pixImage->setPixmap(px[9]);
                                //um... The word is not guessed... Let's show it...
                                QStringList charList=QStringList::split("",word);
                                QString theWord=charList.join(" ");
                                //if (language =="de")
                                        //theWord = theWord.replace(0,1, theWord.left(1).upper());
                                //paintHangman();
                                
                                //usability: change that
                                /*QString newGameString;
                                if (Prefs::pictures() == Prefs::EnumPictures::mild) 
                                        newGameString = i18n("You lost. Do you want to play again?");
                                else
                                        newGameString = i18n("You are dead. Do you want to play again?");
                                if (KMessageBox::questionYesNo(this, newGameString) == 3)
                                        slotNewGame();
                                else
                                        kapp->quit();*/
                        }
                }
        }
        else
        {
                //usability: highlight it in Missed if it is there
                if (missedL.contains(sChar)>0) {
                    /*KPassivePopup *popup = new KPassivePopup( this, "popup" );
                    popup->setAutoDelete( true );
                    popup->setTimeout( 1000 );
                    popup->setView(i18n("This letter has already been guessed.") );
                    popup->show();
                    int redIndex = missedL.find(sChar,0);
                    //put the letter in red for 1 second
                    QTimer *timer = new QTimer( this);
                    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
                    timer->start( 1000, TRUE ); // 1 second single-shot timer
                    missedLetters->setText("<qt>"+missedL.left(redIndex)+"<font color=\"#ff0000\">"+missedL[redIndex]+"</font>"+missedL.right(missedL.length()-redIndex-1)+"</qt>");
                    missedLetters->setTextFormat( QLabel::AutoText );
                    //disable any possible entry
                    charWrite->setEnabled(false);*/

                }
                //usability: hilight it in the word
                if (goodWord.contains(sChar)>0) {/*
                    KPassivePopup *popup = new KPassivePopup( mainLabel, "popup" );
                    popup->setAutoDelete( true );
                    popup->setTimeout( 1000 );
                    popup->setView(i18n("This letter has already been guessed.") );
                    popup->show();
                    int redIndex = goodWord.find(sChar,0);
                    QTimer *timer = new QTimer( this);
                    connect( timer, SIGNAL(timeout()), this, SLOT(timerDoneWord()) );
                    timer->start( 1000, TRUE ); // 1 second single-shot timer
                    //put the letter in red for 1 second
                    mainLabel->setTextFormat( QLabel::RichText );
                    mainLabel->setText("<qt>"+goodWord.left(redIndex)+"<font color=\"#ff0000\"><b>"+goodWord[redIndex]+"</b></font>"+goodWord.right(goodWord.length()-redIndex-1)+"</qt>");
                    mainLabel->setAlignment( int( QLabel::AlignCenter ) );
                    //disable any possible entry
                    charWrite->setEnabled(false);	*/
                }			
        }
    }
    //reset after guess...
    charWrite->setText("");
}

#include "khangmanview.moc"

