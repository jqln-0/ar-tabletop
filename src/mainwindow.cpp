#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      frame_timer_(nullptr),
      capturing_(true),
      show_threshold_(false),
      show_markers_(false) {
  ui->setupUi(this);

  frame_timer_ = new QTimer(this);
  connect(frame_timer_, SIGNAL(timeout()), this, SLOT(ProcessFrame()));
  frame_timer_->start(100);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::ProcessFrame() {
  if (!capturing_ || processor_.source_fetcher() == nullptr) {
    return;
  }

  // Perform the processing pipeline.
  processor_.ProcessFrame();

  // Get the background image.
  QImage backdrop;
  bool result;
  if (show_threshold_) {
    result = processor_.GetThresholdedFrame(&backdrop);
  } else {
    result = processor_.GetFrame(&backdrop);
  }

  // Do nothing if the frame was bad.
  if (!result) {
    return;
  }

  // Set the view to the correct size.
  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
  view->setFixedSize(backdrop.size());

  // Draw image to the view.
  scene_.clear();
  scene_.addPixmap(QPixmap::fromImage(backdrop));
  view->setScene(&scene_);
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

  DenoisingFrameFetcher *fetcher = dialog.MakeFilter();
  processor_.set_wrapping_fetcher(fetcher);
}

void MainWindow::OpenGenerateBoardDialog() {
  std::cout << "Generate board dialog.\n";
}

void MainWindow::OpenGenerateMarkerDialog() {
  GenerateMarkerDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenIntrinsicsFile() { ProcessFrame(); }

void MainWindow::OpenSceneFile() { std::cout << "Open scene file.\n"; }

void MainWindow::OpenSourceFile() {
  // Get the user's file.
  QFileDialog dialog(this, "Open source file");
  dialog.setNameFilter("Image files (*.png *.jpg);;Video files (*.avi)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.exec();

  QStringList filenames = dialog.selectedFiles();
  if (filenames.size() != 1) {
    QMessageBox msg(this);
    msg.setText("You must specify a filename!");
    msg.exec();
    return;
  }

  // Determine the filetype and try to create a fetcher.
  FrameFetcher *fetcher = nullptr;
  QStringList parts = filenames[0].split(".");
  QString suffix = parts[parts.size() - 1];
  if (suffix == "png") {
    fetcher = new PhotoFrameFetcher(filenames[0].toStdString());
  } else if (suffix == "avi") {
    fetcher = new VideoFrameFetcher(filenames[0].toStdString());
  } else {
    QMessageBox msg(this);
    msg.setText("Could not recognise file format.");
    msg.exec();
    return;
  }

  // Check if the fetcher is valid.
  if (!fetcher->HasNextFrame()) {
    delete fetcher;
    QMessageBox msg(this);
    msg.setText("Could not read from file.");
    msg.exec();
    return;
  }

  // Assign the fetcher.
  processor_.set_source_fetcher(fetcher);
}

void MainWindow::OpenSourceWebcam() {
  // Try to create the appropriate fetcher.
  FrameFetcher *fetcher = new WebcamFrameFetcher();

  // See if we can get frames from it.
  if (!fetcher->HasNextFrame()) {
    delete fetcher;
    QMessageBox msg(this);
    msg.setText("Error: Could not open webcam for reading.");
    msg.exec();
    return;
  }

  // Assign the fetcher to the pipeline.
  processor_.set_source_fetcher(fetcher);
}

void MainWindow::OpenThresholdDialog() {
  std::cout << "Open threshold dialog.\n";
}

void MainWindow::ToggleShowMarkers() { show_markers_ = !show_markers_; }

void MainWindow::ToggleShowThreshold() { show_threshold_ = !show_threshold_; }
