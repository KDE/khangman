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
#include <string>
#include <time.h>
#include <stdlib.h>
#include <qtimer.h>
#include <klocale.h>
#include <qabstractlayout.h>

#include "khangman.moc"
#define KHM_VERSION 0.71
//rd=random number <=30
//k to position the labels with letters
//z and y1 to positionne the labels with guesses letters
//te : count the number of letters which are shown
//i=word lenght
//inc : integer to increment the array of labels for guessed letters
//let1 : QString for entered text
//a1 : char for let1
//s[20] : store each letter of the displayed word
//st : store the randomly choosen word
//str[30] : store the words for level 'easy'
//sto[26] store the already guessed letters

int  rd, k, z, y1, te, length, inc, b, u, ind=0, c0, c1, drap, drap2;
QString let1, a1, s[20],st, str[45], sto[26];


KHangMan::KHangMan(QWidget *parent, const char *name) : QWidget(parent, name)
{
	setCaption(i18n("KHangMan - v%1").arg(KHM_VERSION));
	//main layout
	QGridLayout *Form1Layout = new QGridLayout( this );
	Form1Layout->setSpacing( 6 );
	Form1Layout->setMargin( 11 );

	QHBoxLayout *Layout4 = new QHBoxLayout;
	Layout4->setSpacing( 6 );
	Layout4->setMargin( 0 );

	help = new QPushButton(i18n( "Help"), this,"help" );
	help->setGeometry
	(10,350, 60, 40);
	help->setFont(QFont("Helvetica", 20, QFont::Bold));
	QToolTip::add( help,i18n( "The KHangMan HandBook") );
	//accel2 : enable F1 for 'Help'
	//refused in porting to kde2
	//accel2=new KAccel(this);
	//accel2->connectItem(KAccel::Help, this , SLOT(slotHelp()));
	Layout4->addWidget( help );

	info = new QPushButton(i18n( "Info"), this,"info" );
	info->setFont(QFont("Helvetica", 20, QFont::Bold));
	QToolTip::add( info,i18n( "Quick Information") );
	QObject::connect (info, SIGNAL(clicked()), this, SLOT(slotInfo()));
	Layout4->addWidget( info);

	level = new QComboBox( "level",this);
	level->insertItem( i18n("Level " ) );
	level->insertItem( i18n("Easy") );
	level->insertItem( i18n("Animals"));
	level->insertItem( i18n("Medium" ));
	level->insertItem( i18n("Hard" ));
	QFont f("Helvetica", 20, QFont::Bold);
	level->setGeometry
	(80,350, 110, 40);
	level->setFont( f );
	QToolTip::add( level,i18n( "Choose easy, animals, medium or hard") );
	QObject::connect( level, SIGNAL(activated(int)),SLOT(choice(int)));
	Layout4->addWidget( level );

	quit = new QPushButton(i18n( "Quit"), this,"quit" );
	quit->setFont(QFont("Helvetica", 20, QFont::Bold));
	QToolTip::add( quit,i18n( "Quit the game") );
	QObject::connect (quit, SIGNAL(clicked()), this, SLOT(slotquit()));
	Layout4->addWidget( quit );

	QSpacerItem* spacer4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding,
	                                        QSizePolicy::Minimum );
	Layout4->addItem( spacer4 );

	Form1Layout->addLayout( Layout4, 2, 0 );

	QHBoxLayout *Layout8 = new QHBoxLayout;
	Layout8->setSpacing( 6 );
	Layout8->setMargin( 0 );

	QVBoxLayout *Layout6 = new QVBoxLayout;
	Layout6->setSpacing( 6 );
	Layout6->setMargin( 0 );

	QHBoxLayout *Layout19 = new QHBoxLayout;
	Layout19->setSpacing( 2 );
	Layout19->setMargin( 0 );

	lab2 = new QLabel(i18n(" The letters below have already\n"
	                       " been guessed :"), this);
	lab2->setFont(QFont("Helvetica", 12, QFont::Bold));
	lab2->setMinimumSize( QSize( 200, 70 ) );
	lab2->setMaximumSize( QSize( 200, 70 ) );
	lab2->setPalette( QPalette( QColor(white)));
	Layout19->addWidget(lab2);

	QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Minimum,
	                        QSizePolicy::Expanding );
	Layout19->addItem( spacer_9 );
	Layout6->addLayout( Layout19);

	QGridLayout *Layout5 = new QGridLayout;
	Layout5->setSpacing( 6 );
	Layout5->setMargin( 0 );

	for (int i=0;i<9;i++)
	{
		lab3[i] = new QLabel(this);
		lab3[i]->setPalette( QPalette( QColor(cyan)));
		lab3[i]->setMinimumSize( QSize( 30, 50 ) );
		lab3[i]->setMaximumSize( QSize( 60, 70 ) );
		lab3[i]->setFont(QFont("Times", 30, QFont::Bold));
		Layout5->addWidget(lab3[i]  , 0,i);
	}
	for (int i=0;i<9;i++)
	{
		lab3[i+9] = new QLabel(this);
		lab3[i+9]->setPalette( QPalette( QColor(cyan)));
		lab3[i+9]->setMinimumSize( QSize( 30, 50 ) );
		lab3[i+9]->setMaximumSize( QSize( 60, 70 ) );
		lab3[i+9]->setFont(QFont("Times", 30, QFont::Bold));
		Layout5->addWidget(lab3[i+9]  , 1,i);
	}

	Layout6->addLayout( Layout5 );
	Layout8->addLayout( Layout6 );

	QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	Layout8->addItem( spacer_8 );

	lab8 = new QLabel( this, "lab8" );
	Layout8->addWidget( lab8 );

	Form1Layout->addLayout( Layout8, 1, 0 );

	QVBoxLayout *Layout11 = new QVBoxLayout;
	Layout11->setSpacing( 6 );
	Layout11->setMargin( 0 );

	QHBoxLayout *Layout2 = new QHBoxLayout;
	Layout2->setSpacing( 6 );
	Layout2->setMargin( 0 );

	for (int i=0; i<12;i++)
	{
		but[i]= new QLabel( this);
		but[i]->setFont(QFont("Times", 30, QFont::Bold));
		guess[i]=new QLabel(but[i]);
		but[i]->setMinimumSize( QSize( 30, 70 ) );
		guess[i]->setFont(QFont("Times", 30, QFont::Bold));
		Layout2->addWidget( but[i] );
	}
	Layout11->addLayout( Layout2 );

	QHBoxLayout *Layout3 = new QHBoxLayout;
	Layout3->setSpacing( 6 );
	Layout3->setMargin( 0 );

	lab1 = new QLabel(i18n(" Please enter a letter :"), this);
	lab1->setFont(QFont("Helvetica", 12, QFont::Bold));
	lab1->setPalette( QPalette( QColor(white)));
	Layout3->addWidget( lab1 );

	QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding,
	                        QSizePolicy::Minimum );
	Layout3->addItem( spacer_3 );
	Layout11->addLayout( Layout3 );

	QHBoxLayout *Layout10 = new QHBoxLayout;
	Layout10->setSpacing( 6 );
	Layout10->setMargin( 0 );

	line=new QLineEdit(this);
	line->setMinimumSize( QSize( 100, 50 ) );
	line->setMaximumSize( QSize( 100, 50 ) );
	line->setCursorPosition(10);
	line->setMaxLength(1);
	line->setFont(QFont("Times", 30, QFont::Bold));
	QToolTip::add( line,i18n( "Enter a letter") );
	Layout10->addWidget( line );

	lab4 = new QLabel(i18n(" Another word ?"), this);
	lab4->setFont(QFont("Times", 28, QFont::Bold));
	lab4->hide();
	Layout10->addWidget( lab4 );

	lab7 = new QPushButton(i18n("Yes"), this);
	lab7->setFont(QFont("Times", 28, QFont::Bold));
	lab7->hide();
	Layout10->addWidget(lab7);

	lab6 = new QPushButton(i18n("No"), this);
	lab6->setFont(QFont("Times", 28, QFont::Bold));
	lab6->hide();
	Layout10->addWidget(lab6);

	QSpacerItem* spacer_4 = new QSpacerItem( 20, 100, QSizePolicy::Expanding, QSizePolicy::Maximum );
	Layout10->addItem( spacer_4 );

	Layout11->addLayout( Layout10 );

	Form1Layout->addLayout( Layout11, 0, 0 );

	QObject::connect (line, SIGNAL(returnPressed()), this, SLOT(slotTreat()));
	QObject::connect (help, SIGNAL(clicked()), this, SLOT(slotHelp()));

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
//	px[10].load(locate("data","khangman/pics/hg11.gif"));
	px[11].load(locate("data","khangman/pics/hg12.jpg"));
	px[12].load(locate("data","khangman/pics/hg13.jpg"));
	px[13].load(locate("data","khangman/pics/hg14.jpg"));
	ind=1;

	if (px[11].isNull()==true)
	{
		KMessageBox::sorry( this, i18n("The first picture was not found.\n \nConfigure with the right prefix please!"),
		                    "KHangman - Error" );
		exit(1);
	}

	lab8->setPixmap(px[11]);//display the presentation picture
	game();
}


