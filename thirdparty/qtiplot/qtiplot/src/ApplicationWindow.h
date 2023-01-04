/***************************************************************************
    File                 : ApplicationWindow.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : QtiPlot's main window

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QListView>
#include <QNetworkAccessManager>
#include <QFile>
#include <QSplitter>
#include <QDesktopServices>
#include <QBuffer>

#include "Table.h"
#include "ScriptingEnv.h"
#include "Script.h"
#include "plot2D/TranslateCurveTool.h"

class QPixmap;
class QCloseEvent;
class QDropEvent;
class QTimerEvent;
class QDragEnterEvent;
class QTranslator;
class QDockWidget;
class QAction;
class QActionGroup;
class QLineEdit;
class QTranslator;
class QToolButton;
class QShortcut;
class QMenu;
class QToolBar;
class QAssistantClient;
class QLocale;
class QMdiArea;
class QUndoView;
class QCompleter;

class Matrix;
class Table;
class Graph;
class ScalePicker;
class Graph3D;
class Note;
class MultiLayer;
class FunctionDialog;
class Folder;
class FolderListItem;
class FolderListView;
class ScriptWindow;
class Plot3DDialog;
class TableStatistics;
class CurveRangeDialog;
class LegendWidget;
class ArrowMarker;
class TextEditor;
class AssociationsDialog;

/**
 * \brief QtiPlot's main window.
 *
 * This class contains the main part of the user interface as well as the central project management facilities.
 *
 * It manages all MdiSubWindow MDI Windows in a project, knows about their organization in Folder objects
 * and contains the parts of the project explorer not implemented in Folder, FolderListItem or FolderListView.
 *
 * Furthermore, it is responsible for displaying most MDI Windows' context menus and opening all sorts of dialogs.
 *
 * \section future Future Plans
 * Split out the project management part into a new Project class.
 * If MdiSubWindow maintains a reference to its parent Project, it should be possible to have its subclasses
 * display their own context menus and dialogs.
 * This is necessary for implementing new plot types or even completely new MdiSubWindow subclasses in plug-ins.
 * It will also make ApplicationWindow more manageable by removing those parts not directly related to the main window.
 *
 * Project would also take care of basic project file reading/writing (using Qt's XML framework), but delegate most of
 * the work to MdiSubWindow and its subclasses. This is necessary for providing save/restore of classes implemented in
 * plug-ins. Support for foreign formats on the other hand could go into import/export classes (which could also be
 * implemented in plug-ins). Those would interface directly with Project and the MyWidgets it manages. Thus, in addition
 * to supporting QtXML-based save/restore, Project, MdiSubWindow and subclasses will also have to provide generalized
 * save/restore methods/constructors.
 *
 * Maybe split out the project explorer into a new ProjectExplorer class, depending on how much code is left
 * in ApplicationWindow after the above reorganizations. Think about whether a Model/View approach can be
 * used for Project/ProjectExplorer.
 */
class ApplicationWindow: public QMainWindow, public scripted
{
    Q_OBJECT
public:
    ApplicationWindow(bool factorySettings = false);
	ApplicationWindow(const QStringList& l);
	~ApplicationWindow();

	enum ShowWindowsPolicy{HideAll, ActiveFolder, SubFolders};
	enum WindowType{NoWindow, TableWindow, MatrixWindow, MultiLayerWindow, NoteWindow, Plot3DWindow};
	enum MatrixToTableConversion{Direct, XYZ, YXZ};
	enum EndLineChar{LF, CRLF, CR};
	enum Analysis{NoAnalysis, Integrate, Diff, FitLinear, FitGauss, FitLorentz, FitSigmoidal};

	FolderListView *lv, *folders;
	QDockWidget *logWindow;

	/*! Generates a new unique name starting with string /param name.
	You can force the output to be a name different from /param name,
	even if 'name' is not used in the project, by setting /param increment = true (the default)
	*/
	QString generateUniqueName(const QString& name, bool increment = true);
	void saveFitFunctions(const QStringList& lst);

	//! \name User custom actions
	//@{
	void loadCustomActions();
	void reloadCustomActions();
    void removeCustomAction(QAction *);
	void addCustomAction(QAction *, const QString& parentName, int index = -1);
    QList<QAction *> customActionsList(){return d_user_actions;};
	QList<QMenu *> customizableMenusList();
	//@}

	QList<QMenu *> menusList();
	QList<QToolBar *> toolBarsList();

	MdiSubWindow *activeWindow(WindowType type = NoWindow);

	int matrixUndoStackSize(){return d_matrix_undo_stack_size;};
	void setMatrixUndoStackSize(int size);

	QString endOfLine();
	bool autoUpdateTableValues(){return d_auto_update_table_values;};
	void setAutoUpdateTableValues(bool on = true);

	QCompleter* completer(){return d_completer;};
	void enableCompletion(bool on = true);

	QLocale clipboardLocale(){return d_clipboard_locale;};
	void setClipboardLocale(const QLocale& locale){d_clipboard_locale = locale;};

public slots:
	//! \name Projects and Project Files
	//@{
	void open();
	ApplicationWindow* open(const QString& fn, bool factorySettings = false, bool newProject = true);
	ApplicationWindow* openProject(const QString& fn, bool factorySettings = false, bool newProject = true);
	ApplicationWindow* importOPJ(const QString& fn, bool factorySettings = false, bool newProject = true);

	/**
	 * \brief Create a new project from a data file.
	 *
	 * \param fn is read as a data file with the default column separator (as set by the user)
	 * and inserted as a table into a new, empty project.
	 * This table is then plotted with the Graph::LineSymbols style.
	 */
	ApplicationWindow * plotFile(const QString& fn);

	/**
	 * \brief Create a new project from a script file.
	 *
	 * \param fn is read as a Python script file and loaded in the command script window.
	 * \param execute specifies if the script should be executed after opening.
	 */
	ApplicationWindow * loadScript(const QString& fn, bool execute = false, bool factorySettings = false);

	QList<MdiSubWindow *> windowsList();
	void updateWindowLists(MdiSubWindow *w);
	/*!
    Arranges all the visible project windows in a cascade pattern.
    */
	void cascade();

	void saveProjectAs(const QString& fileName = QString(), bool compress = false);
	bool saveProject(bool compress = false);

	//! Set the project status to modifed
	void modifiedProject();
	//! Set the project status to saved (not modified)
	void savedProject();
	//! Set the project status to modified and save 'w' as the last modified widget
	void modifiedProject(MdiSubWindow *w);
	//@}

