#ifndef TRANSCRIPTIONWINDOW_H
#define TRANSCRIPTIONWINDOW_H

#include "logic/dictation/iinputvolumesubscriber.h"
#include "logic/dictation/itranscriptionresultsubscriber.h"
#include "logic/dictation/transcriberwrapper.h"

#include <QEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMetaType>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QWidget>

// The Transcription Window displays UI for transcription. It shows
// a microphone with an animation to indicate that the app is listening
// for input.

enum class TranscriptionLanguage : int { CANTONESE, MANDARIN, ENGLISH };
Q_DECLARE_METATYPE(TranscriptionLanguage)

class AnimateableEllipse : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)

public:
    AnimateableEllipse(qreal x,
                       qreal y,
                       qreal width,
                       qreal height,
                       QGraphicsItem *parent = nullptr)
        : QGraphicsEllipseItem(x, y, width, height, parent)
        , QObject()
    {}

    bool collidesWithItem(const QGraphicsItem *other,
                          Qt::ItemSelectionMode mode) const override
    {
        Q_UNUSED(other);
        Q_UNUSED(mode);
        return false;
    }

    bool collidesWithPath(const QPainterPath &path,
                          Qt::ItemSelectionMode mode) const override
    {
        Q_UNUSED(path);
        Q_UNUSED(mode);
        return false;
    }
};

class TranscriptionWindow : public QWidget,
                            public IInputVolumeSubscriber,
                            public ITranscriptionResultSubscriber
{
    Q_OBJECT

public:
    explicit TranscriptionWindow(QWidget *parent = nullptr);
    ~TranscriptionWindow();

    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void volumeResult(std::variant<std::system_error, float> result) override;
    void transcriptionResult(
        std::variant<std::system_error, std::string> result) override;

private:
    void setupUI(void);
    void translateUI(void);
    void setStyle(bool use_dark);

    void setTranscriptionLang(void);
    void stopTranscription(void);

#ifdef Q_OS_WIN
    // On Windows, the window widget's background colour
    // can't be changed. The workaround is to create
    // a new QWidget (whose background colour can be changed)
    // and then add it to the window's layout.
    QWidget *_innerWidget;
    QGridLayout *_outerWidgetLayout;
#endif

    QLabel *_titleLabel;

    QGraphicsScene *_graphicsScene;
    QGraphicsView *_graphicsView;
    QGraphicsPixmapItem *_icon;
    AnimateableEllipse *_ellipse;
    QPropertyAnimation *_anim;

    QPushButton *_cantoneseButton;
    QPushButton *_mandarinButton;
    QPushButton *_englishButton;

    QPushButton *_doneButton;

    QGridLayout *_dialogLayout;

    std::unique_ptr<QSettings> _settings;
    std::unique_ptr<TranscriberWrapper> _wrapper = nullptr;

signals:
    void transcription(QString result);
    void newRadius(float radius);

public slots:
    void startAnimation(float radius);

    void doneAction(void);
};

#endif // TRANSCRIPTIONWINDOW_H
