/***************************************************************************
                          khangman.cpp  -  description
                             -------------------
    begin                : Thu Jul 19 16:42:53 EDT 2001
    copyright            : (C) 2001 by Anne-Marie Mahfouf
    email                : a-m.mahfouf@lineone.net
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "khangman.h"
#include <klocale.h>
#include <kdebug.h>
#include "khangman.moc"

KHangMan::KHangMan(QWidget *parent, const char *name) : MainW(parent, name)
{
//set the usuall stuff...
	setIcon(locate("icon","hicolor/16x16/apps/khangman.png"));
	setCaption(i18n("KHangMan %1").arg(KHM_VERSION));
	missedChar = 0;

	
//plugs in the Help menu...
	helpMenu = new KHelpMenu(this, KGlobal::instance()->aboutData(), true);
	btnHelp->setPopup(helpMenu->menu());

//focus is now set to the letter pad...
	charWrite->setFocus();
	
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

//...and we se the first image into the pixmap container...
	pixImage->setPixmap(px[10]);

//start with the game...
	goodWord="";
	game();
}

void KHangMan::game()
{
pixImage->setPixmap(px[10]);
//code from previous KHM....
	switch(boxLevel->currentItem())
	{
		default: 0;
		case 0: //easy
			kdDebug() << "Easy!" << endl;
			levelFile="easy.txt";
		break;

		case 1: //animals
			kdDebug() << "Animals" << endl;
			levelFile="animals.txt";
		break;

		case 2: //medium
			kdDebug() << "Medium!" << endl;
			levelFile="medium.txt";
		break;

		case 3://hard
			kdDebug() << "Hard!" << endl;
			levelFile="hard.txt";
		break;
	}

//we open the file and store info into the stream...
	QFile openFileStream(locate("data","khangman/data/")+levelFile);
	openFileStream.open(IO_ReadOnly);
	QTextStream readFileStr(&openFileStream);
//	kdDebug() << readFileStr.read() << endl;
	QStringList allData=QStringList::split("\n", readFileStr.read(), true);
	openFileStream.close();

//now the calculations...
	int objects = allData.count();
	word = allData[random.getLong(objects)]; //gives us a single word...
	int wrdLen=word.length(); //the length of the word...
	if (word.stripWhiteSpace() == "") //prevents to display the empty places...
	{
		slotNewGame();
	}
//engine...
	for(unsigned int i = 0; i < word.length(); i++)
	{
		goodWord.append("_ ");
	}

	kdDebug() << word << endl;
	mainLabel-> setText(goodWord);
}

void KHangMan::slotGetLevel(int level)
{
	slotNewGame();
}

void KHangMan::slotNewGame()
{
	wipeout();
	game();
}

void  KHangMan::slotClose()
{
	kapp->quit(); //this closes ONLY this widget... QCloseEvent kills the app...
}

//display a screen with a quick information
void KHangMan::slotInfo()
{
	Info * info;
	info=new Info();
	info->show();
}

void KHangMan::slotTry()
{
	QString sChar = charWrite->text();
	missedL=missedLetters->text();

//if contains more than 1 character...
	if (sChar!="") //it won't react to empty box...
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
				if (rightWord.stripWhiteSpace() == word.stripWhiteSpace()) //you are hanged!
				{
					//we reset everything...
					pixImage->setPixmap(px[12]);
					if (KMessageBox::questionYesNo(this, "Congratulations! You won!... Wanna play again?") == 3)
					{
						slotNewGame();
					}
					else
					{
						close();
					}
				}
			}
			else //if the char is missed...
			{
				allWords << sChar; //appends the list...
				missedL=missedL.replace(2*missedChar,1, sChar);
				if (missedChar==6)
				missedL=missedL.replace(2*missedChar,1, sChar+"\n");
				
				missedLetters->setText(missedL);
				pixImage->setPixmap(px[missedChar]);
				missedChar++;
				if (missedChar >= 11) //you are hanged!
				{
					//we reset everything...
					pixImage->setPixmap(px[11]);
					if (KMessageBox::questionYesNo(this, "You are dead... Wanna play again?") == 3)
					{
						slotNewGame();
					}
					else
					{
						close();
					}
				}
				kdDebug() << missedChar << endl;
			}
		}
		else //do something drastic... Word has already been guessed...
		{
			KMessageBox::information (this, "The letter has already been guessed...");
		}
	}
	kdDebug() <<word.contains(sChar) << endl;

//reset after guess...
	charWrite->setText("");
}

KHangMan::~KHangMan()
{
}

void KHangMan::closeEvent(QCloseEvent *)
{
	exit(0);
}

void KHangMan::wipeout()
{
	//this is odd... This won't work in front of the game() function :(
	goodWord="";
	missedChar=0;
	missedLetters->setText("_ _ _ _ _ _ \n_ _ _ _ _ _ _");
	allWords.clear();
}