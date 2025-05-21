#ifndef HANDWRITINGPANEL_H
#define HANDWRITINGPANEL_H

#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QWidget>

// The handwriting panel tracks and displays a user's click-and-drag
// events while they draw a character.

class HandwritingPanel : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;

    explicit HandwritingPanel(QWidget *parent = nullptr);

    void clearPanel(void);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void draw(const QPoint &pos);

    QPixmap _pixmap;
    QPoint _lastPos;

signals:
    void pixmapDimensions(int width, int height);

    void strokeStart(int x, int y);
    void strokeUpdate(int x, int y);
    void strokeComplete(int x, int y);
};

#endif // HANDWRITINGPANEL_H
