#include "genmarkerdialog.h"
#include "ui_genmarker.h"

#include <iostream>

GenerateMarkerDialog::GenerateMarkerDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::GenerateMarkerDialog) {
  ui->setupUi(this);
  // Generate an initial marker.
  Generate(1);
}

GenerateMarkerDialog::~GenerateMarkerDialog() { delete ui; }

void GenerateMarkerDialog::Save() {
  // Get the filename to save as.
  QFileDialog dialog(this, "Save marker");
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter("Image files (*.png *.jpg)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
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

  // Save the file.
  try {
    cv::imwrite(filenames[0].toStdString(), marker_);
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText(
        "Failed to write file! Ensure that the extension is correct "
        "and that you have permission to write to this location.");
    msg.exec();
  }
}

void GenerateMarkerDialog::Generate(int id) {
  // Work out the size of the marker to generate.
  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
  QSize size = view->size();
  int marker_size = std::min<int>(size.width(), size.height());

  // Generate the marker.
  cv::Mat marker = aruco::FiducidalMarkers::createMarkerImage(id, marker_size);

  // We roll our own conversion to QImage here since the colorspace used is
  // very strange and buggy.
  cv::cvtColor(marker, marker_, cv::COLOR_GRAY2RGB);
  QImage converted((unsigned char *)marker_.data, marker_.cols, marker_.rows,
                   QImage::Format_RGB888);

  // Draw the image into the view.
  QPixmap pixmap = QPixmap::fromImage(converted);
  scene_.addPixmap(pixmap);
  view->setScene(&scene_);
  view->show();
}
