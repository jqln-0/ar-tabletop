#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  /*
// Create the fetcher.
frames_ = new BilateralDenoisingFrameFetcher(new WebcamFrameFetcher(), 5, 30);

// Create the timer.
frame_timer_ = new QTimer(this);
connect(frame_timer_, SIGNAL(timeout()), this, SLOT(ProcessFrame()));
frame_timer_->start(60.0 / 1000.0);
  */
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::ProcessFrame() {
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

void MainWindow::OpenCalibrateDialog() { std::cout << "Webcam calibration.\n"; }

void MainWindow::OpenFilteringDialog() {
  NoiseFilterDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();

  if (dialog.result() == dialog.Rejected) {
    return;
  }

  std::cout << "You accepted changes!\n";
  delete dialog.MakeFilter();
}

void MainWindow::OpenGenerateBoardDialog() {
  std::cout << "Generate board dialog.\n";
}

void MainWindow::OpenGenerateMarkerDialog() {
  GenerateMarkerDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenIntrinsicsFile() {
  std::cout << "Open intrinsics file.\n";
  QString filename = QFileDialog::getOpenFileName(this, "Hello, World!");
  if (!filename.isEmpty()) {
    std::cout << filename.toStdString() << "\n";
  } else {
    std::cout << "Got nothing.\n";
  }
}

void MainWindow::OpenSceneFile() { std::cout << "Open scene file.\n"; }

void MainWindow::OpenSourceFile() { std::cout << "Open source file.\n"; }

void MainWindow::OpenSourceWebcam() { std::cout << "Open webcam.\n"; }

void MainWindow::OpenThresholdDialog() {
  std::cout << "Open threshold dialog.\n";
}

void MainWindow::ToggleShowMarkers() { std::cout << "Toggle markers.\n"; }

void MainWindow::ToggleShowThreshold() { std::cout << "Toggle threshold.\n"; }
