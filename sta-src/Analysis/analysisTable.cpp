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

#include "Analysis/analysisTable.h"

#include <QtGui>

static QString encode_pos(int row, int col) {
    return QString(col + 'A') + QString::number(row + 1);
}

static void decode_pos(const QString &pos, int *row, int *col)
{
    if (pos.isEmpty()) {
	*col = -1;
	*row = -1;
    } else {
	*col = pos.at(0).toLatin1() - 'A';
	*row = pos.right(pos.size() - 1).toInt() - 1;
    }
}


SpreadSheetDelegate::SpreadSheetDelegate(QObject *parent)
    : QItemDelegate(parent) {}

QWidget *SpreadSheetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
					   const QModelIndex &index) const
{

    /*
    if(index.column() == 1){
	QDateTimeEdit *editor = new QDateTimeEdit(parent);
	editor->setDisplayFormat("dd/M/yyyy");
	editor->setCalendarPopup(true);
	return editor;
    }
    */

    QLineEdit *editor = new QLineEdit(parent);
    //create a completer with the strings in the column as model.
    QStringList allStrings;
    for(int i = 1; i<index.model()->rowCount(); i++){
	QString strItem(index.model()->data(index.sibling(i, index.column()), Qt::EditRole).toString());
	if(!allStrings.contains(strItem))
	    allStrings.append(strItem);
    }
    QCompleter *autoComplete = new QCompleter(allStrings);
    editor->setCompleter(autoComplete);
    connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
    return editor;
}

void SpreadSheetDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void SpreadSheetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {
	edit->setText(index.model()->data(index, Qt::EditRole).toString());
    }
    else {
	QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
	if (dateEditor) {
	    dateEditor->setDate(QDate::fromString(index.model()->data(index, Qt::EditRole).toString(), "d/M/yyyy"));
	}
    }
}

void SpreadSheetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
				       const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {
	model->setData(index, edit->text());
    }
    else {
	QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
	if (dateEditor) {
	    model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
	}
    }
}



SpreadSheetItem::SpreadSheetItem()
    : QTableWidgetItem(), isResolving(false) {}


SpreadSheetItem::SpreadSheetItem(const QString &text)
    : QTableWidgetItem(text), isResolving(false) {}


QTableWidgetItem *SpreadSheetItem::clone() const
{
    SpreadSheetItem *item = new SpreadSheetItem();
    *item = *this;
    return item;
}

QVariant SpreadSheetItem::data(int role) const
{
    if (role == Qt::EditRole || role == Qt::StatusTipRole)
	return formula();

    if (role == Qt::DisplayRole)
	return display();

    QString t = display().toString();
    bool isNumber = false;
    int number = t.toInt(&isNumber);

    if (role == Qt::TextColorRole) {
	if (!isNumber)
	    return qVariantFromValue(QColor(Qt::black));
	else if (number < 0)
	    return qVariantFromValue(QColor(Qt::red));
	return qVariantFromValue(QColor(Qt::blue));
    }

    if (role == Qt::TextAlignmentRole)
	if (!t.isEmpty() && (t.at(0).isNumber() || t.at(0) == '-'))
	    return (int)(Qt::AlignRight | Qt::AlignVCenter);

    return QTableWidgetItem::data(role);
}

void SpreadSheetItem::setData(int role, const QVariant &value)
{
    QTableWidgetItem::setData(role, value);
    if (tableWidget())
	tableWidget()->viewport()->update();
}

QVariant SpreadSheetItem::display() const
{
    // avoid circular dependencies
    if (isResolving)
	return QVariant();
    isResolving = true;
    QVariant result = computeFormula(formula(), tableWidget(), this);
    isResolving = false;
    return result;
}

