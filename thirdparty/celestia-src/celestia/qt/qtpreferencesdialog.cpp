// qtpreferencesdialog.cpp
//
// Copyright (C) 2007-2008, Celestia Development Team
// celestia-developers@lists.sourceforge.net
//
// Preferences dialog for Celestia's Qt front-end. Based on
// kdepreferencesdialog.h by Christophe Teyssier.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QFrame>
#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "qtappwin.h"
#include "qtpreferencesdialog.h"
#include "celengine/render.h"
#include "celengine/glcontext.h"
#include "celestia/celestiacore.h"

static uint32 FilterOtherLocations = ~(Location::City |
                                       Location::Observatory |
                                       Location::LandingSite |
                                       Location::Crater |
                                       Location::Mons |
                                       Location::Terra |
                                       Location::Vallis |
                                       Location::Mare);

PreferencesDialog::PreferencesDialog(QWidget* parent, CelestiaCore* core) :
    QDialog(parent),
    appCore(core)
{
    QTabWidget* tabWidget = new QTabWidget(this);
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok,
                                                           Qt::Horizontal,
                                                           this);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()));
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tabWidget);
    layout->addWidget(dialogButtons);
    setLayout(layout);

    QWidget* renderPage = new QWidget();
    QWidget* locationsPage = new QWidget();
    QWidget* timePage = new QWidget();

    tabWidget->addTab(renderPage, "Rendering");
	// Next line patch by Guillemro to avoid showing in STA am empty widget of LOCATIONS
    // tabWidget->addTab(locationsPage, "Locations");
    tabWidget->addTab(timePage, "Time");

    // Render page
    int renderFlags = appCore->getRenderer()->getRenderFlags();
    QHBoxLayout* renderPageLayout = new QHBoxLayout();
    renderPage->setLayout(renderPageLayout);
    
    // Objects group
    QGroupBox* objectsGroup = new QGroupBox(tr("Objects"));
    QVBoxLayout* objectsGroupLayout = new QVBoxLayout();
    objectsGroup->setLayout(objectsGroupLayout);

    QCheckBox* showStarsCheck = new QCheckBox(tr("Stars"));
    connect(showStarsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowStars(bool)));
    showStarsCheck->setChecked(renderFlags & Renderer::ShowStars);
    objectsGroupLayout->addWidget(showStarsCheck);

    QCheckBox* showPlanetsCheck = new QCheckBox(tr("Planets"));
    connect(showPlanetsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowPlanets(bool)));
    showPlanetsCheck->setChecked(renderFlags & Renderer::ShowPlanets);
    objectsGroupLayout->addWidget(showPlanetsCheck);

    QCheckBox* showGalaxiesCheck = new QCheckBox(tr("Galaxies"));
    connect(showGalaxiesCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowGalaxies(bool)));
    showGalaxiesCheck->setChecked(renderFlags & Renderer::ShowGalaxies);
    objectsGroupLayout->addWidget(showGalaxiesCheck);

    QCheckBox* showNebulaeCheck = new QCheckBox(tr("Nebulae"));
    connect(showNebulaeCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowNebulae(bool)));
    showNebulaeCheck->setChecked(renderFlags & Renderer::ShowNebulae);
    objectsGroupLayout->addWidget(showNebulaeCheck);

    QCheckBox* showOpenClustersCheck = new QCheckBox(tr("Open Clusters"));
    connect(showOpenClustersCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowOpenClusters(bool)));
    showOpenClustersCheck->setChecked(renderFlags & Renderer::ShowOpenClusters);
    objectsGroupLayout->addWidget(showOpenClustersCheck);

    objectsGroupLayout->addStretch();

    // Features group
    QGroupBox* featuresGroup = new QGroupBox(tr("Features"));
    QVBoxLayout* featuresGroupLayout = new QVBoxLayout();
    featuresGroup->setLayout(featuresGroupLayout);

    QCheckBox* showAtmospheresCheck = new QCheckBox(tr("Atmospheres"));
    connect(showAtmospheresCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowAtmospheres(bool)));
    showAtmospheresCheck->setChecked(renderFlags & Renderer::ShowAtmospheres);
    featuresGroupLayout->addWidget(showAtmospheresCheck);

    QCheckBox* showCloudMapsCheck = new QCheckBox(tr("Clouds"));
    connect(showCloudMapsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowCloudMaps(bool)));
    showCloudMapsCheck->setChecked(renderFlags & Renderer::ShowCloudMaps);
    featuresGroupLayout->addWidget(showCloudMapsCheck);

    QCheckBox* showNightMapsCheck = new QCheckBox(tr("Night Side Lights"));
    connect(showNightMapsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowNightMaps(bool)));
    showNightMapsCheck->setChecked(renderFlags & Renderer::ShowNightMaps);
    featuresGroupLayout->addWidget(showNightMapsCheck);

    QCheckBox* showEclipseShadowsCheck = new QCheckBox(tr("Eclipse Shadows"));
    connect(showEclipseShadowsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowEclipseShadows(bool)));
    showEclipseShadowsCheck->setChecked(renderFlags & Renderer::ShowEclipseShadows);
    featuresGroupLayout->addWidget(showEclipseShadowsCheck);

    QCheckBox* showRingShadowsCheck = new QCheckBox(tr("Ring Shadows"));
    connect(showRingShadowsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowRingShadows(bool)));
    showRingShadowsCheck->setChecked(renderFlags & Renderer::ShowRingShadows);
    featuresGroupLayout->addWidget(showRingShadowsCheck);

    QCheckBox* showCloudShadowsCheck = new QCheckBox(tr("Cloud Shadows"));
    connect(showCloudShadowsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowCloudShadows(bool)));
    showCloudShadowsCheck->setChecked(renderFlags & Renderer::ShowCloudShadows);
    featuresGroupLayout->addWidget(showCloudShadowsCheck);

    QCheckBox* showCometTailsCheck = new QCheckBox(tr("Comet Tails"));
    connect(showCometTailsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowCometTails(bool)));
    showCometTailsCheck->setChecked(renderFlags & Renderer::ShowCometTails);
    featuresGroupLayout->addWidget(showCometTailsCheck);

    featuresGroupLayout->addStretch();

    // Guides group
    QGroupBox* guidesGroup = new QGroupBox(tr("Guides"));
    QVBoxLayout* guidesGroupLayout = new QVBoxLayout();
    guidesGroup->setLayout(guidesGroupLayout);

    QCheckBox* showOrbitsCheck = new QCheckBox(tr("Orbits"));
    connect(showOrbitsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowOrbits(bool)));
    showOrbitsCheck->setChecked(renderFlags & Renderer::ShowOrbits);
    guidesGroupLayout->addWidget(showOrbitsCheck);

    QCheckBox* showPartialTrajectoriesCheck = new QCheckBox(tr("Partial Trajectories"));
    connect(showPartialTrajectoriesCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowPartialTrajectories(bool)));
    showPartialTrajectoriesCheck->setChecked(renderFlags & Renderer::ShowPartialTrajectories);
    guidesGroupLayout->addWidget(showPartialTrajectoriesCheck);

    QCheckBox* showSmoothLinesCheck = new QCheckBox(tr("Smooth Orbit Lines"));
    connect(showSmoothLinesCheck, SIGNAL(clicked(bool)), this, SLOT(slotSmoothLines(bool)));
    showSmoothLinesCheck->setChecked(renderFlags & Renderer::ShowSmoothLines);
    guidesGroupLayout->addWidget(showSmoothLinesCheck);

    QCheckBox* showCelestialSphereCheck = new QCheckBox(tr("Celestial Grid"));
    connect(showCelestialSphereCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowCelestialSphere(bool)));
    showCelestialSphereCheck->setChecked(renderFlags & Renderer::ShowCelestialSphere);
    guidesGroupLayout->addWidget(showCelestialSphereCheck);

    QCheckBox* showMarkersCheck = new QCheckBox(tr("Markers"));
    connect(showMarkersCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowMarkers(bool)));
    showMarkersCheck->setChecked(renderFlags & Renderer::ShowMarkers);
    guidesGroupLayout->addWidget(showMarkersCheck);

    QCheckBox* showDiagramsCheck = new QCheckBox(tr("Constellations"));
    connect(showDiagramsCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowDiagrams(bool)));
    showDiagramsCheck->setChecked(renderFlags & Renderer::ShowDiagrams);
    guidesGroupLayout->addWidget(showDiagramsCheck);

    QCheckBox* showBoundariesCheck = new QCheckBox(tr("Constellation Boundaries"));
    connect(showBoundariesCheck, SIGNAL(clicked(bool)), this, SLOT(slotShowBoundaries(bool)));
    showBoundariesCheck->setChecked(renderFlags & Renderer::ShowBoundaries);
    guidesGroupLayout->addWidget(showBoundariesCheck);

    guidesGroupLayout->addStretch();

    // Time page
    QHBoxLayout* timePageLayout = new QHBoxLayout();
    timePage->setLayout(timePageLayout);

    QGridLayout* timeLayout = new QGridLayout();
    timeLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QLabel* dateFormatLabel = new QLabel(tr("Date Format: "));
    timeLayout->addWidget(dateFormatLabel, 0, 0);

    dateFormatBox = new QComboBox(this);
    dateFormatBox->setEditable(false);
