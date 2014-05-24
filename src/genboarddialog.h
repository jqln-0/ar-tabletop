#ifndef __GEN_BOARD_DIALOG_H_
#define __GEN_BOARD_DIALOG_H_

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QtDebug>

#include <aruco/aruco.h>
#include <aruco/arucofidmarkers.h>

#include <string>
#include <vector>

#include "matconv.h"
#include "message.h"

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

  // We don't need a wrapper here since we expect boards we generate will always
  // be valid.
  aruco::BoardConfiguration board_;
  cv::Mat board_image_;

  // Scene to display generated board images.
  QGraphicsScene scene_;

  // Marker ids which boards are forbidden from using.
  std::vector<int> disallowed_ids_;
};

#endif  // __GEN_BOARD_DIALOG_H_
