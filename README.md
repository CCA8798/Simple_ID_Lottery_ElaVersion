# Simple ID Lottery

基于 Qt 6 + ElaWidgetTools 的桌面抽奖应用。

## 功能

- 候选人名单管理（增删、JSON 导入）
  - JSON 格式：顶层为字符串数组，`["张三", "李四", "王五"]`
  - 列表非空时导入会询问"覆盖还是追加"
- 轮盘式抽奖动画（先快后慢，结果预确定）
- 每轮抽取人数可配、随机种子可选（QRandomGenerator / std::mt19937_64）
- 每轮后自动删除中奖者
- 历史记录持久化（saves/history.json），支持还原与删除

## 构建

依赖：Qt 6.7+ MinGW、CMake ≥ 3.12、Ninja、C++20

```bash
cmake --preset default
cmake --build cmake-build-debug-mingw_qt1310 --target Simple_ID_Lottery
```

ElaWidgetTools 作为 git submodule 引入，首次克隆后执行：

```bash
git submodule update --init --recursive
```

## 许可

MIT