#ifndef _WIN32
    dateFormatBox->addItem(tr("Local Format"), 0);
#endif
    dateFormatBox->addItem(tr("Time Zone Name"), 1);
    dateFormatBox->addItem(tr("UTC Offset"), 2);

    astro::Date::Format dateFormat = appCore->getDateFormat();
#ifndef _WIN32
    dateFormatBox->setCurrentIndex((int)dateFormat);
#else
    dateFormatBox->setCurrentIndex(dateFormat == 2 ? 1 : 0);
#endif
    
    dateFormatBox->setToolTip(tr("Select Date Format"));
    timeLayout->addWidget(dateFormatBox, 0, 1);
    connect(dateFormatBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotDateFormatChanged()));

    timePageLayout->addLayout(timeLayout);

#if 0
    // TODO: Make a page dedicated to star brightness control
    QCheckBox* showAutoMagCheck = new QCheckBox(tr("Auto Magnitudes"));
    showAutoMagCheck->setChecked(renderFlags & Renderer::ShowAutoMag);
    objectsGroupLayout->addWidget(showAutoMagCheck);
#endif

    renderPageLayout->addWidget(objectsGroup);
    renderPageLayout->addWidget(featuresGroup);
    renderPageLayout->addWidget(guidesGroup);
    
    resize(550, 400);
}