	//! \name Settings
	//@{
	void readSettings();
	void saveSettings();
	void setSaveSettings(bool autoSaving, int min);
	void changeAppStyle(const QString& s);
	void changeAppFont(const QFont& f);
	void updateAppFonts();
	void setAppColors(const QColor& wc,const QColor& pc,const QColor& tpc, bool force = false);

	void initWindow();
	//@}

	//! \name Multilayer Plots
	//@{
	//! Creates a new empty multilayer plot
	MultiLayer* newGraph(const QString& caption = tr("Graph"));
	MultiLayer* multilayerPlot(int c, int r, int style);
	MultiLayer* multilayerPlot(Table* w, const QStringList& colList, int style, int startRow = 0, int endRow = -1);
	//! used when restoring a plot from a project file
	MultiLayer* multilayerPlot(const QString& caption, int layers = 1, int rows = 1, int cols = 1);
	//! used by the plot wizard
	MultiLayer* multilayerPlot(const QStringList& colList);
	void connectMultilayerPlot(MultiLayer *g);
	void addLayer();
	void addInsetLayer(bool curves = false);
	void addInsetCurveLayer();
	void deleteLayer();
	void extractGraphs();
    void extractLayers();

	//! Creates a new spectrogram graph
  	MultiLayer* plotSpectrogram(Matrix *m, Graph::CurveType type);
  	MultiLayer* plotGrayScale(Matrix *m = 0);
  	MultiLayer* plotContour(Matrix *m = 0);
  	MultiLayer* plotColorMap(Matrix *m = 0);
  	MultiLayer* plotImage(Matrix *m = 0);

	//! Rearrange the layersin order to fit to the size of the plot window
  	void autoArrangeLayers();
	void initMultilayerPlot(MultiLayer* g, const QString& name);
	void polishGraph(Graph *g, int style);
	void plot2VerticalLayers();
	void plot2HorizontalLayers();
	void plot4Layers();
	void plotStackedLayers();
	void plotStackedHistograms();
	//@}

	//! \name 3D Data Plots
	//@{
    Graph3D* newPlot3D();
	Graph3D* openMatrixPlot3D(const QString& caption, const QString& matrix_name,
							 double xl,double xr,double yl,double yr,double zl,double zr);
	Graph3D* plotXYZ(Table* table,const QString& zColName, int type);
		//when reading from .qti file
	Graph3D* dataPlot3D(const QString& caption,const QString& formula,
						double xl, double xr, double yl, double yr, double zl, double zr);
	Graph3D* openPlotXYZ(const QString& caption,const QString& formula,
 						double xl, double xr, double yl, double yr, double zl, double zr);
	//@}

	//! \name Surface Plots
	//@{
    Graph3D* plotSurface(const QString& formula, double xl, double xr,
					   double yl, double yr, double zl, double zr, int columns = 40, int rows = 30);
	Graph3D* plotParametricSurface(const QString& xFormula, const QString& yFormula,
						const QString& zFormula, double ul, double ur, double vl, double vr,
						int columns, int rows, bool uPeriodic, bool vPeriodic);

	void connectSurfacePlot(Graph3D *plot);
	void newSurfacePlot();
	void editSurfacePlot();
	void remove3DMatrixPlots(Matrix *m);
	void updateMatrixPlots(MdiSubWindow *);
	void add3DData();
	void change3DData();
	void change3DData(const QString& colName);
	void change3DMatrix();
	void change3DMatrix(const QString& matrix_name);
	void insertNew3DData(const QString& colName);
	void add3DMatrixPlot();
	void insert3DMatrixPlot(const QString& matrix_name);
	void initPlot3D(Graph3D *plot);
	void customPlot3D(Graph3D *plot);
	void setPlot3DOptions();

	void plot3DWireframe();
	void plot3DHiddenLine();
	void plot3DPolygons();
	void plot3DWireSurface();

	Graph3D* plot3DMatrix(Matrix *m = 0, int style = 5);

	void plot3DRibbon();
	void plot3DScatter();
	void plot3DTrajectory();
	void plot3DBars();
	//@}

	//! \name User-defined Functions
	//@{
	MultiLayer * newFunctionPlot(QStringList &formulas, double start, double end, int points = 100, const QString& var = "x", int type = 0);

	FunctionDialog* functionDialog();
	FunctionDialog* showFunctionDialog();
	FunctionDialog* showFunctionDialog(Graph * g, int curve);
	void addFunctionCurve();
	void clearSurfaceFunctionsList();
	void clearLogInfo();
	void clearParamFunctionsList();
	void clearPolarFunctionsList();
	void updateFunctionLists(int type, QStringList &formulas);
	void updateSurfaceFuncList(const QString& s);
	//@}

	//! \name Matrices
	//@{
	//! Creates a new empty matrix
	Matrix* newMatrix(int rows = 32, int columns = 32);
	//! To be used when opening a project file only!
	Matrix* newMatrix(const QString& caption, int r, int c);
	Matrix* matrix(const QString& name);
	Matrix* convertTableToMatrix();
	Matrix* tableToMatrix(Table* t);
	void initMatrix(Matrix* m, const QString& caption);
	void transposeMatrix();
	void invertMatrix();
	void matrixDeterminant();
	void flipMatrixVertically();
	void flipMatrixHorizontally();
	void rotateMatrix90();
	void rotateMatrixMinus90();
	void viewMatrixImage();
	void viewMatrixTable();
	void exportMatrix(const QString& exportFilter = QString::null);
	void setMatrixGrayScale();
	void setMatrixRainbowScale();
	void viewMatrixColumnRow();
	void viewMatrixXY();
    void matrixDirectFFT();
    void matrixInverseFFT();
	//@}

	//! \name Tables
	//@{
	//! Creates an empty table
	Table* newTable();

    // getCurrent... functions for python interface
	Table* currentTable();
	MultiLayer* currentPlot();
	Note* currentNote();
	Matrix* currentMatrix();

