#include "ElaDxgi.h"
#include "ElaIconButton.h"
#include "mw_header.h"

mainWindow::mainWindow(QWidget *parent)
    : ElaWindow(parent)
    , ui(new Ui::mainWindow)
{
    using namespace constValues;
    ui->setupUi(this);

    contentUi.setupUi(contentWidget);
    addCentralWidget(contentWidget);
    setCurrentStackIndex(1);

    mw_initLogService();
    mw_initWindowUiCore();
    PLOG_INFO<<"Application Start | Logging Start | Ui Core Setup Finished";

    PLOG_INFO<<"Window UI Initialization Start";
    mw_initWindowTitle();
    mw_initComboBoxes();
    PLOG_INFO<<"Window UI Initialization Finished";

    PLOG_INFO<<"Random Seed Initialization Start";
    mw_initRandomSeed();
    PLOG_INFO<<"Random Seed Initialization Finished";

    PLOG_INFO<<"People List Initialization Start";
    contentUi.mw_lvPeopleList->setModel(mw_qlmPeopleList);
    PLOG_INFO<<"People List Initialization Finished";

    contentUi.mw_lvReverseToSelection->setModel(mw_historyModel);
    contentUi.mw_lvReverseToSelection->setIsTransparent(false);
    mw_loadHistory();

    PLOG_INFO<<"Signal Slot Connections Start";
    connect(contentUi.mw_btnAddPerson,&ElaPushButton::clicked,this,&mainWindow::mw_pl_addMember);
    connect(contentUi.mw_btnDeletePerson,&ElaPushButton::clicked,this,&mainWindow::mw_pl_deleteMember);
    connect(contentUi.mw_btnImportJson,&ElaPushButton::clicked,this,&mainWindow::mw_pl_importJson);
    mw_lotteryTimer->setInterval(10);
    connect(mw_lotteryTimer, &QTimer::timeout, this, &mainWindow::mw_lotteryTick);
    connect(contentUi.mw_btnStartLottery, &ElaPushButton::clicked, this, [this] {
        if (mw_lotteryTimer->isActive()) return;
        int total = mw_qlmPeopleList->rowCount();
        if (total == 0) {
            QMessageBox::warning(this, "提示", "名单为空，请先添加候选者");
            return;
        }
        int drawCount = contentUi.mw_leNumberOfPeople->text().toInt();
        if (drawCount < 1) drawCount = 1;
        if (drawCount > total) drawCount = total;

        mw_lotteryWinners = mw_pickWinners(drawCount);
        contentUi.mw_btnStartLottery->setEnabled(false);
        mw_lotteryTimer->setInterval(10);
        mw_lotteryTimer->start();
        PLOG_INFO << "Lottery started, " << drawCount << "winner(s) predetermined";
    });
    connect(contentUi.mw_btnReverseToSeletion, &ElaPushButton::clicked, this, &mainWindow::mw_restoreHistoryRound);
    connect(contentUi.mw_btnDeleteHistory, &ElaPushButton::clicked, this, [this] {
        QModelIndex idx = contentUi.mw_lvReverseToSelection->currentIndex();
        if (!idx.isValid()) {
            QMessageBox::warning(this, "提示", "请先在历史记录中选择一轮");
            return;
        }
        int historyIndex = mw_lotteryHistory.size() - 1 - idx.row();
        if (historyIndex < 0 || historyIndex >= static_cast<int>(mw_lotteryHistory.size())) return;

        mw_lotteryHistory.erase(mw_lotteryHistory.begin() + historyIndex);

        QStringList historyDisplay;
        for (size_t i = mw_lotteryHistory.size(); i > 0; i--) {
            const auto &entry = mw_lotteryHistory[i - 1];
            historyDisplay.push_back(QString("第%1轮 | %2").arg(entry.round).arg(entry.winners.join(", ")));
        }
        mw_historyModel->setStringList(historyDisplay);
        mw_saveHistory();
        PLOG_INFO << "Deleted history entry at index " << historyIndex;
    });
    PLOG_INFO<<"Signal Slot Connections Finished";
    // auto *listView = contentUi.listView;
    // if (listView)
    // {
    //     listView->setItemHeight(40);
    //     listView->setIsTransparent(false);
    //     auto *listModel = new QStringListModel({"项目 A", "项目 B", "项目 C"}, contentWidget);
    //     listView->setModel(listModel);
    // }
}

mainWindow::~mainWindow()
{
    delete ui;
}

namespace ownUtils {
    inline auto clearFile(const std::filesystem::path& filepath)->void{
        std::ofstream(filepath, std::ios::trunc).close();
    }

