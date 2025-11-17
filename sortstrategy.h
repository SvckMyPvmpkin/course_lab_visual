#ifndef SORTSTRATEGY_H
#define SORTSTRATEGY_H

#include "book.h"
#include <QList>
#include <memory>

// Strategy Pattern - интерфейс стратегии сортировки
class ISortStrategy {
public:
    virtual ~ISortStrategy() = default;
    virtual void sort(QList<Book>& books) const = 0;
    virtual QString getName() const = 0;
};

// Concrete Strategy - сортировка по названию (алфавит)
class SortByTitleStrategy : public ISortStrategy {
public:
    void sort(QList<Book>& books) const override;
    QString getName() const override { return "По названию"; }
};

// Concrete Strategy - сортировка по автору (алфавит)
class SortByAuthorStrategy : public ISortStrategy {
public:
    void sort(QList<Book>& books) const override;
    QString getName() const override { return "По автору"; }
};

// Concrete Strategy - сортировка по году
class SortByYearStrategy : public ISortStrategy {
public:
    void sort(QList<Book>& books) const override;
    QString getName() const override { return "По году"; }
};

// Concrete Strategy - сортировка по жанру
class SortByGenreStrategy : public ISortStrategy {
public:
    void sort(QList<Book>& books) const override;
    QString getName() const override { return "По жанру"; }
};

// Context - использует стратегию
class BookSorter {
private:
    std::unique_ptr<ISortStrategy> m_strategy;

public:
    BookSorter();
    void setStrategy(std::unique_ptr<ISortStrategy> strategy);
    void sortBooks(QList<Book>& books) const;
    QString getCurrentStrategyName() const;
};

#endif // SORTSTRATEGY_H