void KHangMan::choice(int index)
{
	for (int jj=0; jj<length;jj++)
		if (but[jj]->isVisible()==true)
		{
			but[jj]->hide();
			but[jj]->setText("");
			guess[jj]->setText("");
		}

	if  (drap==1) //te
	{
		for (int js=0; js<=inc;js++)
			lab3[js]->hide();
	}
	if (drap2==1)  //(lab4->isVisible()==true)
	{
		lab4->hide();
		lab6->hide();
		lab7->hide();
	}

	for (int y=0;y<=inc;y++)
		sto[y]="";

	switch ( index)
	{
	default:
	case 1:
		ind=1;
		break;

	case 2:
		ind=2;
		break;

	case 3:
		ind=3;
		break;

	case 4:
		ind=4;
		break;
	}
	game();
}

void KHangMan::game()
{
	lab8->setPixmap(px[11]);//display the presentation picture
	lab8->show();

	y1=250 ;
	te=0;
	length=0;
	k=10;
	z=10 ;
	inc=0;
	b=0;
	u=0;
	c0=0;
	c1=0;
	drap=0;
	drap2=0;
	//seed random function
	srand((unsigned int)time((time_t *)NULL));
	rd=rand()%44;  //pick a random number

	//02 char name[20];  //store the letters of the word
	QString name;  //store the letters of the word
	switch (ind)
	{
	case 1:
		file1="khangman/data/easy.txt";
		break;
	case 2:
		file1="khangman/data/level3.txt";
		break;
	case 3:
		file1="khangman/data/level2.txt";
		break;
	case 4:
		file1="khangman/data/level4.txt";
		break;
	}
	choice1();


	line->setFocus();  //enable keyboard focus for 'line'
	//02 strncpy(name,st.latin1(),19);//put the word into 'name'
	//02 QString sg;
	//02 sg.sprintf("_");
	//02frerich i=strlen(st.latin1());   //i = lenght of the word
	name = st; //put the word into 'name'
	QString sg("_");
	length=st.length();   //i = lenght of the word
	for (int j=0; j<length;j++)
	{
		//02 QString s[20];
		//02 s[j].sprintf("%c",name[j]);
		QString s(name[j]); //read the j th letter
		//02 but[j]->setText(s[j]);
		but[j]->setText(s);
		guess[j]->setText(sg);
		but[j]->show();
	}
	for (int i=0;i<12;i++)
	{
		but[i]->show();
		if (but[i]->isVisible()==true)
			guess[i]->show();
	}

	resize(720,400);
}


