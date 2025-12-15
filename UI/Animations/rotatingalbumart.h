#pragma once

#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

class RotatingAlbumArt : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(float rotationAngle READ rotationAngle WRITE setRotationAngle)

public:
    explicit RotatingAlbumArt(QWidget *parent = nullptr);
    ~RotatingAlbumArt();

    void startRotation();
    void stopRotation();
    void setPixmap(const QPixmap &pixmap);
    float rotationAngle() const;
    void setRotationAngle(float angle);
    void pauseRotation();
    void resumeRotation();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateRoundedPixmap();

    QPixmap roundedPixmap(const QPixmap &source);

    QPixmap m_originalPixmap;
    QPixmap m_roundedPixmap;
    QPropertyAnimation *m_rotationAnimation;
    float m_rotationAngle;
    QGraphicsDropShadowEffect *m_shadowEffect;
};
