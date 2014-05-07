#include "filterdialog.h"
#include "ui_filtering.h"

using std::make_shared;
using std::shared_ptr;

NoiseFilterDialog::NoiseFilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::NoiseFilterDialog) {
  ui->setupUi(this);
}

NoiseFilterDialog::~NoiseFilterDialog() { delete ui; }

shared_ptr<DenoisingFrameFetcher> NoiseFilterDialog::MakeFilter() {
  // Work out which kind of filter the user has chosen.
  QTabWidget *tabs = this->findChild<QTabWidget *>("Filters");
  QWidget *current_tab = tabs->currentWidget();
  QString filter_name = current_tab->objectName();

  shared_ptr<DenoisingFrameFetcher> fetcher;
  if (filter_name == "GaussianTab") {
    fetcher = make_shared<GaussianDenoisingFrameFetcher>(nullptr, GetKernel(),
                                                         GetGaussianSigma());
  } else if (filter_name == "MedianTab") {
    fetcher = make_shared<MedianDenoisingFrameFetcher>(nullptr, GetSize());
  } else if (filter_name == "BilateralTab") {
    fetcher = make_shared<BilateralDenoisingFrameFetcher>(nullptr, GetD(),
                                                          GetBilateralSigma());
  } else if (filter_name == "NonLocalMeansTab") {
    fetcher = make_shared<NonLocalMeansDenoisingFrameFetcher>(nullptr);
  }

  return fetcher;
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
