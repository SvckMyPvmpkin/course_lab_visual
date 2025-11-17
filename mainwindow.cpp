#include "mainwindow.h"
#include "bookdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_library(new LibraryManager(this)) {
    
    setupUI();
    
    connect(m_library, &LibraryManager::libraryChanged, 
            this, &MainWindow::onLibraryChanged);
    
    setWindowTitle("Персональная библиотека книг");
    resize(1000, 700);
    
    onLibraryChanged();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    createMenuBar();
    
    m_tabWidget = new QTabWidget(this);
    
    createMainTab();
    createListsTabs();
    createStatisticsPanel();
    
    mainLayout->addWidget(m_tabWidget);
    
    setCentralWidget(centralWidget);
    statusBar()->showMessage("Готов");
}

void MainWindow::createMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("&Файл");
    
    QAction* importAction = new QAction("&Импорт библиотеки", this);
    importAction->setShortcut(QKeySequence("Ctrl+I"));
    connect(importAction, &QAction::triggered, this, &MainWindow::onImport);
    
    QAction* exportAction = new QAction("&Экспорт библиотеки", this);
    exportAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExport);
    
    QAction* exitAction = new QAction("&Выход", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    fileMenu->addAction(importAction);
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    
    QMenu* helpMenu = menuBar()->addMenu("&Справка");
    QAction* aboutAction = new QAction("&О программе", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "О программе",
            "Персональная библиотека книг\n\n"
            "Курсовая работа по ООП\n"
            "Паттерны: Strategy, Composite\n\n"
            "Версия 1.0");
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::createMainTab() {
    QWidget* mainTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(mainTab);
    
    // Search section
    QGroupBox* searchGroup = new QGroupBox("Поиск");
    QHBoxLayout* searchLayout = new QHBoxLayout();
    
    m_searchLine = new QLineEdit();
    m_searchLine->setPlaceholderText("Введите текст для поиска...");
    
    m_searchTypeCombo = new QComboBox();
    m_searchTypeCombo->addItems({"По названию", "По автору", "По жанру", "По тегам"});
    
    m_searchButton = new QPushButton("Найти");
    m_clearSearchButton = new QPushButton("Очистить");
    
    searchLayout->addWidget(m_searchLine);
    searchLayout->addWidget(m_searchTypeCombo);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addWidget(m_clearSearchButton);
    searchGroup->setLayout(searchLayout);
    
    // Sort section
    QGroupBox* sortGroup = new QGroupBox("Сортировка");
    QHBoxLayout* sortLayout = new QHBoxLayout();
    
    m_sortCombo = new QComboBox();
    m_sortCombo->addItems({"По названию", "По автору", "По году", "По жанру"});
    
    m_sortButton = new QPushButton("Сортировать");
    
    sortLayout->addWidget(new QLabel("Сортировать:"));
    sortLayout->addWidget(m_sortCombo);
    sortLayout->addWidget(m_sortButton);
    sortLayout->addStretch();
    sortGroup->setLayout(sortLayout);
    
    // Buttons section
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Добавить книгу");
    m_editButton = new QPushButton("Редактировать");
    m_deleteButton = new QPushButton("Удалить");
    m_refreshButton = new QPushButton("Обновить");
    
    buttonsLayout->addWidget(m_addButton);
    buttonsLayout->addWidget(m_editButton);
    buttonsLayout->addWidget(m_deleteButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_refreshButton);
    
    // Table
    m_booksTable = new QTableWidget();
    m_booksTable->setColumnCount(8);
    m_booksTable->setHorizontalHeaderLabels(
        {"ID", "Название", "Автор", "Жанр", "Год", "Статус", "Теги", "Оценка"});
    m_booksTable->horizontalHeader()->setStretchLastSection(true);
    m_booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_booksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_booksTable->setAlternatingRowColors(true);
    
    layout->addWidget(searchGroup);
    layout->addWidget(sortGroup);
    layout->addLayout(buttonsLayout);
    layout->addWidget(m_booksTable);
    
    m_tabWidget->addTab(mainTab, "Все книги");
    
    // Connect signals
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddBook);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::onEditBook);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteBook);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onLibraryChanged);
    connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(m_clearSearchButton, &QPushButton::clicked, this, &MainWindow::onClearSearch);
    connect(m_sortButton, &QPushButton::clicked, this, &MainWindow::onSort);
    connect(m_booksTable, &QTableWidget::cellDoubleClicked, 
            this, &MainWindow::onTableDoubleClick);
}

