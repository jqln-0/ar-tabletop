#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;

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
  frame_timer_->start(50);

  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
  scene_3d_ = new SceneWidget(view);
  scene_3d_->hide();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::ProcessFrame() {
  // Check if we are ready and permitted to process frames.
  if (!capturing_ || !processor_.IsReady()) {
    return;
  }

  // Run through the processing pipeline.
  processor_.ProcessFrame();

  // Get the background image.
  QImage backdrop;
  if (show_threshold_) {
    backdrop = processor_.GetThresholdedFrame(show_markers_);
  } else {
    backdrop = processor_.GetFrame(show_markers_);
  }

  // Do nothing if the frame was bad.
  if (backdrop.size().isEmpty()) {
    return;
  }

  // Inform the SceneWidget of the detected markers.
  scene_3d_->set_markers(processor_.markers());

  // Set the view and scene to the correct size.
  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
  scene_3d_->setFixedSize(backdrop.size());
  view->setFixedSize(backdrop.size());

  // Draw image and scene to view.
  scene_2d_.clear();
  scene_2d_.addPixmap(QPixmap::fromImage(backdrop));
  scene_2d_.addPixmap(scene_3d_->renderPixmap(backdrop.size().width(),
                                              backdrop.size().height()));
  view->setScene(&scene_2d_);
  view->show();
}

void MainWindow::OpenCalibrateDialog() {
  // We need to have a source of frames to pass to the calibrator.
  shared_ptr<FrameFetcher> fetcher = processor_.fetcher();
  if (!fetcher) {
    QMessageBox msg(this);
    msg.setText("Select a source (webcam or file) before calibrating.");
    msg.exec();
    return;
  }

  // Pause capturing until the calibrator is done since it'll want to use the
  // FrameFetcher.
  capturing_ = false;
  CalibrateDialog calibrator(fetcher, this);
  calibrator.exec();
  capturing_ = true;
}

void MainWindow::OpenFilteringDialog() {
  NoiseFilterDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
  if (dialog.result() == dialog.Rejected) {
    return;
  }

  shared_ptr<FrameFetcher> fetcher = dialog.MakeFilter();
  if (fetcher) {
    processor_.set_fetcher(fetcher);
  }
}

void MainWindow::OpenGenerateBoardDialog() {
  GenerateBoardDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenGenerateMarkerDialog() {
  GenerateMarkerDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenIntrinsicsFile() {
  // Get the user's file.
  QFileDialog dialog(this, "Open camera file.");
  dialog.setNameFilter("Camera files (*.camera)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  // Try to read camera parameters from the file.
  try {
    aruco::CameraParameters params;
    params.readFromFile(dialog.selectedFiles()[0].toStdString());
    processor_.set_camera(params);
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText("Failed to read camera parameters from file.");
    msg.exec();
    return;
  }
}

void MainWindow::OpenSceneFile() { std::cout << "Open scene file.\n"; }

void MainWindow::OpenSourceFile() {
  // Get the user's file.
  QFileDialog dialog(this, "Open source file");
  dialog.setNameFilter("Image files (*.png *.jpg);;Video files (*.avi)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  QStringList filenames = dialog.selectedFiles();
  if (filenames.size() != 1) {
    QMessageBox msg(this);
    msg.setText("You must specify a filename!");
    msg.exec();
    return;
  }

  // Try both kinds of possible fetcher, use whichever one is valid.

  // Try the photo fetcher first.
  shared_ptr<FrameFetcher> fetcher =
      make_shared<PhotoFrameFetcher>(filenames[0].toStdString());

  // Alternatively try the video fetcher.
  if (!fetcher->HasNextFrame()) {
    fetcher = make_shared<VideoFrameFetcher>(filenames[0].toStdString());
  }

  // Check if the fetcher is valid.
  if (!fetcher->HasNextFrame()) {
    QMessageBox msg(this);
    msg.setText("Failed to read file! The format may be incorrect.");
    msg.exec();
    return;
  }

  // Assign the fetcher.
  processor_.set_fetcher(fetcher);
}

void MainWindow::OpenSourceWebcam() {
  // Try to create the appropriate fetcher.
  shared_ptr<FrameFetcher> fetcher = make_shared<WebcamFrameFetcher>();

  // See if we can get frames from it.
  if (!fetcher->HasNextFrame()) {
    QMessageBox msg(this);
    msg.setText("Error: Could not open webcam for reading.");
    msg.exec();
    return;
  }

  // Assign the fetcher to the pipeline.
  processor_.set_fetcher(fetcher);
}

void MainWindow::OpenThresholdDialog() {
  std::cout << "Open threshold dialog.\n";
}

void MainWindow::ToggleShowMarkers() { show_markers_ = !show_markers_; }

void MainWindow::ToggleShowThreshold() { show_threshold_ = !show_threshold_; }
