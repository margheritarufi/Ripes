#include "cachewidget.h"
#include "ui_cachewidget.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include "cachegraphic.h"
#include "cacheview.h"
#include "ripessettings.h"

#include "../cachetabwidget.h"
#include "../hwdescriptioncache.h" //rufi
#include <QDebug>

namespace Ripes {

CacheWidget::CacheWidget(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::CacheWidget) {
  m_ui->setupUi(this);

  m_scene = std::make_unique<QGraphicsScene>(this);
  m_cacheSim = std::make_shared<CacheSim>(this);
  m_ui->cacheConfig->setCache(m_cacheSim);
  m_ui->cachePlot->setCache(m_cacheSim);

  // rufi
  // Check if the parent of the current widget is a QWidget tab (data cache)
  // or QWidget tab_2 (instruction cache). Then, save the number of ways,
  // lines and blocks of the cache so that they can be written in the
  // "params.vh" file.
  // Note that this constructor is called before the launch of Ripes GUI,
  // so the cache parameters saved in the following if clauses are the default
  // ones, they are not chosen by the user.
  QWidget *typeOfCache = parent;
  if (typeOfCache->objectName().compare("tab", Qt::CaseInsensitive) == 0) {
    qDebug() << "Initial number of ways of data cache: "
             << m_cacheSim->getWaysBits();
    qDebug() << "Initial number of lines of data cache: "
             << m_cacheSim->getLineBits();
    qDebug() << "Initial number of blocks of data cache: "
             << m_cacheSim->getBlockBits();
    saveNbWaysDataCache(m_cacheSim);
    saveNbLinesDataCache(m_cacheSim);
    saveNbBlocksDataCache(m_cacheSim);
  }

  if (typeOfCache->objectName().compare("tab_2", Qt::CaseInsensitive) == 0) {
    qDebug() << "Initial number of ways of instr cache: "
             << m_cacheSim->getWaysBits();
    qDebug() << "Initial number of lines of instr cache: "
             << m_cacheSim->getLineBits();
    qDebug() << "Initial number of blocks of instr cache: "
             << m_cacheSim->getBlockBits();
    saveNbWaysInstrCache(m_cacheSim);
    saveNbLinesInstrCache(m_cacheSim);
    saveNbBlocksInstrCache(m_cacheSim);
  }
  // end rufi

  auto *cacheGraphic = new CacheGraphic(*m_cacheSim);
  m_scene->addItem(cacheGraphic);
  connect(m_cacheSim.get(), &CacheSim::configurationChanged, this, [=] {
    RipesSettings::getObserver(RIPES_GLOBALSIGNAL_REQRESET)->trigger();

    auto cacheViews = m_scene->views();
    if (cacheViews.size() > 0) {
      static_cast<CacheView *>(cacheViews.at(0))->fitScene();
    }

    // rufi
    // Check if the parent of the current widget is a QWidget tab (data cache)
    // or QWidget tab_2 (instruction cache). Then, save the number of ways,
    // lines and blocks of the cache so that they can be written in the
    // "params.vh" file.
    // Note that this code section is executed any time the user changes the
    // cache parameters from Ripes GUI. Therefore, the cache parameters saved
    // in the following if clauses overwrite the default ones.
    QWidget *typeOfCache = parent;
    if (typeOfCache->objectName().compare("tab", Qt::CaseInsensitive) == 0) {
      saveNbWaysDataCache(m_cacheSim);
      saveNbLinesDataCache(m_cacheSim);
      saveNbBlocksDataCache(m_cacheSim);
    }
    if (typeOfCache->objectName().compare("tab_2", Qt::CaseInsensitive) == 0) {
      saveNbWaysInstrCache(m_cacheSim);
      saveNbLinesInstrCache(m_cacheSim);
      saveNbBlocksInstrCache(m_cacheSim);
    }
    // end rufi
  });
}

void CacheWidget::setNextLevelCache(const std::shared_ptr<CacheSim> &cache) {
  m_cacheSim.get()->setNextLevelCache(cache);
}

CacheWidget::~CacheWidget() { delete m_ui; }

} // namespace Ripes