void KHangMan::slotNext()
{
	lab4->setText(i18n(" Another word?"));
	lab4->show();
	drap2=1;
	lab7->show();
	lab6->show();
	line->clearFocus();
	QObject::connect (lab7, SIGNAL(clicked()), this, SLOT(slotNewgame()));
	QObject::connect (lab6, SIGNAL(clicked()), this, SLOT(slotquit()));
}

void KHangMan::slotTreat()
{
	let1 = line->text();
	line->selectAll();
	line->cut();
	a1.sprintf(let1.latin1());
	//if (a1="")
	drap=1;
	char name[20];
	strncpy(name,st.latin1(),19);
	//QString name;
	//name = st;
	c1=0;
	b=0;
	for (int j=0; j<20;j++)
	{
		//QString s(name[j]);
		// if (!s)
		if (!name[j])
			break;
		s[j].sprintf("%c",name[j]);
		if (s[j]==a1)
			// if (s==a1)
		{
			guess[j]->hide();
			c1=1;
		}
	}

	if (te>0)
	{
		for (int kl=0;kl<te+1;kl++)
			if (sto[kl]==a1)
			{
				warning();
				b=1;
			} //flag =1 if letter already guessed =0 otherwise
	}
	//if letter not already guessed and letter not in word

	if (( b==0 )&&( c1==0 ))
		slotShow();

	if (( b==0 )&&( c1==1 ))
	{
		sto[te]=a1;
		te=te+1;
	}
	//test for displaying 'another' message
	for (int y=0; y<20; y++ )
	{
		if ( !name[y] )
			//QString s(name[y]);
			// if (!s)
			break;
		if (guess[y]->isVisible()==false)
			u=u+1;
		if ((u==length))  //wait before displaying 'Another word'
		{
			if (c0<11)
			{
				lab8->setPixmap(px[13]);//display the 'Well Done' picture
				lab8->show();
			}
			{
				QTimer *timer = new QTimer(this);
				connect( timer, SIGNAL(timeout()),
				         this, SLOT(slotNext()) );
				timer->start( 2000, TRUE );
			}
		}
	}
	u=0;
}



