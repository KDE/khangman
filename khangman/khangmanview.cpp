/*
 * Copyright (C) 2001-2003 Anne-Marie Mahfouf <annma@kde.org>

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

//project headers
#include "khangman.h"
#include "khangmanview.h"
#include "version.h"
#include "prefs.h"
//Qt headers
#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtooltip.h>
//KDE headers
#include <kapplication.h>
#include <kaudioplayer.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kpassivepopup.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
//standard C++ headers
#include <stdlib.h>


KHangManView::KHangManView(KHangMan*parent, const char *name)
    : MainW(parent, name)
{
	khangman = parent;
	//set the usuall stuff...
	setIcon(locate("icon","hicolor/16x16/apps/khangman.png"));
	setCaption(i18n("KHangMan %1").arg(KHM_VERSION));
	//now we preload the pixmaps...
	px[0].load(locate("data","khangman/pics/hg1.png"));
	px[1].load(locate("data","khangman/pics/hg2.png"));
	px[2].load(locate("data","khangman/pics/hg3.png"));
	px[3].load(locate("data","khangman/pics/hg4.png"));
	px[4].load(locate("data","khangman/pics/hg5.png"));
	px[5].load(locate("data","khangman/pics/hg6.png"));
	px[6].load(locate("data","khangman/pics/hg7.png"));
	px[7].load(locate("data","khangman/pics/hg8.png"));
	px[8].load(locate("data","khangman/pics/hg9.png"));
	px[9].load(locate("data","khangman/pics/hg10.png"));
	px[10].load(locate("data","khangman/pics/hg12.png"));

	bluePix = QPixmap(locate("data","khangman/pics/blue.png") );
	naturePix = QPixmap(locate("data","khangman/pics/nature.png") );
	if (bluePix.isNull() || naturePix.isNull())
	{
		QString mString=i18n("The backgrounds files are not found.\n"
                                     "Check your installation, please!");
		KMessageBox::sorry( this, mString,
		                    i18n("Error") );
		exit(1);
	}

	setMinimumSize(380, 410); /* Selected to get px aspect ratio */

	temp="";
	missedChar=0;
	tmp = 0;
	accent_b = false;
	m_accent = false;
	tip="";

	connect( charWrite, SIGNAL( textChanged(const QString &) ), this, SLOT( slotValidate(const QString &) ) );
	connect( charWrite, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
	connect (guessButton, SIGNAL( clicked() ), this, SLOT( slotTry() ));
}

KHangManView::~KHangManView()
{
}

void KHangManView::slotNewGame()
{
	if (Prefs::sound()) {
		QString soundFile = locate("data", "khangman/sounds/new_game.ogg");
		if (soundFile != 0) 
  			KAudioPlayer::play(soundFile);
	}
	
	wipeout();
	//language=Prefs::selectedLanguage();
	QFont tFont;
	if (language =="tg")  {
    		tFont.setFamily( "URW Bookman" );
	}
	if (language =="cs")  {
    		tFont.setFamily( "Arial" );
	}
	tFont.setPointSize( 22 );
	missedLetters->setFont(tFont);
	charWrite->setFont(tFont);
	mainLabel->setFont(tFont);
	//distinction between upper and lower case letters
	if (levelFile == "world_capitals.txt" || levelFile == "departements.txt")
		upperBool = true;
	 else upperBool = false;
	game();
	charWrite->setFocus();
}

void KHangManView::slotValidate(const QString &text)
{
	disconnect(charWrite, SIGNAL( textChanged(const QString &) ),
		   this, SLOT( slotValidate(const QString &) ) );
	charWrite->clear();
	if (!text.isEmpty() && text[0].isLetter())
		charWrite->setText(text);
	connect(charWrite, SIGNAL( textChanged(const QString &) ),
		this, SLOT( slotValidate(const QString &) ) );
}

