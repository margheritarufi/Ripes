#include "hwdescriptioncache.h"
#include "cachewidget.h"
//#include "ui_cachewidget.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include "cachegraphic.h"
#include "cacheview.h"
#include "ripessettings.h"

void keepWaysValue(std::shared_ptr<Ripes::CacheSim> cacheSimPtr){
  int ways = cacheSimPtr->getWaysBits();
  qDebug() << "Nb of ways:" << ways;
}
