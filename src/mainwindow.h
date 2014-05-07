#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGraphicsView>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <memory>

#include <aruco/aruco.h>

#include "denoising.h"
#include "filterdialog.h"
#include "framefetcher.h"
#include "genmarkerdialog.h"
#include "matconv.h"
#include "pipeline.h"

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
  void ProcessFrame();

  // Slots for gui.
  void OpenCalibrateDialog();
  void OpenFilteringDialog();
  void OpenGenerateBoardDialog();
  void OpenGenerateMarkerDialog();
  void OpenIntrinsicsFile();
  void OpenSceneFile();
  void OpenSourceFile();
  void OpenSourceWebcam();
  void OpenThresholdDialog();
  void ToggleShowMarkers();
  void ToggleShowThreshold();

 private:
  Ui::MainWindow *ui;
  QTimer *frame_timer_;
  Pipeline processor_;
  QGraphicsScene scene_;

  bool capturing_;
  bool show_threshold_;
  bool show_markers_;
};

#endif  // MAINWINDOW_H
