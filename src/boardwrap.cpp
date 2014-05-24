#include "boardwrap.h"

using std::string;

Board::Board() : config_valid_(false), board_valid_(false) {}
Board::~Board() {}

bool Board::HasBoard() const { return board_valid_; }

bool Board::HasConfig() const { return config_valid_; }

aruco::BoardConfiguration Board::GetConfig() const {
  if (!HasConfig()) {
    qFatal("Attempted to get invalid config.");
  }
  return config_;
}

aruco::Board Board::GetBoard() const {
  if (!HasBoard()) {
    qFatal("Attempted to get invalid board.");
  }
  return board_;
}

bool Board::LoadConfig(const QString &filename) {
  return LoadConfig(filename.toStdString());
}

bool Board::LoadConfig(const string &filename) {
  try {
    config_.readFromFile(filename);
    config_valid_ = true;
    return true;
  }
  catch (cv::Exception e) {
    qWarning() << "Failed to read board from" << filename.c_str() << e.what();
    return false;
  }
}

void Board::SetConfig(const aruco::BoardConfiguration &config) {
  config_ = config;
  config_valid_ = true;
}

void Board::SetBoard(const aruco::Board &board) {
  board_ = board;
  board_valid_ = true;

  // Boards have an associated config, so set that as well.
  config_ = board.conf;
  config_valid_ = true;
}