    auto centerWidgetInVBox(const QWidget *w)->void {
        auto *parent = w->parentWidget();
        if (!parent) return;
        auto *mainLayout = parent->layout();
        if (!mainLayout) return;
        for (int i = 0; i < mainLayout->count(); i++)
        {
            auto *leftLayout = mainLayout->itemAt(i)->layout();
            if (!leftLayout) continue;
            for (int j = 0; j < leftLayout->count(); j++)
            {
                auto *vbox = leftLayout->itemAt(j)->layout();
                if (!vbox) continue;
                auto *box = qobject_cast<QBoxLayout*>(vbox);
                if (!box) continue;
                for (int k = 0; k < box->count(); k++)
                {
                    if (box->itemAt(k)->widget() != w) continue;
                    if (k > 0)
                    {
                        auto *spacer = box->takeAt(k - 1);
                        delete spacer;
                        box->insertStretch(k - 1);
                    }
                    return;
                }
            }
        }
    }

}

auto mainWindow::mw_initLogService()->void {
    using namespace constValues;
    if(!(std::filesystem::exists("logs")&&std::filesystem::exists("logs/main.log"))) {
        std::filesystem::create_directory("logs");
        ownUtils::clearFile("logs/main.log");
        plog::init(plog::verbose, "logs/main.log",maxLogFileSize,1);
        PLOG_WARNING<<"Logging file did not exist | Log file has been created";
    }else {
        ownUtils::clearFile("logs/main.log");
        plog::init(plog::verbose, "logs/main.log",maxLogFileSize,1);
    }
}

auto mainWindow::mw_initWindowUiCore()->void {
    using namespace constValues;
    setMinimumSize(mw_minWindowSize);
    setMaximumSize(mw_maxWindowSize);
    PLOG_INFO<<"Minimum Window Size: "<<mw_minWindowSize.width()<<"x"<<mw_minWindowSize.height();
    PLOG_INFO<<"Current Window Size: "<<mw_defaultWindowSize.width()<<"x"<<mw_defaultWindowSize.height();
    PLOG_INFO<<"Maximum Window Size: "<<mw_maxWindowSize.width()<<"x"<<mw_maxWindowSize.height();
    setWindowButtonFlags(ElaAppBarType::MinimizeButtonHint|
ElaAppBarType::MaximizeButtonHint|ElaAppBarType::CloseButtonHint);
    setIsNavigationBarEnable(false);
    setUserInfoCardVisible(false);
}

auto mainWindow::mw_initWindowTitle()->void {
    for (auto *child : this->children())
    {
        auto *appBar = qobject_cast<ElaAppBar*>(child);
        if (!appBar) continue;
        for (auto *appChild : appBar->children())
        {
            auto *label = qobject_cast<QLabel*>(appChild);
            if (!label) continue;
            if (qobject_cast<ElaText*>(label) || !label->pixmap().isNull())
                ownUtils::centerWidgetInVBox(label);
        }
        break;
    }
}

auto mainWindow::mw_initComboBoxes()->void {
    if (auto *comboView = contentUi.mw_cbRandomSeed->view())
        comboView->setStyleSheet("background-color: palette(window);");
}

auto mainWindow::mw_initRandomSeed()->void{
    using namespace constValues;
    std::vector<std::string> mw_stringEnumRandomSeeds={"QRandomGenerator","std::mt19937_64"};
    for (int index = 0; index < static_cast<int>(mw_stringEnumRandomSeeds.size()); ++index) {
        const auto& seed = mw_stringEnumRandomSeeds.at(index);
        mw_mapIntStringEnumRandomSeed.emplace(index, seed);
        PLOG_DEBUG<<"mw_cbRandomSeed Insert Index "<<index<<" With "<<seed;
    }
    for (auto& i:mw_mapIntStringEnumRandomSeed) {
        contentUi.mw_cbRandomSeed->addItem(QString::fromStdString(i.second));
    }
}

