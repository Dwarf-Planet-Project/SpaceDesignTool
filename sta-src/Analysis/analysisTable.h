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
 This code is based ona  demo file by TrollTech

 */


#include <QtGui>
//#include <QtDBus>

static QString encode_pos(int row, int col);

static void decode_pos(const QString &pos, int *row, int *col);



class SpreadSheetDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SpreadSheetDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
	const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const;
private slots:
    void commitAndCloseEditor();
};






class SpreadSheetItem : public QTableWidgetItem
{
public:
    SpreadSheetItem();
    SpreadSheetItem(const QString &text);

    QTableWidgetItem *clone() const;

    QVariant data(int role) const;
    void setData(int role, const QVariant &value);
    QVariant display() const;

    inline QString formula() const
	{ return QTableWidgetItem::data(Qt::DisplayRole).toString(); }

    static QVariant computeFormula(const QString &formula,
				   const QTableWidget *widget,
				   const QTableWidgetItem *self = 0);

private:
    mutable bool isResolving;
};



#if !defined(QT_NO_DBUS) && defined(Q_OS_UNIX)
#include <QtDBus>

class SpreadSheetAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.stasb.sta.AnalysisTable")

public:
    SpreadSheetAdaptor(QTableWidget *table) : QDBusAbstractAdaptor(table), table(table)
    { QDBusConnection::sessionBus().registerObject("/AnalysisTable", table); }

public slots:
    QString computeFormula(const QString &formula)
    { return SpreadSheetItem::computeFormula(formula, table).toString(); }
private:
    QTableWidget *table;
};

#endif // QT_NO_DBUS






class SpreadSheet : public QMainWindow
{
    Q_OBJECT
public:
    SpreadSheet(int rows, int cols, QWidget *parent = 0);

public slots:
    void updateStatus(QTableWidgetItem *item);
    void updateColor(QTableWidgetItem *item);
    void updateLineEdit(QTableWidgetItem *item);
    void returnPressed();
    void selectColor();
    void selectFont();
    void clear();
    void showAbout();

    void actionSum();
    void actionSubtract();
    void actionAdd();
    void actionMultiply();
    void actionDivide();

    void setupContents(QString analysisFileOutput);

	void exportMyTable();


protected:
    void setupContextMenu();

    void setupMenuBar();
    void createActions();

    void actionMath_helper(const QString &title, const QString &op);
    bool runInputDialog(const QString &title,
			const QString &c1Text,
			const QString &c2Text,
			const QString &opText,
			const QString &outText,
			QString *cell1, QString *cell2, QString *outCell);

private:
    QToolBar *toolBar;
    QAction *colorAction;
    QAction *fontAction;
    QAction *firstSeparator;
    QAction *cell_sumAction;
    QAction *cell_addAction;
    QAction *cell_subAction;
    QAction *cell_mulAction;
    QAction *cell_divAction;
    QAction *secondSeparator;
    QAction *clearAction;
    QAction *aboutSpreadSheet;
    QAction *exitAction;

    QLabel *cellLabel;
    QTableWidget *table;
    QLineEdit *formulaInput;

	int numberOfRows, numberOfColumns;

};