	//! Used when loading a table from a project file
	Table* newTable(const QString& caption,int r, int c);
	Table* newTable(int r, int c, const QString& name = QString(),const QString& legend = QString());
	Table* newTable(const QString& caption, int r, int c, const QString& text);
	/**
	 * \brief Create a Table which is initially hidden; used to return the result of an analysis operation.
	 *
	 * \param name window name (compare MdiSubWindow::MdiSubWindow)
	 * \param label window label (compare MdiSubWindow::MdiSubWindow)
	 * \param r number of rows
	 * \param c number of columns
     * \param text tab/newline - seperated initial content; may be empty
	 */
	Table* newHiddenTable(const QString& name, const QString& label, int r, int c, const QString& text=QString());
	Table* table(const QString& name);
	Table* convertMatrixToTableDirect();
	Table* convertMatrixToTableXYZ();
	Table* convertMatrixToTableYXZ();
	Table* matrixToTable(Matrix* m, MatrixToTableConversion conversionType = Direct);
	QList<MdiSubWindow *> tableList();
    //! Returns true if the project contains tables
	bool hasTable();
	//! Returns a list containing the names of all tables in the project
	QStringList tableNames();

	void connectTable(Table* w);
	void initTable(Table* w, const QString& caption);
	void customTable(Table* w);

	void importASCII();
	void importASCII(const QStringList& files, int import_mode, const QString& local_column_separator, int local_ignored_lines, bool local_rename_columns,
        bool local_strip_spaces, bool local_simplify_spaces, bool local_import_comments,
        QLocale local_separators, const QString& local_comment_string, bool import_read_only, int endLineChar);
	void exportAllTables(const QString& sep, bool colNames, bool colComments, bool expSelection);
	void exportASCII(const QString& tableName, const QString& sep, bool colNames, bool colComments, bool expSelection);

	//! recalculate selected cells of current table
	void recalculateTable();

	TableStatistics *newTableStatistics(Table *base, int type, QList<int>,
	    const QString &caption=QString::null);
	//@}

	//! \name Graphs
	//@{
	void setPreferences(Graph* g);
	void setGraphDefaultSettings(bool autoscale,bool scaleFonts,bool resizeLayers,bool antialiasing);
	void setArrowDefaultSettings(double lineWidth,  const QColor& c, Qt::PenStyle style,
								int headLength, int headAngle, bool fillHead);

	void plotL();
	void plotP();
	void plotLP();
	void plotPie();
	void plotVerticalBars();
	void plotHorizontalBars();
	void plotArea();
	void plotVertSteps();
	void plotHorSteps();
	void plotSpline();
	void plotVerticalDropLines();
	MultiLayer* plotHistogram();
	MultiLayer* plotHistogram(Matrix *m);
	void plotVectXYXY();
	void plotVectXYAM();
	void plotBoxDiagram();
	void plotDoubleYAxis();
	void zoomRectanglePlot();

    //! Check whether a table is valid for a 3D plot and display an appropriate error if not
    bool validFor3DPlot(Table *table);
    //! Check whether a table is valid for a 2D plot and display an appropriate error if not
    bool validFor2DPlot(Table *table);
    //! Generate a new 2D graph
    MultiLayer* generate2DGraph(Graph::CurveType type);
	//@}

	//! \name Image Analysis
	//@{
	void intensityTable();
	void pixelLineProfile();
	void loadImage();
	void loadImage(const QString& fn);
  	Matrix* importImage(const QString& = QString(), bool newWindow = false);
	//@}

	//! \name Export and Print
	//@{
	void exportLayer();
	void exportGraph(const QString& exportFilter = QString::null);
	void exportAllGraphs();
	void exportPDF();
	void print();
	void printAllPlots();
	//@}

	QStringList columnsList(Table::PlotDesignation plotType = Table::All);

	void undo();
	void redo();

	//! \name MDI Windows
	//@{
	MdiSubWindow* clone(MdiSubWindow* w = 0);
	void rename();
	void renameWindow();

	//!  Called when the user presses F2 and an item is selected in lv.
	void renameWindow(Q3ListViewItem *item, int, const QString &s);

	//!  Checks weather the new window name is valid and modifies the name.
	bool setWindowName(MdiSubWindow *w, const QString &text);

	void maximizeWindow(Q3ListViewItem * lbi = 0);
	void maximizeWindow(MdiSubWindow *w);
	void minimizeWindow(MdiSubWindow *w = 0);

	void updateWindowStatus(MdiSubWindow* );

	bool hidden(QWidget* window);
	void closeActiveWindow();
	void closeWindow(MdiSubWindow* window);

	//!  Does all the cleaning work before actually deleting a window!
	void removeWindowFromLists(MdiSubWindow* w);

	void hideWindow(MdiSubWindow* window);
	void hideWindow();
	void hideActiveWindow();
	void activateWindow();
	void activateWindow(MdiSubWindow *);
	//@}

	//! Show about dialog
	static void about();
	//! Return a version string ("QtiPlot x.y.z")
	static QString versionString();
	void removeCurves(const QString& name);
	QStringList dependingPlots(const QString& caption);
	QStringList depending3DPlots(Matrix *m);
	QStringList multilayerDependencies(QWidget *w);

	void saveAsTemplate(MdiSubWindow* w = 0, const QString& = QString());
	void openTemplate();
	MdiSubWindow* openTemplate(const QString& fn);

	QString windowGeometryInfo(MdiSubWindow *w);
	void restoreWindowGeometry(ApplicationWindow *app, MdiSubWindow *w, const QString s);
	void restoreApplicationGeometry();
	void resizeActiveWindow();
	void resizeWindow();

	//! \name List View in Project Explorer
	//@{
	void setListView(const QString& caption,const QString& view);
	void renameListViewItem(const QString& oldName,const QString& newName);
	void setListViewDate(const QString& caption,const QString& date);
	QString listViewDate(const QString& caption);
	void setListViewSize(const QString& caption,const QString& size);
	void setListViewLabel(const QString& caption,const QString& label);
	//@}

	void updateColNames(const QString& oldName, const QString& newName);
	void updateTableNames(const QString& oldName, const QString& newName);
	void changeMatrixName(const QString& oldName, const QString& newName);
	void updateCurves(Table *t, const QString& name);

	void showTable(const QString& curve);
	void showTable(int i);

	void addColToTable();
	void cutSelection();
	void copySelection();
	void copyMarker();
	void pasteSelection();
	void clearSelection();
	void copyActiveLayer();

	void newProject();

	//! \name Reading from a Project File
	//@{
	Matrix* openMatrix(ApplicationWindow* app, const QStringList &flist);
	Table* openTable(ApplicationWindow* app, const QStringList &flist);
	TableStatistics* openTableStatistics(const QStringList &flist);
	Graph3D* openSurfacePlot(ApplicationWindow* app, const QStringList &lst);
	Graph* openGraph(ApplicationWindow* app, MultiLayer *plot, const QStringList &list);