auto mainWindow::mw_pl_importJson() -> void {
    PLOG_INFO<<"Json Importing Start";
    const QString filePath = QFileDialog::getOpenFileName(
        this,                       // 父窗口（用于模态）
        tr("选择将要导入的Json文件"),              // 对话框标题
        QCoreApplication::applicationDirPath(),           // 默认打开的目录（可自定义）
        tr("Json文件 (*.json)") // 过滤器
    );
    if (!filePath.isEmpty()) {
        PLOG_INFO<<"Import Json:"<<filePath;
        try {
            nlohmann::json json;
            std::ifstream file(filePath.toStdString());
            file>>json; //报错点
            if (!mw_qlmPeopleList->stringList().isEmpty()) {
                QMessageBox overrideOrAddMsgBox;
                overrideOrAddMsgBox.setIcon(QMessageBox::Question);
                overrideOrAddMsgBox.setWindowTitle("Json导入");
                overrideOrAddMsgBox.setText("覆盖列表?");
                overrideOrAddMsgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
                if (const int result=overrideOrAddMsgBox.exec();
                    result==QMessageBox::Ok) {
                    mw_pl_deleteAllMembers();
                }
            }
            for (size_t i=0;i<json.size();i++) {
                mw_pl_addMemberDirectly(json.at(i));
            }
        }catch (const nlohmann::json::parse_error&) {
            QMessageBox::critical(this,"Json错误","Json文件内有格式错误！");
            PLOG_ERROR<<"Json: nlohmann::json::parse_error with file "<<filePath;
        }
    }
    PLOG_INFO<<"Json Importing Finished";
}

auto mainWindow::mw_pl_addMember()->void {
    using namespace constValues;
    auto *dlg = new PeopleList::AddDialog(this);
    connect(dlg, &PeopleList::AddDialog::onWindowOkButtonClicked, this, [this](const std::string &name) {
        PLOG_INFO << "Successfully added member: " << name;
        QStringList list = mw_qlmPeopleList->stringList();
        list.append(QString::fromStdString(name));
        mw_qlmPeopleList->setStringList(list);
    });
    dlg->exec();
    delete dlg;
}

auto mainWindow::mw_pl_addMemberDirectly(const std::string& name)->void {
    using namespace constValues;
    PLOG_INFO << "Successfully added member: " << name;
    QStringList list = mw_qlmPeopleList->stringList();
    list.append(QString::fromStdString(name));
    mw_qlmPeopleList->setStringList(list);
}

auto mainWindow::mw_pl_deleteMember()->void {
    QModelIndex index=contentUi.mw_lvPeopleList->selectionModel()->currentIndex();
    if (index.isValid()) {
        mw_pl_deleteMemberDirectly(index.row());
    }
}

auto mainWindow::mw_pl_deleteMemberDirectly(int row)->void {
    using namespace constValues;
    mw_qlmPeopleList->removeRow(row);
    PLOG_INFO << "Successfully deleted member: " << row;
}

auto mainWindow::mw_pl_deleteAllMembers()->void {
    QStringList stringList=mw_qlmPeopleList->stringList();
    stringList.clear();
    mw_qlmPeopleList->setStringList(stringList);
}

auto mainWindow::mw_lotteryTick()->void {
    int count = mw_qlmPeopleList->rowCount();
    if (count == 0) {
        mw_lotteryTimer->stop();
        return;
    }

    int interval = mw_lotteryTimer->interval();
    if (interval >= 800) {
        mw_lotteryTimer->stop();
        contentUi.mw_btnStartLottery->setEnabled(true);
        QString lastName = mw_lotteryWinners.last();
        contentUi.mw_lt_lbLotteryMainWidget->setText(lastName);
        PLOG_INFO << "Lottery result: " << lastName;
        mw_lotteryFinish();
        return;
    }

    QStringList list = mw_qlmPeopleList->stringList();
    QString name = list.at(QRandomGenerator::global()->bounded(count));
    contentUi.mw_lt_lbLotteryMainWidget->setText(name);
    mw_lotteryTimer->setInterval(static_cast<int>(interval * 1.15));
}

auto mainWindow::mw_lotteryFinish()->void {
    mw_lotteryRound++;

    QStringList beforeRound = mw_qlmPeopleList->stringList();

    if (contentUi.mw_ckbDeleteAfterRound->isChecked()) {
        QStringList remaining = beforeRound;
        for (const auto &w : mw_lotteryWinners) {
            remaining.removeAll(w);
        }
        mw_qlmPeopleList->setStringList(remaining);
    }

    mw_lotteryHistory.push_back({mw_lotteryRound, mw_lotteryWinners, beforeRound});

    QStringList historyDisplay;
    for (size_t i = mw_lotteryHistory.size(); i > 0; i--) {
        const auto &entry = mw_lotteryHistory[i - 1];
        historyDisplay.push_back(QString("第%1轮 | %2").arg(entry.round).arg(entry.winners.join(", ")));
    }
    mw_historyModel->setStringList(historyDisplay);

    mw_saveHistory();

    QString msg = QString("幸运儿：%1").arg(mw_lotteryWinners.join(", "));
    QMessageBox::information(this, "抽奖结果", msg);
}

