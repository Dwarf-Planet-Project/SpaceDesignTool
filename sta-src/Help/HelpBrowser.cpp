/*
 File name: loitering.cpp
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.
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
			 QWidget *parent)
    : QWidget(parent)
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
    //QString path = QApplication::applicationDirPath() + "/sta-data/help";

    QString ResourcesPath = QDir::currentPath ();
    QString HelpBrowserPath = ResourcesPath + "/help";

    out << "HelpBrowserPath: " << HelpBrowserPath << endl;

    HelpBrowser *browser = new HelpBrowser(HelpBrowserPath, page);
    browser->resize(800, 600);
    browser->show();
}

