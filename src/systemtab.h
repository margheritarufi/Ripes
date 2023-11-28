#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QListWidgetItem>


#include "ripestab.h"

namespace Ripes {

namespace Ui {
class SystemTab;
}

class SystemTab : public RipesTab {
  Q_OBJECT

public:
  SystemTab(QToolBar *toolbar, QWidget *parent = nullptr);
  ~SystemTab() override;

private:
  Ui::SystemTab *m_ui = nullptr;
  QGraphicsScene *scene;
  QGraphicsView *view;
  QGraphicsRectItem *cpuItem;
  QGraphicsRectItem *memoryItem;
  QGraphicsRectItem *busItem;
  QGraphicsRectItem *periphItem;

  void setItemStyles(QGraphicsRectItem *item, const QColor &color);
  void connectItemsWithArrow(QGraphicsRectItem *startItem, QGraphicsRectItem *endItem, const QColor &color);


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

  void setItemStyles(QGraphicsRectItem *item, const QColor &color);
  void connectItemsWithArrow(QGraphicsRectItem *startItem, QGraphicsRectItem *endItem, const QColor &color);
  void zoomIn();
  void zoomOut();
};
} // namespace Ripes
