#include "rotatingalbumart.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>

RotatingAlbumArt::RotatingAlbumArt(QWidget *parent)
    : QLabel(parent), m_rotationAngle(0)
{
    setMinimumSize(250, 250);
    setMaximumSize(300, 300);
    setAlignment(Qt::AlignCenter);

    // 创建旋转动画
    m_rotationAnimation = new QPropertyAnimation(this, "rotationAngle", this);
    m_rotationAnimation->setDuration(15000); // 15秒旋转一圈
    m_rotationAnimation->setStartValue(0);
    m_rotationAnimation->setEndValue(360);
    m_rotationAnimation->setLoopCount(-1); // 无限循环

    // 添加阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(20);
    m_shadowEffect->setColor(QColor(0, 0, 0, 100));
    m_shadowEffect->setOffset(0, 4);
    setGraphicsEffect(m_shadowEffect);

    // 设置默认样式
    setStyleSheet("background-color: rgba(40,40,60,0.4);"
                  "color: rgba(255,255,255,0.5);"
                  "font-style: italic;"
                  "font-size: 14px;");
}

RotatingAlbumArt::~RotatingAlbumArt()
{
    delete m_rotationAnimation;
}

void RotatingAlbumArt::startRotation()
{
    if (!m_originalPixmap.isNull()) {
        // 添加1秒延迟
        QTimer::singleShot(1000, this, [this]() {
            m_rotationAnimation->start();
        });
    }
}

void RotatingAlbumArt::stopRotation()
{
    m_rotationAnimation->stop();
    setRotationAngle(0);
}

void RotatingAlbumArt::pauseRotation()
{
    m_rotationAnimation->pause();
}

void RotatingAlbumArt::resumeRotation()
{
    if (m_rotationAnimation->state() == QAbstractAnimation::Paused) {
        m_rotationAnimation->resume();
    } else {
        // 如果动画完全停止，则重新开始（带延迟）
        QTimer::singleShot(1000, this, [this]() {
            m_rotationAnimation->start();
        });
    }
}

void RotatingAlbumArt::setPixmap(const QPixmap &pixmap)
{
    m_originalPixmap = pixmap;
    updateRoundedPixmap();
}

float RotatingAlbumArt::rotationAngle() const
{
    return m_rotationAngle;
}

void RotatingAlbumArt::setRotationAngle(float angle)
{
    if (qFuzzyCompare(m_rotationAngle, angle))
        return;

    m_rotationAngle = angle;
    update();
}

void RotatingAlbumArt::paintEvent(QPaintEvent *event)
{
    if (m_originalPixmap.isNull()) {
        QLabel::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 使用控件实际尺寸
    const int size = width(); // 因为是正方形，width = height
    const QRectF rect(0, 0, size, size);

    // 保存原始状态
    painter.save();

    // 移动到中心点并旋转
    painter.translate(size/2.0, size/2.0);
    painter.rotate(m_rotationAngle);
    painter.translate(-size/2.0, -size/2.0);

    // 绘制圆形专辑封面 - 修复 drawPixmap 调用
    QPainterPath path;
    path.addEllipse(rect);
    painter.setClipPath(path);

    // 正确调用 drawPixmap
    painter.drawPixmap(0, 0, size, size, m_roundedPixmap);

    // 恢复原始状态
    painter.restore();

    // 可选：绘制边框
    painter.setPen(QPen(QColor(123, 104, 238, 150), 3));
    painter.drawEllipse(rect);
}

void RotatingAlbumArt::resizeEvent(QResizeEvent *event)
{
    // 确保控件保持正方形
    int size = qMin(width(), height());
    if (size != width() || size != height()) {
        setFixedSize(size, size);
    }

    QLabel::resizeEvent(event);
    updateRoundedPixmap();
}

void RotatingAlbumArt::updateRoundedPixmap()
{
    if (m_originalPixmap.isNull()) {
        m_roundedPixmap = QPixmap();
        setText("Album Art");
        return;
    }

    setText("");
    m_roundedPixmap = roundedPixmap(m_originalPixmap);
    update();
}

QPixmap RotatingAlbumArt::roundedPixmap(const QPixmap &source)
{
    // 使用控件实际尺寸
    int size = width(); // 因为是正方形

    QPixmap target(size, size);
    target.fill(Qt::transparent);

    QPainter painter(&target);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // 创建圆形剪裁路径
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    // 缩放并居中绘制图像
    QPixmap scaled = source.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawPixmap(0, 0, size, size, scaled);

    return target;
}

