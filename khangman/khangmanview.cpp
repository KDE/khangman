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
#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <qimage.h>
#include <qpainter.h>
#include <qregexp.h>
//project headers
#include "prefs.h"
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
    paint.setFont(QFont("Arial", 32));
    QRect aux;
    //TODO the rectangle does not adapt correctly to the word size
    aux = paint.boundingRect(QRect(), AlignLeft, goodWord);
    aux.moveBy(width()/50, height()-height()/10);
    //TODO fill it so it blends into the background
    paint.fillRect(aux, QColor(41, 60, 123));
    paint.drawText(aux, AlignLeft, goodWord);
    //paint.setPen( QColor(148, 156, 167));
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
    charWrite_font.setPointSize( height()/18 );
    charWrite_font.setFamily( "Dustimo Roman" );
    charWrite->setFont( charWrite_font ); 
    guessButton->setFont(QFont("Dustimo Roman", height()/20, QFont::Bold));
    guessButton->setGeometry(width()-2*height()/12-guessButton->width()-5, height()-2*height()/16, guessButton->width(), height()/10);
    //bitBlt(this, 0, 0, paletteBackgroundPixmap());
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
    QPainter p;
    p.begin(paletteBackgroundPixmap());
    p.drawPixmap(QRect(0,0, width()*630/700, height()*285/535), px[missedChar]);
    p.end();
    QPainter paint;
    paint.begin(paletteBackgroundPixmap());
    paint.setPen( QColor(148, 156, 167));
    QFont f = QFont("Domestic Manners");
    f.setPointSize(height()/14);
    paint.setFont(f);
    //paint.setFont(QFont("Steve", height()/14));
    QString misses = i18n("Misses");
    paint.drawText(width()/2+width()/30, height()/12, misses);
    paint.setFont(QFont("Bitstream Charter", height()/13, QFont::Bold));
    paint.drawText( width()/2+width()/4, 0, 0, 0, AlignLeft|AlignTop|DontClip, missedL );
    paint.end();
    bitBlt(this, 0, 0, paletteBackgroundPixmap());
}

void KHangManView::slotTry()
{
    QString sChar = charWrite->text();
    kdDebug() << "sChar as entered: " << sChar << endl;
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
                    paintWord();
                    sword.remove(QRegExp(" "));
                    if (rightWord.stripWhiteSpace().lower() == sword.stripWhiteSpace().lower()) {   //you made it!
                        //we reset everything...
                       // pixImage->setPixmap(px[10]);
                       //TODO find a better way to finish
                       slotNewGame();
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

                        missedChar++;
                        kdDebug() << missedChar << endl;
                        paintHangman();

                        if (missedChar >= 10) //you are hanged!
                        {
                                //we reset everything...
                                //pixImage->setPixmap(px[9]);
                                //um... The word is not guessed... Let's show it...
                                kdDebug() << "Hanged!!! " << endl;
                                QStringList charList=QStringList::split("",word);
                                QString theWord=charList.join(" ");
                                //if (language =="de")
                                //theWord = theWord.replace(0,1, theWord.left(1).upper());
                                goodWord = theWord;
                                slotNewGame();
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

void KHangManView::slotNewGame()
{
    /* if (Prefs::sound()) {
            QString soundFile = locate("data", "khangman/sounds/new_game.ogg");
            if (soundFile != 0) 
                    KAudioPlayer::play(soundFile);
    }*/
    
    reset();
    //language=Prefs::selectedLanguage();
    //TODO: see if that's necessary
    /*QFont tFont;
    if (language =="tg")  {
        tFont.setFamily( "URW Bookman" );
    }
    if (language =="cs")  {
        tFont.setFamily( "Arial" );
    }
    tFont.setPointSize( 22 );
    missedLetters->setFont(tFont);
    charWrite->setFont(tFont);*/
    
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
    missedL = "_ _ _ _ _  \n_ _ _ _ _  ";
    allWords.clear();
    update();
}

void KHangManView::game()
{
        //pixImage->setPixmap(px[10]);
        //if the data files are not installed in the correct dir
        QString myString=QString("khangman/data/%1/%2").arg(Prefs::selectedLanguage()).arg(Prefs::levelFile());
        kdDebug() << "language " << Prefs::selectedLanguage() << endl;
        kdDebug() << "level " << Prefs::levelFile() << endl;
        QFile myFile;
        myFile.setName(locate("data",myString));
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
            emit(signalKvtml(true));
            readFile();
        }
        else {//TODO abort if not a kvtml file maybe
            if (allData.count()<=1) {
                emit(signalChangeLanguage(3));
                return;
            }
            emit(signalKvtml(false));
            //now the calculations...
            int objects = allData.count();//number of words in the file
            //picks a random word from allData
            while (word.isEmpty())
                word = allData[random.getLong(objects)]; //gives us a single word...
            //test if the word is not the same than the previous one
            if (temp.isEmpty())
                    temp=word;
            else {
                while (word.lower()==temp.lower())
                    word = allData[random.getLong(objects)];
                temp=word;
            }//end of test
            if (!upperBool)
                word = word.lower(); //because of German
        }//end else if language=fr
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
        //paintWord();
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
                return;
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
        kdDebug() << "tip : " << tip << endl;
        if (tip.isEmpty()) {
                hintBool = false;
                Prefs::setHint(false);
                Prefs::writeConfig();
                khangman ->changeStatusbar("", 103);}
}

#include "khangmanview.moc"

