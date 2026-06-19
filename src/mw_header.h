#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ElaWindow.h"
#include "ElaIcon.h"
#include "ElaAppBar.h"
#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaWidget.h"
#include "ElaListView.h"
#include "ElaLineEdit.h"
#include "ElaMenu.h"
#include "ElaMenuBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaToolBar.h"
#include "plog/Log.h"
#include "plog/Init.h"
#include "plog/Initializers/RollingFileInitializer.h"
#include "nlohmann/json.hpp"
#include <QStringListModel>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <unordered_map>
#include <vector>
#include <string_view>
#include <utility>

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <random>
#include "pl_AddDialog_header.hpp"
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
    QWidget *contentWidget = new QWidget(this);
    Ui::pageContent contentUi;
    QStringListModel *mw_qlmPeopleList = new QStringListModel(contentWidget);

    QTimer *mw_lotteryTimer = new QTimer(this);
    QStringList mw_lotteryWinners;
    int mw_lotteryRound = 0;

    struct mw_lotteryHistoryEntry {
        int round;
        QStringList winners;
        QStringList candidates;
    };
    std::vector<mw_lotteryHistoryEntry> mw_lotteryHistory;
    QStringListModel *mw_historyModel = new QStringListModel(this);

    static auto mw_initLogService()->void;
    auto mw_initWindowUiCore()->void;
    auto mw_initWindowTitle()->void;

    auto mw_initComboBoxes()->void;
    auto mw_initRandomSeed()->void;
    auto mw_pl_importJson()->void;
    auto mw_pl_addMember()->void;
    auto mw_pl_addMemberDirectly(const std::string& name)->void;
    auto mw_pl_deleteMember()->void;
    auto mw_pl_deleteMemberDirectly(int row)->void;
    auto mw_pl_deleteAllMembers()->void;
    auto mw_lotteryTick()->void;
    auto mw_lotteryFinish()->void;
    auto mw_pickWinners(int count)->QStringList;
    auto mw_restoreHistoryRound()->void;
    auto mw_saveHistory()->void;
    auto mw_loadHistory()->void;
};

namespace constValues {
    constexpr uint32_t maxLogFileSize = 1024*1024*5; //5MB
    constexpr QSize mw_minWindowSize = QSize(650, 500);
    constexpr QSize mw_maxWindowSize = QSize(1000, 750);
    constexpr QSize mw_defaultWindowSize = QSize(800, 600);
    inline std::unordered_map<std::string,size_t> mw_mapStringIntEnumRandomSeed;
    inline std::unordered_map<size_t,std::string> mw_mapIntStringEnumRandomSeed;
}

namespace ownUtils {
    auto clearFile(const std::filesystem::path& filepath)->void; //清除指定路径的文件
    auto centerWidgetInVBox(const QWidget *w)->void; // 图标和标题竖直居中（替换 VBox 顶部 6px 定距为 stretch）
}
#endif // MAINWINDOW_H

