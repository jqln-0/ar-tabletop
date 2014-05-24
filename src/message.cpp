#include "message.h"

using std::string;

void ShowMessage(QWidget *parent, const string &text) {
  QMessageBox msg(parent);
  msg.setText(text.c_str());
  msg.exec();
}