PreferencesDialog::~PreferencesDialog()
{
}


static void setRenderFlag(CelestiaCore* appCore,
                          int flag,
                          bool state)
{
    int renderFlags = appCore->getRenderer()->getRenderFlags() & ~flag;
    appCore->getRenderer()->setRenderFlags(renderFlags | (state ? flag : 0));
}


void PreferencesDialog::slotShowStars(bool show)
{
    setRenderFlag(appCore, Renderer::ShowStars, show);
}


void PreferencesDialog::slotShowPlanets(bool show)
{
    setRenderFlag(appCore, Renderer::ShowPlanets, show);
}


void PreferencesDialog::slotShowGalaxies(bool show)
{
    setRenderFlag(appCore, Renderer::ShowGalaxies, show);
}


void PreferencesDialog::slotShowNebulae(bool show)
{
    setRenderFlag(appCore, Renderer::ShowNebulae, show);
}

void PreferencesDialog::slotShowOpenClusters(bool show)
{
    setRenderFlag(appCore, Renderer::ShowOpenClusters, show);
}


// Features

void PreferencesDialog::slotShowAtmospheres(bool show)
{
    setRenderFlag(appCore, Renderer::ShowAtmospheres, show);
}

void PreferencesDialog::slotShowCloudMaps(bool show)
{
    setRenderFlag(appCore, Renderer::ShowCloudMaps, show);
}

void PreferencesDialog::slotShowNightMaps(bool show)
{
    setRenderFlag(appCore, Renderer::ShowNightMaps, show);
}

void PreferencesDialog::slotShowEclipseShadows(bool show)
{
    setRenderFlag(appCore, Renderer::ShowEclipseShadows, show);
}