QVariant SpreadSheetItem::computeFormula(const QString &formula,
					 const QTableWidget *widget,
					 const QTableWidgetItem *self)
{
    // check if the string is actually a formula or not
    QStringList list = formula.split(' ');
    if (list.isEmpty() || !widget)
	return formula; // its a normal string

    QString op = list.value(0).toLower();

    int firstRow = -1;
    int firstCol = -1;
    int secondRow = -1;
    int secondCol = -1;

    if (list.count() > 1)
	decode_pos(list.value(1), &firstRow, &firstCol);
    if (list.count() > 2)
	decode_pos(list.value(2), &secondRow, &secondCol);

    const QTableWidgetItem *start = widget->item(firstRow, firstCol);
    const QTableWidgetItem *end = widget->item(secondRow, secondCol);

    int firstVal = start ? start->text().toInt() : 0;
    int secondVal = end ? end->text().toInt() : 0;

    QVariant result;
    if (op == "sum") {
	int sum = 0;
	for (int r = firstRow; r <= secondRow; ++r) {
	    for (int c = firstCol; c <= secondCol; ++c) {
		const QTableWidgetItem *tableItem = widget->item(r, c);
		if (tableItem && tableItem != self)
		    sum += tableItem->text().toInt();
	    }
	}
	result = sum;
    } else if (op == "+") {
	result = (firstVal + secondVal);
    } else if (op == "-") {
	result = (firstVal - secondVal);
    } else if (op == "*") {
	result = (firstVal * secondVal);
    } else if (op == "/") {
	if (secondVal == 0)
	    result = QString("nan");
	else
	    result = (firstVal / secondVal);
    } else if (op == "=") {
	if (start)
	    result = start->text();
    } else {
	result = formula;
    }

    return result;
}


SpreadSheet::SpreadSheet(int rows, int cols, QWidget *parent)
    : QMainWindow(parent)
{
    addToolBar(toolBar = new QToolBar());
    formulaInput = new QLineEdit();

    cellLabel = new QLabel(toolBar);
    cellLabel->setMinimumSize(120, 0);

    toolBar->addWidget(cellLabel);
    toolBar->addWidget(formulaInput);

    table = new QTableWidget(rows, cols, this);
    for (int c = 0; c < cols; ++c)
    {
	QString character(QChar('A' + c));
	table->setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    }
    table->setItemPrototype(table->item(rows - 1, cols - 1));
    table->setItemDelegate(new SpreadSheetDelegate());


#if !defined(QT_NO_DBUS) && defined(Q_OS_UNIX)
    new SpreadSheetAdaptor(table);
#endif
    createActions();

    updateColor(0);
    setupMenuBar();
    setupContextMenu();
    //setupContents();
    setCentralWidget(table);

    statusBar();
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
	    this, SLOT(updateStatus(QTableWidgetItem*)));
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
	    this, SLOT(updateColor(QTableWidgetItem*)));
    connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
	    this, SLOT(updateLineEdit(QTableWidgetItem*)));
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)),
	    this, SLOT(updateStatus(QTableWidgetItem*)));
    connect(formulaInput, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(table, SIGNAL(itemChanged(QTableWidgetItem*)),
	    this, SLOT(updateLineEdit(QTableWidgetItem*)));
    //setWindowTitle(tr("Spreadsheet"));
}

void SpreadSheet::createActions()
{
    cell_sumAction = new QAction(tr("Sum"), this);
    connect(cell_sumAction, SIGNAL(triggered()), this, SLOT(actionSum()));

    cell_addAction = new QAction(tr("&Add"), this);
    cell_addAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
    connect(cell_addAction, SIGNAL(triggered()), this, SLOT(actionAdd()));

    cell_subAction = new QAction(tr("&Subtract"), this);
    cell_subAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
    connect(cell_subAction, SIGNAL(triggered()), this, SLOT(actionSubtract()));

    cell_mulAction = new QAction(tr("&Multiply"), this);
    cell_mulAction->setShortcut(Qt::CTRL | Qt::Key_multiply);
    connect(cell_mulAction, SIGNAL(triggered()), this, SLOT(actionMultiply()));

    cell_divAction = new QAction(tr("&Divide"), this);
    cell_divAction->setShortcut(Qt::CTRL | Qt::Key_division);
    connect(cell_divAction, SIGNAL(triggered()), this, SLOT(actionDivide()));

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(Qt::CTRL|Qt::Key_F);
    connect(fontAction, SIGNAL(triggered()), this, SLOT(selectFont()));

    colorAction = new QAction(QPixmap(16, 16), tr("Background &Color..."), this);
    connect(colorAction, SIGNAL(triggered()), this, SLOT(selectColor()));

    clearAction = new QAction(tr("Clear"), this);
    clearAction->setShortcut(Qt::Key_Delete);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

    aboutSpreadSheet = new QAction(tr("About Spreadsheet"), this);
    connect(aboutSpreadSheet, SIGNAL(triggered()), this, SLOT(showAbout()));

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    firstSeparator = new QAction(this);
    firstSeparator->setSeparator(true);

    secondSeparator = new QAction(this);
    secondSeparator->setSeparator(true);

}

