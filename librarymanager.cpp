#include "librarymanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

LibraryManager::LibraryManager(QObject* parent)
    : QObject(parent), 
      m_allBooks(new BookCollection("Все книги")),
      m_readBooks(new BookCollection("Прочитанные")),
      m_planToRead(new BookCollection("Планирую прочитать")),
      m_reading(new BookCollection("Читаю сейчас")),
      m_lists(new BookList("Списки")),
      m_nextId(1),
      m_username("Пользователь") {
    // Инициализируем корневой композит списков (не владеет дочерними коллекциями)
    m_lists->addComponent(std::shared_ptr<IBookComponent>(m_readBooks.get(), [](IBookComponent*){}));
    m_lists->addComponent(std::shared_ptr<IBookComponent>(m_planToRead.get(), [](IBookComponent*){}));
    m_lists->addComponent(std::shared_ptr<IBookComponent>(m_reading.get(), [](IBookComponent*){}));
}

void LibraryManager::addBook(const Book& book) {
    Book newBook = book;
    if (newBook.getId() == 0) {
        newBook.setId(getNextId());
    }
    m_allBooks->add(newBook);
    rebuildLists();
    emit bookAdded(newBook);
    emit libraryChanged();
}

void LibraryManager::updateBook(const Book& book) {
    Book* existingBook = m_allBooks->findById(book.getId());
    if (existingBook) {
        *existingBook = book;
        rebuildLists();
        emit bookUpdated(book);
        emit libraryChanged();
    }
}

void LibraryManager::removeBook(int bookId) {
    m_allBooks->remove(bookId);
    rebuildLists();
    emit bookRemoved(bookId);
    emit libraryChanged();
}

Book* LibraryManager::findBookById(int id) {
    return m_allBooks->findById(id);
}

QList<Book> LibraryManager::searchByTitle(const QString& title) const {
    QList<Book> result;
    QString searchTerm = title.toLower();
    for (const Book& book : m_allBooks->getBooks()) {
        if (book.getTitle().toLower().contains(searchTerm)) {
            result.append(book);
        }
    }
    return result;
}

QList<Book> LibraryManager::searchByAuthor(const QString& author) const {
    QList<Book> result;
    QString searchTerm = author.toLower();
    for (const Book& book : m_allBooks->getBooks()) {
        if (book.getAuthor().toLower().contains(searchTerm)) {
            result.append(book);
        }
    }
    return result;
}

QList<Book> LibraryManager::searchByGenre(const QString& genre) const {
    QList<Book> result;
    QString searchTerm = genre.toLower();
    for (const Book& book : m_allBooks->getBooks()) {
        if (book.getGenre().toLower().contains(searchTerm)) {
            result.append(book);
        }
    }
    return result;
}

QList<Book> LibraryManager::searchByStatus(ReadingStatus status) const {
    QList<Book> result;
    for (const Book& book : m_allBooks->getBooks()) {
        if (book.getStatus() == status) {
            result.append(book);
        }
    }
    return result;
}

QList<Book> LibraryManager::searchByTag(const QString& tag) const {
    QList<Book> result;
    QString term = tag.toLower();
    for (const Book& book : m_allBooks->getBooks()) {
        for (const QString& t : book.getTags()) {
            if (t.toLower().contains(term)) { result.append(book); break; }
        }
    }
    return result;
}

void LibraryManager::setSortStrategy(std::unique_ptr<ISortStrategy> strategy) {
    m_sorter.setStrategy(std::move(strategy));
}

QList<Book> LibraryManager::getSortedBooks() const {
    QList<Book> books = m_allBooks->getBooks();
    m_sorter.sortBooks(books);
    return books;
}

QString LibraryManager::getCurrentSortStrategy() const {
    return m_sorter.getCurrentStrategyName();
}

QList<Book> LibraryManager::getAllBooks() const {
    return m_allBooks->getBooks();
}

QList<Book> LibraryManager::getReadBooks() const {
    return m_readBooks->getBooks();
}

QList<Book> LibraryManager::getPlanToReadBooks() const {
    return m_planToRead->getBooks();
}

QList<Book> LibraryManager::getCurrentlyReading() const {
    return m_reading->getBooks();
}

int LibraryManager::getTotalBookCount() const {
    return m_allBooks->getCount();
}

int LibraryManager::getReadBookCount() const {
    return m_readBooks->getCount();
}

int LibraryManager::getPlanToReadCount() const {
    return m_planToRead->getCount();
}

QList<Book> LibraryManager::getTopRatedBooks(int count) const {
    QList<Book> allBooks = m_allBooks->getBooks();
    // Фильтруем только книги с оценкой > 0
    QList<Book> ratedBooks;
    for (const Book& book : allBooks) {
        if (book.getRating() > 0) {
            ratedBooks.append(book);
        }
    }
    // Сортируем по убыванию оценки
    std::sort(ratedBooks.begin(), ratedBooks.end(), 
        [](const Book& a, const Book& b) {
            return a.getRating() > b.getRating();
        });
    // Возвращаем топ N
    if (ratedBooks.size() > count) {
        ratedBooks = ratedBooks.mid(0, count);
    }
    return ratedBooks;
}

bool LibraryManager::exportToJson(const QString& filename) const {
    QJsonObject root;
    root["username"] = m_username;
    root["nextId"] = m_nextId;
    
    QJsonArray booksArray;
    for (const Book& book : m_allBooks->getBooks()) {
        booksArray.append(book.toJson());
    }
    root["books"] = booksArray;
    
    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool LibraryManager::importFromJson(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    m_username = root["username"].toString();
    m_nextId = root["nextId"].toInt();
    
    m_allBooks->clear();
    QJsonArray booksArray = root["books"].toArray();
    for (const QJsonValue& val : booksArray) {
        Book book = Book::fromJson(val.toObject());
        m_allBooks->add(book);
    }

    rebuildLists();
    emit libraryChanged();
    return true;
}

void LibraryManager::rebuildLists() {
    m_readBooks->clear();
    m_planToRead->clear();
    m_reading->clear();
    for (const Book& b : m_allBooks->getBooks()) {
        switch (b.getStatus()) {
            case ReadingStatus::Read: m_readBooks->add(b); break;
            case ReadingStatus::PlanToRead: m_planToRead->add(b); break;
            case ReadingStatus::Reading: m_reading->add(b); break;
            case ReadingStatus::None: break;
        }
    }
}
