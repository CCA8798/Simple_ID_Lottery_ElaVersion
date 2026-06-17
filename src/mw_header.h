#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"
#include "ElaIcon.h"
#include "ElaAppBar.h"
#include "ElaComboBox.h"
#include "ElaWidget.h"
#include "ElaListView.h"
#include "ElaLineEdit.h"
#include "ElaMenu.h"
#include "ElaMenuBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include <QStringListModel>
#include <concepts>
#include <filesystem>
#include <fstream>
#include "ui_mw_ui.h"
#include "ui_page_content.h"

class mainWindow : public ElaWindow
{
    Q_OBJECT

public:
    explicit mainWindow(QWidget *parent = nullptr);
    ~mainWindow() override;

private:
    Ui::mainWindow *ui;

    void mw_initLogService();
    void mw_initWindowTitle() const;
};

namespace constValues {
    constexpr uint32_t maxLogFileSize = 1024*1024*5; //5MB
    constexpr QSize mw_minWindowSize = QSize(400, 300);
    constexpr QSize mw_maxWindowSize = QSize(1000, 750);
    constexpr QSize mw_defaultWindowSize = QSize(800, 600);
}

namespace ownUtils {
    void clearFile(const std::filesystem::path& filepath);
    void centerWidgetInVBox(const QWidget *w); // 图标和标题竖直居中（替换 VBox 顶部 6px 定距为 stretch）
}
#endif // MAINWINDOW_H

