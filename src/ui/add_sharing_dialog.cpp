#include "ui/add_sharing_dialog.h"

#include <QFileSystemModel>
#include <QCompleter>
#include <QCoreApplication>
#include <QMessageBox>

#include <lmerr.h>

#include "sharing/sharing_service.h"
#include "sharing/shared_resource.h"

namespace ui {

using sharing::SharingService;
using sharing::SharedResource;

AddSharingDialog::AddSharingDialog(QWidget *parent) : QDialog(parent) {
  this->ui_.setupUi(this);
  auto fs_model = new QFileSystemModel(this);
  fs_model->setFilter(QDir::AllDirs | QDir::Drives);
  fs_model->setRootPath("");
  auto cmpltr = new QCompleter(fs_model, this);
  this->ui_.txtPath->setCompleter(cmpltr);
  this->layout()->setSizeConstraint(QLayout::SetFixedSize);

  connect(this->ui_.buttonBox, &QDialogButtonBox::accepted, this, [this]() {
    SharedResource resource;
    resource.path = this->ui_.txtPath->text().toStdWString();
    if (this->ui_.txtName->text().isEmpty())
      resource.name = this->ui_.txtPath->text()
                          .replace(QRegularExpression(":|\\\\"), "_")
                          .toStdWString();
    else
      resource.name = this->ui_.txtName->text().toStdWString();
    auto ret = SharingService::instance()->add(resource);
    if (ret != NERR_Success) {
      QMessageBox::critical(this, "error", QCoreApplication::translate(STR(SharingService), SharingService::getErrorString(ret).c_str()));
    } else {
      this->accept();
    }
  });
}
}
