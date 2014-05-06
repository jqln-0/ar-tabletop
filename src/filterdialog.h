#ifndef __FILTER_DIALOG_H_
#define __FILTER_DIALOG_H_

#include <QDialog>

namespace Ui {
class NoiseFilterDialog;
}

class NoiseFilterDialog : public QDialog {
  Q_OBJECT

 public:
  explicit NoiseFilterDialog(QWidget *parent = 0);
  ~NoiseFilterDialog();

 private:
  Ui::NoiseFilterDialog *ui;
};

#endif  // __FILTER_DIALOG_H_
