/*
 * Copyright (C) 2003 Anne-Marie Mahfouf <annma@kde.org>
 */

//project headers
#include "khangmanview.h"
#include "version.h"
//Qt headers
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextstream.h>
//KDE headers
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
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
	px[0].load(locate("data","khangman/pics/hg1.jpg"));
	px[1].load(locate("data","khangman/pics/hg2.jpg"));
	px[2].load(locate("data","khangman/pics/hg3.jpg"));
	px[3].load(locate("data","khangman/pics/hg4.jpg"));
	px[4].load(locate("data","khangman/pics/hg5.jpg"));
	px[5].load(locate("data","khangman/pics/hg6.jpg"));
	px[6].load(locate("data","khangman/pics/hg7.jpg"));
	px[7].load(locate("data","khangman/pics/hg8.jpg"));
	px[8].load(locate("data","khangman/pics/hg9.jpg"));
	px[9].load(locate("data","khangman/pics/hg10.jpg"));
	px[10].load(locate("data","khangman/pics/hg12.jpg"));
	px[11].load(locate("data","khangman/pics/hg13.jpg"));
	px[12].load(locate("data","khangman/pics/hg14.jpg"));

	bluePix = QPixmap(QPixmap(locate("data","khangman/pics/blue.png") ) );
	naturePix = QPixmap(QPixmap(locate("data","khangman/pics/nature.png") ) );
	//put some warning if background file is not found

	pixImage->setFixedSize(190, 230);
	mainLabel->setMinimumSize(380, 95);
	//setMinimumSize(560, 430);

	QObject::connect( charWrite, SIGNAL( returnPressed() ), this, SLOT( slotTry() ) );
}

KHangManView::~KHangManView()
{
}


void KHangManView::slotSetTitle(const QString& title)
{
    emit signalChangeCaption(title);
}

void KHangManView::slotNewGame()
{
	wipeout();
	game();
	charWrite->setFocus();
}

void KHangManView::slotTry()
{
	QString sChar = charWrite->text();
	missedL= missedLetters->text();
	//if contains more than 1 character...
	if (!sChar.isEmpty() && sChar.toInt() ==0 && sChar != "0") //it won't react to empty box, neither if someone enters number...
	{
		if (allWords.contains(sChar) == 0)
		{
			if (word.contains(sChar) > 0)
			{
				int index=0;
				for (int count=0; count <word.contains(sChar); count++)
				{
					//searching for letter location
					index = word.find(sChar,index);
					//we replace it...
					goodWord.replace((2*index), 1,sChar);
					index++;
				}
				QStringList rightChars=QStringList::split(" ", goodWord, true);
				QString rightWord= rightChars.join("");
				mainLabel->setText(goodWord);
				allWords << sChar; //appends the list...
				if (rightWord.stripWhiteSpace() == word.stripWhiteSpace()) //you made it!
				{
					//we reset everything...
					pixImage->setPixmap(px[12]);
					if (KMessageBox::questionYesNo(this, i18n("Congratulations! You won! Do you want to play again?")) == 3)
					{
						slotNewGame();
					}
					else
					{
						kapp->quit();
					}
				}
			}
			else //if the char is missed...
			{
				allWords << sChar; //appends the list...
				if (missedChar<6)
				missedL=missedL.replace(2*missedChar,1, sChar);
				else if(missedChar>6)
				missedL=missedL.replace((2*missedChar)+1,1, sChar);

				if (missedChar==6) //we actually need to relace one underscore too much..
				{
					missedL=missedL.replace(2*missedChar,1, "\n"+sChar+" ");
					missedL=missedL.replace(24,2, "");
				}

				missedLetters->setText(missedL);
				pixImage->setPixmap(px[missedChar]);
				missedChar++;
				if (missedChar >= 11) //you are hanged!
				{
					//we reset everything...
				    pixImage->setPixmap(px[11]);
					//um... The word is not guessed... Let's show it...
					QStringList charList=QStringList::split("",word);
					QString theWord=charList.join(" ");
					mainLabel->setText(theWord);

					if (KMessageBox::questionYesNo(this, i18n("You are dead. Do you want to play again?")) == 3)
					{
						slotNewGame();
					}
					else
					{
						kapp->quit();
					}
				}
			}
		}
		else //do something drastic... Word has already been guessed...
		{
			KMessageBox::information (this, i18n("The letter has already been guessed."));
		}
	}

//reset after guess...
	charWrite->setText("");

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
		                    i18n("KHangMan - Error") );
		exit(1);
	}

	//we open the file and store info into the stream...
	QFile openFileStream(locate("data","khangman/data/")+language+"/"+levelFile);
	openFileStream.open(IO_ReadOnly);
	QTextStream readFileStr(&openFileStream);
	//allData contains all the words from the file
	QStringList allData=QStringList::split("\n", readFileStr.read(), true);
	openFileStream.close();
	  /*
	  // debug to see if the QStringList contains all words
	  for ( QStringList::Iterator it = allData.begin(); it != allData.end(); ++it ) {
          kdDebug() << *it << ":"<<endl;
          }*/

	//now the calculations...
	int objects = allData.count();//number of words in the file
	//picks a random word from allData
	word = allData[random.getLong(objects)]; //gives us a single word...
	  //debug to see what word is picked
	  //kdDebug() << word << endl;
	// int wrdLen=word.length(); //the length of the word...
	if (word.stripWhiteSpace().isEmpty()) //prevents to display the empty places...
	{
		slotNewGame();
	}
	//display the number of letters to guess with _
	for(unsigned int i = 0; i < word.length(); i++)
	{
		goodWord.append("_ ");
	}
	mainLabel-> setText(goodWord);//display the _
}

void KHangManView::wipeout()
{
	goodWord="";
	missedChar=0;
	missedLetters->setText("_ _ _ _ _ _ \n_ _ _ _ _ _ ");
	allWords.clear();
}

void KHangManView::slotBlue(QPixmap& bgPix)
{
	setBackgroundPixmap(bgPix);
	TextLabel1->setBackgroundPixmap(bgPix);
	TextLabel2->setBackgroundPixmap(bgPix);
	TextLabel3->setBackgroundPixmap(bgPix);
	mainLabel->setBackgroundPixmap(bgPix);
	missedLetters->setBackgroundPixmap(bgPix);
	Frame11->setBackgroundPixmap(bgPix);
	charWrite->setFocus();
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
	charWrite->setFocus();
}



#include "khangmanview.moc"
