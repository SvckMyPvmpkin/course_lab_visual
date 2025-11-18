#ifndef BOOKDETAILSDIALOG_H
#define BOOKDETAILSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include "book.h"

class BookDetailsDialog : public QDialog {
    Q_OBJECT

private:
    QLabel* m_coverLabel;
    QLabel* m_titleLabel;
    QLabel* m_authorLabel;
    QLabel* m_genreLabel;
    QLabel* m_yearLabel;
    QLabel* m_tagsLabel;

    QSpinBox* m_ratingSpin;
    QComboBox* m_statusCombo;
    QTextEdit* m_reviewEdit;

    Book m_book; // копия редактируемой книги

public:
    explicit BookDetailsDialog(QWidget* parent, const Book& book);
    Book getUpdatedBook() const { return m_book; }

private:
    void setupUI();
    void loadBook(const Book& book);
};

#endif // BOOKDETAILSDIALOG_H
