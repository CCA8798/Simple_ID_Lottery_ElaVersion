#include "ElaToolBar.h"
#include "plog/Log.h"
#include "plog/Init.h"
#include "plog/Initializers/RollingFileInitializer.h"

#include "mw_header.h"

//ui_page_content.H已被迁移到mw_header.h

mainWindow::mainWindow(QWidget *parent)
    : ElaWindow(parent)
    , ui(new Ui::mainWindow)
{
    using namespace constValues;
    ui->setupUi(this);
    auto *contentWidget = new QWidget(this);
    Ui::pageContent contentUi;
    contentUi.setupUi(contentWidget);
    addCentralWidget(contentWidget);
    setCurrentStackIndex(1);

    mw_initLogService();
    PLOG_INFO<<"Application Start | Logging Start | Ui Core Setup Finished";

    mw_initWindowTitle();
    setMinimumSize(mw_minWindowSize);
    setMaximumSize(mw_maxWindowSize);
    PLOG_INFO<<"Minimum Window Size: "<<mw_minWindowSize.width()<<"x"<<mw_minWindowSize.height();
    PLOG_INFO<<"Maximum Window Size: "<<mw_maxWindowSize.width()<<"x"<<mw_maxWindowSize.height();
    setWindowButtonFlags(ElaAppBarType::NoneButtonHint|ElaAppBarType::MinimizeButtonHint|
ElaAppBarType::MaximizeButtonHint|ElaAppBarType::CloseButtonHint);
    setIsNavigationBarEnable(false);
    setUserInfoCardVisible(false);

    PLOG_INFO<<"Window UI Initialization Finished";

    auto *listView = contentUi.listView;
    if (listView)
    {
        listView->setItemHeight(40);
        listView->setIsTransparent(false);
        auto *listModel = new QStringListModel({"项目 A", "项目 B", "项目 C"}, contentWidget);
        listView->setModel(listModel);
    }
}

mainWindow::~mainWindow()
{
    delete ui;
}

namespace ownUtils {
    inline void clearFile(const std::filesystem::path& filepath){
        std::ofstream(filepath, std::ios::trunc).close();
    }

    void centerWidgetInVBox(const QWidget *w) {
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

void mainWindow::mw_initLogService() {
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

void mainWindow::mw_initWindowTitle() const {
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
