#include "bookcollection.h"
#include <algorithm>

// BookCollection implementation
BookCollection::BookCollection(const QString& name) 
    : m_name(name) {}

void BookCollection::add(const Book& book) {
    m_books.append(book);
}

void BookCollection::remove(int bookId) {
    m_books.erase(
        std::remove_if(m_books.begin(), m_books.end(),
                      [bookId](const Book& b) { return b.getId() == bookId; }),
        m_books.end()
    );
}

QList<Book> BookCollection::getBooks() const {
    return m_books;
}

int BookCollection::getCount() const {
    return m_books.size();
}

QString BookCollection::getName() const {
    return m_name;
}

void BookCollection::clear() {
    m_books.clear();
}

Book* BookCollection::findById(int id) {
    for (Book& book : m_books) {
        if (book.getId() == id) {
            return &book;
        }
    }
    return nullptr;
}

// BookList implementation
BookList::BookList(const QString& name) 
    : m_name(name) {}

void BookList::add(const Book& book) {
    for (auto& component : m_components) {
        component->add(book);
    }
}

void BookList::remove(int bookId) {
    for (auto& component : m_components) {
        component->remove(bookId);
    }
}

QList<Book> BookList::getBooks() const {
    QList<Book> allBooks;
    for (const auto& component : m_components) {
        allBooks.append(component->getBooks());
    }
    return allBooks;
}

int BookList::getCount() const {
    int total = 0;
    for (const auto& component : m_components) {
        total += component->getCount();
    }
    return total;
}

QString BookList::getName() const {
    return m_name;
}

void BookList::addComponent(std::shared_ptr<IBookComponent> component) {
    m_components.append(component);
}

void BookList::removeComponent(const QString& name) {
    m_components.erase(
        std::remove_if(m_components.begin(), m_components.end(),
                      [&name](const std::shared_ptr<IBookComponent>& c) { 
                          return c->getName() == name; 
                      }),
        m_components.end()
    );
}
