#ifndef BOOKCOLLECTION_H
#define BOOKCOLLECTION_H

#include "book.h"
#include <QList>
#include <QString>
#include <memory>

// Composite Pattern - базовый компонент
class IBookComponent {
public:
    virtual ~IBookComponent() = default;
    virtual void add(const Book& book) = 0;
    virtual void remove(int bookId) = 0;
    virtual QList<Book> getBooks() const = 0;
    virtual int getCount() const = 0;
    virtual QString getName() const = 0;
};

// Leaf - простая коллекция книг
class BookCollection : public IBookComponent {
private:
    QString m_name;
    QList<Book> m_books;

public:
    explicit BookCollection(const QString& name);
    
    void add(const Book& book) override;
    void remove(int bookId) override;
    QList<Book> getBooks() const override;
    int getCount() const override;
    QString getName() const override;
    
    void clear();
    Book* findById(int id);
};

// Composite - составная коллекция (списки книг)
class BookList : public IBookComponent {
private:
    QString m_name;
    QList<std::shared_ptr<IBookComponent>> m_components;

public:
    explicit BookList(const QString& name);
    
    void add(const Book& book) override;
    void remove(int bookId) override;
    QList<Book> getBooks() const override;
    int getCount() const override;
    QString getName() const override;
    
    void addComponent(std::shared_ptr<IBookComponent> component);
    void removeComponent(const QString& name);
};

#endif // BOOKCOLLECTION_H
