#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Create the fetcher.
  frames_ = new WebcamFrameFetcher();

  // Create the timer.
  frame_timer_ = new QTimer(this);
  connect(frame_timer_, SIGNAL(timeout()), this, SLOT(processFrame()));
  frame_timer_->start(100);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::processFrame() {
  /*
  QGraphicsScene *scene = new(QGraphicsScene);

  // Get the mat and convert to rgb.
  cv::Mat original = frames_->next_frame();
  cv::Mat converted;
  cv::cvtColor(original, converted, cv::COLOR_BGR2RGB);

  // Now convert that Mat to a QPixmap.
  QPixmap pixmap = QPixmap::fromImage(QImage((unsigned char*)converted.data,
  converted.cols, converted.rows,
                          QImage::Format_RGB888));

  // And show the pixmap.
  scene->addPixmap(pixmap);

  QGraphicsView *view = this->findChild<QGraphicsView*>("graphicsView");
  view->setScene(scene);
  view->show();
  */
}
