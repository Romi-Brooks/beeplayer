#ifndef SCROLLLABEL_H
#define SCROLLLABEL_H

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

class ScrollLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int scrollOffset READ scrollOffset WRITE setScrollOffset)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment) // 添加对齐属性

public:
    explicit ScrollLabel(QWidget *parent = nullptr);

    void setText(const QString &text);
    int scrollOffset() const;
    void setScrollOffset(int offset);

    // 添加设置滚动参数的方法
    void setScrollSpeed(int pixelsPerSecond);
    void setPauseDuration(int milliseconds);

    // 对齐属性访问器
    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void startScrolling();
    void updateScrolling();
    void resetAnimationFinished();

private:
    void updateScrollState(); // 添加内部状态更新函数

    QString m_fullText;
    int m_scrollOffset = 0;
    QTimer m_scrollTimer;
    QPropertyAnimation m_resetAnimation;
    int m_textWidth = 0;
    bool m_scrollEnabled = false;
    bool m_isScrolling = false;

    // 添加滚动参数
    int m_scrollSpeed = 60;
    int m_pauseDuration = 500;

    Qt::Alignment m_alignment = Qt::AlignCenter; // 默认居中
};

#endif // SCROLLLABEL_H
