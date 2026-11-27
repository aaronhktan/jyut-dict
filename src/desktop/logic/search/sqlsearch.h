#ifndef SQLSEARCH_H
#define SQLSEARCH_H

#include "logic/search/isearch.h"
#include "logic/search/isearchobservable.h"

#include <QFutureWatcher>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <unordered_set>
#include <vector>

class Entry;
class SQLDatabaseManager;

// SQLSearch searches the database provided by SQLDatabaseManager.

struct FutureList
{
    std::mutex mut;
    std::unordered_set<QFutureWatcher<void> *> set;
};

class SQLSearch : virtual public ISearch,
                  virtual public ISearchObservable
{
public:
    SQLSearch();
    SQLSearch(std::shared_ptr<SQLDatabaseManager> manager);
    ~SQLSearch() override;

    // Disable copy constructor and copy assignment, since the list of futures
    // and observers shouldn't be copied.
    SQLSearch(const SQLSearch &) = delete;
    SQLSearch(SQLSearch &&) = delete;

    void registerObserver(ISearchObserver *observer) override;
    void deregisterObserver(ISearchObserver *observer) override;

    void searchSimplified(const QString &searchTerm) override;
    void searchTraditional(const QString &searchTerm) override;
    void searchJyutping(const QString &searchTerm, bool fuzzyJyutping) override;
    void searchPinyin(const QString &searchTerm, bool fuzzyPinyin) override;
    void searchEnglish(const QString &searchTerm) override;
    void searchAutoDetect(const QString &searchTerm) override;

    void searchByUnique(const QString &traditional,
                        const QString &simplified,
                        const QString &jyutping,
                        const QString &pinyin) override;

    void searchTraditionalExamples(const QString &searchTerm);

private:
    void notifyObservers(SearchParameters params) override;
    void notifyObservers(const std::vector<Entry> &results, bool emptyQuery) override;
    void notifyObservers(const std::vector<Example> &results,
                         bool emptyQuery) override;
    void notifyObserversOfEmptySet(bool emptyQuery,
                                   const unsigned long long queryID);
    void notifyObserversIfQueryIdCurrent(SearchParameters params,
                                         const unsigned long long queryID);
    void notifyObserversIfQueryIdCurrent(const std::vector<Entry> &results,
                                         bool emptyQuery,
                                         const unsigned long long queryID);
    void notifyObserversIfQueryIdCurrent(const std::vector<Example> &results,
                                         bool emptyQuery,
                                         const unsigned long long queryID);

    unsigned long long generateAndSetQueryID(void);
    bool checkQueryIDCurrent(const unsigned long long queryID) const;

    void populateJyutpingCache(unsigned int hash);
    void populatePinyinCache(unsigned int hash);

    std::vector<QString> jyutpingFilteredBy(const QString &prefix);
    std::vector<QString> pinyinFilteredBy(const QString &prefix);

    std::shared_ptr<std::vector<QString>> allJyutping();
    std::shared_ptr<std::vector<QString>> allPinyin();

    std::vector<int> allRowIds();
    std::vector<int> rowIdFilteredByJyutping(const QString &jyutping);
    std::vector<int> rowIdFilteredByPinyin(const QString &pinyin);

    template<typename F, typename... A>
    void runThread(F &&fun, A &&...args)
    {
        if (!_manager) {
            std::cout << "No database specified!" << std::endl;
            return;
        }

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
        QObject::connect(watcher,
                         &QFutureWatcher<void>::finished,
                         watcher,
                         [this, watcher] {
                             {
                                 std::lock_guard lock(_watchers.mut);
                                 _watchers.set.erase(watcher);
                             }
                             delete watcher;
                         });
        {
            std::lock_guard lock(_watchers.mut);
            _watchers.set.emplace(watcher);
        }
        watcher->setFuture(
            QtConcurrent::run([fn = std::forward<F>(fun),
                               ... args = std::forward<A>(args)]() mutable {
                std::invoke(fn, std::move(args)...);
            }));
    }
    void searchSimplifiedThread(const QString &searchTerm,
                                const unsigned long long queryID);
    void searchTraditionalThread(const QString &searchTerm,
                                 const unsigned long long queryID);
    void searchJyutpingThread(const QString &searchTerm,
                              bool fuzzyJyutping,
                              const unsigned long long queryID);
    void searchPinyinThread(const QString &searchTerm,
                            bool fuzzyPinyin,
                            const unsigned long long queryID);
    void searchEnglishThread(const QString &searchTerm,
                             const unsigned long long queryID);
    void searchAutoDetectThread(const QString &searchTerm,
                                const unsigned long long queryID);

    void searchByUniqueThread(const QString &traditional,
                              const QString &simplified,
                              const QString &jyutping,
                              const QString &pinyin,
                              const unsigned long long queryID);

    void searchTraditionalExamplesThread(const QString &searchTerm,
                                         const unsigned long long queryID);

    std::shared_mutex _jyutpingCacheMutex;
    std::optional<std::tuple<unsigned int, std::shared_ptr<std::vector<QString>>>>
        _jyutpingCache = std::nullopt;
    std::shared_mutex _pinyinCacheMutex;
    std::optional<std::tuple<unsigned int, std::shared_ptr<std::vector<QString>>>>
        _pinyinCache = std::nullopt;

    std::mutex _notifyMutex;
    std::list<ISearchObserver *> _observers;

    std::shared_ptr<SQLDatabaseManager> _manager;
    std::unique_ptr<QSettings> _settings;

    std::atomic<unsigned long long> _queryID;
    std::mt19937_64 _generator;
    std::uniform_int_distribution<unsigned long long> _dist;

    FutureList _watchers;
};

#endif // SQLSEARCH_H