void KHangManView::slotTry()
{
	QString sChar = charWrite->text();
	if (!upperBool)
	sChar = sChar.lower();
	missedL= missedLetters->text();

	if (sChar.at(0).isLetter())
	{
		if (allWords.contains(sChar) == 0) //if letter not alreasy guessed
		{

			if (containsChar(sChar))
			{
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
				if (language =="de")
					goodWord = goodWord.replace(0,1, goodWord.left(1).upper());
				mainLabel->setText(goodWord);
				sword.remove(QRegExp(" "));
				if (rightWord.stripWhiteSpace().lower() == sword.stripWhiteSpace().lower()) //you made it!
				{
					//we reset everything...
					pixImage->setPixmap(px[10]);
					if (Prefs::sound()) {
						QString soundFile = locate("data", "khangman/sounds/EW_Dialogue_Appear.ogg");
						if (soundFile != 0) 
  							KAudioPlayer::play(soundFile);
					}
					if (Prefs::wonDialog())
					{
						QTimer::singleShot( 3*1000, this, SLOT(slotNewGame()) );
					}
					else{
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
				if (missedChar<5)
				  missedL=missedL.replace(2*missedChar, 1, sChar);
				else if(missedChar>5)
				  missedL=missedL.replace((2*missedChar)+2, 1, sChar);

				if (missedChar==5) //we actually need to relace one underscore too much..
				{
					missedL=missedL.replace((2*missedChar)+1,1, "\n"+sChar+" ");
					missedL=missedL.replace(22,2, "");
				}

				missedLetters->setText(missedL);
				pixImage->setPixmap(px[missedChar+1]);
				missedChar++;

				if (missedChar >= 10) //you are hanged!
				{
					//we reset everything...
				        pixImage->setPixmap(px[9]);
					//um... The word is not guessed... Let's show it...
					QStringList charList=QStringList::split("",word);
					QString theWord=charList.join(" ");
					if (language =="de")
						theWord = theWord.replace(0,1, theWord.left(1).upper());
					mainLabel->setText(theWord);
					
					//usability: change that
					QString newGameString;
					if (Prefs::milder()) 
						newGameString = i18n("You lost. Do you want to play again?");
					else
						newGameString = i18n("You are dead. Do you want to play again?");
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
			if (missedL.contains(sChar)>0)
			{
				KPassivePopup *popup = new KPassivePopup( missedLetters, "popup" );
  				popup->setAutoDelete( true );
  				popup->setTimeout( 1000 );
				popup->setView(i18n("This letter has already been guessed.") );
				popup->show();
				int redIndex = missedL.find(sChar,0);
				//missedLetters->setTextFormat( QLabel::RichText );
				missedLetters->setText(missedL.left(redIndex)+"<font color=\"#ff0000\">"+missedL[redIndex]+"</font>"+missedL.right(missedL.length()-redIndex-1));
				//disable any possible entry
				charWrite->setEnabled(false);
				//put the letter in red for 1 second
				QTimer *timer = new QTimer( this);
        			connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
        			timer->start( 1000, TRUE ); // 1 second single-shot timer
			}
			//usability: hilight it in the word
			if (goodWord.contains(sChar)>0)
			{
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
				mainLabel->setText(goodWord.left(redIndex)+"<font color=\"#ff0000\"><b>"+goodWord[redIndex]+"</b></font>"+goodWord.right(goodWord.length()-redIndex-1));
				mainLabel->setAlignment( int( QLabel::AlignCenter ) );
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
	missedLetters->setTextFormat( QLabel::AutoText);
	missedLetters->setText(missedL);
	charWrite->setEnabled(true);
	charWrite->setFocus();
}

void KHangManView::timerDoneWord()
{
	mainLabel->setText(goodWord);
	charWrite->setEnabled(true);
	charWrite->setFocus();
}

void KHangManView::replaceLetters(QString sChar)
{
	//replace letter in the word
	int index=0;
	bool b_end = false;
	if (b_oneLetter) //we just replace the next instance
	{
		for (int count=0; count <word.contains(sChar); count++)
		{
			index = word.find(sChar,index);
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
		for (int count=0; count <word.contains(sChar); count++)
		{
			//searching for letter location
			index = word.find(sChar,index);
			//we replace it...
			goodWord.replace((2*index), 1,sChar);
			index++;
		}
	}
	if (m_accent && !accent_b)
	{
		//characters must be lisible as ISO 8859-1 and file must be saved with this encoding. 
		if (sChar=="i") replaceLetters(QString("í"));
		if (sChar=="a") replaceLetters(QString("à"));
		if (sChar=="a") replaceLetters(QString("á"));
		if (sChar=="a") replaceLetters(QString("ã"));
		if (sChar=="u") replaceLetters(QString("ü"));
		if (sChar=="o") replaceLetters(QString("ò"));
		if (sChar=="o") replaceLetters(QString("ó"));
		if (sChar=="o") replaceLetters(QString("õ"));
		if (sChar=="e") replaceLetters(QString("è"));
		if (sChar=="e") replaceLetters(QString("é¨"));
		if (sChar=="u") replaceLetters(QString("ù"));
	}
	if (!b_oneLetter) allWords << sChar; //appends the list only if not in One Letter only mode...
	if (word.contains(sChar)==1) allWords << sChar; //append if only one instance
	if (b_oneLetter && b_end) allWords << sChar; 
}

bool KHangManView::containsChar(QString &sChar)
{
	bool b = false;
	
	if (m_accent && !accent_b)
	{
		if (sChar=="i") b = word.contains(QString("í")) > 0;
		if (sChar=="a") b = word.contains(QString("à")) > 0 || word.contains(QString("á")) > 0 || word.contains(QString("ã")) > 0;
		if (sChar=="u") b = word.contains(QString("ü")) > 0 || word.contains(QString("ù")) > 0;
		if (sChar=="o") b = word.contains(QString("ò")) > 0 || word.contains(QString("ó")) || word.contains(QString("õ")) > 0;
		if (sChar=="e") b = word.contains(QString("è")) > 0 || word.contains(QString("é")) > 0;
	}
	
	return ((word.contains(sChar) > 0) || b);
}

void KHangManView::game()
{
	pixImage->setPixmap(px[10]);
	//if the data files are not installed in the correct dir
	QString myString=QString("khangman/data/%1/%2").arg(language).arg(levelFile);
	QFile myFile;
	myFile.setName(locate("data",myString));
        if (!myFile.exists())
        {
		QString mString=i18n("File $KDEDIR/share/apps/khangman/data/%1/%2 not found!\n"
                                     "Check your installation, please!").arg(language).arg(levelFile);
		KMessageBox::sorry( this, mString,
		                    i18n("Error") );
		exit(1);
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
		emit(signalKvtml(true));
		readFile();
		}
	else {
		if (allData.count()<=1) {
			emit(signalChangeLanguage(3));
			return;
			}
		emit(signalKvtml(false));
		//now the calculations...
		int objects = allData.count();//number of words in the file
		word ="";
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
	kdDebug() << "length: " <<  word.length() << endl;
	goodWord ="";
	mainLabel->setText(goodWord);
	//display the number of letters to guess with _
	for(unsigned int i = 0; i < word.length(); i++)
	{
		goodWord.append("_");
		goodWord.append(" ");
	}
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
	mainLabel-> setText(goodWord);//display the _
	mainLabel->setAlignment(AlignCenter|AlignCenter);
}

void KHangManView::wipeout()
{
	goodWord="";
	charWrite->setText("");
	missedChar=0;
	missedLetters->setText("_ _ _ _ _  \n_ _ _ _ _  ");
	allWords.clear();
}

void KHangManView::resizeEvent(QResizeEvent *)
{
	if(!bgPixmap.isNull())
		slotSetPixmap(bgPixmap);
}

void KHangManView::slotSetPixmap(QPixmap& bgPix)
{
	QImage img = bgPix.convertToImage();
	QPixmap bg(size());
	QColor myColor;
	if (Prefs::blue()) {
		myColor=Qt::white;
		guessButton->setPaletteBackgroundColor( QColor( 24, 165, 16 ) );
	}
	else
	if (Prefs::nature()) {
		myColor=Qt::black;
		guessButton->setPaletteBackgroundColor( QColor( 32, 141, 16 ) );
	}
	missedLetters->setPaletteForegroundColor(myColor);
	TextLabel1->setPaletteForegroundColor(myColor);
	TextLabel2->setPaletteForegroundColor(myColor);
	TextLabel3->setPaletteForegroundColor(myColor);
	mainLabel->setPaletteForegroundColor(myColor);
	bg.convertFromImage(img.smoothScale( width(), height()));
	setPaletteBackgroundPixmap(bg);
	Frame11->setPaletteBackgroundPixmap(bg);
	TextLabel1->setPaletteBackgroundPixmap(bg);
	TextLabel2->setPaletteBackgroundPixmap(bg);
	TextLabel3->setPaletteBackgroundPixmap(bg);
	mainLabel->setPaletteBackgroundPixmap(bg);
	missedLetters->setPaletteBackgroundPixmap(bg);
        pixImage->setPaletteBackgroundPixmap(bg);
	charWrite->setFocus();
	bgPixmap = bgPix;
}

void KHangManView::slotNoBkgd()
{
	setBackgroundColor("#DCDCDC");
	TextLabel1->setBackgroundColor("#DCDCDC");
	TextLabel2->setBackgroundColor("#DCDCDC");
	TextLabel3->setBackgroundColor("#DCDCDC");
	mainLabel->setBackgroundColor("#DCDCDC");
	missedLetters->setBackgroundColor("#DCDCDC");
	Frame11->setBackgroundColor("#DCDCDC");
	pixImage->setBackgroundColor("#ECECEC");
	charWrite->setFocus();
	missedLetters->setPaletteForegroundColor(Qt::black);
	TextLabel1->setPaletteForegroundColor(Qt::black);
	TextLabel2->setPaletteForegroundColor(Qt::black);
	TextLabel3->setPaletteForegroundColor(Qt::black);
	mainLabel->setPaletteForegroundColor(Qt::black);
	bgPixmap.resize(0,0);
	guessButton->setPaletteBackgroundColor("#DCDCDC");
}

void KHangManView::slotMilder()
{
	if (Prefs::milder())
	{
		px[6].load(locate("data","khangman/pics/hg7c.png"));
		px[7].load(locate("data","khangman/pics/hg8c.png"));
		px[8].load(locate("data","khangman/pics/hg9c.png"));
		px[9].load(locate("data","khangman/pics/hg10c.png"));
	}
	else
	{
		px[6].load(locate("data","khangman/pics/hg7.png"));
		px[7].load(locate("data","khangman/pics/hg8.png"));
		px[8].load(locate("data","khangman/pics/hg9.png"));
		px[9].load(locate("data","khangman/pics/hg10.png"));
	}
	//update the pic immediatly
	if (missedChar==0)
		pixImage->setPixmap(px[10]);
	else
		pixImage->setPixmap(px[missedChar]);

}

void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
	if (kvtmlBool && hintBool && (mouse->button() == RightButton))
	{
		KPassivePopup *myPopup = new KPassivePopup( Frame11);
		myPopup->setView(i18n("Hint"), tip );
		myPopup->setPalette(QToolTip::palette());
		myPopup->setTimeout(4000); //show for 4 seconds
		myPopup->show();	
	}
        update();//this is nice!
}

void KHangManView::readFile()
{
	QString myString=QString("khangman/data/%1/%2").arg(language).arg(levelFile);
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
}


#include "khangmanview.moc"
