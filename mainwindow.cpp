#include "mainwindow.h"
#include "bookdialog.h"
#include "bookdetailsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QListWidget>
#include <QPixmap>
#include <QPainter>
#include <QFileInfo>
#include <QApplication>
#include <QCursor>
#include <QTabBar>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_library(new LibraryManager(this)), m_isTableView(false) {
    
    setupUI();
    
    connect(m_library, &LibraryManager::libraryChanged, 
            this, &MainWindow::onLibraryChanged);
    
    setWindowTitle("Персональная библиотека книг");
    resize(1100, 750);
    
    // Автоматическая загрузка тестовой библиотеки при запуске
    QString testFile = "тест1.json";
    if (QFile::exists(testFile)) {
        m_library->importFromJson(testFile);
    }
    
    onLibraryChanged();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // App stylesheet (dark background, rounded panels, colored buttons)
    QString ss = R"(
        QWidget { background: #2f2f2f; }
        QMainWindow { background: #2f2f2f; }
        QWidget#Panel { background: #dcdcdc; border-radius: 18px; border: none; }
        QLabel { color: #212121; background: transparent; }
        QLineEdit, QComboBox, QSpinBox, QTextEdit { color: #212121; }
        QLineEdit { background: #dcdcdc; border: 2px solid #bdbdbd; border-radius: 4px; padding: 6px; }
        QComboBox { background: #dcdcdc; border: 2px solid #bdbdbd; border-radius: 4px; padding: 6px; }
        QComboBox::drop-down { border: none; background: transparent; }
        QComboBox QAbstractItemView { background: #dcdcdc; color: #212121; selection-background-color: #bdbdbd; border: 1px solid #bdbdbd; }
        QMenu { background: #dcdcdc; color: #212121; border: 1px solid #bdbdbd; }
        QMenu::item { background: transparent; padding: 8px 24px; }
        QMenu::item:selected { background: #bdbdbd; }
        QSpinBox { background: #dcdcdc; border: 2px solid #bdbdbd; border-radius: 4px; padding: 6px; }
        QSpinBox::up-button, QSpinBox::down-button { background: transparent; border: none; }
        QTextEdit { background: #dcdcdc; border: 2px solid #bdbdbd; border-radius: 4px; padding: 6px; }
        QGroupBox { background: #dcdcdc; border: 2px solid #bdbdbd; border-radius: 4px; padding-top: 28px; margin-top: 8px; color: #212121; }
        QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 8px; color: #212121; }
        QPushButton { background: #757575; color: white; border-radius: 6px; padding: 6px 16px; border: none; }
        QPushButton:hover { opacity: 0.9; }
        QPushButton.round { width: 40px; height: 40px; border-radius: 20px; border: none; padding: 0; }
        QPushButton.round.green { background: #66BB6A; }
        QPushButton.round.blue { background: #42A5F5; }
        QPushButton.round.red { background: #EF5350; }
        QPushButton.round.yellow { background: #FFEE58; }
        QPushButton.round.gray { background: #E0E0E0; }
        QPushButton.round.active { background: #42A5F5; }
        QPushButton.big { min-height: 44px; border-radius: 10px; color: white; font-weight: 600; padding: 8px 22px; }
        QPushButton.big.blue { background: #2E6BD1; }
        QPushButton.big.green { background: #43A047; }
        QListWidget { background: transparent; border: 0; }
        QTableWidget { background: #dcdcdc; color: #212121; border: none; gridline-color: #bdbdbd; }
        QTableWidget::item { background: #dcdcdc; color: #212121; padding: 4px; }
        QTableWidget::item:selected { background: #bdbdbd; color: #212121; }
        QTableWidget::item:alternate { background: #e8e8e8; }
        QTableWidget::item:alternate:selected { background: #bdbdbd; }
        QHeaderView::section { background: #dcdcdc; color: #212121; padding: 8px; border: 1px solid #bdbdbd; font-weight: 600; }
        QDialogButtonBox QPushButton { min-height: 32px; min-width: 80px; border-radius: 6px; color: white; font-weight: 500; padding: 6px 16px; border: none; background: #757575; }
        QDialogButtonBox QPushButton[text*="OK"], QDialogButtonBox QPushButton[text*="ОК"] { background: #43A047; }
        QDialogButtonBox QPushButton[text*="Cancel"], QDialogButtonBox QPushButton[text*="Отмена"] { background: #757575; }
        QDialogButtonBox QPushButton:hover { opacity: 0.9; }
        QMessageBox { background: white; color: #212121; }
        QMessageBox QLabel { color: #212121; background: white; }
        QMessageBox QPushButton { min-height: 32px; min-width: 80px; border-radius: 6px; color: white; font-weight: 500; padding: 6px 16px; border: none; }
        QMessageBox QPushButton[text*="OK"], QMessageBox QPushButton[text*="ОК"] { background: #43A047; }
        QMessageBox QPushButton[text*="Yes"], QMessageBox QPushButton[text*="Да"] { background: #43A047; }
        QMessageBox QPushButton[text*="No"], QMessageBox QPushButton[text*="Нет"] { background: #757575; }
        QMessageBox QPushButton[text*="Cancel"], QMessageBox QPushButton[text*="Отмена"] { background: #757575; }
    )";
    qApp->setStyleSheet(ss);

// Панель меню скрыта для чистого макета (кнопки экспорта/импорта есть на Stat)
    // createMenuBar();
    
    m_tabWidget = new QTabWidget(this);
    
    createMainTab();
    createListsTabs();
    createStatisticsPanel();
    
mainLayout->addWidget(m_tabWidget);

    // Прячем системную полосу вкладок — переключение через иконки
    if (m_tabWidget->tabBar()) m_tabWidget->tabBar()->hide();
    
    // Подключаем сигнал изменения вкладки для подсветки кнопок
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    // Инициализируем подсветку для текущей вкладки
    updateTabButtons();
    
    setCentralWidget(centralWidget);
statusBar()->hide();
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
    
    // Верхняя строка: только кнопки переключения страниц слева
    QHBoxLayout* topRow = new QHBoxLayout();
    m_libIconMain = new QPushButton(); m_libIconMain->setFlat(true); m_libIconMain->setProperty("class","round gray"); m_libIconMain->setIcon(QIcon(":/icons/library")); m_libIconMain->setIconSize(QSize(20,20));
    m_statsIconMain = new QPushButton(); m_statsIconMain->setFlat(true); m_statsIconMain->setProperty("class","round gray"); m_statsIconMain->setIcon(QIcon(":/icons/stat")); m_statsIconMain->setIconSize(QSize(20,20));
    topRow->addWidget(m_libIconMain); topRow->addWidget(m_statsIconMain); topRow->addStretch();

    // Строка поиска (на всю ширину блока с книгами)
    m_searchLine = new QLineEdit();
    m_searchLine->setPlaceholderText("Поиск по названию, автору, жанру, году, статусу или тегам...");

    m_searchTypeCombo = new QComboBox();
    m_searchTypeCombo->addItems({"По названию", "По автору", "По жанру", "По тегам"});
    m_searchTypeCombo->setCurrentIndex(0); // По умолчанию поиск по названию
    m_searchTypeCombo->setVisible(false); // Скрыт, используется для логики

    // Навигация между страницами из main page
    connect(m_statsIconMain, &QPushButton::clicked, [this]() { if (m_tabWidget->count()>0) m_tabWidget->setCurrentIndex(m_tabWidget->count()-1); });
    connect(m_libIconMain,  &QPushButton::clicked, [this]() { m_tabWidget->setCurrentIndex(0); });

    m_sortDropdownButton = new QPushButton();
    m_sortDropdownButton->setToolTip("Сортировка");
    m_sortDropdownButton->setProperty("class", "round gray");
    m_sortDropdownButton->setIcon(QIcon(":/icons/sort"));
    m_sortDropdownButton->setIconSize(QSize(20,20));

    m_searchButton = new QPushButton();
    m_searchButton->setToolTip("Поиск");
    m_searchButton->setProperty("class", "round gray");
    m_searchButton->setIcon(QIcon(":/icons/search"));
    m_searchButton->setIconSize(QSize(20,20));

    // Кнопка переключения режима отображения
    m_viewModeButton = new QPushButton();
    m_viewModeButton->setToolTip("Переключить режим отображения");
    m_viewModeButton->setProperty("class", "round gray");
    m_viewModeButton->setText("📋"); // Иконка таблицы
    m_viewModeButton->setIconSize(QSize(20,20));
    m_isTableView = false;

    // Сортировка через меню (по макету — стрелка раскрытия)
    m_sortCombo = new QComboBox();
    m_sortCombo->addItems({"По названию", "По автору", "По году", "По жанру"});
    m_sortCombo->setVisible(false);
    m_sortMenu = new QMenu(this);
    for (int i = 0; i < m_sortCombo->count(); ++i) {
        QAction* act = m_sortMenu->addAction(m_sortCombo->itemText(i));
        connect(act, &QAction::triggered, this, [this, i]() {
            m_sortCombo->setCurrentIndex(i);
            onSort();
        });
    }
    connect(m_sortDropdownButton, &QPushButton::clicked, [this]() {
        m_sortMenu->exec(QCursor::pos());
    });

    // Левая колонка круглых кнопок действий
    QVBoxLayout* leftButtons = new QVBoxLayout();
    leftButtons->setSpacing(16);
m_addButton = new QPushButton(); m_addButton->setToolTip("Добавить книгу"); m_addButton->setProperty("class","round green"); m_addButton->setIcon(QIcon(":/icons/add"));
m_editButton = new QPushButton(); m_editButton->setToolTip("Редактировать"); m_editButton->setProperty("class","round blue"); m_editButton->setIcon(QIcon(":/icons/edit"));
m_deleteButton = new QPushButton(); m_deleteButton->setToolTip("Удалить"); m_deleteButton->setProperty("class","round red"); m_deleteButton->setIcon(QIcon(":/icons/delete"));
m_refreshButton = new QPushButton(); m_refreshButton->setToolTip("Обновить"); m_refreshButton->setProperty("class","round yellow"); m_refreshButton->setIcon(QIcon(":/icons/refresh"));
    leftButtons->addWidget(m_addButton); leftButtons->addWidget(m_editButton); leftButtons->addWidget(m_deleteButton); leftButtons->addWidget(m_refreshButton); leftButtons->addStretch();

    // Центральная панель c карточками в светлом блоке с радиусами
    QFrame* cardsPanel = new QFrame();
    cardsPanel->setObjectName("Panel");
    QVBoxLayout* cardsLayout = new QVBoxLayout(cardsPanel);

    m_booksList = new QListWidget();
    m_booksList->setViewMode(QListView::IconMode);
    m_booksList->setIconSize(QSize(128, 180));
    m_booksList->setGridSize(QSize(160, 220));
    m_booksList->setResizeMode(QListView::Adjust);
    m_booksList->setMovement(QListView::Static);
    m_booksList->setSpacing(12);
    m_booksList->setSelectionMode(QAbstractItemView::SingleSelection);

    // Таблица для книг
    m_booksTable = new QTableWidget();
    m_booksTable->setColumnCount(7);
    m_booksTable->setHorizontalHeaderLabels(
        {"Название", "Автор", "Жанр", "Год", "Статус", "Теги", "Оценка"});
    m_booksTable->horizontalHeader()->setStretchLastSection(true);
    m_booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_booksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_booksTable->setAlternatingRowColors(true);
    m_booksTable->setVisible(false); // По умолчанию скрыта

    cardsLayout->addWidget(m_booksList);
    cardsLayout->addWidget(m_booksTable);

    // Правая колонка контента: поиск + панель карточек/таблицы
    QVBoxLayout* rightColumn = new QVBoxLayout();
    
    // Строка поиска на всю ширину
    rightColumn->addWidget(m_searchLine);
    rightColumn->addWidget(cardsPanel, 1);

    // Вычисляем отступ выравнивания по высоте строки поиска
    int alignOffset = m_searchLine->sizeHint().height() + 12;

    // Левая колонка: отступ = высоте строки поиска, затем кнопки действий
    QVBoxLayout* leftColumn = new QVBoxLayout();
    leftColumn->addSpacing(alignOffset);
    leftColumn->addLayout(leftButtons);

    // Компоновка всего таба
    QHBoxLayout* contentRow = new QHBoxLayout();
    contentRow->setContentsMargins(0,0,0,0);
    contentRow->addLayout(leftColumn);
    contentRow->addLayout(rightColumn, 1);

    // Правая вертикальная колонка: кнопка поиска на уровне строки поиска, остальные ниже
    QVBoxLayout* rightSide = new QVBoxLayout();
    // Кнопка поиска без отступа сверху - на уровне строки поиска
    rightSide->addWidget(m_searchButton);
    // Отступ до уровня кнопок действий
    rightSide->addSpacing(alignOffset - m_searchButton->sizeHint().height());
    rightSide->setSpacing(16);
    rightSide->addWidget(m_viewModeButton);
    rightSide->addWidget(m_sortDropdownButton);
    rightSide->addStretch();
    contentRow->addLayout(rightSide);

    layout->addLayout(topRow);
    layout->addLayout(contentRow);

    m_tabWidget->addTab(mainTab, "Все книги");
    
    // Connect signals
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddBook);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::onEditBook);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteBook);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onLibraryChanged);
    connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(m_searchLine, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(m_viewModeButton, &QPushButton::clicked, this, &MainWindow::onViewModeChanged);
    connect(m_booksList, &QListWidget::itemActivated, this, &MainWindow::onCardActivated);
    connect(m_booksTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableItemActivated);
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

    // Светлая панель с TOP 5 и статистикой
    QFrame* panel = new QFrame();
    panel->setObjectName("Panel");
    QVBoxLayout* pLayout = new QVBoxLayout(panel);

    QLabel* topLabel = new QLabel("TOP 5");
    topLabel->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(topLabel);

    // верхняя строка иконок как в макете
    QHBoxLayout* topIcons = new QHBoxLayout();
    m_libIconStats = new QPushButton(); m_libIconStats->setFlat(true); m_libIconStats->setProperty("class","round gray"); m_libIconStats->setIcon(QIcon(":/icons/library")); m_libIconStats->setIconSize(QSize(20,20));
    m_statsIconStats = new QPushButton(); m_statsIconStats->setFlat(true); m_statsIconStats->setProperty("class","round gray"); m_statsIconStats->setIcon(QIcon(":/icons/stat")); m_statsIconStats->setIconSize(QSize(20,20));
    topIcons->addWidget(m_libIconStats); topIcons->addWidget(m_statsIconStats); topIcons->addStretch();
// Навигация между страницами из stat page
    connect(m_libIconStats, &QPushButton::clicked, [this]() { m_tabWidget->setCurrentIndex(0); });
    connect(m_statsIconStats, &QPushButton::clicked, [this, statsTab]() { m_tabWidget->setCurrentWidget(statsTab); });
    pLayout->insertLayout(0, topIcons);

    m_topBooksList = new QListWidget();
    m_topBooksList->setViewMode(QListView::IconMode);
    m_topBooksList->setIconSize(QSize(96, 140));
    m_topBooksList->setGridSize(QSize(130, 180));
    m_topBooksList->setResizeMode(QListView::Adjust);
    m_topBooksList->setMovement(QListView::Static);
    m_topBooksList->setSpacing(12);
    pLayout->addWidget(m_topBooksList);

    // Статистические подписи
    QVBoxLayout* statsLayout = new QVBoxLayout();
    m_totalBooksLabel = new QLabel("Books in library: 0");
    m_readBooksLabel = new QLabel("Read: 0");
    m_planToReadLabel = new QLabel("In plan: 0");
    statsLayout->addWidget(new QLabel("Stats:"));
    statsLayout->addWidget(m_totalBooksLabel);
    statsLayout->addWidget(m_readBooksLabel);
    statsLayout->addWidget(m_planToReadLabel);
    pLayout->addLayout(statsLayout);

    // Крупные кнопки Export/Import внутри панели
    QHBoxLayout* buttons = new QHBoxLayout();
    m_exportButton = new QPushButton("Export library");
    m_exportButton->setProperty("class", "big blue");
    m_importButton = new QPushButton("Import library");
    m_importButton->setProperty("class", "big green");
    buttons->addWidget(m_exportButton);
    buttons->addSpacing(24);
    buttons->addWidget(m_importButton);
    pLayout->addLayout(buttons);

    layout->addWidget(panel, 1); // Расширяем панель

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
    int bookId = 0;
    if (m_isTableView) {
        QList<QTableWidgetItem*> selection = m_booksTable->selectedItems();
        if (selection.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", 
                               "Выберите книгу для редактирования");
            return;
        }
        int row = selection.first()->row();
        QTableWidgetItem* firstItem = m_booksTable->item(row, 0);
        if (!firstItem) return;
        bookId = firstItem->data(Qt::UserRole).toInt();
    } else {
        QList<QListWidgetItem*> selection = m_booksList->selectedItems();
        if (selection.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", 
                               "Выберите книгу для редактирования");
            return;
        }
        bookId = selection.first()->data(Qt::UserRole).toInt();
    }
    
    Book* book = m_library->findBookById(bookId);
    if (book) {
        // Используем тот же диалог, что и для добавления, но в режиме редактирования
        BookDialog dialog(this, *book);
        if (dialog.exec() == QDialog::Accepted) {
            Book updated = dialog.getBook();
            // id уже сохранён в BookDialog::getBook, но на всякий случай явно выставим
            updated.setId(bookId);
            m_library->updateBook(updated);
            statusBar()->showMessage("Изменения сохранены", 3000);
        }
    }
}

void MainWindow::onDeleteBook() {
    int bookId = 0;
    QString title;
    
    if (m_isTableView) {
        QList<QTableWidgetItem*> selection = m_booksTable->selectedItems();
        if (selection.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", 
                               "Выберите книгу для удаления");
            return;
        }
        int row = selection.first()->row();
        QTableWidgetItem* firstItem = m_booksTable->item(row, 0);
        if (!firstItem) return;
        bookId = firstItem->data(Qt::UserRole).toInt();
        title = firstItem->text();
    } else {
        QList<QListWidgetItem*> selection = m_booksList->selectedItems();
        if (selection.isEmpty()) {
            QMessageBox::warning(this, "Предупреждение", 
                               "Выберите книгу для удаления");
            return;
        }
        bookId = selection.first()->data(Qt::UserRole).toInt();
        title = selection.first()->text();
    }
    
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
        // Если строка поиска пуста, показываем все книги
        updateBooksTable(m_library->getAllBooks());
        statusBar()->showMessage("Показаны все книги", 3000);
        return;
    }
    
    // Поиск по всем полям одновременно (название, автор, жанр, год, статус, теги)
    QList<Book> allBooks = m_library->getAllBooks();
    QList<Book> results;
    QString searchTerm = searchText.toLower();
    
    for (const Book& book : allBooks) {
        bool matches = false;
        
        // Поиск по названию
        if (book.getTitle().toLower().contains(searchTerm)) {
            matches = true;
        }
        // Поиск по автору
        else if (book.getAuthor().toLower().contains(searchTerm)) {
            matches = true;
        }
        // Поиск по жанру
        else if (book.getGenre().toLower().contains(searchTerm)) {
            matches = true;
        }
        // Поиск по году издания
        else if (QString::number(book.getYear()).contains(searchTerm)) {
            matches = true;
        }
        // Поиск по статусу
        else if (Book::statusToString(book.getStatus()).toLower().contains(searchTerm)) {
            matches = true;
        }
        // Поиск по тегам
        else {
            for (const QString& tag : book.getTags()) {
                if (tag.toLower().contains(searchTerm)) {
                    matches = true;
                    break;
                }
            }
        }
        
        if (matches) {
            results.append(book);
        }
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

void MainWindow::onCardActivated(QListWidgetItem* item) {
    if (!item) return;
    int bookId = item->data(Qt::UserRole).toInt();
    Book* book = m_library->findBookById(bookId);
    if (!book) return;
    BookDetailsDialog dlg(this, *book);
    if (dlg.exec() == QDialog::Accepted) {
        Book updated = dlg.getUpdatedBook();
        updated.setId(bookId);
        m_library->updateBook(updated);
    }
}

void MainWindow::updateBooksTable(const QList<Book>& books) {
    if (m_isTableView) {
        // Обновляем таблицу
        fillTableWithBooks(m_booksTable, books);
    } else {
        // Обновляем карточки
        m_booksList->clear();
        for (const Book& b : books) {
            QListWidgetItem* it = new QListWidgetItem(makeCoverIcon(b), b.getTitle());
            it->setData(Qt::UserRole, b.getId());
            it->setToolTip(QString("%1\n%2 (%3, %4)\n%5")
                           .arg(b.getTitle())
                           .arg(b.getAuthor())
                           .arg(b.getGenre())
                           .arg(b.getYear())
                           .arg(b.getTags().join(", ")));
            m_booksList->addItem(it);
        }
    }
}

void MainWindow::updateStatistics() {
    m_totalBooksLabel->setText(
        QString("Всего книг: %1").arg(m_library->getTotalBookCount()));
    m_readBooksLabel->setText(
        QString("Прочитано: %1").arg(m_library->getReadBookCount()));
    m_planToReadLabel->setText(
        QString("Планирую прочитать: %1").arg(m_library->getPlanToReadCount()));
    
    // Обновляем TOP 5 книг
    QList<Book> topBooks = m_library->getTopRatedBooks(5);
    m_topBooksList->clear();
    for (const Book& b : topBooks) {
        QListWidgetItem* it = new QListWidgetItem(makeCoverIcon(b), b.getTitle());
        it->setData(Qt::UserRole, b.getId());
        it->setToolTip(QString("%1\n%2 (%3, %4)\nОценка: %5\n%6")
                       .arg(b.getTitle())
                       .arg(b.getAuthor())
                       .arg(b.getGenre())
                       .arg(b.getYear())
                       .arg(b.getRating())
                       .arg(b.getTags().join(", ")));
        m_topBooksList->addItem(it);
    }
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
        // Проверяем, это основная таблица книг (без ID) или таблицы списков (с ID)
        if (table == m_booksTable) {
            // Основная таблица - без ID
            table->setItem(row, 0, new QTableWidgetItem(book.getTitle()));
            table->setItem(row, 1, new QTableWidgetItem(book.getAuthor()));
            table->setItem(row, 2, new QTableWidgetItem(book.getGenre()));
            table->setItem(row, 3, new QTableWidgetItem(QString::number(book.getYear())));
            table->setItem(row, 4, new QTableWidgetItem(Book::statusToString(book.getStatus())));
            table->setItem(row, 5, new QTableWidgetItem(book.getTags().join(", ")));
            table->setItem(row, 6, new QTableWidgetItem(book.getRating() > 0 ? QString::number(book.getRating()) : "-"));
            // Сохраняем ID в UserRole для доступа
            QTableWidgetItem* firstItem = table->item(row, 0);
            if (firstItem) {
                firstItem->setData(Qt::UserRole, book.getId());
            }
        } else {
            // Таблицы списков - с ID
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
}

QIcon MainWindow::makeCoverIcon(const Book& book) const {
    const int w = 128, h = 180;
    QPixmap canvas(w, h);
    QString path = book.getCoverPath();
    if (!path.isEmpty()) {
        QPixmap src(path);
        if (!src.isNull()) {
            QPixmap scaled = src.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            canvas.fill(QColor(255, 255, 255));
            QPainter p(&canvas);
            const int x = (w - scaled.width()) / 2;
            const int y = (h - scaled.height()) / 2;
            p.drawPixmap(x, y, scaled);
            p.end();
            return QIcon(canvas);
        }
    }
    canvas.fill(QColor(230,230,230));
    QPainter p(&canvas);
    p.setPen(Qt::darkGray);
    p.drawRect(0,0,w-1,h-1);
    p.drawText(canvas.rect(), Qt::AlignCenter|Qt::TextWordWrap, "Нет обложки");
    p.end();
    return QIcon(canvas);
}

void MainWindow::onViewModeChanged() {
    m_isTableView = !m_isTableView;
    
    if (m_isTableView) {
        m_booksList->setVisible(false);
        m_booksTable->setVisible(true);
        m_viewModeButton->setText("🖼️"); // Иконка карточек
        m_viewModeButton->setToolTip("Переключить на карточки");
    } else {
        m_booksList->setVisible(true);
        m_booksTable->setVisible(false);
        m_viewModeButton->setText("📋"); // Иконка таблицы
        m_viewModeButton->setToolTip("Переключить на таблицу");
    }
    
    // Обновляем отображение
    onLibraryChanged();
}

void MainWindow::onTableItemActivated(int row, int column) {
    Q_UNUSED(column);
    if (row < 0 || row >= m_booksTable->rowCount()) return;
    // Получаем ID из UserRole первого элемента строки
    QTableWidgetItem* firstItem = m_booksTable->item(row, 0);
    if (!firstItem) return;
    int bookId = firstItem->data(Qt::UserRole).toInt();
    Book* book = m_library->findBookById(bookId);
    if (!book) return;
    BookDetailsDialog dlg(this, *book);
    if (dlg.exec() == QDialog::Accepted) {
        Book updated = dlg.getUpdatedBook();
        updated.setId(bookId);
        m_library->updateBook(updated);
    }
}

void MainWindow::onTabChanged(int index) {
    Q_UNUSED(index);
    updateTabButtons();
}

void MainWindow::updateTabButtons() {
    int currentIndex = m_tabWidget->currentIndex();
    int statsIndex = m_tabWidget->count() - 1; // Последняя вкладка - статистика
    
    // Обновляем кнопки на главной странице
    if (currentIndex == 0) {
        // Главная страница активна
        m_libIconMain->setProperty("class", "round active");
        m_statsIconMain->setProperty("class", "round gray");
    } else {
        m_libIconMain->setProperty("class", "round gray");
        if (currentIndex == statsIndex) {
            m_statsIconMain->setProperty("class", "round active");
        } else {
            m_statsIconMain->setProperty("class", "round gray");
        }
    }
    
    // Обновляем кнопки на странице статистики
    if (currentIndex == statsIndex) {
        // Страница статистики активна
        m_statsIconStats->setProperty("class", "round active");
        m_libIconStats->setProperty("class", "round gray");
    } else {
        m_statsIconStats->setProperty("class", "round gray");
        if (currentIndex == 0) {
            m_libIconStats->setProperty("class", "round active");
        } else {
            m_libIconStats->setProperty("class", "round gray");
        }
    }
    
    // Применяем стили
    m_libIconMain->style()->unpolish(m_libIconMain);
    m_libIconMain->style()->polish(m_libIconMain);
    m_statsIconMain->style()->unpolish(m_statsIconMain);
    m_statsIconMain->style()->polish(m_statsIconMain);
    m_libIconStats->style()->unpolish(m_libIconStats);
    m_libIconStats->style()->polish(m_libIconStats);
    m_statsIconStats->style()->unpolish(m_statsIconStats);
    m_statsIconStats->style()->polish(m_statsIconStats);
}
