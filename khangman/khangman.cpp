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

int  rd, k, z, y1, te, i, inc, b, u, ind=0, c0, c1, drap, drap2;
QString let1, a1, s[20],st, str[45], sto[26];

KHangMan::KHangMan(QWidget *parent, const char *name) : QWidget(parent, name)
{
setCaption(i18n("KHangMan - v0.3"));

QString sl1;
sl1.sprintf(i18n(" Please enter a letter :"));
lab1 = new QLabel(this);
lab1->setGeometry(10,100,140,40);
lab1->setFont(QFont("Helvetica", 12, QFont::Bold));
lab1->setPalette( QPalette( QColor(white)));
lab1->setText(sl1);

line=new QLineEdit(this);
line->setCursorPosition(10);
line->setMaxLength(1);
line->setGeometry (10,150,70,40);
line->setFont(QFont("Times", 30, QFont::Bold));
QToolTip::add( line,i18n( "Enter a letter") );
QObject::connect (line, SIGNAL(returnPressed()), this, SLOT(slotTreat()));

QString sl2, sl3;
sl2.sprintf(i18n(" The letters below have already\n") );
sl3.sprintf(i18n(" been guessed :"));
lab2 = new QLabel(this);
lab2->setGeometry(10,200,200,45);
lab2->setFont(QFont("Helvetica", 12, QFont::Bold));
lab2->setPalette( QPalette( QColor(white)));
lab2->setText(sl2+sl3);

help = new QPushButton(i18n( "Help"), this,"help" );
help->setGeometry (10,350, 60, 40);
help->setFont(QFont("Helvetica", 20, QFont::Bold));
help->setPalette( QPalette( QColor(cyan)));
QToolTip::add( help,i18n( "The KHangMan HandBook") );
QObject::connect (help, SIGNAL(clicked()), this, SLOT(slotHelp()));
//accel2 : enable F1 for 'Help'
//refused in porting to kde2
//accel2=new KAccel(this);
//accel2->connectItem(KAccel::Help, this , SLOT(slotHelp()));

info = new QPushButton(i18n( "Info"), this,"info" );
info->setGeometry (270,350, 60, 40);
info->setFont(QFont("Helvetica", 20, QFont::Bold));
info->setPalette( QPalette( QColor(cyan)));
QToolTip::add( info,i18n( "Quick Information") );
QObject::connect (info, SIGNAL(clicked()), this, SLOT(slotInfo()));

level = new QComboBox( "level",this);
level->insertItem( i18n("Level " ) );
level->insertItem( i18n("Easy") );
level->insertItem( i18n("Animals"));
level->insertItem( i18n("Medium" ));
level->insertItem( i18n("Hard" ));
QFont f("Helvetica", 20, QFont::Bold);
level->setGeometry (80,350, 110, 40);
level->setFont( f );
QPalette pal=level->palette();
QColorGroup cg (cyan, black , lightGray, black, darkGray, black,
                  cyan);
pal.setNormal(cg);
level->setPalette(pal);
level->show();
QToolTip::add( level,i18n( "Choose easy, medium or hard") );
QObject::connect( level, SIGNAL(activated(int)),SLOT(choice(int)));


quit = new QPushButton(i18n( "Quit"), this,"quit" );
quit->setGeometry (200,350, 60, 40);
quit->setFont(QFont("Helvetica", 20, QFont::Bold));
quit->setPalette( QPalette( QColor(cyan)));
QToolTip::add( quit,i18n( "Quit the game") );
QObject::connect (quit, SIGNAL(clicked()), this, SLOT(slotquit()));
//accel1 : enable Ctrl-Q for 'Quit'
//refused while porting to kde2
//accel1=new KAccel(this);
//accel1->connectItem(KAccel::Quit, this , SLOT(slotquit()));

lab8 = new QLabel(this);
lab8->setGeometry(420,100,190,230);

px[0].load("/usr/share/icons/hg1.gif");  //array for stocking the pairs
px[1].load("/usr/share/icons/hg2.gif"); //of images
px[2].load("/usr/share/icons/hg3.gif");
px[3].load("/usr/share/icons/hg4.gif");
px[4].load("/usr/share/icons/hg5.gif");
px[5].load("/usr/share/icons/hg6.gif");
px[6].load("/usr/share/icons/hg7.gif");
px[7].load("/usr/share/icons/hg8.gif");
px[8].load("/usr/share/icons/hg9.gif");
px[9].load("/usr/share/icons/hg10.gif");
px[10].load("/usr/share/icons/hg11.gif");
px[11].load("/usr/share/icons/hg12.gif");
px[12].load("/usr/share/icons/hg13.gif");
px[13].load("/usr/share/icons/hg14.gif");

ind=1;
game();
}


