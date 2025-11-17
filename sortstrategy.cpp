#include "sortstrategy.h"
#include <algorithm>

void SortByTitleStrategy::sort(QList<Book>& books) const {
    std::sort(books.begin(), books.end(), 
              [](const Book& a, const Book& b) {
                  return a.getTitle().toLower() < b.getTitle().toLower();
              });
}

void SortByAuthorStrategy::sort(QList<Book>& books) const {
    std::sort(books.begin(), books.end(), 
              [](const Book& a, const Book& b) {
                  return a.getAuthor().toLower() < b.getAuthor().toLower();
              });
}

void SortByYearStrategy::sort(QList<Book>& books) const {
    std::sort(books.begin(), books.end(), 
              [](const Book& a, const Book& b) {
                  return a.getYear() < b.getYear();
              });
}

void SortByGenreStrategy::sort(QList<Book>& books) const {
    std::sort(books.begin(), books.end(), 
              [](const Book& a, const Book& b) {
                  return a.getGenre().toLower() < b.getGenre().toLower();
              });
}

BookSorter::BookSorter() 
    : m_strategy(new SortByTitleStrategy()) {}

void BookSorter::setStrategy(std::unique_ptr<ISortStrategy> strategy) {
    m_strategy = std::move(strategy);
}

void BookSorter::sortBooks(QList<Book>& books) const {
    if (m_strategy) {
        m_strategy->sort(books);
    }
}

QString BookSorter::getCurrentStrategyName() const {
    return m_strategy ? m_strategy->getName() : "Не выбрана";
}