auto mainWindow::mw_pickWinners(int count)->QStringList {
    QStringList pool = mw_qlmPeopleList->stringList();
    QStringList result;
    bool useMt = contentUi.mw_cbRandomSeed->currentText().toStdString() ==
                 constValues::mw_mapIntStringEnumRandomSeed.at(1);

    if (useMt) {
        std::random_device rd;
        std::mt19937_64 mt(rd());
        for (int i = 0; i < count && !pool.isEmpty(); i++) {
            std::uniform_int_distribution<int> dist(0, static_cast<int>(pool.size()) - 1);
            int idx = dist(mt);
            result.push_back(pool.at(idx));
            pool.removeAt(idx);
        }
    } else {
        for (int i = 0; i < count && !pool.isEmpty(); i++) {
            int idx = QRandomGenerator::global()->bounded(pool.size());
            result.push_back(pool.at(idx));
            pool.removeAt(idx);
        }
    }
    return result;
}

auto mainWindow::mw_restoreHistoryRound()->void {
    QModelIndex idx = contentUi.mw_lvReverseToSelection->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "提示", "请先在历史记录中选择一轮");
        return;
    }
    int historyIndex = mw_lotteryHistory.size() - 1 - idx.row();
    if (historyIndex < 0 || historyIndex >= static_cast<int>(mw_lotteryHistory.size())) return;

    PLOG_DEBUG << "Restore request: row=" << idx.row() << " -> historyIndex=" << historyIndex << " history.size=" << mw_lotteryHistory.size();
    for (size_t i = 0; i < mw_lotteryHistory.size(); i++) {
        PLOG_DEBUG << "  history[" << i << "]: round=" << mw_lotteryHistory[i].round;
    }

    mw_qlmPeopleList->setStringList(mw_lotteryHistory.at(historyIndex).candidates);
    PLOG_INFO << "Restored to round " << mw_lotteryHistory.at(historyIndex).round;
}

auto mainWindow::mw_saveHistory()->void {
    std::filesystem::create_directory("saves");
    nlohmann::json j;
    j["round"] = mw_lotteryRound;
    j["entries"] = nlohmann::json::array();
    for (const auto &entry : mw_lotteryHistory) {
        nlohmann::json e;
        e["round"] = entry.round;
        e["winners"] = nlohmann::json::array();
        e["candidates"] = nlohmann::json::array();
        for (const auto &w : entry.winners) e["winners"].push_back(w.toStdString());
        for (const auto &c : entry.candidates) e["candidates"].push_back(c.toStdString());
        j["entries"].push_back(e);
    }
    std::ofstream file("saves/history.json");
    file << j.dump(2);
    PLOG_INFO << "History saved to saves/history.json";
}

auto mainWindow::mw_loadHistory()->void {
    std::filesystem::create_directory("saves");
    std::ifstream file("saves/history.json");
    if (!file.is_open()) {
        PLOG_INFO << "No saved history found";
        return;
    }
    try {
        nlohmann::json j;
        file >> j;
        if (!j.is_object()) return;
        mw_lotteryRound = j.value("round", 0);
        mw_lotteryHistory.clear();
        if (j.contains("entries") && j["entries"].is_array()) {
            for (const auto &e : j["entries"]) {
                mw_lotteryHistoryEntry entry;
                entry.round = e.value("round", 0);
                if (e.contains("winners")) {
                    for (const auto &w : e["winners"]) entry.winners.push_back(QString::fromStdString(w.get<std::string>()));
                }
                if (e.contains("candidates")) {
                    for (const auto &c : e["candidates"]) entry.candidates.push_back(QString::fromStdString(c.get<std::string>()));
                }
                mw_lotteryHistory.push_back(entry);
            }
        }
        QStringList historyDisplay;
        for (size_t i = mw_lotteryHistory.size(); i > 0; i--) {
            const auto &entry = mw_lotteryHistory[i - 1];
            historyDisplay.push_back(QString("第%1轮 | %2").arg(entry.round).arg(entry.winners.join(", ")));
        }
        mw_historyModel->setStringList(historyDisplay);
        PLOG_INFO << "History loaded from saves/history.json, " << mw_lotteryHistory.size() << " entries";
    } catch (const nlohmann::json::parse_error &) {
        PLOG_WARNING << "Failed to parse saves/history.json";
    } catch (const nlohmann::json::type_error &) {
        PLOG_WARNING << "Type error parsing saves/history.json";
    }
}