#ifndef __FILTER_DIALOG_H_
#define __FILTER_DIALOG_H_

#include <QDialog>
#include <QTabWidget>
#include <memory>

#include "denoising.h"
#include "framefetcher.h"

namespace Ui {
class NoiseFilterDialog;
}

class NoiseFilterDialog : public QDialog {
  Q_OBJECT

 public:
  explicit NoiseFilterDialog(QWidget *parent = 0);
  ~NoiseFilterDialog();

  FrameFetcher *MakeFilter();

 protected:
  // Gaussian properties.
  int GetKernel();
  double GetGaussianSigma();

  // Median properties.
  int GetSize();

  // Bilateral properties.
  int GetD();
  double GetBilateralSigma();

 private:
  Ui::NoiseFilterDialog *ui;
};

#endif  // __FILTER_DIALOG_H_
