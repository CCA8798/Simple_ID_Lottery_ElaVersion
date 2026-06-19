#include "pl_AddDialog_header.hpp"
#include "ui_pl_AddDialog_ui.h"
#include <QDialogButtonBox>

namespace PeopleList {
	AddDialog::AddDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddDialog) {
		ui->setupUi(this);
		setWindowFlag(Qt::WindowContextHelpButtonHint, false);
		setFixedSize(size());
		connect(ui->pl_bbName, &QDialogButtonBox::accepted, this, [this] {
			emit onWindowOkButtonClicked(ui->pl_pteName->text().trimmed().toStdString());
			accept();
		});
		connect(ui->pl_bbName, &QDialogButtonBox::rejected, this, &QDialog::reject);
	}

	AddDialog::~AddDialog() {
		delete ui;
	}
} // PeopleList
