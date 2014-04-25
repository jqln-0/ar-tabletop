#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Create the fetcher.
  frames_ = new BilateralDenoisingFrameFetcher(new WebcamFrameFetcher(), 5, 30);

  // Create the timer.
  frame_timer_ = new QTimer(this);
  connect(frame_timer_, SIGNAL(timeout()), this, SLOT(processFrame()));
  frame_timer_->start(60.0 / 1000.0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::processFrame() {
  QGraphicsScene *scene = new (QGraphicsScene);

  // Convert the Mat to a QPixmap.
  cv::Mat frame;
  frames_->GetNextFrame(&frame);
  QImage conv;
  matToQImage(frame, &conv);
  QPixmap pixmap = QPixmap::fromImage(conv);

  // And show the pixmap.
  scene->addPixmap(pixmap);

  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
	view->setFixedSize(pixmap.size());
  view->setScene(scene);
  view->show();
}
