// SPDX-FileCopyrightText: 2001-2009 Anne-Marie Mahfouf <annma@kde.org>
// SPDX-FileCopyCopyright: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "khangman.h"
#include "khangman_version.h"

#include <KAboutData>

#include <KLocalizedString>
#include <KLocalizedContext>
#include <KCrash>

#include <QApplication>
#include <QFontDatabase>
#include <QFontInfo>
#include <QStandardPaths>
#include <QQmlEngine>
#include <QCommandLineParser>
#include <QQmlContext>
#include <QIcon>
#include <QQmlApplicationEngine>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("khangman"));

    app.connect(&app, &QGuiApplication::lastWindowClosed, &app, &QCoreApplication::quit);

    KAboutData aboutData(QStringLiteral("khangman"),
                         i18n("KHangMan"),
                         QStringLiteral(KHANGMAN_VERSION_STRING),
                         i18n("Classical hangman game by KDE"),
                         KAboutLicense::GPL,
                         i18n("(c) 2001-2011, Anne-Marie Mahfouf"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/khangman"));
    aboutData.addAuthor(i18n("Primoz Anzur"), i18n("Previous maintainer"),
            QStringLiteral("zerokode@gmx.net"));

    // Write the names with accents and all and this file save with
    // utf8 encoding.
    aboutData.addAuthor(i18n("Anne-Marie Mahfouf"), i18n("Original Author"),
                        QStringLiteral("annma@kde.org"), QStringLiteral("https://annma.blogspot.com"), QStringLiteral("annma"));
    aboutData.addAuthor(i18n("Rahul Chowdhury"), i18n("Developer"),
                        QStringLiteral("rahul.chowdhury@kdemail.net"), QStringLiteral("https://rahulc93.wordpress.com/"), QStringLiteral("rahulch"));
    aboutData.addCredit(i18n("Stefan Böhmann"),
                        i18n("Coding help"), QStringLiteral("kde@hilefoks.org"));
    aboutData.addCredit(i18n("Stefan Asserhäll"),
                        i18n("Swedish data files, coding help, transparent pictures and i18n fixes"),
                                QStringLiteral("stefan.asserhall@telia.com"));
    aboutData.addCredit(i18n("eXParTaKus"),
                        i18n("Spanish data files"), QStringLiteral("expartakus@expartakus.com"));
    aboutData.addCredit(i18n("Erik Kjær Pedersenn"),
                        i18n("Danish data files"), QStringLiteral("erik@mpim-bonn.mpg.de") );
    aboutData.addCredit(i18n("Niko Lewman"),
                        i18n("Finnish data files"), QStringLiteral("niko.lewman@edu.hel.fi") );
    aboutData.addCredit(i18n("João Sebastião de Oliveira Bueno"),
                        i18n("Brazilian Portuguese data files"), QStringLiteral("gwidion@mpc.com.br") );
    aboutData.addCredit(i18n("Antoni Bella"),
                        i18n("Catalan data files"), QStringLiteral("bella5@teleline.es"));
    aboutData.addCredit(i18n("Giovanni Venturi"),
                        i18n("Italian data files"), QStringLiteral("jumpyj@tiscali.it"));
    aboutData.addCredit(i18n("Rinse"),
                        i18n("Dutch data files"), QStringLiteral("rinse@kde.nl"));
    aboutData.addCredit(i18n("Lukáš Tinkl"),
                        i18n("Czech data files"), QStringLiteral("lukas@kde.org"));
    aboutData.addCredit(i18n("Kristóf Kiszel"),
                        i18n("Hungarian data files"), QStringLiteral("ulysses@kubuntu.org"));
    aboutData.addCredit(i18n("Torger Åge Sinnes"),
                        i18n("Norwegian (Bokmål) data files"), QStringLiteral("org-a-s@online.no"));
    aboutData.addCredit(i18n("Roger Kovacs"),
                        i18n("Tajik data files"), QStringLiteral("rkovacs@khujand.org"));
    aboutData.addCredit(i18n("Chusslove Illich"),
                        i18n("Serbian (Cyrillic and Latin) data files"), QStringLiteral("chaslav@sezampro.yu"));
    aboutData.addCredit(i18n("Jure Repinc"),
                        i18n("Slovenian data files"), QStringLiteral("jlp@holodeck1.com"));
    aboutData.addCredit(i18n("Pedro Morais"),
                        i18n("Portuguese data files"), QStringLiteral("Morais@kde.org"));
    aboutData.addCredit(i18n("Gaute Hvoslef Kvalnes"),
                        i18n("Norwegian (Nynorsk) data files"), QStringLiteral("gaute@verdsveven.com"));
    aboutData.addCredit(i18n("Mehmet Özel"),
                        i18n("Turkish data files"), QStringLiteral("mehmet_ozel2003@hotmail.com"));
    aboutData.addCredit(i18n("Черепанов Андрей"),
                        i18n("Russian data files"), QStringLiteral("sibskull@mail.ru"));
    aboutData.addCredit(i18n("Radostin Radnev"),
                        i18n("Bulgarian data files"), QStringLiteral("radnev@yahoo.com"));
    aboutData.addCredit(i18n("Kevin Patrick Scannell"),
                        i18n("Irish (Gaelic) data files"), QStringLiteral("scannell@slu.edu"));
    aboutData.addCredit(i18n("Matt Howe"),
                        i18n("Softer Hangman Pictures"), QStringLiteral("mdhowe@bigfoot.com"));
    aboutData.addCredit(i18n("Benjamin Meyer"),
                        i18n("Coding help"), QStringLiteral("ben@meyerhome.net"));
    aboutData.addCredit(i18n("Robert Gogolok"),
                        i18n("Coding help"), QStringLiteral("robertgogolok@gmx.de"));
    aboutData.addCredit(i18n("Lubos Lunàk"),
                        i18n("Coding help"), QStringLiteral("l.lunak@kde.org"));
    aboutData.addCredit(i18n("Albert Astals Cid"),
                        i18n("Coding help, fixed a lot of things"), QStringLiteral("aacid@kde.org"));
    aboutData.addCredit(i18n("Peter Hedlund"),
                        i18n("Code for generating icons for the characters toolbar"), QStringLiteral("peter.hedlund@kdemail.net"));
    aboutData.addCredit(i18n("Inge Wallin"),
                        i18n("Code cleaning"), QStringLiteral("inge@lysator.liu.se"));
    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("khangman")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KCrash::initialize();

    QFont f(QStringLiteral("Domestic Manners"), 12, QFont::Normal, true);
    if (!QFontInfo(f).exactMatch())
    {
        QFontDatabase::addApplicationFont(QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("fonts/Domestic_Manners.ttf")));
    }

    QFont fd(QStringLiteral("Dustismo Roman"), 12, QFont::Normal, true);
    if (!QFontInfo(fd).exactMatch())
    {
        QFontDatabase::addApplicationFont(QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QStringLiteral("fonts/Dustismo_Roman.ttf")));
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    engine.loadFromModule(QStringLiteral("org.kde.khangman"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QObject::connect(&engine, &QQmlEngine::quit, &app, &QCoreApplication::quit);
    return app.exec();
}

// kate: space-indent on; tab-width 4; indent-width 4; mixed-indent off; replace-tabs on;
// vim: set et sw=4 ts=4 cino=l1,cs,U1:

