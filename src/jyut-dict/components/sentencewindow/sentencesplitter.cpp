#include "sentencesplitter.h"

#include "components/sentencesearchresult/sentenceresultlistview.h"
#include "logic/settings/settingsutils.h"

#include <QList>
#include <QVariant>

SentenceSplitter::SentenceSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                                   QWidget *parent)
    : QSplitter(parent)
    , _manager{manager}
{
    _sqlSearch = std::make_shared<SQLSearch>(manager);
    _model = new SentenceResultListModel{_sqlSearch, {}, this};

    _sentenceScrollArea = new SentenceScrollArea{this};
    _resultListView = new SentenceResultListView{this};
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_sentenceScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &SentenceSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &SentenceSplitter::handleDoubleClick);

#ifdef Q_OS_LINUX
    setMinimumSize(600, 400);
#else
    setMinimumHeight(400);
#endif

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
    setStyleSheet("QSplitter::handle { background-color: none; }");
}

SentenceSplitter::~SentenceSplitter()
{

}

void SentenceSplitter::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void SentenceSplitter::setSourceSentences(
    std::vector<SourceSentence> sourceSentences)
{
    static_cast<SentenceResultListModel *>(_model)->setSentences(sourceSentences);
    _size = static_cast<int>(sourceSentences.size());
}

void SentenceSplitter::setSearchTerm(QString searchTerm)
{
    _searchTerm = searchTerm;
    translateUI();
}

void SentenceSplitter::translateUI(void)
{
    QString title;
    if (_size == 1) {
        title = QString{tr("Sentences for %1 (%2 result)")}.arg(_searchTerm,
                                                                QString::number(
                                                                    _size));
    } else {
        title = QString{tr("Sentences for %1 (%2 results)")}
                    .arg(_searchTerm, QString::number(_size));
    }
    setWindowTitle(title);
}

void SentenceSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void SentenceSplitter::handleClick(const QModelIndex &selection)
{
    SourceSentence sentence = qvariant_cast<SourceSentence>(selection.data());

    _sentenceScrollArea->setSourceSentence(sentence);
}

void SentenceSplitter::handleDoubleClick(const QModelIndex &selection)
{
    SourceSentence sentence = qvariant_cast<SourceSentence>(selection.data());

    SentenceScrollArea *area = new SentenceScrollArea{nullptr};
    area->setParent(this, Qt::Window);
    area->setSourceSentence(sentence);
#ifndef Q_OS_MAC
    area->setWindowTitle(" ");
#endif
    area->show();
}
