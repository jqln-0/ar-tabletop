#ifndef __GEN_BOARD_DIALOG_H_
#define __GEN_BOARD_DIALOG_H_

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QMessageBox>
#include <string>
#include <vector>

#include <aruco/aruco.h>
#include <aruco/arucofidmarkers.h>

#include "matconv.h"

namespace Ui {
class GenerateBoardDialog;
}

class GenerateBoardDialog : public QDialog {
  Q_OBJECT

 public:
  explicit GenerateBoardDialog(QWidget *parent = 0);
  ~GenerateBoardDialog();

 public
slots:
  void Save();
  void Generate(int type);

 private:
  Ui::GenerateBoardDialog *ui;
  cv::Mat board_image_;
  aruco::BoardConfiguration board_;
  QGraphicsScene scene_;
  std::vector<int> disallowed_ids_;
};

#endif  // __GEN_BOARD_DIALOG_H_
