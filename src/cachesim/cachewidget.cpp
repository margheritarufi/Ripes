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
  // or QWidget tab_2 (instruction cache). Then, use the pointer of the
  // CacheSim related to this CacheWidget instantiation so that the cache
  // parameters can be written in the "params.vh" file. Here the initial values
  // of the parameters are saved.
  QWidget *typeOfCache = parent;
  if (typeOfCache->objectName().compare("tab", Qt::CaseInsensitive) == 0) {
    // saveDataCacheSimPointer(m_cacheSim); NO
    saveDataCacheSettings(m_cacheSim);
    // saveNbWaysDataCache(m_cacheSim);
    // saveNbLinesDataCache(m_cacheSim);
    // saveNbBlocksDataCache(m_cacheSim);
  }

  if (typeOfCache->objectName().compare("tab_2", Qt::CaseInsensitive) == 0) {
    // saveInstrCacheSimPointer(m_cacheSim); NO
    saveInstrCacheSettings(m_cacheSim);
    /*saveNbWaysInstrCache(m_cacheSim);
    saveNbLinesInstrCache(m_cacheSim);
    saveNbBlocksInstrCache(m_cacheSim);*/
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
    // in the following if clauses overwrite the initial ones.
    QWidget *typeOfCache = parent;
    if (typeOfCache->objectName().compare("tab", Qt::CaseInsensitive) == 0) {
      saveDataCacheSettings(m_cacheSim);
      // saveDataCacheSimPointer(m_cacheSim);
      /*saveNbWaysDataCache(m_cacheSim);
      saveNbLinesDataCache(m_cacheSim);
      saveNbBlocksDataCache(m_cacheSim);*/
    }
    if (typeOfCache->objectName().compare("tab_2", Qt::CaseInsensitive) == 0) {
      saveInstrCacheSettings(m_cacheSim);
      /*saveNbWaysInstrCache(m_cacheSim);
      saveNbLinesInstrCache(m_cacheSim);
      saveNbBlocksInstrCache(m_cacheSim);*/
    }
    // end rufi
  });
}

void CacheWidget::setNextLevelCache(const std::shared_ptr<CacheSim> &cache) {
  m_cacheSim.get()->setNextLevelCache(cache);
}

CacheWidget::~CacheWidget() { delete m_ui; }

} // namespace Ripes
