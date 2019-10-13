#ifndef RESIZEABLELABEL_H
#define RESIZEABLELABEL_H

#include <QLabel>

class ResizeableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ResizeableLabel(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    void resizeEvent(QResizeEvent *event) override;
signals:

public slots:
};

#endif // RESIZEABLELABEL_H
