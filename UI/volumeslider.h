#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H

#include <QSlider>

class VolumeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit VolumeSlider(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void volumeChanged(float volume);
};

#endif // VOLUMESLIDER_H