	void openRecentProject(int index);
	//@}

	//! \name Table Tools
	//@{
	void sortSelection();
	void sortActiveTable();
	void normalizeSelection();
	void normalizeActiveTable();
	void correlate();
	void autoCorrelate();
	void convolute();
	void deconvolute();
	void clearTable();
	void goToRow();
	void goToColumn();
	//@}

	//! \name Plot Tools
	//@{
	void newLegend();
	void addTimeStamp();
	void drawLine();
	void drawArrow();
	void drawPoints();
	void addText();
	void addTexFormula();
	void addRectangle();
	void addImage();
	void zoomIn();
	void zoomOut();
	void setAutoScale();
	void showRangeSelectors();
	void showCursor();
	void showScreenReader();
	void pickPointerCursor();
	void disableTools();
	void pickDataTool( QAction* action );

	void updateLog(const QString& result);
	//@}

	//! \name Fitting
	//@{
	void deleteFitTables();
	void fitLinear();
	void fitSigmoidal();
	void fitGauss();
	void fitLorentz();
	void fitMultiPeak(int profile);
	void fitMultiPeakGauss();
	void fitMultiPeakLorentz();
	//@}

	//! \name Calculus
	//@{
	void integrate();
	void differentiate();
	void analysis(Analysis operation);
	void analyzeCurve(Graph *g, Analysis operation, const QString& curveTitle);
	void showDataSetDialog(Analysis operation);
	//@}

	void addErrorBars();
	void defineErrorBars(const QString& name,int type,const QString& percent,int direction);
	void defineErrorBars(const QString& curveName,const QString& errColumnName, int direction);
	void movePoints();
	void removePoints();

	//! \name Event Handlers
	//@{
	void closeEvent( QCloseEvent*);
	void timerEvent ( QTimerEvent *e);
	void dragEnterEvent( QDragEnterEvent* e );
	void dropEvent( QDropEvent* e );
	void customEvent( QEvent* e);
	//@}

	//! \name Dialogs
	//@{
	void showFindDialogue();
	//! Show plot style dialog for the active MultiLayer / activeLayer / specified curve or the activeLayer options dialog if no curve is specified (curveKey = -1).
	void showPlotDialog(int curveKey = -1);
	QDialog* showScaleDialog();
	QDialog* showPlot3dDialog();
	AxesDialog* showScalePageFromAxisDialog(int axisPos);
	AxesDialog* showAxisPageFromAxisDialog(int axisPos);
	void showAxisDialog();
	void showGridDialog();
	void showGeneralPlotDialog();
	void showResults(bool ok);
	void showResults(const QString& s, bool ok=true);
	void showEnrichementDialog();
	void showLineDialog();
	void showTitleDialog();
	void showExportASCIIDialog();
	void showCurvesDialog();
	void showCurveRangeDialog();
	CurveRangeDialog* showCurveRangeDialog(Graph *g, int curve);
	AssociationsDialog* showPlotAssociations(int curve);

	void showAxisTitleDialog();
	void showColumnOptionsDialog();
	void showRowsDialog();
	void showDeleteRowsDialog();
	void showColsDialog();
	void showColMenu(int c);
	void showColumnValuesDialog();

	void showGraphContextMenu();
	void showTableContextMenu(bool selection);
	void showWindowContextMenu();
	void customWindowTitleBarMenu(MdiSubWindow *w, QMenu *menu);
	void showCurveContextMenu(QwtPlotCurve *);
	void showCurvePlotDialog();
	void showCurveWorksheet();
    void showCurveWorksheet(Graph *g, int curveIndex);
	void showWindowPopupMenu(Q3ListViewItem *it, const QPoint &p, int);

	//! Connected to the context menu signal from lv; it's called when there are several items selected in the list
	void showListViewSelectionMenu(const QPoint &p);

	//! Connected to the context menu signal from lv; it's called when there are no items selected in the list
	void showListViewPopupMenu(const QPoint &p);

	void showScriptWindow();
	void showMoreWindows();
	void showMarkerPopupMenu();
	void showHelp();
	static void showStandAloneHelp();
	void chooseHelpFolder();
	void showPlotWizard();
	void showFitPolynomDialog();
    void showFrequencyCountDialog();
	void showIntegrationDialog();
	void showInterpolationDialog();
	void showExpGrowthDialog();
	void showExpDecayDialog();
	void showExpDecayDialog(int type);
	void showTwoExpDecayDialog();
	void showExpDecay3Dialog();
	void showRowStatistics();
	void showColStatistics();
	void showFitDialog();
	void showLayerDialog();
	void showPreferencesDialog();
	void showMatrixDialog();
	void showMatrixSizeDialog();
	void showMatrixValuesDialog();
	void showSmoothSavGolDialog();
	void showSmoothFFTDialog();
	void showSmoothAverageDialog();
    void showSmoothDialog(int m);
	void showFilterDialog(int filter);
	void lowPassFilterDialog();
	void highPassFilterDialog();
	void bandPassFilterDialog();
	void bandBlockFilterDialog();
	void showFFTDialog();
	void showColorMapDialog();
	//@}

	void translateCurveHor();
	void translateCurve(TranslateCurveTool::Direction direction = TranslateCurveTool::Vertical);

	//! Removes the curve identified by a key stored in the data() of actionRemoveCurve.
	void removeCurve();
	void hideCurve();
	void hideOtherCurves();
	void showAllCurves();
	void setCurveFullRange();

	void setAscValues();
	void setRandomValues();
	void setXCol();
	void setYCol();
	void setZCol();
	void setXErrCol();
	void setYErrCol();
	void setLabelCol();
	void disregardCol();
	void setReadOnlyCol();
	void setReadOnlyColumns();
	void setReadWriteColumns();
	void swapColumns();
	void moveColumnRight();
	void moveColumnLeft();
	void moveColumnFirst();
	void moveColumnLast();

	void updateConfirmOptions(bool askTables, bool askMatrixes, bool askPlots2D, bool askPlots3D, bool askNotes);