void SpreadSheet::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    QMenu *cellMenu = menuBar()->addMenu(tr("&Cell"));
    cellMenu->addAction(cell_addAction);
    cellMenu->addAction(cell_subAction);
    cellMenu->addAction(cell_mulAction);
    cellMenu->addAction(cell_divAction);
    cellMenu->addAction(cell_sumAction);
    cellMenu->addSeparator();
    cellMenu->addAction(colorAction);
    cellMenu->addAction(fontAction);

    menuBar()->addSeparator();

    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutSpreadSheet);
}

void SpreadSheet::updateStatus(QTableWidgetItem *item)
{
    if (item && item == table->currentItem()) {
	statusBar()->showMessage(item->data(Qt::StatusTipRole).toString(), 1000);
	cellLabel->setText("Cell: (" + encode_pos(table->row(item), table->column(item)) + ")");
    }
}

void SpreadSheet::updateColor(QTableWidgetItem *item)
{
    QPixmap pix(16, 16);
    QColor col;
    if (item)
	col = item->backgroundColor();
    if (!col.isValid())
	col = palette().base().color();

    QPainter pt(&pix);
    pt.fillRect(0, 0, 16, 16, col);

    QColor lighter = col.light();
    pt.setPen(lighter);
    QPoint lightFrame[] = { QPoint(0, 15), QPoint(0, 0), QPoint(15, 0) };
    pt.drawPolyline(lightFrame, 3);

    pt.setPen(col.dark());
    QPoint darkFrame[] = { QPoint(1, 15), QPoint(15, 15), QPoint(15, 1) };
    pt.drawPolyline(darkFrame, 3);

    pt.end();

    colorAction->setIcon(pix);
}

void SpreadSheet::updateLineEdit(QTableWidgetItem *item)
{
    if (item != table->currentItem())
	return;
    if (item)
	formulaInput->setText(item->data(Qt::EditRole).toString());
    else
	formulaInput->clear();
}

void SpreadSheet::returnPressed()
{
    QString text = formulaInput->text();
    int row = table->currentRow();
    int col = table->currentColumn();
    QTableWidgetItem *item = table->item(row, col);
    if (!item)
	table->setItem(row, col, new SpreadSheetItem(text));
    else
	item->setData(Qt::EditRole, text);
    table->viewport()->update();
}

void SpreadSheet::selectColor()
{
    QTableWidgetItem *item = table->currentItem();
    QColor col = item ? item->backgroundColor() : table->palette().base().color();
    col = QColorDialog::getColor(col, this);
    if (!col.isValid())
	return;

    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (selected.count() == 0)
	return;

    foreach(QTableWidgetItem *i, selected)
	if (i) i->setBackgroundColor(col);

    updateColor(table->currentItem());
}

void SpreadSheet::selectFont()
{
    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (selected.count() == 0)
	return;
    bool ok = false;
    QFont fnt = QFontDialog::getFont(&ok, font(), this);
    if (!ok)
	return;
    foreach(QTableWidgetItem *i, selected)
	if (i) i->setFont(fnt);
}

