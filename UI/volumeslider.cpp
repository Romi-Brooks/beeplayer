// VolumeSlider.cpp
#include "volumeslider.h"
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>

VolumeSlider::VolumeSlider(QWidget *parent)
    : QSlider(Qt::Horizontal, parent)
{
    setRange(0, 100);
    setValue(80); // 默认音量80%
    setMinimumWidth(100);
    setMaximumHeight(20);
}

void VolumeSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 计算点击位置对应的值
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt,
                                                         QStyle::SC_SliderGroove, this);
        const QPoint clickPos = event->pos();

        // 计算点击位置对应的值
        double position = 0.0;
        if (orientation() == Qt::Horizontal) {
            position = static_cast<double>(clickPos.x() - sliderRect.x()) / sliderRect.width();
        } else {
            position = static_cast<double>(sliderRect.bottom() - clickPos.y()) / sliderRect.height();
        }

        int newValue = minimum() + static_cast<int>(position * (maximum() - minimum()));
        setValue(newValue);
        emit volumeChanged(static_cast<float>(newValue) / 100.0f);

        event->accept();
    } else {
        QSlider::mousePressEvent(event);
    }
}

void VolumeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 计算鼠标位置对应的值
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        const QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt,
                                                         QStyle::SC_SliderGroove, this);
        const QPoint clickPos = event->pos();

        // 计算点击位置对应的值
        double position = 0.0;
        if (orientation() == Qt::Horizontal) {
            position = static_cast<double>(clickPos.x() - sliderRect.x()) / sliderRect.width();
        } else {
            position = static_cast<double>(sliderRect.bottom() - clickPos.y()) / sliderRect.height();
        }

        // 限制在0-1范围内
        position = qBound(0.0, position, 1.0);

        int newValue = minimum() + static_cast<int>(position * (maximum() - minimum()));
        setValue(newValue);
        emit volumeChanged(static_cast<float>(newValue) / 100.0f);

        event->accept();
    } else {
        QSlider::mouseMoveEvent(event);
    }
}
