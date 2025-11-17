#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "book.h"
#include "bookcollection.h"
#include "sortstrategy.h"
#include <QObject>
#include <QString>
#include <memory>

class LibraryManager : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<BookCollection> m_allBooks;
    std::unique_ptr<BookCollection> m_readBooks;
    std::unique_ptr<BookCollection> m_planToRead;
    std::unique_ptr<BookCollection> m_reading;
    std::unique_ptr<BookList> m_lists; // Composite корневой узел списков
    BookSorter m_sorter;
    int m_nextId;
    QString m_username;

    void rebuildLists();

public:
    explicit LibraryManager(QObject* parent = nullptr);
    
    // Book operations
    void addBook(const Book& book);
    void updateBook(const Book& book);
    void removeBook(int bookId);
    Book* findBookById(int id);
    
    // Search operations
    QList<Book> searchByTitle(const QString& title) const;
    QList<Book> searchByAuthor(const QString& author) const;
    QList<Book> searchByGenre(const QString& genre) const;
    QList<Book> searchByStatus(ReadingStatus status) const;
    QList<Book> searchByTag(const QString& tag) const;
    
    // Sorting
    void setSortStrategy(std::unique_ptr<ISortStrategy> strategy);
    QList<Book> getSortedBooks() const;
    QString getCurrentSortStrategy() const;
    
    // Lists
    QList<Book> getAllBooks() const;
    QList<Book> getReadBooks() const;
    QList<Book> getPlanToReadBooks() const;
    QList<Book> getCurrentlyReading() const;
    
    // Statistics
    int getTotalBookCount() const;
    int getReadBookCount() const;
    int getPlanToReadCount() const;
    
    // Import/Export
    bool exportToJson(const QString& filename) const;
    bool importFromJson(const QString& filename);
    
    // User
    void setUsername(const QString& username) { m_username = username; }
    QString getUsername() const { return m_username; }
    
    int getNextId() { return m_nextId++; }

signals:
    void bookAdded(const Book& book);
    void bookUpdated(const Book& book);
    void bookRemoved(int bookId);
    void libraryChanged();
};

#endif // LIBRARYMANAGER_H