	//! \name Plot3D Tools
	//@{
	void toggle3DAnimation(bool on = true);
	 //! Turns perspective mode on or off
  	void togglePerspective(bool on = true);
  	//! Resets rotation of 3D plots to default values
  	void resetRotation();
  	//! Finds best layout for the 3D plot
  	void fitFrameToLayer();
	void setFramed3DPlot();
	void setBoxed3DPlot();
	void removeAxes3DPlot();
	void removeGrid3DPlot();
	void setHiddenLineGrid3DPlot();
	void setLineGrid3DPlot();
	void setPoints3DPlot();
	void setCrosses3DPlot();
	void setCones3DPlot();
	void setBars3DPlot();
	void setFilledMesh3DPlot();
	void setEmptyFloor3DPlot();
	void setFloorData3DPlot();
	void setFloorIso3DPlot();
	void setFloorGrid3DPlot(bool on);
	void setCeilGrid3DPlot(bool on);
	void setRightGrid3DPlot(bool on);
	void setLeftGrid3DPlot(bool on);
	void setFrontGrid3DPlot(bool on);
	void setBackGrid3DPlot(bool on);
	void pickPlotStyle( QAction* action );
	void pickCoordSystem( QAction* action);
	void pickFloorStyle( QAction* action);
	void custom3DActions(QMdiSubWindow *w);
	void custom3DGrids(int grids);
	//@}

	void updateRecentProjectsList();

	//!  connected to the done(bool) signal of the http object
	void receivedVersionFile(bool error);
	//!  called when the user presses the actionCheckUpdates
	void searchForUpdates();
#ifdef QTIPLOT_SUPPORT
	void showDonationDialog();
#endif
	//! Open support page in external browser
	void showSupportPage();
	//! Open donation page in external browser
	void showDonationsPage();
	//! Open QtiPlot homepage in external browser
	void showHomePage();
	//! Open forums page at berliOS in external browser
	void showForums();
	//! Open bug tracking system at berliOS in external browser
	void showBugTracker();
	//! Show download page in external browser
	void downloadManual();
	//! Show translations page in external browser
	void downloadTranslation();
#ifdef QTIPLOT_DEMO
	//! Shown when the user tries to save the project.
	void showDemoVersionMessage();
#endif

	void parseCommandLineArguments(const QStringList& args);
	void createLanguagesList();
	void switchToLanguage(int param);
	void switchToLanguage(const QString& locale);

	bool alreadyUsedName(const QString& label);
	bool projectHas2DPlots();

	//! Returns a pointer to the window named "name"
	MdiSubWindow* window(const QString& name);

	//! Returns a list with the names of all the matrices in the project
	QStringList matrixNames();

	//! \name Notes
	//@{
 	//! Creates a new empty note window
	Note* newNote(const QString& caption = QString());
	Note* openNote(ApplicationWindow* app, const QStringList &flist);
	void saveNoteAs();
	void showNoteLineNumbers(bool show = true);
	//@}

	//! \name Folders
	//@{
	//! Returns a pointer to the current folder in the project
	Folder* currentFolder(){return current_folder;};
	//! Adds a new folder to the project
	void addFolder();
	Folder* addFolder(QString name, Folder* parent = NULL);
	//! Deletes the current folder
	void deleteFolder();

	//! Ask confirmation from user, deletes the folder f if user confirms and returns true, otherwise returns false;
	bool deleteFolder(Folder *f);

	//! Deletes the currently selected items from the list view #lv.
	void deleteSelectedItems();
	//! Hides the currently selected windows from the list view #lv.
	void hideSelectedWindows();
    //! Show the currently selected windows from the list view #lv.
	void showSelectedWindows();

	//! Sets all items in the folders list view to be desactivated (QPixmap = folder_closed_xpm)
	void desactivateFolders();

	//! Changes the current folder. Returns true if successfull
	bool changeFolder(Folder *newFolder, bool force = false);

	//! Changes the current folder when the user changes the current item in the QListView "folders"
	void folderItemChanged(Q3ListViewItem *it);
	//! Changes the current folder when the user double-clicks on a folder item in the QListView "lv"
	void folderItemDoubleClicked(Q3ListViewItem *it);

	//!  creates and opens the context menu of a folder list view item
	/**
	 * \param it list view item
	 * \param p mouse global position
	 * \param fromFolders: true means that the user clicked right mouse buttom on an item from QListView "folders"
	 *					   false means that the user clicked right mouse buttom on an item from QListView "lv"
	 */
	void showFolderPopupMenu(Q3ListViewItem *it, const QPoint &p, bool fromFolders);

	//!  connected to the SIGNAL contextMenuRequested from the list views
	void showFolderPopupMenu(Q3ListViewItem *it, const QPoint &p, int);

	//!  starts renaming the selected folder by creating a built-in text editor
	void startRenameFolder();

	//!  starts renaming the selected folder by creating a built-in text editor
	void startRenameFolder(Q3ListViewItem *item);

	//!  checks weather the new folder name is valid and modifies the name
	void renameFolder(Q3ListViewItem *it, int col, const QString &text);

	//!  forces showing all windows in the current folder and subfolders, depending on the user's viewing policy
	void showAllFolderWindows();

	//!  forces hidding all windows in the current folder and subfolders, depending on the user's viewing policy
	void hideAllFolderWindows();

	//!  hides all windows in folder f
	void hideFolderWindows(Folder *f);

	//!  pops up folder information
	void folderProperties();

	//!  pops up information about the selected window item
	void windowProperties();

	//!  pops up information about the current project
	void projectProperties();

	//! Pops up a file dialog and invokes appendProject(const QString&) on the result.
	void appendProject();
	//! Open the specified project file and add it as a subfolder to the parentFolder or to the current folder if no parent folder is specified.
	Folder* appendProject(const QString& file_name, Folder* parentFolder = 0);
	void saveAsProject();
	void saveFolderAsProject(Folder *f);
	void saveFolder(Folder *folder, const QString& fn, bool compress = false);

	//!  adds a folder list item to the list view "lv"
	void addFolderListViewItem(Folder *f);

	//!  adds a widget list item to the list view "lv"
	void addListViewItem(MdiSubWindow *w);

	//!  hides or shows windows in the current folder and changes the view windows policy
	void setShowWindowsPolicy(int p);

	//!  returns a pointer to the root project folder
	Folder* projectFolder();

	//!  used by the findDialog
	void find(const QString& s, bool windowNames, bool labels, bool folderNames,
			  bool caseSensitive, bool partialMatch, bool subfolders);

	//!  initializes the list of items dragged by the user
	void dragFolderItems(QList<Q3ListViewItem *> items){draggedItems = items;};

	//!  Drop the objects in the list draggedItems to the folder of the destination item
	void dropFolderItems(Q3ListViewItem *dest);