void KHangMan::slotShow()
{
	lab3[inc]->setText(a1);
	lab3[inc]->show();

	lab8->setPixmap(px[c0]);  //display pictures of the hangman
	lab8->show();

	z=z+20;
	if (z>220)
	{
		z=10;
		y1=290;
	}

	sto[te]=a1;
	te=te+1;
	inc=inc+1;
	c0=c0+1;

	//if the game is lost
	if (c0==11)
	{
		lab8->setPixmap(px[c0+1]); //dislay 'game over' picture
		lab8->show();
		for (int j=0; j<length;j++) //show word
		{
			//  if (but[j]->isVisible()==false)
			guess[j]->hide();
		}
	}
}

void KHangMan::warning()
{
	lab4->setText(" Already guessed !!!");
	lab4->show();
	QTimer *timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()),
	         lab4, SLOT(hide()) );
	timer->start( 1000, TRUE );
}

//prepare for a new game
void KHangMan::slotNewgame()
{
	for (int j=0; j<length;j++)
	{
		but[j]->hide();
		but[j]->setText("");
		guess[j]->setText("");
	}

	switch (drap)
	{
	case 1:
		{
			for (int js=0; js<inc;js++)
				lab3[js]->setText("");
			lab8->hide();
		}
	case 2:  //(lab4->isVisible()==true
		{
			lab4->hide();
			lab6->hide();
			lab7->hide();
		}
	}

	for (int y=0;y<inc;y++)
		sto[y]="";
	QString string1;
	string1=locate("data","khangman/sounds/new_game.wav");
	KAudioPlayer::play(string1);
	game();
}

// enable to type 'y' or 'n' instead of clicking the mouse
void KHangMan::keyPressEvent(QKeyEvent *ke)
{
	switch(tolower(ke->ascii()))
	{
	case 'y':
		slotNewgame();
		break;
	case 'n':
		slotquit();
		break;
	}
}

void  KHangMan:: slotquit()
{
	kapp->quit();
}

//display the KHangMan HandBook
void KHangMan::slotHelp()
{
	kapp->invokeHTMLHelp("","");
}

//display a screen with a quick information
void KHangMan::slotInfo()
{
	helpla = new KDialog(this);
	QVBoxLayout *helpLayout = new QVBoxLayout(helpla);
	helpla1=new QLabel(helpla);
	helpla1->setAlignment ( AlignHCenter);
	helpla1->setFont(QFont("Times", 26, QFont::Bold));
	helpla1->setText(i18n("   Type in small letters. \n"
	                      "   Ctrl-Q : quit the game \n"
	                      " F1 for Help \n"
	                      "Default : level 'easy'\n"
	                      "TIP : try the vowels first \n\n"
	                      " KHangMan - July 2001 \n"
	                      "by Anne-Marie Mahfouf\n"));
	helpla1->setAlignment ( AlignHCenter);
	helpLayout->addWidget(helpla1);

	exit1 = new QPushButton(i18n("Exit"),helpla);
	exit1->setGeometry
	(200,270,90,60);
	exit1->setPalette( QPalette( QColor(yellow) ) );
	exit1->setFont(QFont("System", 42, QFont::Bold));
	helpLayout->addWidget(exit1);
	helpla->show();

	QObject::connect (exit1, SIGNAL(clicked()), helpla, SLOT(hide()));
	QTimer *timer2 = new QTimer(this);
	connect( timer2, SIGNAL(timeout()),
	         this, SLOT(test1()) );
	timer2->start( 10000, TRUE );
}

void KHangMan::test1()
{
	if (helpla->isVisible()==true)
		helpla->hide();
}
//store the words for level 'easy'
void KHangMan::choice1()
{
	int count=0;
	QFile easyFile(locate("data",file1));
	if (!easyFile.exists())
	{
		KMessageBox::sorry( this, i18n("File $KDEDIR/share/apps/")+file1+i18n(" not found!\n Configure with the right prefix please!"),
		                    "KHangman - Error" );
		exit(1);
	}
	easyFile.open(IO_ReadOnly);
	QTextStream namesStream( &easyFile);
	QString nameString;
	while (namesStream.atEnd()==0)
	{
		count=count+1;
		//read one line from the text
		nameString=namesStream.readLine();
		if (count==rd)
			st=nameString; //store the choosen word in variable st
	}
	easyFile.close();
}

//allow the guess labels to resize exactly the same as the but labels
void KHangMan::resizeEvent(QResizeEvent*)
{
	for (int i=0; i<12;i++)
		guess[i]->resize( QSize(but[i]-> width(), but[i]->height() ) );
}

KHangMan::~KHangMan()
{}
