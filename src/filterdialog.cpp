#include "filterdialog.h"
#include "ui_filtering.h"

#include <iostream>

NoiseFilterDialog::NoiseFilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::NoiseFilterDialog) {
  ui->setupUi(this);
}

NoiseFilterDialog::~NoiseFilterDialog() { delete ui; }

DenoisingFrameFetcher *NoiseFilterDialog::MakeFilter() {
  // Work out which kind of filter the user has chosen.
  QTabWidget *tabs = this->findChild<QTabWidget *>("Filters");
  QWidget *current_tab = tabs->currentWidget();
  QString filter_name = current_tab->objectName();

  std::cout << filter_name.toStdString() << "\n";
  if (filter_name == "NoneTab") {
    return nullptr;
  } else if (filter_name == "GaussianTab") {
    return new GaussianDenoisingFrameFetcher(nullptr, GetKernel(),
                                             GetGaussianSigma());
  } else if (filter_name == "MedianTab") {
    return new MedianDenoisingFrameFetcher(nullptr, GetSize());
  } else if (filter_name == "BilateralTab") {
    return new BilateralDenoisingFrameFetcher(nullptr, GetD(),
                                              GetBilateralSigma());
  } else if (filter_name == "NonLocalMeansTab") {
    return new NonLocalMeansDenoisingFrameFetcher(nullptr);
  } else {
    std::cerr << "Warning: Unknown filter selected!\n";
    return nullptr;
  }

  return nullptr;
}

int NoiseFilterDialog::GetKernel() {
  QSpinBox *box = this->findChild<QSpinBox *>("kernel");
  return box->value();
}

double NoiseFilterDialog::GetGaussianSigma() {
  QDoubleSpinBox *box = this->findChild<QDoubleSpinBox *>("gaussianSigma");
  return box->value();
}

int NoiseFilterDialog::GetSize() {
  QSpinBox *box = this->findChild<QSpinBox *>("size");
  return box->value();
}

int NoiseFilterDialog::GetD() {
  QSpinBox *box = this->findChild<QSpinBox *>("d");
  return box->value();
}

double NoiseFilterDialog::GetBilateralSigma() {
  QDoubleSpinBox *box = this->findChild<QDoubleSpinBox *>("bilateralSigma");
  return box->value();
}
