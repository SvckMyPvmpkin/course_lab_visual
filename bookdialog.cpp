#include "bookdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QFileDialog>

BookDialog::BookDialog(QWidget* parent, const Book& book)
    : QDialog(parent), m_book(book), m_isEditMode(book.getId() != 0) {
    
    setupUI();
    
    if (m_isEditMode) {
        setWindowTitle("Редактирование книги");
        loadBook(book);
    } else {
        setWindowTitle("Добавление книги");
    }
    
    resize(480, 520);
}

void BookDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    m_titleEdit = new QLineEdit();
    m_titleEdit->setPlaceholderText("Введите название книги");
    formLayout->addRow("Название*:", m_titleEdit);
    
    m_authorEdit = new QLineEdit();
    m_authorEdit->setPlaceholderText("Введите автора книги");
    formLayout->addRow("Автор*:", m_authorEdit);
    
    m_genreEdit = new QLineEdit();
    m_genreEdit->setPlaceholderText("Например: Фантастика, Детектив");
    formLayout->addRow("Жанр*:", m_genreEdit);
    
    m_yearSpinBox = new QSpinBox();
    m_yearSpinBox->setRange(1000, 2100);
    m_yearSpinBox->setValue(2024);
    formLayout->addRow("Год издания*:", m_yearSpinBox);
    
    m_statusCombo = new QComboBox();
    m_statusCombo->addItem("Без статуса");
    m_statusCombo->addItem("Читаю");
    m_statusCombo->addItem("Прочитана");
    m_statusCombo->addItem("Планирую прочитать");
    formLayout->addRow("Статус:", m_statusCombo);
    
    m_tagsEdit = new QLineEdit();
    m_tagsEdit->setPlaceholderText("Теги через запятую");
    formLayout->addRow("Теги:", m_tagsEdit);

    m_ratingSpin = new QSpinBox();
    m_ratingSpin->setRange(0, 10); // 0 = нет оценки
    m_ratingSpin->setToolTip("Оценка 1..10, 0 — без оценки");
    formLayout->addRow("Оценка:", m_ratingSpin);

    QHBoxLayout* coverLayout = new QHBoxLayout();
    m_coverPathEdit = new QLineEdit();
    m_browseCoverBtn = new QPushButton("Выбрать...");
    coverLayout->addWidget(m_coverPathEdit);
    coverLayout->addWidget(m_browseCoverBtn);
    QWidget* coverWidget = new QWidget();
    coverWidget->setLayout(coverLayout);
    formLayout->addRow("Обложка:", coverWidget);

    m_reviewEdit = new QTextEdit();
    m_reviewEdit->setPlaceholderText("Короткая рецензия...");
    formLayout->addRow("Рецензия:", m_reviewEdit);

    mainLayout->addLayout(formLayout);

    QLabel* noteLabel = new QLabel("* - обязательные поля");
    noteLabel->setStyleSheet("color: gray; font-size: 9pt;");
    mainLayout->addWidget(noteLabel);

    mainLayout->addStretch();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_browseCoverBtn, &QPushButton::clicked, [this]() {
        QString file = QFileDialog::getOpenFileName(this, "Выбор обложки", "", 
            "Изображения (*.png *.jpg *.jpeg *.bmp)");
        if (!file.isEmpty()) m_coverPathEdit->setText(file);
    });

    mainLayout->addWidget(buttonBox);
}

void BookDialog::loadBook(const Book& book) {
    m_titleEdit->setText(book.getTitle());
    m_authorEdit->setText(book.getAuthor());
    m_genreEdit->setText(book.getGenre());
    m_yearSpinBox->setValue(book.getYear());
    
    // Set status
    switch (book.getStatus()) {
        case ReadingStatus::None:
            m_statusCombo->setCurrentIndex(0);
            break;
        case ReadingStatus::Reading:
            m_statusCombo->setCurrentIndex(1);
            break;
        case ReadingStatus::Read:
            m_statusCombo->setCurrentIndex(2);
            break;
        case ReadingStatus::PlanToRead:
            m_statusCombo->setCurrentIndex(3);
            break;
    }
    
    // Set tags and extra fields
    m_tagsEdit->setText(book.getTags().join(", "));
    m_ratingSpin->setValue(book.getRating());
    m_coverPathEdit->setText(book.getCoverPath());
    m_reviewEdit->setPlainText(book.getReview());
}

Book BookDialog::getBook() const {
    Book book;
    book.setTitle(m_titleEdit->text().trimmed());
    book.setAuthor(m_authorEdit->text().trimmed());
    book.setGenre(m_genreEdit->text().trimmed());
    book.setYear(m_yearSpinBox->value());
    
    // Get status
    ReadingStatus status;
    switch (m_statusCombo->currentIndex()) {
        case 0: status = ReadingStatus::None; break;
        case 1: status = ReadingStatus::Reading; break;
        case 2: status = ReadingStatus::Read; break;
        case 3: status = ReadingStatus::PlanToRead; break;
        default: status = ReadingStatus::None;
    }
    book.setStatus(status);
    
    // Get tags
    QString tagsText = m_tagsEdit->text().trimmed();
    if (!tagsText.isEmpty()) {
        QStringList tags = tagsText.split(",");
        for (QString& tag : tags) {
            tag = tag.trimmed();
        }
        book.setTags(tags);
    }
    
    // Extra fields
    book.setRating(m_ratingSpin->value());
    book.setCoverPath(m_coverPathEdit->text().trimmed());
    book.setReview(m_reviewEdit->toPlainText().trimmed());
    
    return book;
}
