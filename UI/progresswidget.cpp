#include "progresswidget.h"
#include <QStyle>
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QVariantAnimation>
#include <QStackedWidget>
#include <QEvent>

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent), isSliderMoving(false) {
    setupUI();
}

void ProgressWidget::setupUI() {
    positionLabel = new QLabel(this);
    positionLabel->setStyleSheet("background: rgba(0,0,0,150); color: white; border-radius: 4px; padding: 2px;");
    positionLabel->setAlignment(Qt::AlignCenter);
    positionLabel->setFixedSize(60, 20);
    positionLabel->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 5, 0, 5);

    // 创建容器（替代堆叠控件）
    QWidget *progressContainer = new QWidget(this);
    progressContainer->setFixedHeight(30);

    // 使用网格布局确保控件重叠
    QGridLayout *containerLayout = new QGridLayout(progressContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // 进度条（底层）
    progressBar = new QProgressBar(progressContainer);
    progressBar->setRange(0, 1000);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(6);

    // 设置霓虹效果样式
    progressBar->setStyleSheet(R"(
        QProgressBar {
            height: 6px;
            border-radius: 3px;
            background: rgba(100, 100, 130, 0.3);
            margin: 0; /* 移除外边距 */
        }
        QProgressBar::chunk {
            background: qlineargradient(
                spread:pad, x1:0, y1:0.5, x2:1, y2:0.5,
                stop:0 #7b68ee,
                stop:0.5 #5d54a4,
                stop:1 #3498db
            );
            border-radius: 3px;
        }
    )");

    // 滑块（顶层，覆盖在进度条上）
    slider = new QSlider(Qt::Horizontal, progressContainer);
    slider->setRange(0, 1000);
    slider->setFixedHeight(30); // 增大高度确保覆盖

    slider->setStyleSheet(R"(
        QSlider {
            background: transparent;
            border-radius: 4px;
        }
        QSlider::groove:horizontal {
            background: transparent;
            height: 6px;  /* 增大轨道高度 */
            border-radius: 4px;
        }
        QSlider::handle {
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: rgba(255, 255, 255, 180);
            width: 10px;
            height: 10px;
            margin: -6px 0; /* 垂直居中 */
            border-radius: 4px;
            border: 2px solid rgba(123, 104, 238, 180);
        }
        QSlider::handle:horizontal:hover {
            background: rgba(255, 255, 255, 220);
            border: 2px solid rgba(123, 104, 238, 220);
            border-radius: 4px;
        }
    )");

    // 关键修改：将两个控件放入同一个单元格，实现重叠
    containerLayout->addWidget(progressBar, 0, 0, Qt::AlignVCenter);
    containerLayout->addWidget(slider, 0, 0, Qt::AlignVCenter);

    mainLayout->addWidget(progressContainer);

    // === 时间容器 ===
    QWidget *timeContainer = new QWidget(this);
    QHBoxLayout *timeLayout = new QHBoxLayout(timeContainer);
    timeLayout->setContentsMargins(5, 0, 5, 0);

    currentTimeLabel = new QLabel("0:00", timeContainer);
    totalTimeLabel = new QLabel("0:00", timeContainer);

    QString timeLabelStyle = R"(
        QLabel {
            color: #a0a0f0;
            font-size: 13px;
            font-weight: 500;
        }
    )";

    currentTimeLabel->setStyleSheet(timeLabelStyle);
    totalTimeLabel->setStyleSheet(timeLabelStyle);

    currentTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    totalTimeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    timeLayout->addWidget(currentTimeLabel);
    timeLayout->addStretch(1);
    timeLayout->addWidget(totalTimeLabel);

    mainLayout->addWidget(timeContainer);

    // 连接信号
    connect(slider, &QSlider::sliderMoved, this, &ProgressWidget::onSliderMoved);
    connect(slider, &QSlider::sliderReleased, this, &ProgressWidget::onSliderReleased);
    slider->installEventFilter(this);
}

QString ProgressWidget::formatTime(float seconds) {
    int totalSeconds = static_cast<int>(seconds);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;

    if (hours > 0) {
        return QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0'));
    } else {
        return QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0'));
    }
}

void ProgressWidget::setCurrentTime(float seconds) {
    currentTimeLabel->setText(formatTime(seconds));
}

void ProgressWidget::setTotalTime(float seconds) {
    totalTimeLabel->setText(formatTime(seconds));
}

void ProgressWidget::setProgress(float progress) {
    if (!isSliderMoving) {
        slider->blockSignals(true);
        progressBar->setValue(static_cast<int>(progress * 1000));
        slider->setValue(static_cast<int>(progress * 1000));
        slider->blockSignals(false);
    }
}

void ProgressWidget::onSliderMoved(int value) {
    isSliderMoving = true;
    float progress = value / 1000.0f;

    // 显示浮动提示
    positionLabel->setText(formatTime(progress *
                                      totalTimeLabel->text().toFloat()));

    QPoint sliderPos = slider->mapToParent(QPoint(slider->width() * progress, 0));
    QPoint tipPos(sliderPos.x() - positionLabel->width() / 2,
                  slider->mapToParent(QPoint(0, -25)).y());
    positionLabel->move(tipPos);
    positionLabel->show();
}

void ProgressWidget::onSliderReleased() {
    positionLabel->hide();
    isSliderMoving = false;

    float progress = slider->value() / 1000.0f;
    emit seekRequested(progress);
}

bool ProgressWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == slider && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            // 计算点击位置对应的进度
            float progress = static_cast<float>(mouseEvent->pos().x()) / slider->width();
            progress = qBound(0.0f, progress, 1.0f);

            // 更新滑块位置
            slider->setValue(static_cast<int>(progress * 1000));

            // 发出跳转请求
            emit seekRequested(progress);
            return true;
        }
    }
    return QWidget::eventFilter(obj , event);
}
