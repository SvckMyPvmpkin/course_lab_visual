#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QJsonObject>

enum class ReadingStatus {
    Reading,
    Read,
    PlanToRead,
    None
};

class Book {
private:
    int m_id;
    QString m_title;
    QString m_author;
    QString m_genre;
    int m_year;
    ReadingStatus m_status;
    QStringList m_tags;
    int m_rating;              // 1..10, 0 — нет оценки
    QString m_coverPath;       // путь к обложке (опционально)
    QString m_review;          // мини-рецензия

public:
    Book();
    Book(int id, const QString& title, const QString& author, 
         const QString& genre, int year, ReadingStatus status = ReadingStatus::None);

    // Getters
    int getId() const { return m_id; }
    QString getTitle() const { return m_title; }
    QString getAuthor() const { return m_author; }
    QString getGenre() const { return m_genre; }
    int getYear() const { return m_year; }
    ReadingStatus getStatus() const { return m_status; }
    QStringList getTags() const { return m_tags; }
    int getRating() const { return m_rating; }
    QString getCoverPath() const { return m_coverPath; }
    QString getReview() const { return m_review; }

    // Setters
    void setId(int id) { m_id = id; }
    void setTitle(const QString& title) { m_title = title; }
    void setAuthor(const QString& author) { m_author = author; }
    void setGenre(const QString& genre) { m_genre = genre; }
    void setYear(int year) { m_year = year; }
    void setStatus(ReadingStatus status) { m_status = status; }
    void setTags(const QStringList& tags) { m_tags = tags; }
    void addTag(const QString& tag);
    void removeTag(const QString& tag);
    void setRating(int r) { m_rating = r; }
    void setCoverPath(const QString& p) { m_coverPath = p; }
    void setReview(const QString& r) { m_review = r; }

    // Serialization
    QJsonObject toJson() const;
    static Book fromJson(const QJsonObject& json);

    // Display
    QString toString() const;
    static QString statusToString(ReadingStatus status);
    static ReadingStatus stringToStatus(const QString& str);
};

#endif // BOOK_H
