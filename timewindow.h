#ifndef TIMEWINDOW_H
#define TIMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QClipboard>
#include <QDateTime>
#include <QTimer>
#include <QApplication>
#include <QDebug>

/**
 * 时间显示窗口类
 */
class TimeWindow : public QWidget
{
    Q_OBJECT

public:
    TimeWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("格式化时间");
        setFixedSize(400, 150);

        // 创建布局
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // 时间显示标签
        timeLabel = new QLabel("当前时间:", this);
        timeLabel->setStyleSheet("font-size: 12pt;");
        mainLayout->addWidget(timeLabel);

        // 时间戳输入框(只读)
        timestampEdit = new QLineEdit(this);
        timestampEdit->setReadOnly(true);
        timestampEdit->setStyleSheet("font-size: 11pt; padding: 5px;");
        mainLayout->addWidget(timestampEdit);

        // 按钮布局
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        // 复制按钮
        QPushButton *copyButton = new QPushButton("复制时间戳", this);
        copyButton->setStyleSheet("font-size: 10pt; padding: 8px;");
        connect(copyButton, &QPushButton::clicked, this, &TimeWindow::copyTimestamp);
        buttonLayout->addWidget(copyButton);

        // 刷新按钮
        QPushButton *refreshButton = new QPushButton("刷新", this);
        refreshButton->setStyleSheet("font-size: 10pt; padding: 8px;");
        connect(refreshButton, &QPushButton::clicked, this, &TimeWindow::updateTime);
        buttonLayout->addWidget(refreshButton);

        mainLayout->addLayout(buttonLayout);

        // 初始化时间
        updateTime();
    }

public slots:
    // 更新时间显示
    void updateTime()
    {
        QDateTime now = QDateTime::currentDateTime();

        // 友好的时间显示
        QString friendlyTime = now.toString("yyyy年MM月dd日 HH:mm:ss.zzz");
        timeLabel->setText("当前时间: " + friendlyTime);

        // 时间戳格式
        currentTimestamp = now.toString("yyyyMMdd-HHmmsszzz");
        timestampEdit->setText(currentTimestamp);
    }

    // 复制时间戳到剪贴板
    void copyTimestamp()
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(currentTimestamp);

        // 临时改变按钮文本提示已复制
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (btn) {
            QString originalText = btn->text();
            btn->setText("已复制!");
            QTimer::singleShot(1000, [btn, originalText]() {
                btn->setText(originalText);
            });
        }

        qDebug() << "已复制时间戳:" << currentTimestamp;
    }

    // 显示窗口时刷新时间
    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        updateTime();
    }

private:
    QLabel *timeLabel;
    QLineEdit *timestampEdit;
    QString currentTimestamp;
};

#endif // TIMEWINDOW_H
