#include "genboarddialog.h"
#include "ui_genboard.h"

#include <iostream>

// TODO: Move these constants into some kind of configuration option.
static const size_t kMarkerSize = 250;
static const size_t kBoardWidth = 10;
static const size_t kBoardHeight = 7;
static const double kMarkerDistance = kMarkerSize * 0.2;

// TODO: Sync this with the marker generator.
static const int kMaxModelMarkerId = 100;

GenerateBoardDialog::GenerateBoardDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::GenerateBoardDialog) {
  ui->setupUi(this);
  // We don't want the space of board markers and model markers to ever overlap,
  // so make sure we don't generate boards with markers in the model space.
  for (int id = 0; id < kMaxModelMarkerId; ++id) {
    disallowed_ids_.push_back(id);
  }

  // Generate an initial board.
  Generate(0);
}

GenerateBoardDialog::~GenerateBoardDialog() { delete ui; }

void GenerateBoardDialog::Save() {
  // First save the board image.
  QString image_filename = QFileDialog::getSaveFileName(
      this, "Save board image.", "", "Image files (*.png *.jpg)");

  if (image_filename == "") {
    return;
  }

  // Save the file.
  try {
    cv::imwrite(image_filename.toStdString(), board_image_);
  }
  catch (cv::Exception e) {
    qWarning() << e.what();
    ShowMessage(this,
                "Failed to write file! Ensure that the extension is correct"
                " and that you have permission to write to this location.");
    return;
  }

  // Next save the board configuration.
  QString board_filename = QFileDialog::getSaveFileName(
      this, "Save board configuration.", "", "Board files (*.board)");

  if (board_filename == "") {
    return;
  }

  // Save the file.
  board_.saveToFile(board_filename.toStdString());
}

void GenerateBoardDialog::Generate(int type) {
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
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  cv::Mat resized;
  cv::resize(board_image_, resized, cv::Size(0, 0), 0.1, 0.1);

  // Draw the image.
  QPixmap pixmap = MatToQPixmap(resized);
  scene_.clear();
  scene_.addPixmap(pixmap);
  view->setScene(&scene_);
  view->show();
}
