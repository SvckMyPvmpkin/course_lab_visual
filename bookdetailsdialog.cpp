#include "bookdetailsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QFileInfo>
#include <QGroupBox>
#include <QPainter>
#include <QFrame>

BookDetailsDialog::BookDetailsDialog(QWidget* parent, const Book& book)
    : QDialog(parent), m_book(book) {
    setupUI();
    loadBook(book);
    setWindowTitle("Информация о книге");
    resize(520, 620);
}

void BookDetailsDialog::setupUI() {
    QVBoxLayout* main = new QVBoxLayout(this);

    // Верх: обложка и базовая информация
    QHBoxLayout* top = new QHBoxLayout();
    m_coverLabel = new QLabel();
    m_coverLabel->setFixedSize(160, 220);
    m_coverLabel->setFrameShape(QFrame::StyledPanel);

    QFormLayout* info = new QFormLayout();
    m_titleLabel = new QLabel();
    m_titleLabel->setWordWrap(true);
    m_authorLabel = new QLabel();
    m_genreLabel = new QLabel();
    m_yearLabel = new QLabel();
    m_tagsLabel = new QLabel();
    m_tagsLabel->setWordWrap(true);

    info->addRow("Название:", m_titleLabel);
    info->addRow("Автор:", m_authorLabel);
    info->addRow("Жанр:", m_genreLabel);
    info->addRow("Год:", m_yearLabel);
    info->addRow("Теги:", m_tagsLabel);

    top->addWidget(m_coverLabel);
    top->addLayout(info);
    main->addLayout(top);

    // Редактируемые поля
    QGroupBox* editable = new QGroupBox("Оценка, статус, рецензия");
    QFormLayout* form = new QFormLayout();

    m_ratingSpin = new QSpinBox();
    m_ratingSpin->setRange(0, 10); // 0 — нет оценки

    m_statusCombo = new QComboBox();
    m_statusCombo->addItem("Без статуса");
    m_statusCombo->addItem("Читаю");
    m_statusCombo->addItem("Прочитана");
    m_statusCombo->addItem("Планирую прочитать");

    m_reviewEdit = new QTextEdit();
    m_reviewEdit->setPlaceholderText("Ваша рецензия...");

    form->addRow("Оценка:", m_ratingSpin);
    form->addRow("Статус:", m_statusCombo);
    form->addRow("Рецензия:", m_reviewEdit);

    editable->setLayout(form);
    main->addWidget(editable);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(box, &QDialogButtonBox::accepted, this, [this]() {
        // Записать только разрешенные поля
        m_book.setRating(m_ratingSpin->value());
        m_book.setReview(m_reviewEdit->toPlainText().trimmed());
        ReadingStatus status = ReadingStatus::None;
        switch (m_statusCombo->currentIndex()) {
            case 1: status = ReadingStatus::Reading; break;
            case 2: status = ReadingStatus::Read; break;
            case 3: status = ReadingStatus::PlanToRead; break;
            default: status = ReadingStatus::None; break;
        }
        m_book.setStatus(status);
        accept();
    });
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    main->addWidget(box);
}

void BookDetailsDialog::loadBook(const Book& book) {
    // Обложка
    QPixmap pix(160, 220);
    if (!book.getCoverPath().isEmpty()) {
        QPixmap src(book.getCoverPath());
        if (!src.isNull()) {
            QPixmap scaled = src.scaled(pix.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            const int x = (pix.width() - scaled.width()) / 2;
            const int y = (pix.height() - scaled.height()) / 2;
            p.drawPixmap(x, y, scaled);
            p.end();
        } else {
            pix.fill(QColor(230,230,230));
            QPainter p(&pix);
            p.setPen(Qt::darkGray);
            p.drawRect(0,0,pix.width()-1,pix.height()-1);
            p.drawText(pix.rect(), Qt::AlignCenter|Qt::TextWordWrap, "Нет обложки");
            p.end();
        }
    } else {
        pix.fill(QColor(230,230,230));
        QPainter p(&pix);
        p.setPen(Qt::darkGray);
        p.drawRect(0,0,pix.width()-1,pix.height()-1);
        p.drawText(pix.rect(), Qt::AlignCenter|Qt::TextWordWrap, "Нет обложки");
        p.end();
    }
    m_coverLabel->setPixmap(pix);

    // Нередактируемая информация
    m_titleLabel->setText(book.getTitle());
    m_authorLabel->setText(book.getAuthor());
    m_genreLabel->setText(book.getGenre());
    m_yearLabel->setText(QString::number(book.getYear()));
    m_tagsLabel->setText(book.getTags().join(", "));

    // Разрешенные поля
    m_ratingSpin->setValue(book.getRating());
    switch (book.getStatus()) {
        case ReadingStatus::Reading: m_statusCombo->setCurrentIndex(1); break;
        case ReadingStatus::Read: m_statusCombo->setCurrentIndex(2); break;
        case ReadingStatus::PlanToRead: m_statusCombo->setCurrentIndex(3); break;
        case ReadingStatus::None: default: m_statusCombo->setCurrentIndex(0); break;
    }
    m_reviewEdit->setPlainText(book.getReview());
}