void MainWindow::createListsTabs() {
    // Read books tab
    QWidget* readTab = new QWidget();
    QVBoxLayout* readLayout = new QVBoxLayout(readTab);
    m_readBooksTable = new QTableWidget();
    m_readBooksTable->setColumnCount(8);
    m_readBooksTable->setHorizontalHeaderLabels(
        {"ID", "Название", "Автор", "Жанр", "Год", "Статус", "Теги", "Оценка"});
    m_readBooksTable->horizontalHeader()->setStretchLastSection(true);
    m_readBooksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_readBooksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_readBooksTable->setAlternatingRowColors(true);
    readLayout->addWidget(m_readBooksTable);
    m_tabWidget->addTab(readTab, "Прочитанные");
    
    // Plan to read tab
    QWidget* planTab = new QWidget();
    QVBoxLayout* planLayout = new QVBoxLayout(planTab);
    m_planToReadTable = new QTableWidget();
    m_planToReadTable->setColumnCount(8);
    m_planToReadTable->setHorizontalHeaderLabels(
        {"ID", "Название", "Автор", "Жанр", "Год", "Статус", "Теги", "Оценка"});
    m_planToReadTable->horizontalHeader()->setStretchLastSection(true);
    m_planToReadTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_planToReadTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_planToReadTable->setAlternatingRowColors(true);
    planLayout->addWidget(m_planToReadTable);
    m_tabWidget->addTab(planTab, "Планирую прочитать");
    
    // Currently reading tab
    QWidget* currentTab = new QWidget();
    QVBoxLayout* currentLayout = new QVBoxLayout(currentTab);
    m_currentlyReadingTable = new QTableWidget();
    m_currentlyReadingTable->setColumnCount(8);
    m_currentlyReadingTable->setHorizontalHeaderLabels(
        {"ID", "Название", "Автор", "Жанр", "Год", "Статус", "Теги", "Оценка"});
    m_currentlyReadingTable->horizontalHeader()->setStretchLastSection(true);
    m_currentlyReadingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_currentlyReadingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_currentlyReadingTable->setAlternatingRowColors(true);
    currentLayout->addWidget(m_currentlyReadingTable);
    m_tabWidget->addTab(currentTab, "Читаю сейчас");
}

void MainWindow::createStatisticsPanel() {
    QWidget* statsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(statsTab);
    
    QGroupBox* statsGroup = new QGroupBox("Статистика библиотеки");
    QVBoxLayout* statsLayout = new QVBoxLayout();
    
    m_totalBooksLabel = new QLabel("Всего книг: 0");
    m_readBooksLabel = new QLabel("Прочитано: 0");
    m_planToReadLabel = new QLabel("Планирую прочитать: 0");
    
    QFont font = m_totalBooksLabel->font();
    font.setPointSize(12);
    m_totalBooksLabel->setFont(font);
    m_readBooksLabel->setFont(font);
    m_planToReadLabel->setFont(font);
    
    statsLayout->addWidget(m_totalBooksLabel);
    statsLayout->addWidget(m_readBooksLabel);
    statsLayout->addWidget(m_planToReadLabel);
    statsLayout->addStretch();
    
    statsGroup->setLayout(statsLayout);
    layout->addWidget(statsGroup);
    
    // Export/Import section
    QGroupBox* dataGroup = new QGroupBox("Импорт/Экспорт");
    QVBoxLayout* dataLayout = new QVBoxLayout();
    
    m_exportButton = new QPushButton("Экспортировать библиотеку");
    m_importButton = new QPushButton("Импортировать библиотеку");
    
    dataLayout->addWidget(m_exportButton);
    dataLayout->addWidget(m_importButton);
    dataGroup->setLayout(dataLayout);
    
    layout->addWidget(dataGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(statsTab, "Статистика");
    
    connect(m_exportButton, &QPushButton::clicked, this, &MainWindow::onExport);
    connect(m_importButton, &QPushButton::clicked, this, &MainWindow::onImport);
}

void MainWindow::onAddBook() {
    BookDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Book book = dialog.getBook();
        m_library->addBook(book);
        statusBar()->showMessage("Книга добавлена", 3000);
    }
}

void MainWindow::onEditBook() {
    int currentRow = m_booksTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Предупреждение", 
                           "Выберите книгу для редактирования");
        return;
    }
    
    int bookId = m_booksTable->item(currentRow, 0)->text().toInt();
    Book* book = m_library->findBookById(bookId);
    
    if (book) {
        BookDialog dialog(this, *book);
        if (dialog.exec() == QDialog::Accepted) {
            Book updatedBook = dialog.getBook();
            updatedBook.setId(bookId);
            m_library->updateBook(updatedBook);
            statusBar()->showMessage("Книга обновлена", 3000);
        }
    }
}

void MainWindow::onDeleteBook() {
    int currentRow = m_booksTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Предупреждение", 
                           "Выберите книгу для удаления");
        return;
    }
    
    int bookId = m_booksTable->item(currentRow, 0)->text().toInt();
    QString title = m_booksTable->item(currentRow, 1)->text();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение", 
        QString("Удалить книгу \"%1\"?").arg(title),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_library->removeBook(bookId);
        statusBar()->showMessage("Книга удалена", 3000);
    }
}

