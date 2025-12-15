#include "scrolllabel.h"
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QStyle>
#include <QStyleOption>

ScrollLabel::ScrollLabel(QWidget *parent)
    : QLabel(parent)
{
    // 设置定时器
    m_scrollTimer.setInterval(25); // 30ms刷新一次，约33fps
    connect(&m_scrollTimer, &QTimer::timeout, this, &ScrollLabel::updateScrolling);

    // 设置复位动画
    m_resetAnimation.setTargetObject(this);
    m_resetAnimation.setPropertyName("scrollOffset");
    m_resetAnimation.setDuration(500); // 500ms动画
    m_resetAnimation.setEasingCurve(QEasingCurve::OutCubic);
    connect(&m_resetAnimation, &QPropertyAnimation::finished,
            this, &ScrollLabel::resetAnimationFinished);
}

void ScrollLabel::setText(const QString &text)
{
    if (m_fullText == text) return;

    m_fullText = text;
    m_scrollOffset = 0;

    // 计算文本宽度
    QFontMetrics fm(font());
    m_textWidth = fm.horizontalAdvance(text);

    // 更新滚动状态
    updateScrollState();

    // 更新显示
    update();
}

int ScrollLabel::scrollOffset() const
{
    return m_scrollOffset;
}

void ScrollLabel::setScrollOffset(int offset)
{
    m_scrollOffset = offset;
    update();
}

void ScrollLabel::setScrollSpeed(int pixelsPerSecond)
{
    if (pixelsPerSecond > 0) {
        m_scrollSpeed = pixelsPerSecond;
        // 更新定时器间隔 (毫秒)
        int interval = 1000 / (m_scrollSpeed / 2); // 基于速度调整
        m_scrollTimer.setInterval(qMax(10, interval)); // 最小10ms
    }
}

void ScrollLabel::setPauseDuration(int milliseconds)
{
    if (milliseconds >= 0) {
        m_pauseDuration = milliseconds;
    }
}

Qt::Alignment ScrollLabel::alignment() const
{
    return m_alignment;
}

void ScrollLabel::setAlignment(Qt::Alignment alignment)
{
    if (m_alignment != alignment) {
        m_alignment = alignment;
        update();
    }
}

void ScrollLabel::updateScrollState()
{
    // 计算文本宽度（如果尚未计算）
    if (m_textWidth == 0 && !m_fullText.isEmpty()) {
        QFontMetrics fm(font());
        m_textWidth = fm.horizontalAdvance(m_fullText);
    }

    // 判断是否需要滚动（文本宽度 > 控件宽度）
    bool needScrolling = m_textWidth > width();

    if (m_scrollEnabled != needScrolling) {
        m_scrollEnabled = needScrolling;

        if (m_scrollEnabled && isVisible()) {
            // 延迟启动滚动
            QTimer::singleShot(m_pauseDuration, this, &ScrollLabel::startScrolling);
        } else {
            // 停止所有动画和滚动
            m_scrollTimer.stop();
            m_resetAnimation.stop();
            m_isScrolling = false;
            m_scrollOffset = 0;
            update();
        }
    }
}

void ScrollLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 应用样式表（背景、边框等）
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setRenderHint(QPainter::Antialiasing);

    // 设置文本颜色（使用样式表设置的颜色）
    painter.setPen(palette().text().color());

    QRect textRect = contentsRect();

    if (!m_scrollEnabled || !m_isScrolling) {
        // 不滚动时：正常显示文本（可能省略）
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(m_fullText, Qt::ElideRight, textRect.width());

        // 使用样式表设置的对齐方式
        painter.drawText(textRect, m_alignment, elidedText);
    } else {
        // 滚动时：显示滚动文本
        int yPos = (height() + painter.fontMetrics().ascent() - painter.fontMetrics().descent()) / 2;
        painter.drawText(-m_scrollOffset, yPos, m_fullText);
    }
}

void ScrollLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);

    // 重新计算文本宽度
    if (!m_fullText.isEmpty()) {
        QFontMetrics fm(font());
        m_textWidth = fm.horizontalAdvance(m_fullText);
    }

    // 更新滚动状态
    updateScrollState();
}

void ScrollLabel::showEvent(QShowEvent *event)
{
    QLabel::showEvent(event);
    if (m_scrollEnabled && !m_isScrolling) {
        QTimer::singleShot(m_pauseDuration, this, &ScrollLabel::startScrolling);
    }
}

void ScrollLabel::hideEvent(QHideEvent *event)
{
    QLabel::hideEvent(event);
    m_scrollTimer.stop();
    m_isScrolling = false;
}

void ScrollLabel::startScrolling()
{
    if (m_scrollEnabled && isVisible()) {
        m_isScrolling = true;
        m_scrollTimer.start();
    }
}

void ScrollLabel::updateScrolling()
{
    // 计算滚动位置 (基于速度)
    int pixelsPerFrame = m_scrollSpeed / 30; // 30fps ≈ 每帧移动像素数
    int newOffset = m_scrollOffset + qMax(1, pixelsPerFrame);

    // 如果文本完全滚动出视图
    if (newOffset > m_textWidth + width()) {
        // 停止定时器
        m_scrollTimer.stop();
        m_isScrolling = false;

        // 回到起点
        m_resetAnimation.setStartValue(m_scrollOffset);
        m_resetAnimation.setEndValue(0);
        m_resetAnimation.start();
    } else {
        m_scrollOffset = newOffset;
        update();
    }
}

void ScrollLabel::resetAnimationFinished()
{
    // 检查是否仍然需要滚动
    if (m_scrollEnabled && isVisible()) {
        // 延迟后重新开始滚动
        QTimer::singleShot(m_pauseDuration, this, &ScrollLabel::startScrolling);
    }
}