void PreferencesDialog::slotShowRingShadows(bool show)
{
    setRenderFlag(appCore, Renderer::ShowRingShadows, show);
}

void PreferencesDialog::slotShowCloudShadows(bool show)
{
    setRenderFlag(appCore, Renderer::ShowCloudShadows, show);
}

void PreferencesDialog::slotShowCometTails(bool show)
{
    setRenderFlag(appCore, Renderer::ShowCometTails, show);
}


// Guides

void PreferencesDialog::slotShowOrbits(bool show)
{
    setRenderFlag(appCore, Renderer::ShowOrbits, show);
}

void PreferencesDialog::slotShowPartialTrajectories(bool show)
{
    setRenderFlag(appCore, Renderer::ShowPartialTrajectories, show);
}

void PreferencesDialog::slotShowSmoothLines(bool show)
{
    setRenderFlag(appCore, Renderer::ShowSmoothLines, show);
}

void PreferencesDialog::slotShowCelestialSphere(bool show)
{
    setRenderFlag(appCore, Renderer::ShowCelestialSphere, show);
}

void PreferencesDialog::slotShowMarkers(bool show)
{
    setRenderFlag(appCore, Renderer::ShowMarkers, show);
}

void PreferencesDialog::slotShowDiagrams(bool show)
{
    setRenderFlag(appCore, Renderer::ShowDiagrams, show);
}

void PreferencesDialog::slotShowBoundaries(bool show)
{
    setRenderFlag(appCore, Renderer::ShowBoundaries, show);    
}


void PreferencesDialog::slotOk()
{
#if 0
    slotApply();
    accept();
#endif
}


// Date Format

void PreferencesDialog::slotDateFormatChanged()
{
    QVariant dateFormatIndex = dateFormatBox->itemData(dateFormatBox->currentIndex());
    astro::Date::Format dateFormat = (astro::Date::Format) dateFormatIndex.toInt();

    appCore->setDateFormat(dateFormat);
}


void PreferencesDialog::slotCancel()
{
#if 0
    appCore->getRenderer()->setRenderFlags(savedRendererFlags);
    appCore->getRenderer()->setLabelMode(savedLabelMode);
    appCore->getRenderer()->setOrbitMask(savedOrbitMask);
    appCore->getRenderer()->setAmbientLightLevel(savedAmbientLightLevel/100.);
    appCore->getSimulation()->setFaintestVisible(savedFaintestVisible/100.);
    appCore->setHudDetail(savedHudDetail);
    appCore->getRenderer()->getGLContext()->setRenderPath((GLContext::GLRenderPath)savedRenderPath);
    appCore->setDistanceToScreen(savedDistanceToScreen);
    appCore->getSimulation()->getActiveObserver()->setLocationFilter(savedLocationFilter);
    appCore->getRenderer()->setMinimumFeatureSize(savedMinFeatureSize);
    appCore->getRenderer()->setVideoSync(savedVideoSync);
    reject();
#endif
}


void PreferencesDialog::slotApply()
{
#if 0
    savedRendererFlags = appCore->getRenderer()->getRenderFlags();
    savedLabelMode = appCore->getRenderer()->getLabelMode();
    savedAmbientLightLevel = int(appCore->getRenderer()->getAmbientLightLevel() * 100);
    savedFaintestVisible = int(appCore->getSimulation()->getFaintestVisible() * 100);
    savedHudDetail = appCore->getHudDetail();
    savedDisplayLocalTime = appCore->getTimeZoneBias();
    savedRenderPath = (int)appCore->getRenderer()->getGLContext()->getRenderPath();
    savedDistanceToScreen = appCore->getDistanceToScreen();
    savedLocationFilter = appCore->getSimulation()->getActiveObserver()->getLocationFilter();
    savedMinFeatureSize = (int)appCore->getRenderer()->getMinimumFeatureSize();
    savedVideoSync = appCore->getRenderer()->getVideoSync();

    keyChooser->commitChanges();

    KGlobal::config()->setGroup("ConfigureDialog");
    KGlobal::config()->writeEntry("SetTimeTimeZoneLocal", setTimezoneCombo->currentItem());
    KGlobal::config()->setGroup(0);

    if (timeHasChanged) {
      if (setTimezoneCombo->currentItem() == 0 || YSpin->value() < 1902 || YSpin->value() > 2037) {
        astro::Date date(0.0);
        date.year=YSpin->value();
        date.month=MSpin->value();
        date.day=DSpin->value();
        date.hour=hSpin->value();
        date.minute=mSpin->value();
        date.seconds=float(sSpin->value());
        appCore->getSimulation()->setTime(astro::UTCtoTDB((double) date));
        appCore->getSimulation()->update(0.0);
      } else {
        struct tm time;
        time.tm_year = YSpin->value() - 1900;
        time.tm_mon = MSpin->value() - 1;
        time.tm_mday = DSpin->value();
        time.tm_hour = hSpin->value();
        time.tm_min = mSpin->value();
        time.tm_sec = sSpin->value();
        appCore->getSimulation()->setTime(astro::UTCtoTDB(astro::secondsToJulianDate(mktime(&time)) + (double) astro::Date(1970, 1, 1)));
        appCore->getSimulation()->update(0.0);
      }
    }
#endif
}