void MainWindow::onSearch() {
    QString searchText = m_searchLine->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::information(this, "Поиск", "Введите текст для поиска");
        return;
    }
    
    QList<Book> results;
    QString searchType = m_searchTypeCombo->currentText();
    
    if (searchType == "По названию") {
        results = m_library->searchByTitle(searchText);
    } else if (searchType == "По автору") {
        results = m_library->searchByAuthor(searchText);
    } else if (searchType == "По жанру") {
        results = m_library->searchByGenre(searchText);
    } else if (searchType == "По тегам") {
        results = m_library->searchByTag(searchText);
    }
    
    updateBooksTable(results);
    statusBar()->showMessage(
        QString("Найдено книг: %1").arg(results.size()), 3000);
}

void MainWindow::onClearSearch() {
    m_searchLine->clear();
    updateBooksTable(m_library->getAllBooks());
    statusBar()->showMessage("Поиск очищен", 3000);
}

void MainWindow::onSort() {
    QString sortType = m_sortCombo->currentText();
    
    if (sortType == "По названию") {
        m_library->setSortStrategy(std::unique_ptr<ISortStrategy>(new SortByTitleStrategy()));
    } else if (sortType == "По автору") {
        m_library->setSortStrategy(std::unique_ptr<ISortStrategy>(new SortByAuthorStrategy()));
    } else if (sortType == "По году") {
        m_library->setSortStrategy(std::unique_ptr<ISortStrategy>(new SortByYearStrategy()));
    } else if (sortType == "По жанру") {
        m_library->setSortStrategy(std::unique_ptr<ISortStrategy>(new SortByGenreStrategy()));
    }
    
    updateBooksTable(m_library->getSortedBooks());
    statusBar()->showMessage(
        QString("Отсортировано: %1").arg(sortType), 3000);
}

void MainWindow::onExport() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Экспорт библиотеки", "", "JSON файлы (*.json)");
    
    if (!filename.isEmpty()) {
        if (m_library->exportToJson(filename)) {
            QMessageBox::information(this, "Экспорт", 
                                   "Библиотека успешно экспортирована");
            statusBar()->showMessage("Экспорт выполнен", 3000);
        } else {
            QMessageBox::critical(this, "Ошибка", 
                                "Не удалось экспортировать библиотеку");
        }
    }
}

void MainWindow::onImport() {
    QString filename = QFileDialog::getOpenFileName(
        this, "Импорт библиотеки", "", "JSON файлы (*.json)");
    
    if (!filename.isEmpty()) {
        if (m_library->importFromJson(filename)) {
            QMessageBox::information(this, "Импорт", 
                                   "Библиотека успешно импортирована");
            statusBar()->showMessage("Импорт выполнен", 3000);
        } else {
            QMessageBox::critical(this, "Ошибка", 
                                "Не удалось импортировать библиотеку");
        }
    }
}

void MainWindow::onLibraryChanged() {
    updateBooksTable(m_library->getAllBooks());
    updateAllLists();
    updateStatistics();
}

void MainWindow::onTableDoubleClick(int row, int column) {
    Q_UNUSED(column);
    if (row >= 0) {
        onEditBook();
    }
}

void MainWindow::updateBooksTable(const QList<Book>& books) {
    fillTableWithBooks(m_booksTable, books);
}

void MainWindow::updateStatistics() {
    m_totalBooksLabel->setText(
        QString("Всего книг: %1").arg(m_library->getTotalBookCount()));
    m_readBooksLabel->setText(
        QString("Прочитано: %1").arg(m_library->getReadBookCount()));
    m_planToReadLabel->setText(
        QString("Планирую прочитать: %1").arg(m_library->getPlanToReadCount()));
}

void MainWindow::updateAllLists() {
    fillTableWithBooks(m_readBooksTable, m_library->getReadBooks());
    fillTableWithBooks(m_planToReadTable, m_library->getPlanToReadBooks());
    fillTableWithBooks(m_currentlyReadingTable, m_library->getCurrentlyReading());
}

void MainWindow::fillTableWithBooks(QTableWidget* table, const QList<Book>& books) {
    table->setRowCount(0);
    
    for (const Book& book : books) {
        int row = table->rowCount();
        table->insertRow(row);
        
        table->setItem(row, 0, new QTableWidgetItem(QString::number(book.getId())));
        table->setItem(row, 1, new QTableWidgetItem(book.getTitle()));
        table->setItem(row, 2, new QTableWidgetItem(book.getAuthor()));
        table->setItem(row, 3, new QTableWidgetItem(book.getGenre()));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(book.getYear())));
        table->setItem(row, 5, new QTableWidgetItem(Book::statusToString(book.getStatus())));
        table->setItem(row, 6, new QTableWidgetItem(book.getTags().join(", ")));
        table->setItem(row, 7, new QTableWidgetItem(book.getRating() > 0 ? QString::number(book.getRating()) : "-"));
    }
}
