/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/


/*
 ------------------ Author:       Guillermo Ortega               -------------------
 ------------------ Affiliation:  European Space Agency (ESA)    -------------------
 -----------------------------------------------------------------------------------
 
*/


#include <QTextStream>

#include <QtGui>
#include "HelpBrowser.h"


HelpBrowser::HelpBrowser(const QString &path, const QString &page,
			 QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_GroupLeader);
    textBrowser = new QTextBrowser;
    homeButton = new QPushButton(tr("&Home"));
    backButton = new QPushButton(tr("&Back"));
    closeButton = new QPushButton(tr("Close"));
    closeButton->setShortcut(tr("Esc"));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(homeButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(textBrowser);
    setLayout(mainLayout);
    connect(homeButton, SIGNAL(clicked()), textBrowser, SLOT(home()));
    connect(backButton, SIGNAL(clicked()),
	    textBrowser, SLOT(backward()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(textBrowser, SIGNAL(sourceChanged(const QUrl &)),
	    this, SLOT(updateWindowTitle()));
    textBrowser->setSearchPaths(QStringList() << path << ":/images");
    textBrowser->setSource(page);
}


void HelpBrowser::updateWindowTitle()
{
    setWindowTitle(tr("STA Help: %1").arg(textBrowser->documentTitle()));
}


void HelpBrowser::showPage(const QString &page)
{

    QTextStream out (stdout);

    QString ResourcesPath = QDir::currentPath ();
    QString HelpBrowserPath = ResourcesPath + "/help";

    HelpBrowser *browser = new HelpBrowser(HelpBrowserPath, page);
    browser->setWindowModality(Qt::NonModal);
    browser->resize(800, 600);
    browser->show();
    browser->raise(); // Required to keep the modeless window alive
    browser->activateWindow(); // Required to keep the modeless window alive
}