bool SpreadSheet::runInputDialog(const QString &title,
				 const QString &c1Text,
				 const QString &c2Text,
				 const QString &opText,
				 const QString &outText,
				 QString *cell1, QString *cell2, QString *outCell)
{

    QStringList rows, cols;
    for (int c = 0; c < table->columnCount(); ++c)
	cols << QChar('A' + c);
    for (int r = 0; r < table->rowCount(); ++r)
	rows << QString::number(1 + r);

    QDialog addDialog(this);
    addDialog.setWindowTitle(title);

    QGroupBox group(title, &addDialog);
    group.setMinimumSize(250, 100);

    QLabel cell1Label(c1Text, &group);
    QComboBox cell1RowInput(&group);
    int c1Row, c1Col;
    decode_pos(*cell1, &c1Row, &c1Col);
    cell1RowInput.addItems(rows);
    cell1RowInput.setCurrentIndex(c1Row);
    QComboBox cell1ColInput(&group);
    cell1ColInput.addItems(cols);
    cell1ColInput.setCurrentIndex(c1Col);

    QLabel operatorLabel(opText, &group);
    operatorLabel.setAlignment(Qt::AlignHCenter);

    QLabel cell2Label(c2Text, &group);
    QComboBox cell2RowInput(&group);
    int c2Row, c2Col;
    decode_pos(*cell2, &c2Row, &c2Col);
    cell2RowInput.addItems(rows);
    cell2RowInput.setCurrentIndex(c2Row);
    QComboBox cell2ColInput(&group);
    cell2ColInput.addItems(cols);
    cell2ColInput.setCurrentIndex(c2Col);

    QLabel equalsLabel("=", &group);
    equalsLabel.setAlignment(Qt::AlignHCenter);

    QLabel outLabel(outText, &group);
    QComboBox outRowInput(&group);
    int outRow, outCol;
    decode_pos(*outCell, &outRow, &outCol);
    outRowInput.addItems(rows);
    outRowInput.setCurrentIndex(outRow);
    QComboBox outColInput(&group);
    outColInput.addItems(cols);
    outColInput.setCurrentIndex(outCol);

    QPushButton cancelButton(tr("Cancel"), &addDialog);
    connect(&cancelButton, SIGNAL(clicked()), &addDialog, SLOT(reject()));

    QPushButton okButton(tr("OK"), &addDialog);
    okButton.setDefault(true);
    connect(&okButton, SIGNAL(clicked()), &addDialog, SLOT(accept()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(&okButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(&cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout(&addDialog);
    dialogLayout->addWidget(&group);
    dialogLayout->addStretch(1);
    dialogLayout->addItem(buttonsLayout);

    QHBoxLayout *cell1Layout = new QHBoxLayout;
    cell1Layout->addWidget(&cell1Label);
    cell1Layout->addSpacing(10);
    cell1Layout->addWidget(&cell1ColInput);
    cell1Layout->addSpacing(10);
    cell1Layout->addWidget(&cell1RowInput);

    QHBoxLayout *cell2Layout = new QHBoxLayout;
    cell2Layout->addWidget(&cell2Label);
    cell2Layout->addSpacing(10);
    cell2Layout->addWidget(&cell2ColInput);
    cell2Layout->addSpacing(10);
    cell2Layout->addWidget(&cell2RowInput);

    QHBoxLayout *outLayout = new QHBoxLayout;
    outLayout->addWidget(&outLabel);
    outLayout->addSpacing(10);
    outLayout->addWidget(&outColInput);
    outLayout->addSpacing(10);
    outLayout->addWidget(&outRowInput);

    QVBoxLayout *vLayout = new QVBoxLayout(&group);
    vLayout->addItem(cell1Layout);
    vLayout->addWidget(&operatorLabel);
    vLayout->addItem(cell2Layout);
    vLayout->addWidget(&equalsLabel);
    vLayout->addStretch(1);
    vLayout->addItem(outLayout);

    if (addDialog.exec()) {
	*cell1 = cell1ColInput.currentText() + cell1RowInput.currentText();
	*cell2 = cell2ColInput.currentText() + cell2RowInput.currentText();
	*outCell = outColInput.currentText() + outRowInput.currentText();
	return true;
    }

    return false;
}

void SpreadSheet::actionSum()
{

    int row_first = 0;
    int row_last = 0;
    int row_cur = 0;

    int col_first = 0;
    int col_last = 0;
    int col_cur = 0;

    QList<QTableWidgetItem*> selected = table->selectedItems();
    if (!selected.isEmpty()) {
	QTableWidgetItem *first = selected.first();
	QTableWidgetItem *last = selected.last();
	row_first = table->row(first);
	row_last = table->row(last);
	col_first = table->column(first);
	col_last = table->column(last);
    }

    QTableWidgetItem *current = table->currentItem();
    if (current) {
	row_cur = table->row(current);
	col_cur = table->column(current);
    }

    QString cell1 = encode_pos(row_first, col_first);
    QString cell2 = encode_pos(row_last, col_last);
    QString out = encode_pos(row_cur, col_cur);

    if (runInputDialog(tr("Sum cells"), tr("First cell:"), tr("Last cell:"),
		       QString("%1").arg(QChar(0x03a3)), tr("Output to:"),
		       &cell1, &cell2, &out)) {
	int row, col;
	decode_pos(out, &row, &col);
	table->item(row, col)->setText("sum " + cell1 + " " + cell2);
    }
}

void SpreadSheet::actionMath_helper(const QString &title, const QString &op)
{
    QString cell1 = "C1";
    QString cell2 = "C2";
    QString out = "C3";

    QTableWidgetItem *current = table->currentItem();
    if (current)
	out = encode_pos(table->currentRow(), table->currentColumn());

    if (runInputDialog(title, tr("Cell 1"), tr("Cell 2"), op, tr("Output to:"),
		       &cell1, &cell2, &out)) {
	int row, col;
	decode_pos(out, &row, &col);
	table->item(row, col)->setText(op + " " + cell1 + " " + cell2);
    }
}

void SpreadSheet::actionAdd()
{
    actionMath_helper(tr("Addition"), "+");
}

void SpreadSheet::actionSubtract()
{
    actionMath_helper(tr("Subtraction"), "-");
}

void SpreadSheet::actionMultiply()
{
    actionMath_helper(tr("Multiplication"), "*");
}

void SpreadSheet::actionDivide()
{
    actionMath_helper(tr("Division"), "/");
}

void SpreadSheet::clear()
{
    foreach (QTableWidgetItem *i, table->selectedItems())
	i->setText("");
}

void SpreadSheet::setupContextMenu()
{
    addAction(cell_addAction);
    addAction(cell_subAction);
    addAction(cell_mulAction);
    addAction(cell_divAction);
    addAction(cell_sumAction);
    addAction(firstSeparator);
    addAction(colorAction);
    addAction(fontAction);
    addAction(secondSeparator);
    addAction(clearAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}



void SpreadSheet::setupContents(QString analysisFileOutput)
{
    // Defining the separator that will segment the variables
    QRegExp spaceSeparator("\\s+");  // the + Means one or more spaces!
    QRegExp tabSeparator("\\t+");  // the + Means one or more tabs!

    QFile file(analysisFileOutput);
    int lineCounter = 0;
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    //stream.setRealNumberPrecision(16);
    QString myFileLine;
    myFileLine.clear();

    // 1st row: title of report
    myFileLine = stream.readLine();
    // 2nd row: satellites and arcs
    myFileLine = stream.readLine();
    // 3rd row: #######Beginning of time 1######
    myFileLine = stream.readLine();
    // 4th row:   start and stop times
    myFileLine = stream.readLine();
    // 5th line the rows
    myFileLine = stream.readLine();
    QStringList myRowOfVariables = myFileLine.split(tabSeparator);
    QColor titleBackground(Qt::yellow);
    QFont titleFont = table->font();
    titleFont.setBold(true);
    for (int i = 0; i < myRowOfVariables.size(); i++ )
    {
	table->setItem(lineCounter, i, new SpreadSheetItem(myRowOfVariables.at(i)));
	table->item(lineCounter, i)->setBackgroundColor(titleBackground);
	table->item(lineCounter, i)->setToolTip("This column shows the variables with the units");
	table->item(lineCounter, i)->setFont(titleFont);
	table->item(lineCounter, i)->setTextAlignment(Qt::AlignHCenter);
    }
    lineCounter = lineCounter + 1;

    while( ! stream.atEnd() )
    {
	myFileLine = stream.readLine();
	QStringList myRowOfVariables = myFileLine.split(tabSeparator);
	for (int i = 0; i < myRowOfVariables.size(); i++ )
	{
	    table->setItem(lineCounter, i, new SpreadSheetItem(myRowOfVariables.at(i)));
	    table->setColumnWidth(i, 180);
	}
	lineCounter = lineCounter + 1;

    }

    file.close();
}


const char *htmlText =
	"<HTML>"
	"<p><b>Table showing analysis output"
	" .</b></p>"
	"</HTML>";

void SpreadSheet::showAbout()
{
    QMessageBox::about(this, "About ANALYSIS table", htmlText);
}
