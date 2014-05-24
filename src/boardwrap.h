#ifndef __BOARD_WRAP_H
#define __BOARD_WRAP_H

#include <QString>
#include <QtDebug>

#include <aruco/aruco.h>

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

  void LoadConfig(const string &filename);
  void LoadConfig(const QString &filename);
  void SetConfig(const aruco::BoardConfiguration &config);
  void SetBoard(const aruco::Board &board);
};

#endif  // __BOARD_WRAP_H
