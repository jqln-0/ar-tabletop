#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTimer>

#include <aruco/aruco.h>

#include "framefetcher.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 public
slots:
  void processFrame();

 private:
  Ui::MainWindow *ui;
  QTimer *frame_timer_;

  // herp derp testing only
  FrameFetcher *frames_;
};

#endif  // MAINWINDOW_H
