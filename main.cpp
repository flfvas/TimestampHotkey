/**
 * @file main.cpp
 * @brief 时间戳热键程序 - 通过全局热键快速生成并粘贴时间戳
 * @version 1.1
 *
 * 功能概述:
 * 1. 监听全局热键 Ctrl+`
 * 2. 触发后生成精确到毫秒的时间戳(格式: yyyyMMdd-HHmmsszzz)
 * 3. 自动复制到剪贴板
 * 4. 自动模拟键盘操作: Ctrl+V(粘贴) -> Ctrl+A(全选) -> Ctrl+C(复制)
 * 5. 以系统托盘方式运行,无主窗口界面
 * 6. 双击托盘图标显示时间窗口,可复制格式化时间
 */

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "timewindow.h"
#include <qhotkey.h>
#include <windows.h>

/**
 * 发送组合键(按下modifier + key,然后释放)
 */
void sendKeyCombo(BYTE modifier, BYTE key)
{
    keybd_event(modifier, 0, 0, 0);
    Sleep(10);
    keybd_event(key, 0, 0, 0);
    Sleep(10);
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    Sleep(10);
    keybd_event(modifier, 0, KEYEVENTF_KEYUP, 0);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("TimestampHotkey");
    app.setApplicationVersion("1.1");
    app.setQuitOnLastWindowClosed(false);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "错误", "系统不支持托盘图标功能!");
        return 1;
    }

    // ========== 创建时间窗口 ==========
    TimeWindow *timeWindow = new TimeWindow();

    // ========== 创建系统托盘图标 ==========
    QSystemTrayIcon trayIcon;
    QPixmap pixmap(32, 32);
    pixmap.fill(QColor(70, 130, 180));
    trayIcon.setIcon(QIcon(pixmap));
    trayIcon.setToolTip("时间戳热键程序\n按  Ctrl+` 触发\n双击显示时间窗口");

    // ========== 创建托盘右键菜单 ==========
    QMenu trayMenu;
    QAction *showWindowAction = trayMenu.addAction("显示时间窗口");
    QAction *aboutAction = trayMenu.addAction("关于程序");
    QAction *statusAction = trayMenu.addAction("状态: 监听中");
    statusAction->setEnabled(false);
    trayMenu.addSeparator();
    QAction *quitAction = trayMenu.addAction("退出程序");

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();

    trayIcon.showMessage("程序已启动",
                         "按  Ctrl+` 生成时间戳\n双击托盘图标显示时间窗口",
                         QSystemTrayIcon::Information,
                         3000);

    // ========== 注册全局热键 ==========
    QHotkey *hotkey = new QHotkey(QKeySequence("Ctrl+`"), true, &app);

    if (hotkey->isRegistered()) {
        qDebug() << "全局热键  Ctrl+` 注册成功";
    } else {
        qDebug() << "全局热键注册失败!";
        QMessageBox::warning(nullptr, "警告", "热键  Ctrl+` 注册失败!\n可能已被其他程序占用。");
    }

    // ========== 热键触发事件 ==========
    QObject::connect(hotkey, &QHotkey::activated, [&trayIcon]() {
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString("yyyyMMdd-HHmmsszzz");

        qDebug() << "热键触发! 生成时间戳:" << timestamp;

        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(timestamp);
        qDebug() << "时间戳已复制到剪贴板";

        QTimer::singleShot(125, []() {
            sendKeyCombo(VK_CONTROL, 'V');
            qDebug() << "发送 Ctrl+V (125ms)";
        });

        QTimer::singleShot(250, []() {
            sendKeyCombo(VK_CONTROL, 'A');
            qDebug() << "发送 Ctrl+A (250ms)";
        });

        QTimer::singleShot(375, []() {
            sendKeyCombo(VK_CONTROL, 'C');
            qDebug() << "发送 Ctrl+C (375ms)";
        });

        trayIcon.showMessage("时间戳已生成",
                             timestamp,
                             QSystemTrayIcon::Information,
                             1000);
    });

    // ========== 显示时间窗口菜单项 ==========
    QObject::connect(showWindowAction, &QAction::triggered, [timeWindow]() {
        timeWindow->show();
        timeWindow->raise();
        timeWindow->activateWindow();
    });

    // ========== 关于对话框 ==========
    QObject::connect(aboutAction, &QAction::triggered, []() {
        QMessageBox::information(nullptr,
                                 "关于",
                                 "时间戳热键程序 v1.1\n\n"
                                 "功能说明:\n"
                                 "• 按  Ctrl+` 生成时间戳\n"
                                 "• 自动复制到剪贴板\n"
                                 "• 自动发送 Ctrl+V/A/C\n"
                                 "• 双击托盘图标显示时间窗口\n\n"
                                 "时间格式: yyyyMMdd-HHmmsszzz\n"
                                 "示例: 20251119-153045789");
    });

    // ========== 双击托盘图标显示时间窗口 ==========
    QObject::connect(&trayIcon,
                     &QSystemTrayIcon::activated,
                     [timeWindow](QSystemTrayIcon::ActivationReason reason) {
                         if (reason == QSystemTrayIcon::DoubleClick) {
                             timeWindow->show();
                             timeWindow->raise();
                             timeWindow->activateWindow();
                         }
                     });

    // ========== 退出程序 ==========
    QObject::connect(quitAction, &QAction::triggered, [timeWindow, &app]() {
        delete timeWindow;
        app.quit();
    });

    qDebug() << "程序启动完成,开始监听热键...";

    return app.exec();
}