void KHangMan::choice(int index)
{
for (int jj=0; jj<i;jj++)
  but[jj]->hide();

if  (drap==1) //te
 {for (int js=0; js<inc;js++)
   lab3[js]->hide();
 }
if (drap2==1)  //(lab4->isVisible()==true)
 { lab4->hide();
  lab6->hide();
  lab7->hide();}

 for (int y=0;y<inc;y++)
   sto[y]="";

switch ( index) {
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
//delete splash;
lab8->setPixmap(px[11]);//display the presentation picture
lab8->show();

y1=250 ;te=0; i=0;k=10; z=10 ; inc=0; b=0;u=0; c0=0; c1=0; drap=0; drap2=0;
//seed random function
srand((unsigned int)time((time_t *)NULL));
rd=rand()%44;  //pick a random number

char name[20];  //store the letters of the word
if (ind==1)choice1();
  else if (ind==2) animals();
     else if (ind==3) choice2();
        else choice3();

line->setFocus();  //enable keyboard focus for 'line'
strncpy(name,st,19);//put the word into 'name'
QString sg;
sg.sprintf("_");
i=strlen(st);   //i = lenght of the word
for (int j=0; j<i;j++)
{
//if (!name[j]) {break;}
but[j]= new QLabel(this);     //draw labels for letters
guess[j]=new QLabel(but[j]);  //to hide the letter
QString s[20];
s[j].sprintf("%c",name[j]);
but[j]->setGeometry (k,20, 50, 65);
but[j]->setFont(QFont("Helvetica", 40, QFont::Bold));
but[j]->setText(s[j]);
but[j]->setAlignment ( AlignCenter);
but[j]->setPalette( QPalette( QColor(yellow)));
but[j]->show();
guess[j]->setGeometry(0,0,50,65);
guess[j]->setFont(QFont("Helvetica", 40, QFont::Bold));
guess[j]->setText(sg);
guess[j]->setAlignment ( AlignCenter);
guess[j]->setPalette( QPalette( QColor(darkGray)));
guess[j]->show();
k=k+55;}
}


void KHangMan::slotNext()
{
lab4 = new QLabel(this);
lab4->setGeometry(90,150,210,35);
lab4->setFont(QFont("Times", 28, QFont::Bold));
lab4->setPalette( QPalette( QColor(cyan)));
QString sl5;
sl5.sprintf(i18n(" Another word ?"));
lab4->setText(sl5);
lab4->show();
drap2=1;
lab7 = new QPushButton(this);
lab7->setGeometry(310,150,30,35);
lab7->setFont(QFont("Times", 28, QFont::Bold));
lab7->setPalette( QPalette( QColor(red)));
QString sl6;
sl6.sprintf(i18n("y"));
lab7->setText(sl6);
lab7->show();
lab6 = new QPushButton(this);
lab6->setGeometry(350,150,30,35);
lab6->setFont(QFont("Times", 28, QFont::Bold));
lab6->setPalette( QPalette( QColor(gray)));
QString sl7;
sl7.sprintf(i18n("n"));
lab6->setText(sl7);
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
a1.sprintf(let1);
//if (a1="")
drap=1;
char name[20];
strncpy(name,st,19);
c1=0; b=0;
for (int j=0; j<20;j++)
 {
if (!name[j]) break;
s[j].sprintf("%c",name[j]);
if (s[j]==a1)
  {guess[j]->hide();
   c1=1;
  }
 }

if (te>0)
 {
for (int kl=0;kl<te+1;kl++)
 if (sto[kl]==a1)
   {warning();
    b=1; } //flag =1 if letter already guessed =0 otherwise
 }
//if letter not already guessed and letter not in word
if ((b==0)&&(c1==0))
        slotShow();

if ((b==0)&&(c1==1))
 { sto[te]=a1;
   te=te+1;}
//test for displaying 'another' message
for (int y=0; y<20; y++)
 {if (!name[y]) break;
  if (guess[y]->isVisible()==false)
        u=u+1;
 if ((u==i))  //wait before displaying 'Another word'
   {if (c0<12)
       {lab8->setPixmap(px[13]);//display the 'Well Done' picture
        lab8->show();}
   { QTimer *timer = new QTimer(this);
     connect( timer, SIGNAL(timeout()),
     this, SLOT(slotNext()) );
     timer->start( 2000, TRUE );}
   }   }
u=0;
}



void KHangMan::slotShow()
{
lab3[inc] = new QLabel(this);
lab3[inc]->setGeometry(z,y1,22,35);
lab3[inc]->setFont(QFont("Times", 28, QFont::Bold));
lab3[inc]->setPalette( QPalette( QColor(lightGray)));
lab3[inc]->setText(a1);
lab3[inc]->setAlignment(AlignCenter);
lab3[inc]->show();

lab8->setPixmap(px[c0]);  //display pictures of the hangman
lab8->show();

z=z+20;
if (z>220)
{z=10; y1=290;}

sto[te]=a1;
te=te+1;
inc=inc+1; c0=c0+1;

//if the game is lost
if (c0==12)
 {lab8->setPixmap(px[c0]); //dislay 'game over' picture
  lab8->show();
  for (int j=0; j<i;j++) //show word
 {
if (but[j]->isVisible()==false);
    guess[j]->hide();
 }
}
}

void KHangMan::warning()
{
 lab5 = new QPushButton(this);
 lab5->setGeometry(90,150,250,35);
 lab5->setFont(QFont("Times", 28, QFont::Bold));
 lab5->setPalette( QPalette( QColor(red)));
 QString sl5;
 sl5.sprintf(i18n(" Already guessed !!!"));
 lab5->setText(sl5);
 lab5->show();

   QTimer *timer = new QTimer(this);
   connect( timer, SIGNAL(timeout()),
   lab5, SLOT(hide()) );
   timer->start( 1000, TRUE );
}

//prepare for a new game
void KHangMan::slotNewgame()
{
for (int j=0; j<i;j++)
  but[j]->hide();

if  (drap==1)  //(te!=0)
{
for (int js=0; js<inc;js++)  ////
  lab3[js]->hide();
lab8->hide();
}

if (drap2==1)  //(lab4->isVisible()==true)
 { lab4->hide();
   lab6->hide();
   lab7->hide(); }

for (int y=0;y<inc;y++)
   sto[y]="";
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
helpla = new QMainWindow(this);
helpla->setGeometry(10,30,520,350);
helpla1=new QLabel(helpla);
helpla1->setGeometry(10,10,500,300);
helpla1->setAlignment ( AlignHCenter);
helpla1->setFont(QFont("Times", 26, QFont::Bold));
QString st1, st2, st3, st4, st5, st6, st7, st8, str1, str2;
st1.sprintf(i18n("   Type in small letters. \n" ));
str1.sprintf(i18n("   Ctrl-Q : quit the game \n" ));
st2.sprintf(i18n(" F1 for Help \n"));
str2.sprintf(i18n("Default : level 'easy'\n"));
st3.sprintf(i18n("TIP : try the vowels first \n\n"));
st4.sprintf(i18n(" KHangMan - July 2001 \n"));
st5.sprintf(i18n("by Anne-Marie Mahfouf\n"));
helpla1->setText(st1+str1+st2+str2+st3+st4+st5);
helpla1->setAlignment ( AlignHCenter);
/*QPalette pal1=helpla->palette();
QColorGroup cg1 (cyan, cyan , lightGray, black, darkGray, black,
cyan);
pal1.setNormal(cg1);
helpla->setPalette(pal1); */
helpla->show();

exit1 = new QPushButton(helpla);
QString str17;
str17.sprintf(i18n(" Exit "));
exit1->setGeometry(200,270,90,60);
exit1->setPalette( QPalette( QColor(yellow) ) );
exit1->setText(str17 );
exit1->setFont(QFont("System", 42, QFont::Bold));
exit1->show();

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
str[1]="playtime"; str[2]="furniture";
str[3]="garden";  str[4]="pupil";
str[5]="vegetables"; str[6]="apple";
str[7]="plate"; str[8]="sweet";
str[9]="kitchen"; str[10]="chocolat";
str[11]="computer"; str[12]="carrot";
str[13]="indication"; str[14]="banana";
str[15]="information"; str[16]="money";
str[17]="adventure"; str[18]="driver";
str[19]="policeman"; str[20]="pineapple";
str[21]="stamp"; str[22]="lunch";
str[23]="limousine"; str[24]="escalator";
str[25]="breakfast"; str[26]="flower";
str[27]="education"; str[28]="table";
str[29]="classroom"; str[30]="television";
str[31]="mistake"; str[32]="yesterday";
str[33]="people"; str[34]="children";
str[35]="surprise"; str[36]="jacket";
str[37]="cheese"; str[38]="chicken";
str[39]="dinner"; str[40]="mountain";
str[41]="forest"; str[42]="potato";
str[43]="water"; str[44]="river";

st=str[rd]; //store the choosen word in variable st
}

//store the words for level 'medium'
void KHangMan::choice2()
{
str[1]="machine"; str[2]="bedroom";
str[3]="magistrate";  str[4]="gymnasium";
str[5]="appointment"; str[6]="antibiotic";
str[7]="delegation"; str[8]="pharmacist";
str[9]="cheeseburger"; str[10]="assessment";
str[11]="surrounding"; str[12]="supermarket";
str[13]="indication"; str[14]="radiography";
str[15]="information"; str[16]="mincemeat";
str[17]="signature"; str[18]="enthusiasm";
str[19]="asparagus"; str[20]="vocabulary";
str[21]="government"; str[22]="conservatory";
str[23]="bookshelf"; str[24]="photograph";
str[25]="intelligence"; str[26]="lawnmower";
str[27]="arithmetic"; str[28]="membership";
str[29]="tranquility"; str[30]="contribution";
str[31]="amusement"; str[32]="umbrella";
str[33]="illumination"; str[34]="millimetre";
str[35]="milligramme"; str[36]="aquarium";
str[37]="intelligence"; str[38]="appointment";
str[39]="population"; str[40]="innovation";
str[41]="strawberry"; str[42]="nephew";
str[43]="countryside"; str[44]="collection";

st=str[rd]; //store the choosen word in variable st

}

void KHangMan::choice3()
{
str[1]="knuckle"; str[2]="catalysis";
str[3]="jewelry";  str[4]="lighthouse";
str[5]="enhancement"; str[6]="federalism";
str[7]="dogmatism"; str[8]="dilettantism";
str[9]="chloroform"; str[10]="ethnography";
str[11]="calligraphy"; str[12]="brainstorm";
str[13]="belladonna"; str[14]="boomerang";
str[15]="accordionist"; str[16]="afterthought";
str[17]="ornithology"; str[18]="osteopathy";
str[19]="rattlesnake"; str[20]="exhibition";
str[21]="radiography"; str[22]="surrealism";
str[23]="syllogism"; str[24]="symposium";
str[25]="theologian"; str[26]="taxidermist";
str[27]="trigonometry"; str[28]="translucence";
str[29]="truthfulness"; str[30]="troposphere";
str[31]="uprightness"; str[32]="upholstery";
str[33]="workaholic"; str[34]="withdrawal";
str[35]="zoology";str[36]="wheelbarrow";
str[37]="xenophobia"; str[38]="yachtsman";
str[39]="astonishment"; str[40]="xylophone";
str[41]="pseudonym";str[42]="prosperity";
str[43]="psychiatrist"; str[44]="protagonist";

st=str[rd]; //store the choosen word in variable st
}

void KHangMan::animals()
{
str[1]="kitten"; str[2]="bear";
str[3]="dolphin";  str[4]="zebra";
str[5]="kangaroo"; str[6]="whale";
str[7]="horse"; str[8]="monkey";
str[9]="woodpecker"; str[10]="kingfisher";
str[11]="snake"; str[12]="giraffe";
str[13]="elephant"; str[14]="mouse";
str[15]="worm"; str[16]="dragonfly";
str[17]="penguin"; str[18]="rabbit";
str[19]="hamster"; str[20]="piglet";
str[21]="wolf"; str[22]="lion";
str[23]="tiger"; str[24]="octopus";
str[25]="rhinoceros"; str[26]="hedgehog";
str[27]="reindeer"; str[28]="squirrel";
str[29]="tortoise"; str[30]="hippopotamus";
str[31]="dinosaur"; str[32]="turkey";
str[33]="eagle"; str[34]="shark";
str[35]="duck";str[36]="fish";
str[37]="parrot"; str[38]="frog";
str[39]="snail"; str[40]="camel";
str[41]="alligator";str[42]="mosquitoe";
str[43]="sheep"; str[44]="shellfish";

st=str[rd]; //store the choosen word in variable st
}

KHangMan::~KHangMan()
{
}