	//!  moves a folder item to another
	/**
	 * \param src source folder item
	 * \param dest destination folder item
	 */
	void moveFolder(FolderListItem *src, FolderListItem *dest);
	//!  copies a folder to another
	/**
	 * \param src source folder
	 * \param dest destination folder
     */
	bool copyFolder(Folder *src, Folder *dest);

	void foldersMenuActivated( int id );
	//@}

	//! \name Scripting
	//@{
	//! notify the user that an error occured in the scripting system
	void scriptError(const QString &message, const QString &scriptName, int lineNumber);
	//! execute all notes marked auto-exec
	void executeNotes();
	//! show scripting language selection dialog
	void showScriptingLangDialog();
	//! create a new environment for the current scripting language
	void restartScriptingEnv();
	//! print to scripting console (if available) or to stdout
	void scriptPrint(const QString &text);
	//! switches to the given scripting language; if this is the same as the current one and force is true, restart it
	bool setScriptingLanguage(const QString &lang, bool force=false);

	void scriptsDirPathChanged(const QString& path);
	//@}

	void showToolBarsMenu();

signals:
	void modified();

private:
    void initCompleter();
	virtual QMenu * createPopupMenu(){return NULL;};

private slots:
    //! \name Initialization
	//@{
	void insertTranslatedStrings();
	void translateActionsStrings();
	void init(bool factorySettings = false);
	void initGlobalConstants();
	void createActions();
	void initMainMenu();
	void initToolBars();
	void initPlot3DToolBar();
	void disableActions();
	void customColumnActions();
	void disableToolbars();
	void customToolBars(QMdiSubWindow* w);
	void customMenu(QMdiSubWindow* w);
	void windowActivated(QMdiSubWindow *w);
	//@}

	void analysisMenuAboutToShow();
	void fileMenuAboutToShow();
	void matrixMenuAboutToShow();
	void plotMenuAboutToShow();
	void plotDataMenuAboutToShow();
	void tableMenuAboutToShow();
	void windowsMenuAboutToShow();
	void windowsMenuActivated( int id );

	//! \name Font Format Functions
	//@{
	void enableTextEditor(Graph *g);
	void setFormatBarFont(const QFont &);
	void setFontSize(int);
	void setFontFamily(const QFont &);
	void setItalicFont(bool);
	void setBoldFont(bool);
    void insertSuperscript();
    void insertSubscript();
    void underline();
    void insertGreekSymbol();
    void insertGreekMajSymbol();
    void insertMathSymbol();
	//@}

	void showCustomActionDialog();
	void performCustomAction(QAction *);

	void hideSelectedColumns();
	void showAllColumns();
	void closedLastCopiedLayer(){lastCopiedLayer = NULL;};

// TODO: a lot of this stuff should be private
public:
	//! End of line convention used for copy/paste operations and when exporting tables/matrices to ASCII files.
	EndLineChar d_eol;
    //! Flag telling if the in-place editing of 2D plot labels is enabled
    bool d_in_place_editing;
    //! Flag telling if completion hints are enabled
    bool d_completion;
    //! Flag telling if line numbers are displayed in note windows
    bool d_note_line_numbers;
	QString d_python_config_folder;
	QString d_translations_folder;
	//! Flag telling if the application is opening a project file or not
	bool d_opening_file;
    QString customActionsDirPath;
	bool d_matrix_tool_bar, d_file_tool_bar, d_table_tool_bar, d_column_tool_bar, d_edit_tool_bar;
	bool d_plot_tool_bar, d_plot3D_tool_bar, d_display_tool_bar, d_format_tool_bar;
	bool d_backup_files;
	WindowType d_init_window_type;
	QRect d_script_win_rect, d_app_rect;
	bool d_script_win_on_top;
	bool d_inform_rename_table;
	QString d_export_col_separator;
	bool d_export_col_names, d_export_table_selection, d_export_col_comment;
    //! Last selected filter in export image dialog
    QString d_image_export_filter;
    bool d_export_transparency;
    int d_export_quality;
    int d_export_resolution;
    bool d_export_color;
	//! Locale used to specify the decimal separators in imported ASCII files
	QLocale d_clipboard_locale;
	//! Locale used to specify the decimal separators in imported ASCII files
	QLocale d_ASCII_import_locale;
	//! End of line convention used to import ASCII files.
	EndLineChar d_ASCII_end_line;
    //! Last selected filter in import ASCII dialog
    QString d_ASCII_file_filter, d_ASCII_comment_string;
	bool d_ASCII_import_comments, d_ASCII_import_read_only, d_ASCII_import_preview;
	int d_ASCII_import_mode, d_preview_lines;
	//! Specifies if only the Tables/Matrices in the current folder should be displayed in the Add/remove curve dialog.
	bool d_show_current_folder;
	bool d_scale_plots_on_print, d_print_cropmarks;
	bool d_show_table_comments;
	bool d_extended_plot_dialog;
	bool d_extended_import_ASCII_dialog;
	bool d_extended_export_dialog;
	bool d_extended_open_dialog;
	bool generateUniformFitPoints;
	bool generatePeakCurves;
	int peakCurvesColor;
	//! User defined size for the Add/Remove curves dialog
	QSize d_add_curves_dialog_size;

	//! Scale the errors output in fit operations with reduced chi^2
	bool fit_scale_errors;

	//! Number of points in a generated fit curve
	int fitPoints;

	//! Calculate only 2 points in a generated linear fit function curve
	bool d_2_linear_fit_points;

	bool pasteFitResultsToPlot;

	//! Write fit output information to Result Log
	bool writeFitResultsToLog;

	//! precision used for the output of the fit operations
	int fit_output_precision;

	//! default precision to be used for all other operations than fitting
	int d_decimal_digits;

	//! pointer to the current folder in the project
	Folder *current_folder;
	//! Describes which windows are shown when the folder becomes the current folder
	ShowWindowsPolicy show_windows_policy;
	enum {MaxRecentProjects = 10};
	//! File version code used when opening project files (= maj * 100 + min * 10 + patch)
	int d_file_version;

	QColor d_rect_default_background;
	QBrush d_rect_default_brush;
	QColor workspaceColor, panelsColor, panelsTextColor;
	QColor d_graph_background_color, d_graph_canvas_color, d_graph_border_color, d_canvas_frame_color;
	int d_graph_background_opacity, d_graph_canvas_opacity, d_graph_border_width;
	QString appStyle, workingDir;

