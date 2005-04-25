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
#include <qregexp.h>
#include <qtimer.h>
#include <qtooltip.h>
//project headers
#include "prefs.h"
#include "khangman.h"
#include "khangmanview.h"


KHangManView::KHangManView(KHangMan*parent, const char *name)
    : QWidget(parent)
{
    khangman = parent;
    
    charWrite = new KLineEdit( this, "charWrite" );
    charWrite->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, charWrite->sizePolicy().hasHeightForWidth() ) );
    charWrite->setMaxLength( 1 );
    charWrite->setAlignment( int( QLineEdit::AlignHCenter ) );

    guessButton = new KPushButton( this, "guessButton" );
    guessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, guessButton->sizePolicy().hasHeightForWidth() ) );
    guessButton->setAutoMask( TRUE );
    guessButton->setFlat( TRUE );
    guessButton->setText( i18n( "G&uess" ) );
    
    //get background from config file - default is sea
    loadAnimation();
    
    bluePic = QPixmap(locate("data","khangman/pics/sea/blue.png") );
    greenPic = QPixmap(locate("data","khangman/pics/desert/green.png") );
    miss_bluePic = QPixmap(locate("data","khangman/pics/sea/miss_blue.png") );
    miss_desertPic= QPixmap(locate("data","khangman/pics/desert/miss_desert.png") );

    setMinimumSize( QSize( 700, 535) );
    slotSetPixmap( bcgdPicture);

    temp="";
    missedChar=0;
    tmp = 0;
    accent_b = false;
    m_accent = true;
    hintBool = true;//assume tip exists
    //tip="";
    //missedL = "_ _ _ _ _  \n_ _ _ _ _  ";
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
    kdDebug() << "sChar " << sChar << endl;
    int index=0;
    bool b_end = false;
    kdDebug() << "word " << word << endl;
    if (b_oneLetter)  {  //we just replace the next instance
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
            kdDebug() << "goodword2 " << goodWord << endl;
            index++;
            }
    }
    if (m_accent && !accent_b) {
        //characters must be lisible as utf-8 and file must be saved with this encoding. 
        kdDebug() << "In accent " << endl;
        if (sChar=="i") replaceLetters(QString("í"));
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
    if (m_accent && !accent_b) {
        if (sChar=="i") b = word.contains(QString("í"))>0;//QChar('í').unicode()) > 0;
        if (sChar=="a") b = word.contains(QString("à")) > 0 || word.contains(QString("á")) > 0 || word.contains(QString("ã")) > 0;
        if (sChar=="u") b = word.contains(QString("ü")) > 0 || word.contains(QString("ù")) > 0;
        if (sChar=="o") b = word.contains(QString("ò")) > 0 || word.contains(QString("ó")) || word.contains(QString("õ")) > 0;
        if (sChar=="e") b = word.contains(QString("è")) > 0 || word.contains(QString("é")) > 0;
    }
    
    return ((word.contains(sChar) > 0) || b);
}

