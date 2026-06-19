# Simple ID Lottery — Agent Guide

## 语言

所有对话、思考和输出必须使用中文。代码、标识符、文件和工具参数保持英文原样。

## Build

```
cmake --build cmake-build-debug --target Simple_ID_Lottery
```

Qt 6.7.0 path is hardcoded in `CMakeLists.txt:5` — requires MinGW or MSVC with `/utf-8` flag. `windeployqt` runs post-build to bundle DLLs. Submodules: `git submodule update --init --recursive`.

## Architecture

- **Window**: `mainWindow` inherits `ElaWindow` (ElaWidgetTools), not `QMainWindow`.
- **Content**: `page_content.ui` (QWidget-based) loaded via `Ui::pageContent::setupUi()` → `addCentralWidget()`.
- **Always call `setCurrentStackIndex(1)` after `addCentralWidget()`** — default is index 0 (navigation page).
- `mw_ui.ui` (QMainWindow) must NOT contain a `<widget>` child for centralwidget — it would replace ElaWindow's internal `ElaCentralStackedWidget`.
- Navigation bar disabled (`setIsNavigationBarEnable(false)`), user info card hidden.
- Custom `ElaMenuBar` placed in `ElaAppBarType::RightArea`.
- Logging: plog → `logs/main.log`, cleared each startup.

## UI Editor (Qt Designer)

- Edit `page_content.ui` for visual design (add widgets, promote to Ela classes).
- Promote an Ela widget in Designer: add `customwidgets` section with `<header location="global">ElaXxx.h</header>`.
- **Data models** must be heap-allocated (`new QStringListModel(...)`) — stack-allocated models become dangling after constructor exits.
- `ElaListView` needs `setModel()` and typically `setIsTransparent(false)`.

## Proxy

- Port `7897` is available as an HTTP/SOCKS5 proxy for accessing GitHub, fetching packages, or any internet resource behind the firewall. Set `http_proxy=http://127.0.0.1:7897` or `HTTPS_PROXY=http://127.0.0.1:7897` as needed.

## Non-obvious

- Window is `WIN32_EXECUTABLE TRUE` — no console by default.
- Resources: `app.ico` (window icon), `Original.png`, `round.png`.
- Namespaces: `constValues{}` (size constants), `ownUtils{}`.
- Generated files (`ui_*.h`, `moc_*.cpp`, `qrc_*.cpp`) are gitignored — AUTOUIC/MOC/RCC handles them.
