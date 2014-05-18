#include "genboarddialog.h"
#include "ui_genboard.h"

#include <iostream>

static const size_t kMarkerSize = 250;
static const size_t kBoardWidth = 10;
static const size_t kBoardHeight = 7;
static const double kMarkerDistance = kMarkerSize * 0.2;

GenerateBoardDialog::GenerateBoardDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::GenerateBoardDialog) {
  ui->setupUi(this);
  // Generate the vector of disallowed ids.
  for (int id = 0; id < 100; ++id) {
    disallowed_ids_.push_back(id);
  }

  // Generate an initial board.
  Generate(0);
}

GenerateBoardDialog::~GenerateBoardDialog() { delete ui; }

void GenerateBoardDialog::Save() {
  // First save the board image.
  QFileDialog img_dialog(this, "Save board image");
  img_dialog.setDefaultSuffix("png");
  img_dialog.setNameFilter("Image files (*.png *.jpg)");
  img_dialog.setFileMode(QFileDialog::AnyFile);
  img_dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (!img_dialog.exec()) {
    return;
  }

  // Save the file.
  try {
    cv::imwrite(img_dialog.selectedFiles()[0].toStdString(), board_image_);
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText(
        "Failed to write file! Ensure that the extension is correct "
        "and that you have permission to write to this location.");
    msg.exec();
  }

  // Next save the board configuration.
  QFileDialog conf_dialog(this, "Save board configuration");
  conf_dialog.setDefaultSuffix("board");
  conf_dialog.setNameFilter("Board files (*.board)");
  conf_dialog.setFileMode(QFileDialog::AnyFile);
  conf_dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (!conf_dialog.exec()) {
    return;
  }

  // Save the file.
  board_.saveToFile(conf_dialog.selectedFiles()[0].toStdString());
}

void GenerateBoardDialog::Generate(int type) {
  // Generate the board.
  cv::Size gridSize(kBoardWidth, kBoardHeight);
  switch (type) {
    case 0:
      // Frame.
      board_image_ = aruco::FiducidalMarkers::createBoardImage_Frame(
          gridSize, kMarkerSize, kMarkerDistance, board_, true,
          &disallowed_ids_);
      break;
    case 1:
      // Solid board.
      board_image_ = aruco::FiducidalMarkers::createBoardImage(
          gridSize, kMarkerSize, kMarkerDistance, board_, &disallowed_ids_);
      break;
    case 2:
      // Checked.
      board_image_ = aruco::FiducidalMarkers::createBoardImage_ChessBoard(
          gridSize, kMarkerSize, board_, true, &disallowed_ids_);
      break;
    default:
      // Unknown type. Ignore it.
      return;
  }

  // Scale the image to a displayable size.
  QGraphicsView *view = this->findChild<QGraphicsView *>("graphicsView");
  cv::Mat resized;
  cv::resize(board_image_, resized, cv::Size(0, 0), 0.1, 0.1);

  // Draw the image.
  QPixmap pixmap = MatToQPixmap(resized);
  scene_.clear();
  scene_.addPixmap(pixmap);
  view->setScene(&scene_);
  view->show();
}
