#include "handwritingpanel.h"

#include <iostream>

HandwritingPanel::HandwritingPanel(QWidget *parent)
    : QWidget{parent}
{
    _pixmap.setDevicePixelRatio(devicePixelRatio());

    _recognizer = zinnia::Recognizer::create();
    if (!_recognizer->open(
            "/Users/aaron/Documents/Github/jyut-dict/src/jyut-dict/vendor/"
            "zinnia/models/handwriting-zh_TW.model")) {
        std::cerr << _recognizer->what() << std::endl;
    }

    _character = zinnia::Character::create();
    _character->clear();

    _characterPreamble = QString{"(character (width %1)(height %1) (strokes "}
                             .arg(rect().width() * devicePixelRatio());
}

HandwritingPanel::~HandwritingPanel()
{
    if (_recognizer) {
        delete _recognizer;
    }
    if (_character) {
        delete _character;
    }
}

void HandwritingPanel::mousePressEvent(QMouseEvent *event)
{
    _lastPos = event->pos();
    _strokePositions.emplace_back(event->pos());
    draw(event->pos());
}

void HandwritingPanel::mouseMoveEvent(QMouseEvent *event)
{
    counter++;
    if (counter % 5 == 0) {
        if (std::abs(event->pos().x() - _lastPos.x()) > 10
            || (std::abs(event->pos().y() - _lastPos.y()) > 10)) {
            _strokePositions.emplace_back(event->pos());
        }
    }
    draw(event->pos());
}

void HandwritingPanel::mouseReleaseEvent(QMouseEvent *event)
{
    _strokePositions.emplace_back(event->pos());
    finalizeStroke(event->pos());
}

void HandwritingPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter{this};
    painter.drawPixmap(0, 0, _pixmap);
}

void HandwritingPanel::resizeEvent(QResizeEvent *event)
{
    auto newRect = _pixmap.rect().united(rect());
    if (newRect == _pixmap.rect()) {
        return;
    }

    _characterPreamble = QString{"(character (width %1)(height %1) (strokes "}
                             .arg(rect().width() * devicePixelRatio());

    QPixmap newPixmap{newRect.size() * devicePixelRatio()};
    QPainter painter{&newPixmap};
    painter.fillRect(newPixmap.rect(), Qt::white);
    painter.drawPixmap(0, 0, _pixmap);
    _pixmap = newPixmap;
    _pixmap.setDevicePixelRatio(devicePixelRatio());
}

void HandwritingPanel::draw(const QPoint &pos)
{
    QPainter painter{&_pixmap};
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen({Qt::blue, 4.0});
    painter.drawLine(_lastPos, pos);
    _lastPos = pos;
    update();
}

void HandwritingPanel::finalizeStroke(const QPoint &pos)
{
    std::cout << _characterPreamble.toStdString() << std::endl;
    QString stroke = "(";
    for (const auto &pos : _strokePositions) {
        stroke += QString{"(%1 %2)"}
                      .arg(pos.x() * devicePixelRatio())
                      .arg(pos.y() * devicePixelRatio());
    }
    stroke += ")";
    _strokes.emplace_back(stroke);
    _strokePositions.clear();

    QString strokes;
    for (const auto &stroke : _strokes) {
        strokes += stroke;
    }

    QString input = _characterPreamble + strokes + "))";
    std::cout << input.toStdString() << std::endl;

    if (!_character->parse(input.toStdString().c_str())) {
        std::cout << _character->what();
    }

    zinnia::Result *res = _recognizer->classify(*_character, 10);
    if (!res) {
        std::cout << "recognizer error: " << _recognizer->what() << std::endl;
        return;
    }
    for (size_t i = 0; i < res->size(); ++i) {
        std::wcout << res->value(i) << "\t" << res->score(i) << std::endl;
    }
    delete res;
}