void PreferencesDialog::slotTimeHasChanged()
{
    timeHasChanged = true;
}


void PreferencesDialog::slotFaintestVisible(int m)
{
#if 0
    char buff[20];

    parent->slotFaintestVisible(m / 100.);
    sprintf(buff, "%.2f", m / 100.);
    faintestLabel->setText(buff);
#endif
}


void PreferencesDialog::slotMinFeatureSize(int s)
{
#if 0
    char buff[20];

    parent->slotMinFeatureSize(s);
    sprintf(buff, "%d", s);
    minFeatureSizeLabel->setText(buff);
#endif
}


void PreferencesDialog::slotAmbientLightLevel(int l)
{
#if 0
    char buff[20];

    parent->slotAmbientLightLevel(l / 100.);
    sprintf(buff, "%.2f", l / 100.);
    ambientLabel->setText(buff);
#endif
}


void PreferencesDialog::slotRenderPath(int pathIdx) 
{
#if 0
    int path=0, ipathIdx=0;
    while (ipathIdx != pathIdx) {
        ipathIdx++;
        do {
            path++;
        } while (!appCore->getRenderer()->getGLContext()->renderPathSupported((GLContext::GLRenderPath)path));
    }

    appCore->getRenderer()->getGLContext()->setRenderPath((GLContext::GLRenderPath)path);
    setRenderPathLabel();
#endif
}


void PreferencesDialog::slotDistanceToScreen(int dts)
{
#if 0
    appCore->setDistanceToScreen(dts * 10);
#endif
}


void PreferencesDialog::setNow() 
{
#if 0
    time_t date_t;
    time(&date_t);
    struct tm* tm;
    if (setTimezoneCombo->currentItem() != 0) {
        tm = localtime ( &date_t);
    } else {
        tm = gmtime ( &date_t);
    }
    YSpin->setValue(tm->tm_year + 1900);
    MSpin->setValue(tm->tm_mon + 1);
    DSpin->setValue(tm->tm_mday);
    hSpin->setValue(tm->tm_hour);
    mSpin->setValue(tm->tm_min);
    sSpin->setValue(tm->tm_sec);
#endif
}


void PreferencesDialog::ltSubstract() 
{
#if 0
    double d;

    d = getTime();

    Selection selection = appCore->getSimulation()->getSelection();
    std::string sel_name;

    // LT-delay only for solar bodies && target-speed < 0.99 c

    if (selection.body() != 0 &&
        (appCore->getSimulation()->getTargetSpeed() < 0.99 *
            astro::kilometersToMicroLightYears(astro::speedOfLight))) {
        sel_name = selection.body()->getName();
        Vec3d v = selection.getPosition(d) -
                  appCore->getSimulation()->getObserver().getPosition();
        appCore->setLightDelayActive(!appCore->getLightDelayActive());
        double dist = astro::lightYearsToKilometers(v.length()*1e-6);
        double lt = dist / astro::speedOfLight;
        if (appCore->getLightDelayActive())
            d -= lt / 86400.;
        else
            d += lt / 86400.;
        setTime(d);
    }
#endif
}