void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
    if (mouse->button() == RightButton && hintBool)//(kvtmlBool && hintBool && (mouse->button() == RightButton))
    {
        KPassivePopup *myPopup = new KPassivePopup( charWrite);
        myPopup->setView(i18n("Hint"), tip );
        myPopup->setPalette(QToolTip::palette());
        myPopup->setTimeout(4000); //show for 4 seconds
        myPopup->show();	
        int x=0, y=0;
        if (Prefs::mode() ==0)  {
            x = width()*300/700;
            y = height()*510/535;
        }
        else  {
            x = width()*254/700;
            y = height()*405/535;
        }
        myPopup->move(x, y);
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

void KHangManView::paintEvent( QPaintEvent * )
{
    paintHangman();
    paintWord();
    paintMisses();
}

void KHangManView::paintWord()
{
    QPainter paint;
    paint.begin(&bg);
    kdDebug() << "width() " << width() << endl;
    QRect myRect;
    if (Prefs::mode() ==0)   //sea
        myRect = QRect(0, height()-height()*126/535, width()*417/700, height()*126/535);
    else  
        myRect = QRect(0, height()-height()*126/535, width()*327/700, height()*126/535);
    QPixmap pix( myRect.size() );
    pix.fill( this, myRect.topLeft() );
    QPainter p(&pix);
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
    if (Prefs::mode() ==0)
        p.drawText(0,0, width()*417/700, height()*126/535, AlignCenter|AlignCenter, goodWord);
    else
        p.drawText(0,0, width()*327/700, height()*126/535, AlignCenter|AlignCenter, goodWord);
    p.end();
    paint.drawPixmap(myRect,pix);
    bitBlt( this, 0, height()-height()*126/535, &pix );
}

void KHangManView::paintWordTwice()
{
    /*QPainter paint;
    paint.begin(&bg);
    QRect myRect = QRect(0, height()-height()*126/535, width()*366/700, height()*126/535);
    QFont tFont;
    if (Prefs::selectedLanguage() =="tg")  {
        tFont.setFamily( "URW Bookman" );
    }
    else
        tFont.setFamily( "Arial" );
    tFont.setPixelSize( 28 ); //this has to be scaled depending of the dpi
    paint.setFont(tFont);
    paint.setPen(QColor(Qt::red));
    QRect aux = paint.boundingRect(myRect, AlignCenter|AlignCenter, goodWord.left(redIndex));
    kdDebug() << aux.width() << endl;
    if (redIndex == 0)
        paint.drawText(aux.width()+2, height()-height()*126/535, width()*366/700, height()*126/535, AlignCenter|AlignCenter, QString(goodWord[redIndex]));
    else //weird that it needs 2 pixels less after first
        paint.drawText(aux.width()+2, height()-height()*126/535, width()*366/700/2, height()*126/535, AlignCenter|AlignCenter, QString(goodWord[redIndex]));
    paint.end();
    bitBlt(this, 0, 0, &bg);*/
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
    charWrite->setGeometry(width()-2*height()/12, height()-2*height()/16, height()/10, height()/10);
    guessButton->setGeometry(width()-2*height()/12-guessButton->width()-5, height()-2*height()/16, guessButton->width(), height()/10);
}

void KHangManView::slotSetPixmap(QPixmap& bgPix)
{
    QImage img = bgPix.convertToImage();
    bg.resize(size());
    bg.convertFromImage(img.smoothScale( width(), height()));
    setPaletteBackgroundPixmap(bg);
}

void KHangManView::paintMisses()
{
    QPainter paint;
    paint.begin(&bg);
    QRect myRect = QRect(width()-width()*360/700, 0, 100, height()*70/535);
    QPixmap pix( myRect.size() );
    pix.fill( this, myRect.topLeft() );
    QPainter pi(&pix);
    if (Prefs::mode() ==0)   //sea
        pi.setPen( QColor(148, 156, 167));
    else
        pi.setPen( QColor(87, 0, 0));
    QString misses = i18n("Misses");
    QFont f = QFont("Domestic Manners");
    f.setPointSize(32);
    pi.setFont(f);
    pi.drawText(0, 0, 100, height()*70/535, AlignLeft|AlignTop, misses);
    bitBlt( this, width()-width()*360/700, 0, &pix);
    paint.end();
}

void KHangManView::paintHangman()
{
    //draw the animated hanged K
    QPainter p;
    p.begin(&bg);
    if (Prefs::mode() ==0) 
        p.drawPixmap(QRect(0,0, width()*630/700, height()*285/535), px[missedChar]);
    else
        p.drawPixmap(QRect(width()*68/700, height()*170/535, width()*259/700, height()*228/535), px[missedChar]);
    p.end();
    bitBlt(this, 0, 0, &bg);
    //draw the Missed letters
    QPainter paint;
    paint.begin(&bg);
    QRect myRect = QRect(width()-width()*360/700+100, 0, width()*360/700-100, height()*70/535);
    QPixmap pix( myRect.size() );
    pix.fill( this, myRect.topLeft() );
    QPainter pi(&pix);
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
    pi.setFont(tFont);
    pi.drawText(0, 0, width()*360/700-100, height()*70/535, AlignRight|AlignCenter, missedL);
    pi.end();
    paint.drawPixmap(myRect,pix);
    bitBlt( this, width()-width()*360/700+100, 0, &pix);
    paint.end();
}

void KHangManView::paintMissedTwice()
{
    /*QPainter paint;
    paint.begin(paletteBackgroundPixmap());

    if (Prefs::mode() ==0) { //sea theme
        QRect myRect = QRect(width()-width()*280/700, 0, width()*280/700, height()*90/535);
        QPixmap pix( myRect.size() );
        pix.fill( this, myRect.topLeft() );
        QPainter p(&pix);
        QFont f = QFont("Domestic Manners");
        f.setPointSize(height()/17);
        p.setFont(f);
        p.setPen( QColor(148, 156, 167));
        QString misses = i18n("Misses");
        QRect aux = paint.boundingRect(QRect(), AlignLeft, misses);
        p.setFont(QFont("Bitstream Charter", height()/17, QFont::Bold));
        p.setPen(QColor(Qt::red));
        QRect auxi = paint.boundingRect(QRect(), AlignCenter|AlignTop|DontClip, missedL.left(redIndex));
        if (redIndex == 0)
            p.drawText(0, 0, width()*280/700, height()*90/535, AlignCenter|AlignTop|DontClip, QString(missedL[redIndex]) );
        p.end();
        paint.drawPixmap(myRect,pix);
        bitBlt( this, width()-width()*280/700, 0, &pix );
    }
    else  { //desert theme
       // if (redIndex == 0)
            //paint.drawText( width()*130/700 +aux.width()+1, height()/13,  QString(missedL[redIndex]));
        //else 
            //paint.drawText( width()*130/700 +aux.width()-4, 0, 0, 0, AlignLeft|AlignTop|DontClip,QString(missedL[redIndex]));
    }
    paint.end();
    //bitBlt(this, 0, 0, paletteBackgroundPixmap());*/
}

void KHangManView::slotTry()
{
    QString sChar = charWrite->text();
    kdDebug() << "sChar as entered: " << sChar << endl;
    //if (language=="de")
      //  upperBool = true;
    //if ((!upperBool))
    if (Prefs::upperCase()) {//TODO see if it's OK in all languages for all special chars
        sChar = sChar.upper();
        //TODO replace ß with SS in German if (Prefs::selectedLanguage() =="de")
    }
    else
        sChar = sChar.lower();

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
                    paintWord();
                    //no update(); here or it'll flicker!!!
                    sword.remove(QRegExp(" "));
                    if (rightWord.stripWhiteSpace().lower() == sword.stripWhiteSpace().lower()) {   //you made it!
                        //we reset everything...
                       // pixImage->setPixmap(px[10]);
                       //TODO find a better way to finish
                       //
                        if (Prefs::sound()) {
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
                       }
                    }

                }
                else //if the char is missed...
                {
                        allWords << sChar;
                       /* if (Prefs::mode() ==0) { //sea theme
                            if (missedChar<5)
                                missedL=missedL.replace(2*missedChar, 1, sChar);
                            else if(missedChar>5)
                                missedL=missedL.replace((2*missedChar)+2, 1, sChar);
    
                            if (missedChar==5) //we actually need to replace one underscore too much..
                            {
                                    missedL=missedL.replace((2*missedChar)+1,1, "\n"+sChar+" ");
                                    missedL=missedL.replace(22,2, "");
                            }
			}	
			else*/	//desert missed all in 1 line
                        missedL=missedL.replace((2*missedChar), 1, sChar);

                        missedChar++;
                        paintHangman();
                        paintMisses();
                        if (missedChar >= 10) //you are hanged!
                        {
                                //TODO sequence to finish when hanged
                                kdDebug() << "------ hanged !!! " << endl;
                                QStringList charList=QStringList::split("",word);
                                QString theWord=charList.join(" ");
                                //if (language =="de")
                                //theWord = theWord.replace(0,1, theWord.left(1).upper());
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
        else
        {
            //usability: highlight it in Missed if it is there
            if (missedL.contains(sChar)>0) { //TODO popup should be better placed
                KPassivePopup *popup = new KPassivePopup( this, "popup" );
                popup->setAutoDelete( true );
                popup->setTimeout( 1000 );
                popup->setView(i18n("This letter has already been guessed.") );
                popup->show();
                if (Prefs::mode() ==0)  {
                    int x = width()*332/700;
                    int y = height()*145/535;
                    popup->move(x, y);
                }
                else  {
                    int x = width()*332/700;
                    int y = height()*145/535;
                    popup->move(x, y);
                }
                redIndex = missedL.find(sChar, 0);
                //put the letter in red for 1 second
                QTimer *timer = new QTimer( this);
                connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
                timer->start( 1000, TRUE ); // 1 second single-shot timer
                paintMissedTwice();
                //disable any possible entry
                charWrite->setEnabled(false);
            }
            //usability: hilight it in the word
            if (goodWord.contains(sChar)>0) {
                KPassivePopup *popup = new KPassivePopup( this, "popup" );
                popup->setAutoDelete( true );
                popup->setTimeout( 1000 );
                popup->setView(i18n("This letter has already been guessed.") );
                popup->show();
                if (Prefs::mode() ==0)  {
                
                }
                else  {
                    int x = this->width()*470/700;
                    int y = this->height()*520/535;
                    popup->move(x, y);
                }
                redIndex = goodWord.find(sChar,0);
                //put the letter in red for 1 second
                QTimer *timer = new QTimer( this);
                connect( timer, SIGNAL(timeout()), this, SLOT(timerWordDone()) );
                timer->start( 1000, TRUE ); // 1 second single-shot timer
                paintWordTwice();
                //disable any possible entry
                charWrite->setEnabled(false);	
            }			
        }
    }
    //reset after guess...
    charWrite->setText("");
}

void KHangManView::timerDone()
{
    charWrite->setEnabled(true);
    charWrite->setFocus();
    //paintHangman();
}

void KHangManView::timerWordDone()
{
    charWrite->setEnabled(true);
    charWrite->setFocus();
    //paintWord();
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
    kdDebug() << "In reset " << endl;
    goodWord="";
    word="";
    charWrite->setText("");
    missedChar=0;
    allWords.clear();
    missedL = "_ _ _ _ _ _ _ _ _ _  ";
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
    update();
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
    int e = word.find( "·" );
    if (e>0) goodWord.replace(2*e, 1, "·");
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
    int wordNumber = random.getLong(NumberOfWords);
    if (wordNumber<=0) 
        readFile();
    //test if not twice the same
    if (tmp==0)
            temp=wordNumber;
    else
    {
        while (wordNumber==tmp)
                wordNumber = random.getLong(NumberOfWords);
        tmp=wordNumber;
    }//end of test
    word = verbs[wordNumber].originalText();
    tip = verbs[wordNumber].translatedText(); 
    if (word.isEmpty()) 
        readFile();
    kdDebug() << "tip : " << tip << endl;
    if (tip.isEmpty()) {
        Prefs::setHint(false);//hint can't be enabled
        Prefs::writeConfig();
        hintBool = false;//hint does not exist
        khangman ->changeStatusbar("", 103);
    }
    else {
        hintBool = true;
        khangman ->changeStatusbar(i18n("Hint available"), 103);
    }
    if (Prefs::upperCase())
        word = word.upper();//TODO see if it's OK in all languages for all special chars
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

