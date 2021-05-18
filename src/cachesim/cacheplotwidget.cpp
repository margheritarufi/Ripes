#include "cacheplotwidget.h"
#include "ui_cacheplotwidget.h"

#include <QCheckBox>
#include <QClipboard>
#include <QFileDialog>
#include <QPushButton>
#include <QToolBar>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <algorithm>

#include "defines.h"
#include "enumcombobox.h"
#include "processorhandler.h"
#include "ripessettings.h"

#include "limits.h"

namespace {

inline QPointF stepPoint(const QPointF& p1, const QPointF& p2) {
    return QPointF(p2.x(), p1.y());
}

}  // namespace

namespace Ripes {

CachePlotWidget::CachePlotWidget(QWidget* parent) : QWidget(parent), m_ui(new Ui::CachePlotWidget) {
    m_ui->setupUi(this);
    setWindowTitle("Cache Access Statistics");

    setupEnumCombobox(m_ui->num, s_cacheVariableStrings);
    setupEnumCombobox(m_ui->den, s_cacheVariableStrings);

    // Set default ratio plot to be hit rate
    setEnumIndex(m_ui->num, Variable::Hits);
    setEnumIndex(m_ui->den, Variable::Accesses);

    m_rangeWidgets = {m_ui->rangeMax, m_ui->rangeMin, m_ui->rangeSlider};

    connect(m_ui->num, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CachePlotWidget::variablesChanged);
    connect(m_ui->den, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CachePlotWidget::variablesChanged);
    connect(m_ui->windowed, &QCheckBox::toggled, this, &CachePlotWidget::variablesChanged);
    connect(m_ui->windowCycles, QOverload<int>::of(&QSpinBox::valueChanged), this, &CachePlotWidget::variablesChanged);

    m_ui->rangeMin->setValue(0);
    m_ui->rangeMax->setValue(ProcessorHandler::getProcessor()->getCycleCount());

    connect(m_ui->rangeMin, QOverload<int>::of(&QSpinBox::valueChanged),
            [=] { rangeChanged(RangeChangeSource::Comboboxes); });
    connect(m_ui->rangeMax, QOverload<int>::of(&QSpinBox::valueChanged),
            [=] { rangeChanged(RangeChangeSource::Comboboxes); });
    connect(m_ui->rangeSlider, &ctkRangeSlider::valuesChanged, [=] { rangeChanged(RangeChangeSource::Slider); });

    const QIcon sizeBreakdownIcon = QIcon(":/icons/info.svg");
    m_ui->sizeBreakdownButton->setIcon(sizeBreakdownIcon);
    connect(m_ui->sizeBreakdownButton, &QPushButton::clicked, this, &CachePlotWidget::showSizeBreakdown);
    connect(m_ui->windowed, &QCheckBox::toggled, m_ui->windowCycles, &QWidget::setEnabled);
    connect(m_ui->windowed, &QCheckBox::toggled, m_ui->mavgCursor, &QWidget::setEnabled);
}

void CachePlotWidget::showSizeBreakdown() {
    QString sizeText;

    const auto cacheSize = m_cache->getCacheSize();

    for (const auto& component : cacheSize.components) {
        sizeText += component + "\n";
    }

    sizeText += "\nTotal: " + QString::number(cacheSize.bits) + " Bits";

    QMessageBox::information(this, "Cache Size Breakdown", sizeText);
}

void CachePlotWidget::setCache(std::shared_ptr<CacheSim> cache) {
    m_cache = cache;

    connect(m_cache.get(), &CacheSim::hitrateChanged, this, &CachePlotWidget::updateHitrate);
    connect(m_cache.get(), &CacheSim::configurationChanged,
            [=] { m_ui->size->setText(QString::number(m_cache->getCacheSize().bits)); });
    m_ui->size->setText(QString::number(m_cache->getCacheSize().bits));

    connect(ProcessorHandler::get(), &ProcessorHandler::processorClockedNonRun, [=] {
        updateAllowedRange(RangeChangeSource::Cycles);
        updateRatioPlot();
    });
    connect(ProcessorHandler::get(), &ProcessorHandler::runFinished, [=] {
        updateAllowedRange(RangeChangeSource::Cycles);
        updateRatioPlot();
    });
    connect(m_cache.get(), &CacheSim::cacheInvalidated, [=] {
        updateAllowedRange(RangeChangeSource::Cycles);
        resetRatioPlot();
    });

    m_plot = new QChart();
    m_series = new QLineSeries(m_plot);
    auto defaultPen = m_series->pen();  // Inherit default pen state
    defaultPen.setColor(QColor(FoundersRock));
    m_series->setName("Total");
    m_series->setPen(defaultPen);
    m_plot->addSeries(m_series);
    m_mavgSeries = new QLineSeries(m_plot);
    m_mavgSeries->setName("Moving avg.");
    defaultPen.setColor(QColor(Medalist));
    m_mavgSeries->setPen(defaultPen);
    m_plot->addSeries(m_mavgSeries);
    m_plot->createDefaultAxes();
    m_plot->legend()->hide();
    m_ui->plotView->setPlot(m_plot);
    setupPlotActions();

    variablesChanged();
    rangeChanged(RangeChangeSource::Comboboxes);
    updateHitrate();
}

void CachePlotWidget::updateAllowedRange(const RangeChangeSource src) {
    // Block all range signals while changing ranges
    for (const auto& w : m_rangeWidgets) {
        w->blockSignals(true);
    }

    // Update allowed ranges
    const unsigned cycles = ProcessorHandler::getProcessor()->getCycleCount();

    bool atMax = false;

    if (src == RangeChangeSource::Cycles) {
        atMax = m_ui->rangeSlider->maximumPosition() == m_ui->rangeSlider->maximum();
    }

    m_ui->rangeMin->setMinimum(0);
    m_ui->rangeSlider->setMinimum(0);
    m_ui->rangeSlider->setMaximum(cycles);
    m_ui->rangeMax->setMaximum(cycles);

    if (src == RangeChangeSource::Comboboxes) {
        m_ui->rangeSlider->setMinimumPosition(m_ui->rangeMin->value());
        m_ui->rangeSlider->setMaximumPosition(m_ui->rangeMax->value());
    } else if (src == RangeChangeSource::Slider) {
        m_ui->rangeMax->setMinimum(m_ui->rangeSlider->minimumValue());
        m_ui->rangeMin->setMaximum(m_ui->rangeSlider->maximumValue());
        m_ui->rangeMin->setValue(m_ui->rangeSlider->minimumPosition());
        m_ui->rangeMax->setValue(m_ui->rangeSlider->maximumPosition());
    }

    if (atMax) {
        m_ui->rangeSlider->setMaximumPosition(cycles);
        m_ui->rangeMax->setValue(cycles);
    }

    for (const auto& w : m_rangeWidgets) {
        w->blockSignals(false);
    }
}

CachePlotWidget::~CachePlotWidget() {
    delete m_ui;
    delete m_plot;
}

void CachePlotWidget::setupPlotActions() {
    auto showMarkerFunctor = [=](const QLineSeries* series) {
        return [=](bool visible) {
            if (visible) {
                m_ui->plotView->showSeriesMarker(series);
            } else {
                m_ui->plotView->hideSeriesMarker(series);
            }
        };
    };

    m_totalMarkerAction = new QAction("Enable total crosshair", this);
    m_totalMarkerAction->setIcon(QIcon(":/icons/crosshair_blue.svg"));
    m_totalMarkerAction->setCheckable(true);
    m_ui->ratioCursor->setDefaultAction(m_totalMarkerAction);
    connect(m_totalMarkerAction, &QAction::toggled, showMarkerFunctor(m_series));
    m_totalMarkerAction->setChecked(true);

    m_mavgMarkerAction = new QAction("Enable moving average crosshair", this);
    m_mavgMarkerAction->setIcon(QIcon(":/icons/crosshair_gold.svg"));
    m_mavgMarkerAction->setCheckable(true);
    m_ui->mavgCursor->setDefaultAction(m_mavgMarkerAction);
    connect(m_mavgMarkerAction, &QAction::toggled, showMarkerFunctor(m_mavgSeries));
    m_mavgMarkerAction->setChecked(true);

    const QIcon copyIcon = QIcon(":/icons/documents.svg");
    m_copyDataAction = new QAction("Copy data to clipboard", this);
    m_copyDataAction->setIcon(copyIcon);
    m_ui->copyData->setDefaultAction(m_copyDataAction);
    connect(m_copyDataAction, &QAction::triggered, this, &CachePlotWidget::copyPlotDataToClipboard);

    const QIcon saveIcon = QIcon(":/icons/saveas.svg");
    m_savePlotAction = new QAction("Save plot to file", this);
    m_savePlotAction->setIcon(saveIcon);
    m_ui->savePlot->setDefaultAction(m_savePlotAction);
    connect(m_savePlotAction, &QAction::triggered, this, &CachePlotWidget::savePlot);
}

void CachePlotWidget::savePlot() {
    const QString filename = QFileDialog::getSaveFileName(this, "Save file", "", "Images (*.png)");
    if (!filename.isEmpty()) {
        const QPixmap p = m_ui->plotView->getPlotPixmap();
        p.save(filename, "PNG");
    }
}

void CachePlotWidget::copyPlotDataToClipboard() const {
    std::vector<Variable> allVariables;
    for (int i = 0; i < N_TraceVars; i++) {
        allVariables.push_back(static_cast<Variable>(i));
    }
    const auto& allData = gatherData();

    std::map<unsigned /*cycle*/, QStringList> dataStrings;
    QStringList header;

    // Write cycles
    header << "cycle";
    for (const auto& dataPoint : allData.begin()->second) {
        dataStrings[dataPoint.x()] << QString::number(dataPoint.x());
    }

    // Write variables
    for (const auto& variableData : allData) {
        header << s_cacheVariableStrings.at(variableData.first);
        for (const auto& dataPoint : variableData.second) {
            dataStrings[dataPoint.x()] << QString::number(dataPoint.y());
        }
    }

    // Assemble string
    QString outString;
    outString += header.join('\t') + '\n';

    for (const auto& dataString : dataStrings) {
        outString += dataString.second.join('\t') + '\n';
    }

    QApplication::clipboard()->setText(outString);
}

void CachePlotWidget::rangeChanged(const RangeChangeSource src) {
    updateAllowedRange(src);
    if (m_plot) {
        m_plot->axes(Qt::Horizontal).first()->setRange(m_ui->rangeMin->value(), m_ui->rangeMax->value());
    }
}

void CachePlotWidget::variablesChanged() {
    m_numerator = getEnumValue<Variable>(m_ui->num);
    m_denominator = getEnumValue<Variable>(m_ui->den);
    resetRatioPlot();
}

std::map<CachePlotWidget::Variable, QList<QPoint>> CachePlotWidget::gatherData(unsigned fromCycle) const {
    std::map<Variable, QList<QPoint>> cacheData;
    const auto& trace = m_cache->getAccessTrace();

    for (int i = 0; i < N_TraceVars; i++) {
        cacheData[static_cast<Variable>(i)].reserve(trace.size());
    }

    // Gather data

    for (auto it = trace.lower_bound(fromCycle); it != trace.end(); it++) {
        const auto& entry = it->second;
        cacheData[Variable::Writes].append(QPoint(it->first, entry.writes));
        cacheData[Variable::Reads].append(QPoint(it->first, entry.reads));
        cacheData[Variable::Hits].append(QPoint(it->first, entry.hits));
        cacheData[Variable::Misses].append(QPoint(it->first, entry.misses));
        cacheData[Variable::Writebacks].append(QPoint(it->first, entry.writebacks));
        cacheData[Variable::Accesses].append(QPoint(it->first, entry.hits + entry.misses));
    }

    return cacheData;
}

void resample(QLineSeries* series, unsigned target, double& step) {
    QVector<QPointF> newPoints;
    const auto& oldPoints = series->pointsVector();
    step = (oldPoints.last().x() / static_cast<double>(target)) * 2;  // *2 to account for steps
    newPoints.reserve(target);
    for (int i = 0; i < oldPoints.size(); i += step) {
        newPoints << oldPoints.at(i);
    }

#if !defined(QT_NO_DEBUG)
    // sanity check
    QPointF plast = QPointF(0, 0);
    bool first = true;
    for (const auto& p : newPoints) {
        if (!first) {
            Q_ASSERT(p.x() - plast.x() < step * 1.1);
            first = false;
        }
        plast = p;
    }
#endif

    series->replace(newPoints);
}

void CachePlotWidget::updateRatioPlot() {
    const auto newCacheData = gatherData(m_lastCyclePlotted);
    const int nNewPoints = newCacheData.at(Accesses).size();
    if (nNewPoints == 0) {
        return;
    }

    // convenience function to move a series on/off the chart based on the number of points to be added.
    // We have to remove series due to append(QList(...)) calling redraw _for each_ point in the list.
    // Not removing the series for low nNewPoints avoids a flicker in plot labels. Everything is a balance...
    const auto plotMover = [=](QLineSeries* series, bool visible) {
        if (nNewPoints > 2) {
            if (visible) {
                this->m_plot->addSeries(series);
            } else {
                this->m_plot->removeSeries(series);
            }
        }
    };

    m_lastCyclePlotted = newCacheData.at(Accesses).last().x();

    QList<QPointF> newPoints;
    QList<QPointF> newWindowPoints;
    QPointF lastPoint;
    if (m_series->pointsVector().size() > 0) {
        lastPoint = m_series->pointsVector().last();
    } else {
        lastPoint = QPointF(-1, 0);
    }
    for (int i = 0; i < nNewPoints; i++) {
        // Cummulative plot. For the unary variable, "Accesses" is just used to index into the cache data for accessing
        // the x variable.
        const auto& p1 =
            m_numerator == Unary ? QPoint(newCacheData.at(Accesses).at(i).x(), 1) : newCacheData.at(m_numerator).at(i);
        const auto& p2 = m_denominator == Unary ? QPoint(newCacheData.at(Accesses).at(i).x(), 1)
                                                : newCacheData.at(m_denominator).at(i);
        Q_ASSERT(p1.x() == p2.x() && "Data inconsistency");
        double ratio = 0;
        if (p2.y() != 0) {
            ratio = static_cast<double>(p1.y()) / p2.y();
            ratio *= 100.0;
        }
        const QPointF newPoint = QPointF(p1.x(), ratio);
        if (lastPoint.x() >= 0) {
            if (newPoint.x() - lastPoint.x() < m_xStep) {
                // Skip point; irrelevant at the current sampling level
                continue;
            }
            newPoints << stepPoint(lastPoint, newPoint);
        }
        newPoints << newPoint;
        m_maxY = ratio > m_maxY ? ratio : m_maxY;
        m_minY = ratio < m_minY ? ratio : m_minY;
        lastPoint = newPoint;

        // Windowed plot
        if (m_ui->windowed->isChecked()) {
            QPointF dNum, dDen;
            if (!m_lastDiffValid) {
                dNum = p1;
                dDen = p2;
                m_lastDiffValid = true;
            } else {
                dNum = p1 - m_lastDiffData.first;
                dDen = p2 - m_lastDiffData.second;
            }
            double wRatio = 0;
            if (dDen.y() != 0) {
                wRatio = static_cast<double>(dNum.y()) / dDen.y();
                wRatio *= 100.0;
            }

            m_maxY = wRatio > m_maxY ? wRatio : m_maxY;
            m_minY = wRatio < m_minY ? wRatio : m_minY;
            m_mavgData.push(wRatio);

            // Plot moving average
            const double wAvg = std::accumulate(m_mavgData.begin(), m_mavgData.end(), 0.0) / m_mavgData.size();
            newWindowPoints << QPointF(p1.x(), wAvg);
            m_lastDiffData.first = p1;
            m_lastDiffData.second = p2;
        }
    }

    if (newPoints.size() == 0) {
        return;
    }

    plotMover(m_series, false);
    m_series->append(newPoints);
    if (m_ui->windowed->isChecked()) {
        plotMover(m_mavgSeries, false);
        m_mavgSeries->append(newWindowPoints);
    }

    // Determine whether to resample;
    // *2 the allowed points to account for the addition of step points.
    // *2 to account for the fact that the setting specifies the minimum # of points, and we resample at a 2x ratio
    const int maxPoints = RipesSettings::value(RIPES_SETTING_CACHE_MAXPOINTS).toInt() * 2 * 2;
    if (m_series->pointsVector().size() >= maxPoints) {
        resample(m_series, maxPoints / 2, m_xStep);
    }

    plotMover(m_series, true);
    if (m_ui->windowed->isChecked()) {
        plotMover(m_mavgSeries, true);
    }

    m_plot->createDefaultAxes();

    // Add space to label to add space between labels and axis
    QValueAxis* axisY = qobject_cast<QValueAxis*>(m_series->chart()->axes(Qt::Vertical).first());
    QValueAxis* axisX = qobject_cast<QValueAxis*>(m_series->chart()->axes(Qt::Horizontal).first());
    Q_ASSERT(axisY);
    Q_ASSERT(axisX);
    axisY->setLabelFormat("%.1f  ");
    axisY->setTitleText("%");
    int tickInterval = (m_maxY - m_minY) / axisY->tickCount();
    tickInterval = ((tickInterval + 5 - 1) / 5) * 5;  // Round to nearest multiple of 5
    if (tickInterval >= 5) {
        axisY->setTickInterval(tickInterval);
        axisY->setTickType(QValueAxis::TicksDynamic);
        axisY->setLabelFormat("%d  ");
    }
    int axisMaxY = m_maxY * 1.1;
    if (m_maxY <= 100 && m_maxY >= 90) {
        axisMaxY = 100;
    }

    axisX->setLabelFormat("%d  ");
    axisX->setTitleText("Cycle");

    axisY->setRange(m_minY, axisMaxY);
    axisX->setRange(m_ui->rangeSlider->minimumPosition(), m_ui->rangeSlider->maximumPosition());
}

void CachePlotWidget::resetRatioPlot() {
    m_maxY = -DBL_MAX;
    m_minY = DBL_MAX;
    m_series->clear();
    m_mavgSeries->clear();
    m_lastCyclePlotted = 0;
    m_xStep = 1;
    m_lastDiffValid = false;

    m_mavgMarkerAction->setChecked(m_ui->windowed->isChecked() && m_mavgMarkerAction->isChecked());

    if (m_ui->windowed->isChecked()) {
        m_mavgData = FixedQueue<double>(m_ui->windowCycles->value());
        m_mavgSeries->setVisible(true);
        m_plot->legend()->setVisible(true);
    } else {
        m_mavgSeries->setVisible(false);
        m_plot->legend()->setVisible(false);
    }

    updateRatioPlot();
}

void CachePlotWidget::updateHitrate() {
    m_ui->hitrate->setText(QString::number(m_cache->getHitRate(), 'G', 4));
    m_ui->hits->setText(QString::number(m_cache->getHits()));
    m_ui->misses->setText(QString::number(m_cache->getMisses()));
    m_ui->writebacks->setText(QString::number(m_cache->getWritebacks()));
}

}  // namespace Ripes
