/**
 * @file main.cpp
 * @brief 时间戳热键程序 - 通过全局热键快速生成并粘贴时间戳
 * @version 1.0
 *
 * 功能概述:
 * 1. 监听全局热键 Ctrl+`
 * 2. 触发后生成精确到毫秒的时间戳(格式: yyyyMMdd-HHmmsszzz)
 * 3. 自动复制到剪贴板
 * 4. 自动模拟键盘操作: Ctrl+V(粘贴) -> Ctrl+A(全选) -> Ctrl+C(复制)
 * 5. 以系统托盘方式运行,无主窗口界面
 */

#include <QApplication>       // Qt应用程序主类
#include <QSystemTrayIcon>    // 系统托盘图标
#include <QMenu>              // 托盘右键菜单
#include <QAction>            // 菜单动作项
#include <QClipboard>         // 剪贴板操作
#include <QDateTime>          // 日期时间处理
#include <QTimer>             // 定时器,用于延迟执行
#include <QMessageBox>        // 消息对话框
#include <QDebug>             // 调试输出
#include <QPixmap>            // 图像处理
#include <QColor>             // 颜色定义
#include <QIcon>              // 图标
#include <qhotkey.h>          // 第三方全局热键库
#include <windows.h>          // Windows API,用于模拟键盘输入

/**
 * 发送组合键(按下modifier + key,然后释放)
 * modifier 修饰键(如 VK_CONTROL, VK_SHIFT, VK_MENU等)
 * param key 主键(如 'V', 'A', 'C'等)
 *
 * 工作原理:
 * 1. 按下修饰键(如Ctrl)
 * 2. 短暂延迟10ms,确保按键被系统识别
 * 3. 按下主键(如V)
 * 4. 释放主键
 * 5. 释放修饰键
 *
 *
 *
 *
 *
 *
 * 每个操作之间都有10ms延迟,确保键盘事件被正确处理
 */
void sendKeyCombo(BYTE modifier, BYTE key) {
    // 按下修饰键(不释放)
    keybd_event(modifier, 0, 0, 0);
    Sleep(10);  // 等待10ms,让系统识别按键

    // 按下主键
    keybd_event(key, 0, 0, 0);
    Sleep(10);

    // 释放主键(KEYEVENTF_KEYUP标志表示释放)
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    Sleep(10);

    // 释放修饰键
    keybd_event(modifier, 0, KEYEVENTF_KEYUP, 0);
}






