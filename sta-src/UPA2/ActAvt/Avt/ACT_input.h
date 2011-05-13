/* 
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
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */
#ifndef _ACT_input_H
#define _ACT_input_H

#include <QWidget>
#include "ui_ACT_input.h"
#include "AVTACT.h"

class QTabWidget;
class AVTACTDialog;

class ACT_input : public QWidget, public Ui_ACT_input
{
    Q_OBJECT

public:
    ACT_input(QWidget* parent = 0);
    ~ACT_input();
    AVTACTDialog* transfer;
    ACT_input* folder;
    void distribution_set_up(QWidget* curr_widg,int ind);
    QGridLayout *file_loading_grid;
    QList<double> samples;
    QString fileName;
    QString directory;

public slots:
    void on_distribution_combo_currentIndexChanged(int);
    void on_load_file_clicked();
    void on_confirm_AVT_clicked();
    void copy_value(QString);

private:
    QList<double> read_data_file(const char* fname);
    int m_curr_distrib;
};

#endif // _ACT_input_H
