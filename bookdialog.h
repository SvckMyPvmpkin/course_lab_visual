#ifndef BOOKDIALOG_H
#define BOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include "book.h"

class QPushButton;

class BookDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* m_titleEdit;
    QLineEdit* m_authorEdit;
    QLineEdit* m_genreEdit;
    QSpinBox* m_yearSpinBox;
    QComboBox* m_statusCombo;
    QLineEdit* m_tagsEdit;
    QSpinBox* m_ratingSpin;
    QLineEdit* m_coverPathEdit;
    QPushButton* m_browseCoverBtn;
    QTextEdit* m_reviewEdit;
    QLabel* m_coverHint;
    
    Book m_book;
    bool m_isEditMode;

public:
    explicit BookDialog(QWidget* parent = nullptr, const Book& book = Book());
    
    Book getBook() const;

private:
    void setupUI();
    void loadBook(const Book& book);
};

#endif // BOOKDIALOG_H
