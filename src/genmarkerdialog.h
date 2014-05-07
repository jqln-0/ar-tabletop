#ifndef __GEN_MARKER_DIALOG_H_
#define __GEN_MARKER_DIALOG_H_

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <string>

#include <aruco/aruco.h>
#include <aruco/arucofidmarkers.h>

#include "matconv.h"

namespace Ui {
class GenerateMarkerDialog;
}

class GenerateMarkerDialog : public QDialog {
  Q_OBJECT

 public:
  explicit GenerateMarkerDialog(QWidget *parent = 0);
  ~GenerateMarkerDialog();

 public
slots:
  void Save();
  void Generate(int id);

 private:
  Ui::GenerateMarkerDialog *ui;
  cv::Mat marker_;
  QGraphicsScene scene_;
};

#endif  // __GEN_MARKER_DIALOG_H_
