#ifndef HANDWRITINGPANEL_H
#define HANDWRITINGPANEL_H

#include "vendor/zinnia/zinnia.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QWidget>

class HandwritingPanel : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;

    explicit HandwritingPanel(QWidget *parent = nullptr);
    ~HandwritingPanel();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void draw(const QPoint &pos);
    void finalizeStroke(const QPoint &pos);

    QString _characterPreamble;
    std::vector<QPoint> _strokePositions;
    std::vector<QString> _strokes;
    zinnia::Recognizer *_recognizer = nullptr;
    zinnia::Character *_character = nullptr;

    int counter = 0;

    QPixmap _pixmap;
    QPoint _lastPos;

signals:
};

#endif // HANDWRITINGPANEL_H
