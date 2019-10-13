#include "definitionheaderwidget.h"

DefinitionHeaderWidget::DefinitionHeaderWidget(QWidget *parent)
    : QWidget(parent)
{
    _layout = new QVBoxLayout{this};
    _layout->setContentsMargins(10, 10, 10, 10);
    _layout->setSpacing(10);

    _titleLabel = new QLabel{this};
    setStyleSheet("QWidget { "
                  " background-color: #3C3C3C; "
                  " border-top-left-radius: 10px; "
                  " border-top-right-radius: 10px; "
                  " border-bottom-left-radius: 0px; "
                  " border-bottom-right-radius: 0px; "
                  "}");
//    _divider = new QFrame{this};
//    _divider->setFrameShape(QFrame::HLine);
//    _divider->setFrameShadow(QFrame::Raised);
//    _divider->setFixedHeight(1);
#ifdef Q_OS_MAC
//    _divider->setStyleSheet("QFrame { border: 1px solid #6f6f6f; padding: 1px }");
#endif

    _layout->addWidget(_titleLabel);
//    _layout->addWidget(_divider);

    setLayout(_layout);
}

DefinitionHeaderWidget::DefinitionHeaderWidget(std::string title, QWidget *parent)
    : DefinitionHeaderWidget(parent)
{
    setSectionTitle(title);
}

DefinitionHeaderWidget::~DefinitionHeaderWidget()
{

}

//QSize DefinitionHeaderWidget::sizeHint() const
//{
//    return QSize(width(), _titleLabel->sizeHint().height() + 20);
//}

void DefinitionHeaderWidget::setSectionTitle(std::string title)
{
    std::string formattedTitle = formatTitle(title);
    _titleLabel->setText(formattedTitle.c_str());
    _titleLabel->setFixedHeight(_titleLabel->fontMetrics().boundingRect(title.c_str()).height());
}

//#include <QResizeEvent>
//void DefinitionHeaderWidget::resizeEvent(QResizeEvent *event) {
//    int width = event->size().width();
//    int height = _titleLabel->heightForWidth(width);
//    _titleLabel->setFixedHeight(height);
//    updateGeometry();
//}

std::string DefinitionHeaderWidget::formatTitle(std::string title)
{
    return "<font color=#6f6f6f>" + title + "</font>";
}
