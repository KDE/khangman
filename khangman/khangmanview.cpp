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
#include "khangmanview.h"
#include "version.h"
//Qt headers
#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtooltip.h>
//#include <qwhatsthis.h>
//KDE headers
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <kstandarddirs.h>
//standard C++ headers
#include <stdlib.h>


KHangManView::KHangManView(QWidget *parent, const char *name)
    : MainW(parent, name)
{
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
	tip="";

	connect( charWrite, SIGNAL( textChanged(const QString &) ), this, SLOT( slotValidate(const QString &) ) );
	connect( charWrite, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
}

KHangManView::~KHangManView()
{
}

void KHangManView::slotNewGame()
{
	wipeout();
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

					if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?")) == 3)
						slotNewGame();
					else
						kapp->quit();
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

					if (KMessageBox::questionYesNo(this, i18n("You are dead. Do you want to play again?")) == 3)
						slotNewGame();
					else
						kapp->quit();
				}
			}
		}
		else
			KMessageBox::information (this, i18n("The letter has already been guessed."));
	}
	//reset after guess...
	charWrite->setText("");
}

void KHangManView::replaceLetters(QString sChar)
{
  //replace letter in the word
  int index=0;
  for (int count=0; count <word.contains(sChar); count++)
  {
    //searching for letter location
    index = word.find(sChar,index);
    //we replace it...
    goodWord.replace((2*index), 1,sChar);
    index++;
  }
  if ((language=="es" || language == "pt" || language == "ca") && (accent_b==true))
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
	allWords << sChar; //appends the list...
}

bool KHangManView::containsChar(QString &sChar)
{
  bool b = false;

  if ((language=="es" || language =="pt" || language == "ca") && (accent_b==true))
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
	QFile openFileStream(locate("data","khangman/data/")+language+"/"+levelFile);
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
			word = word.lower(); //because of German
		}//end else if lanlanguage=fr
	//kdDebug() << word << endl;
	goodWord ="";
	mainLabel->setText(goodWord);
	//display the number of letters to guess with _
	for(unsigned int i = 0; i < word.length(); i++)
	{
		goodWord.append("_");
		goodWord.append(" ");
	}
	stripWord=goodWord;
	//if needed, display white space or - if in word or semi dot
	c = word.find( "-" );
	if (c>0)
		goodWord.replace(2*c, 1, "-");
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

	bg.convertFromImage(img.smoothScale( width(), height()));
	setPaletteBackgroundPixmap(bg);
	Frame11->setPaletteBackgroundPixmap(bg);
	TextLabel1->setPaletteBackgroundPixmap(bg);
	TextLabel2->setPaletteBackgroundPixmap(bg);
	TextLabel3->setPaletteBackgroundPixmap(bg);
	mainLabel->setPaletteBackgroundPixmap(bg);
	missedLetters->setPaletteBackgroundPixmap(bg);
	if (transparent)
		pixImage->setPaletteBackgroundPixmap(bg);
	else
		pixImage->setBackgroundColor("#ECECEC");
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
	bgPixmap.resize(0,0);
}

void KHangManView::slotTransparent()
{
	if (bgPixmap.isNull())
		slotNoBkgd();
	else
		slotSetPixmap(bgPixmap);
}

void KHangManView::slotSofter()
{
	if (softer)
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
	pixImage->setPixmap(px[missedChar]);

}

//Tip on right-click
void KHangManView::mousePressEvent(QMouseEvent *mouse)
{
	if (kvtmlBool && hintBool && (mouse->button() == RightButton))
	{
		myPopup = new KPassivePopup(TextLabel3);
		myPopup->setView(i18n("Hint"), tip );
		myPopup->setPalette(QToolTip::palette());
		myPopup->setTimeout(4000); //show for 4 seconds
		myPopup->show();
	}
        update();//this is nice!
}

///enable tips
void KHangManView::readFile()
{
	///TODO: be sure it's opened in UNICODE
	KEduVocDataItemList verbs = KEduVocData::parse(locate("data","khangman/data/")+language+"/"+levelFile);
	///how many words in the file
	int NumberOfWords = verbs.count();
	//pick a number in random
	int wordNumber = random.getLong(NumberOfWords);
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
