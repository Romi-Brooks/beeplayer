#pragma once

#include <QWidget>
#include <QProgressBar>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>

class ProgressWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProgressWidget(QWidget *parent = nullptr);

    void setCurrentTime(float seconds);
    void setTotalTime(float seconds);
    void setProgress(float progress);
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void seekRequested(float progress);

private slots:
    void onSliderMoved(int value);
    void onSliderReleased();

private:
    void setupUI();
    QString formatTime(float seconds);

    QProgressBar *progressBar;
    QSlider *slider;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    QLabel *positionLabel; // 浮动提示
    bool isSliderMoving;
};