	//! Path to the folder where the last template file was opened/saved
	QString templatesDir;
	bool smooth3DMesh, autoScaleFonts, autoResizeLayers, autoSearchUpdates;
	bool confirmCloseTable, confirmCloseMatrix, confirmClosePlot2D, confirmClosePlot3D;
	bool confirmCloseFolder, confirmCloseNotes;
	bool titleOn, autoSave, drawBackbones, allAxesOn, autoscale2DPlots, antialiasing2DPlots;
	int majTicksStyle, minTicksStyle, legendFrameStyle, autoSaveTime, axesLineWidth, canvasFrameWidth;
	QColor legendBackground, legendTextColor, defaultArrowColor;
	int defaultArrowHeadLength, defaultArrowHeadAngle, d_legend_default_angle;
	double defaultArrowLineWidth, defaultCurveLineWidth;
	bool defaultArrowHeadFill;
	Qt::PenStyle defaultArrowLineStyle;
	QPen d_frame_widget_pen;
	int majTicksLength, minTicksLength, defaultPlotMargin;
	int defaultCurveStyle, defaultSymbolSize;
	QFont appFont, plot3DTitleFont, plot3DNumbersFont, plot3DAxesFont;
	QFont tableTextFont, tableHeaderFont, plotAxesFont, plotLegendFont, plotNumbersFont, plotTitleFont;
	QColor tableBkgdColor, tableTextColor, tableHeaderColor;
	QString projectname,columnSeparator, helpFilePath, appLanguage;
	QString configFilePath, fitPluginsPath, fitModelsPath, asciiDirPath, imagesDirPath, scriptsDirPath;
	int ignoredLines, savingTimerId, plot3DResolution, recentMenuID;
	bool renameColumns, strip_spaces, simplify_spaces;
	QStringList recentProjects;
	bool saved, showPlot3DProjection, showPlot3DLegend, orthogonal3DPlots, autoscale3DPlots;
	QStringList plot3DColors, locales;
	QStringList functions; //user-defined functions;
	QStringList xFunctions, yFunctions, rFunctions, thetaFunctions; // user functions for parametric and polar plots
	QStringList surfaceFunc; //user-defined surface functions;
    QStringList d_param_surface_func; //user-defined parametric surface functions;
	//! List of tables and matrices renamed in order to avoid conflicts when appending a project to a folder
	QStringList renamedTables;

	//! \name variables used when user copy/paste markers
	//@{
	FrameWidget *d_enrichement_copy;
	ArrowMarker *d_arrow_copy;
	//@}

	//! Equals true if an automatical search for updates was performed on start-up otherwise is set to false;
	bool autoSearchUpdatesRequest;

	//! The scripting language to use for new projects.
	QString defaultScriptingLang;

private:
	MdiSubWindow *d_active_window;
    TextEditor *d_text_editor;
	// Flag telling if table values should be automatically recalculated when values in a column are modified.
	bool d_auto_update_table_values;
    int d_matrix_undo_stack_size;

	//! Workaround for the new colors introduced in rev 447
	int convertOldToNewColorIndex(int cindex);

	//! Stores the pointers to the dragged items from the FolderListViews objects
	QList<Q3ListViewItem *> draggedItems;

	//! Used when checking for new versions
	QHttp http;
	//! Used when checking for new versions
	QBuffer version_buffer;

	Graph *lastCopiedLayer;
	QSplitter *explorerSplitter;

	QAssistantClient *assistant;
	ScriptWindow *scriptWindow;
	QTranslator *appTranslator, *qtTranslator;
	QDockWidget *explorerWindow, *undoStackWindow;
	QTextEdit *results;
#ifdef SCRIPTING_CONSOLE
	QDockWidget *consoleWindow;
	QTextEdit *console;
#endif
	QMdiArea *d_workspace;

    QToolBar *fileTools, *plotTools, *tableTools, *columnTools, *plot3DTools, *displayBar, *editTools, *plotMatrixBar;
	QToolBar *formatToolBar;
	QToolButton *btnResults;
	QWidgetList *hiddenWindows;
	QLineEdit *info;
	//! Completer used in notes and in the script window
	QCompleter *d_completer;

	QMenu *windowsMenu, *foldersMenu, *view, *graph, *fileMenu, *format, *edit, *recent;
	QMenu *help, *plot2DMenu, *analysisMenu, *multiPeakMenu;
	QMenu *matrixMenu, *plot3DMenu, *plotDataMenu, *tablesDepend, *scriptingMenu;
	QMenu *tableMenu, *fillMenu, *normMenu, *newMenu, *exportPlotMenu, *smoothMenu, *filterMenu, *decayMenu;

