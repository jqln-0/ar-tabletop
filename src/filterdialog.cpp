#include "filterdialog.h"
#include "ui_filtering.h"

#include <iostream>

NoiseFilterDialog::NoiseFilterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::NoiseFilterDialog) {
  ui->setupUi(this);
}

NoiseFilterDialog::~NoiseFilterDialog() { delete ui; }