int main(int argc, char *argv[]) {
    // 创建Qt应用程序实例
    QApplication app(argc, argv);

    // ========== 应用程序基本信息设置 ==========
    app.setApplicationName("TimestampHotkey");     // 应用名称
    app.setApplicationVersion("1.0");              // 版本号
    app.setQuitOnLastWindowClosed(false);          // 关闭所有窗口时不退出程序(托盘程序需要)

    // ========== 检查系统托盘支持 ==========
    // 部分系统可能不支持托盘图标功能
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "错误", "系统不支持托盘图标功能!");
        return 1;  // 返回错误码
    }

    // ========== 创建系统托盘图标 ==========
    QSystemTrayIcon trayIcon;

    // 创建一个32x32像素的纯色图标(钢蓝色)
    QPixmap pixmap(32, 32);
    pixmap.fill(QColor(70, 130, 180));  // RGB: 钢蓝色
    trayIcon.setIcon(QIcon(pixmap));

    // 设置托盘图标的悬停提示文本
    trayIcon.setToolTip("时间戳热键程序\n按  Ctrl+` 触发");

    // ========== 创建托盘右键菜单 ==========
    QMenu trayMenu;

    // 添加"关于程序"菜单项
    QAction *aboutAction = trayMenu.addAction("关于程序");

    // 添加状态显示项(禁用状态,仅显示信息)
    QAction *statusAction = trayMenu.addAction("状态: 监听中");
    statusAction->setEnabled(false);  // 禁用点击

    // 添加分隔线
    trayMenu.addSeparator();

    // 添加"退出程序"菜单项
    QAction *quitAction = trayMenu.addAction("退出程序");

    // 将菜单关联到托盘图标
    trayIcon.setContextMenu(&trayMenu);

    // 显示托盘图标
    trayIcon.show();

    // ========== 显示程序启动通知 ==========
    trayIcon.showMessage("程序已启动",
                         "按  Ctrl+` 生成时间戳\n右键托盘图标查看菜单",
                         QSystemTrayIcon::Information,  // 信息类型图标
                         3000);  // 显示3秒









    // ========== 注册全局热键 Ctrl+` ==========
    // QHotkey参数说明:
    // - QKeySequence("Ctrl+`"): 热键组合
    // - true: 注册为全局热键(在任何程序中都有效)
    // - &app: 父对象,用于内存管理
    QHotkey *hotkey = new QHotkey(QKeySequence("Ctrl+`"), true, &app);

    // 检查热键注册是否成功
    if (hotkey->isRegistered()) {
        qDebug() << "全局热键  Ctrl+` 注册成功";
    } else {
        qDebug() << "全局热键注册失败!";
        // 热键可能被其他程序占用
        QMessageBox::warning(nullptr, "警告",
                             "热键  Ctrl+` 注册失败!\n可能已被其他程序占用。");
    }

    // ========== 连接热键触发事件 ==========
    // 当用户按下 Ctrl+` 时,执行以下lambda函数
    QObject::connect(hotkey, &QHotkey::activated, [&trayIcon]() {

        // ===== 步骤1: 生成时间戳 =====
        // 获取当前系统时间
        QDateTime now = QDateTime::currentDateTime();

        // 格式化时间戳
        // yyyy: 4位年份 (2025)
        // MM: 2位月份 (01-12)
        // dd: 2位日期 (01-31)
        // HH: 24小时制小时 (00-23)
        // mm: 分钟 (00-59)
        // ss: 秒 (00-59)
        // zzz: 毫秒 (000-999)
        // 示例: 20251119-153045789
        QString timestamp = now.toString("yyyyMMdd-HHmmsszzz");

        qDebug() << "热键触发! 生成时间戳:" << timestamp;

        // ===== 步骤2: 复制到剪贴板 =====
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(timestamp);
        qDebug() << "时间戳已复制到剪贴板";








        // ===== 步骤3: 延迟125ms后发送 Ctrl+V (粘贴) =====
        // 使用QTimer::singleShot创建单次定时器
        // 延迟是为了确保剪贴板内容已更新
        QTimer::singleShot(125, []() {
            sendKeyCombo(VK_CONTROL, 'V');  // 模拟按下Ctrl+V
            qDebug() << "发送 Ctrl+V (125ms)";
        });

        // ===== 步骤4: 延迟250ms后发送 Ctrl+A (全选) =====
        // 全选刚粘贴的时间戳内容
        QTimer::singleShot(250, []() {
            sendKeyCombo(VK_CONTROL, 'A');
            qDebug() << "发送 Ctrl+A (250ms)";
        });

        // ===== 步骤5: 延迟375ms后发送 Ctrl+C (复制) =====
        // 将全选的内容重新复制(可能用于某些特殊场景)
        QTimer::singleShot(375, []() {
            sendKeyCombo(VK_CONTROL, 'C');
            qDebug() << "发送 Ctrl+C (375ms)";
        });








        // ===== 显示托盘通知 =====
        trayIcon.showMessage("时间戳已生成",
                             timestamp,
                             QSystemTrayIcon::Information,
                             1000);  // 显示1秒
    });

    // ========== 关于对话框 ==========
    // 点击"关于程序"菜单项时显示信息
    QObject::connect(aboutAction, &QAction::triggered, []() {
        QMessageBox::information(nullptr,
                                 "关于",
                                 "时间戳热键程序 v1.0\n\n"
                                 "功能说明:\n"
                                 "• 按  Ctrl+` 生成时间戳\n"
                                 "• 自动复制到剪贴板\n"
                                 "• 自动发送 Ctrl+V/A/C\n\n"
                                 "时间格式: yyyyMMdd-HHmmsszzz\n"
                                 "示例: 20251119-153045789");
    });

    // ========== 双击托盘图标显示关于 ==========
    // 监听托盘图标的激活事件
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated,
                     [aboutAction](QSystemTrayIcon::ActivationReason reason) {
                         // 如果是双击事件
                         if (reason == QSystemTrayIcon::DoubleClick) {
                             // 触发"关于"动作
                             emit aboutAction->triggered();
                         }
                     });

    // ========== 退出程序 ==========
    // 点击"退出程序"菜单项时退出应用
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    qDebug() << "程序启动完成,开始监听热键...";

    // ========== 进入Qt事件循环 ==========
    // 程序在此阻塞,等待用户操作和事件触发
    // 直到调用 QApplication::quit() 才会退出
    return app.exec();
}








/*
 * ========== 原始代码(已注释) ==========
 * 这是Qt Creator自动生成的带主窗口的模板代码
 * 本程序不需要主窗口,因此被注释掉

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
*/
