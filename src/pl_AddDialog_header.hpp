#ifndef PL_ADDDIALOG_HEADER_HPP
#define PL_ADDDIALOG_HEADER_HPP

#include <QDialog>
#include "ElaLineEdit.h"
#include "ElaText.h"

namespace PeopleList {
	QT_BEGIN_NAMESPACE
	namespace Ui { class AddDialog; }
	QT_END_NAMESPACE

	class AddDialog : public QDialog {
		Q_OBJECT

	public:
		explicit AddDialog(QWidget *parent = nullptr);
		~AddDialog() override;

	signals:
		void onWindowOkButtonClicked(const std::string& name);

	private:
		Ui::AddDialog *ui;
	};
} // PeopleList

#endif // PL_ADDDIALOG_HEADER_HPP