	QAction *actionEditCurveRange, *actionCurveFullRange, *actionShowAllCurves, *actionHideCurve, *actionHideOtherCurves;
	QAction *actionEditFunction, *actionRemoveCurve, *actionShowCurveWorksheet, *actionShowCurvePlotDialog;
    QAction *actionNewProject, *actionNewNote, *actionNewTable, *actionNewFunctionPlot;
    QAction *actionNewSurfacePlot, *actionNewMatrix, *actionNewGraph, *actionNewFolder;
    QAction *actionOpen, *actionLoadImage, *actionSaveProject, *actionSaveProjectAs, *actionImportImage;
    QAction *actionLoad, *actionUndo, *actionRedo;
    QAction *actionCopyWindow, *actionShowAllColumns, *actionHideSelectedColumns;
    QAction *actionCutSelection, *actionCopySelection, *actionPasteSelection, *actionClearSelection;
    QAction *actionShowExplorer, *actionShowLog, *actionAddLayer, *actionShowLayerDialog, *actionAutomaticLayout;
#ifdef SCRIPTING_CONSOLE
    QAction *actionShowConsole;
#endif
    QAction *actionSwapColumns, *actionMoveColRight, *actionMoveColLeft, *actionMoveColFirst, *actionMoveColLast;
    QAction *actionExportGraph, *actionExportAllGraphs, *actionPrint, *actionPrintAllPlots, *actionShowExportASCIIDialog;
    QAction *actionExportPDF, *actionReadOnlyCol;
    QAction *actionCloseAllWindows, *actionClearLogInfo, *actionShowPlotWizard, *actionShowConfigureDialog;
    QAction *actionShowCurvesDialog, *actionAddErrorBars, *actionAddFunctionCurve, *actionUnzoom, *actionNewLegend, *actionAddImage, *actionAddText;
    QAction *actionPlotL, *actionPlotP, *actionPlotLP, *actionPlotVerticalDropLines, *actionPlotSpline;
    QAction *actionPlotVertSteps, *actionPlotHorSteps, *actionPlotVerticalBars;
	QAction *actionPlotHorizontalBars, *actionPlotArea, *actionPlotPie, *actionPlotVectXYAM, *actionPlotVectXYXY;
    QAction *actionPlotHistogram, *actionPlotStackedHistograms, *actionPlot2VerticalLayers, *actionPlot2HorizontalLayers, *actionPlot4Layers, *actionPlotStackedLayers;
    QAction *actionPlot3DRibbon, *actionPlot3DBars, *actionPlot3DScatter, *actionPlot3DTrajectory;
    QAction *actionPlotDoubleYAxis, *actionAddInsetLayer, *actionAddInsetCurveLayer;
    QAction *actionShowColStatistics, *actionShowRowStatistics, *actionShowIntDialog, *actionIntegrate;
    QAction *actionDifferentiate, *actionFitLinear, *actionShowFitPolynomDialog;
    QAction *actionShowExpDecayDialog, *actionShowTwoExpDecayDialog, *actionShowExpDecay3Dialog;
    QAction *actionFitExpGrowth, *actionFitSigmoidal, *actionFitGauss, *actionFitLorentz, *actionShowFitDialog;
    QAction *actionShowAxisDialog, *actionShowTitleDialog;
    QAction *actionShowColumnOptionsDialog, *actionShowColumnValuesDialog, *actionShowColsDialog, *actionShowRowsDialog;
    QAction *actionTableRecalculate, *actionExtractGraphs, *actionExtractLayers;
    QAction *actionAbout, *actionShowHelp, *actionChooseHelpFolder;
    QAction *actionRename, *actionCloseWindow, *actionConvertTable;
    QAction *actionAddColToTable, *actionDeleteLayer, *actionInterpolate;
    QAction *actionResizeActiveWindow, *actionHideActiveWindow;
    QAction *actionShowMoreWindows, *actionPixelLineProfile, *actionIntensityTable;
    QAction *actionShowLineDialog, *actionShowTextDialog;
    QAction *actionActivateWindow, *actionMinimizeWindow, *actionMaximizeWindow, *actionHideWindow, *actionResizeWindow;
    QAction *actionEditSurfacePlot, *actionAdd3DData;
	QAction *actionMatrixDeterminant, *actionSetMatrixProperties, *actionConvertMatrixXYZ, *actionConvertMatrixYXZ;
	QAction *actionSetMatrixDimensions, *actionConvertMatrixDirect, *actionSetMatrixValues, *actionTransposeMatrix, *actionInvertMatrix;
	QAction *actionPlot3DWireFrame, *actionPlot3DHiddenLine, *actionPlot3DPolygons, *actionPlot3DWireSurface;
	QAction *actionColorMap, *actionContourMap, *actionGrayMap;
	QAction *actionDeleteFitTables, *actionShowGridDialog, *actionTimeStamp;
	QAction *actionSmoothSavGol, *actionSmoothFFT, *actionSmoothAverage, *actionFFT;
    QAction *actionUncertaintyPropagationAnalysis;
	QAction *actionLowPassFilter, *actionHighPassFilter, *actionBandPassFilter, *actionBandBlockFilter;
	QAction *actionSortTable, *actionSortSelection, *actionNormalizeSelection;
	QAction *actionNormalizeTable, *actionConvolute, *actionDeconvolute, *actionCorrelate, *actionAutoCorrelate;
	QAction *actionTranslateHor, *actionTranslateVert, *actionSetAscValues, *actionSetRandomValues;
	QAction *actionSetXCol, *actionSetYCol, *actionSetZCol, *actionSetLabelCol, *actionDisregardCol, *actionSetXErrCol, *actionSetYErrCol;
	QAction *actionBoxPlot, *actionMultiPeakGauss, *actionMultiPeakLorentz, *actionCheckUpdates;
	QAction *actionDonate, *actionHomePage, *actionDownloadManual, *actionTechnicalSupport, *actionTranslations;
	QAction *actionHelpForums, *actionHelpBugReports;
	QAction *actionShowPlotDialog, *actionShowScaleDialog, *actionOpenTemplate, *actionSaveTemplate;
	QAction *actionNextWindow, *actionPrevWindow;
	QAction *actionScriptingLang, *actionRestartScripting, *actionClearTable, *actionGoToRow, *actionGoToColumn;
	QAction *actionNoteExecute, *actionNoteExecuteAll, *actionNoteEvaluate, *actionSaveNote;
	QAction *actionShowScriptWindow, *actionFrequencyCount;
	QAction *actionAnimate, *actionPerspective, *actionFitFrame, *actionResetRotation;
    QAction *actionDeleteRows, *actionDrawPoints, *actionAddZoomPlot;
	QAction *btnCursor, *btnSelect, *btnPicker, *btnRemovePoints, *btnMovePoints;
	QAction *btnZoomIn, *btnZoomOut, *btnPointer, *btnLine, *btnArrow;
	QAction *actionFlipMatrixVertically, *actionFlipMatrixHorizontally, *actionRotateMatrix;
	QAction *actionViewMatrixImage, *actionViewMatrix, *actionExportMatrix;
    QAction *actionMatrixGrayScale, *actionMatrixRainbowScale, *actionMatrixCustomScale, *actionRotateMatrixMinus;
    QAction *actionMatrixXY, *actionMatrixColumnRow, *actionImagePlot, *actionToolBars;
    QAction *actionMatrixFFTDirect, *actionMatrixFFTInverse;
	QAction *actionFontBold, *actionFontItalic, *actionFontBox, *actionFontSize;
	QAction *actionSuperscript, *actionSubscript, *actionUnderline, *actionGreekSymbol, *actionCustomActionDialog;
	QAction *actionGreekMajSymbol, *actionMathSymbol;
	QAction *Box, *Frame, *None;
    QAction *front, *back, *right, *left, *ceil, *floor, *floordata, *flooriso, *floornone;
    QAction *wireframe, *hiddenline, *polygon, *filledmesh, *pointstyle, *barstyle, *conestyle, *crossHairStyle;
    QAction *actionShowUndoStack, *actionShowNoteLineNumbers, *actionAddFormula, *actionAddRectangle;
    QActionGroup *coord, *floorstyle, *grids, *plotstyle, *dataTools;
    QList<QAction *> d_user_actions;
    QUndoView *d_undo_view;
};
#endif
