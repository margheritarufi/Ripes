#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidgetItem>
#include <QWidget>

#include "iomanager.h"
#include "ripestab.h"

namespace Ripes {

namespace Ui {
class SystemTab;
}

class SystemTabView;
class SystemBlock;
class Arrow;

class SystemTab : public RipesTab {
  Q_OBJECT

public:
  SystemTab(QToolBar *toolbar, QWidget *parent = nullptr);
  ~SystemTab() override;

private:
  Ui::SystemTab *m_ui = nullptr;

  int baseBlockDimension = 150;
  int baseBusHeight = 45;
  int baseSpaceBetweenBlocks = 20;
  int baseDistanceFromBus = 50;
  int startSceneX = 0;
  int startSceneY = 0;
  int arrowThickness = 5;

  QGraphicsScene *scene;
  SystemTabView *view;

  SystemBlock *cpuRect;
  SystemBlock *memoryRect;
  SystemBlock *busRect;

  QVector<SystemBlock *> blocksVector;

  int i = 0;
  int j = 0;

  void setItemStyles(SystemBlock *item, const QColor &color);
  void updateSystemTab();
  void deletePeripheralBlocks(int nbActivePeripherals);
  void addNewPeripherals(std::set<IOBase *> &activePeripherals);
  void connectItemsWithArrow(SystemBlock *startItem, SystemBlock *endItem,
                             const QColor &color);
  void connectItemsWithSecondArrow(SystemBlock *startItem,
                                              SystemBlock *endItem,
                                   const QColor &color);
  int busLength(int nbActivePeripherals);

private slots:
  void onListItemClicked(QListWidgetItem *item);
};

class SystemTabView : public QGraphicsView {
  Q_OBJECT

public:
  SystemTabView(QWidget *parent);

protected:
  void wheelEvent(QWheelEvent *event) override;

private:
  void zoomIn();
  void zoomOut();
};

class Arrow {

public:
  QGraphicsLineItem &getArrowLine() { return m_arrowLine; }
  QPen &getPen() { return m_pen; }
  QGraphicsPolygonItem &getArrowHeadItemUp() { return m_arrowHeadItemUp; }
  QGraphicsPolygonItem &getArrowHeadItemDown() { return m_arrowHeadItemDown; }

private:
  QGraphicsLineItem m_arrowLine; // = nullptr; //it was *arrowLine
  QPen m_pen;
  QGraphicsPolygonItem m_arrowHeadItemUp;
  QGraphicsPolygonItem m_arrowHeadItemDown;
};

enum BlockType { CPU, MEMORY, BUS, PERIPHERAL };

class SystemBlock : public QGraphicsRectItem {
  // Q_OBJECT

public:
  SystemBlock(BlockType type, std::string name, qreal x, qreal y, qreal width,
              qreal height, QGraphicsItem *parent = nullptr);
  QString getQName() { return QString::fromStdString(m_name); }
  BlockType getBlockType() { return m_type; }
  void setName(std::string name) { m_name = name; }
  void updateLabel(const QString &newName);
  Arrow &getArrow() { return m_arrow; }
  Arrow &getSecondArrow() { return m_secondarrow; }
  QGraphicsTextItem &getLabel() { return m_label; }

private:
  BlockType m_type;
  std::string m_name;
  QGraphicsTextItem m_label; // perché pointer?
  QPointF labelPosition;
  Arrow m_arrow; // = nullptr;
  //QGraphicsPolygonItem m_arrowHeadItemUp;
  Arrow m_secondarrow; // = nullptr;
  //QGraphicsPolygonItem m_secondarrowHeadItemUp;
};

} // namespace Ripes
