#ifndef HANDWRITINGPANEL_H
#define HANDWRITINGPANEL_H

#include <QPixmap>
#include <QPoint>
#include <QWidget>

class QMouseEvent;
class QPainter;

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
    void pixmapDimensions(const int width, const int height);

    void strokeStart(const int x, const int y);
    void strokeUpdate(const int x, const int y);
    void strokeComplete(const int x, const int y);
};

#endif // HANDWRITINGPANEL_H
