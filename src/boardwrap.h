#ifndef __BOARD_WRAP_H
#define __BOARD_WRAP_H

#include <QString>
#include <QtDebug>

#include <aruco/aruco.h>

#include <string>

// ArUco doesn't provide any capability to check the validity of a board or
// board configuration. This class therefore wraps these classes to provide
// such functionality.
class Board {
 protected:
  aruco::BoardConfiguration config_;
  aruco::Board board_;

  bool config_valid_;
  bool board_valid_;

 public:
  Board();
  ~Board();

  bool HasBoard() const;
  bool HasConfig() const;

  aruco::BoardConfiguration GetConfig() const;
  aruco::Board GetBoard() const;

  // Attempts to read the board configuration from the given filename. Returns
  // true on success.
  bool LoadConfig(const std::string &filename);
  bool LoadConfig(const QString &filename);

  void SetConfig(const aruco::BoardConfiguration &config);
  void SetBoard(const aruco::Board &board);
};

#endif  // __BOARD_WRAP_H
