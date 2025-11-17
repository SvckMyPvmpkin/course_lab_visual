#include "book.h"
#include <QJsonArray>

Book::Book() 
    : m_id(0), m_year(0), m_status(ReadingStatus::None), m_rating(0) {}

Book::Book(int id, const QString& title, const QString& author, 
           const QString& genre, int year, ReadingStatus status)
    : m_id(id), m_title(title), m_author(author), 
      m_genre(genre), m_year(year), m_status(status), m_rating(0) {}

void Book::addTag(const QString& tag) {
    if (!m_tags.contains(tag)) {
        m_tags.append(tag);
    }
}

void Book::removeTag(const QString& tag) {
    m_tags.removeAll(tag);
}

QJsonObject Book::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["title"] = m_title;
    obj["author"] = m_author;
    obj["genre"] = m_genre;
    obj["year"] = m_year;
    obj["status"] = static_cast<int>(m_status);
    obj["rating"] = m_rating;
    obj["coverPath"] = m_coverPath;
    obj["review"] = m_review;
    
    QJsonArray tagsArray;
    for (const QString& tag : m_tags) {
        tagsArray.append(tag);
    }
    obj["tags"] = tagsArray;
    
    return obj;
}

Book Book::fromJson(const QJsonObject& json) {
    Book book;
    book.setId(json["id"].toInt());
    book.setTitle(json["title"].toString());
    book.setAuthor(json["author"].toString());
    book.setGenre(json["genre"].toString());
    book.setYear(json["year"].toInt());
    book.setStatus(static_cast<ReadingStatus>(json.value("status").toInt()));
    book.setRating(json.value("rating").toInt());
    book.setCoverPath(json.value("coverPath").toString());
    book.setReview(json.value("review").toString());
    
    QStringList tags;
    QJsonArray tagsArray = json["tags"].toArray();
    for (const QJsonValue& val : tagsArray) {
        tags.append(val.toString());
    }
    book.setTags(tags);
    
    return book;
}

QString Book::toString() const {
    return QString("%1 - %2 (%3, %4)")
        .arg(m_title)
        .arg(m_author)
        .arg(m_genre)
        .arg(m_year);
}

QString Book::statusToString(ReadingStatus status) {
    switch (status) {
        case ReadingStatus::Reading: return "Читаю";
        case ReadingStatus::Read: return "Прочитана";
        case ReadingStatus::PlanToRead: return "Планирую прочитать";
        case ReadingStatus::None: return "Без статуса";
        default: return "Неизвестно";
    }
}

ReadingStatus Book::stringToStatus(const QString& str) {
    if (str == "Читаю") return ReadingStatus::Reading;
    if (str == "Прочитана") return ReadingStatus::Read;
    if (str == "Планирую прочитать") return ReadingStatus::PlanToRead;
    return ReadingStatus::None;
}
