#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTabWidget>
#include <QIcon>
#include <QFrame>
#include <QMenu>
#include "librarymanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    LibraryManager* m_library;
    
    // Main widgets
    QTabWidget* m_tabWidget;
    
    // All books tab
    QListWidget* m_booksList; // карточки книг
    QTableWidget* m_booksTable; // таблица книг
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_refreshButton;
    QPushButton* m_sortDropdownButton; // круглая кнопка со стрелкой
    QPushButton* m_viewModeButton; // переключатель режима отображения
    
    // Search section
    QLineEdit* m_searchLine;
    QComboBox* m_searchTypeCombo;
    QPushButton* m_searchButton; // круглая кнопка-лупа
    QPushButton* m_clearSearchButton;
    
    // View mode
    bool m_isTableView; // true = таблица, false = карточки
    
    // Sort section
    QComboBox* m_sortCombo;
    QPushButton* m_sortButton; // не отображается, используется программно
    QMenu* m_sortMenu;
    
    // Lists tabs
    QTableWidget* m_readBooksTable;
    QTableWidget* m_planToReadTable;
    QTableWidget* m_currentlyReadingTable;
    
    // Statistics
    QListWidget* m_topBooksList; // TOP 5 книг
    QLabel* m_totalBooksLabel;
    QLabel* m_readBooksLabel;
    QLabel* m_planToReadLabel;
    
    // Import/Export
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
    
    // Navigation buttons
    QPushButton* m_libIconMain; // кнопка библиотеки на главной странице
    QPushButton* m_statsIconMain; // кнопка статистики на главной странице
    QPushButton* m_libIconStats; // кнопка библиотеки на странице статистики
    QPushButton* m_statsIconStats; // кнопка статистики на странице статистики

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddBook();
    void onEditBook();
    void onDeleteBook();
    void onSearch();
    void onClearSearch();
    void onSort();
    void onExport();
    void onImport();
    void onLibraryChanged();
    void onCardActivated(QListWidgetItem* item);
    void onViewModeChanged();
    void onTableItemActivated(int row, int column);
    void onTabChanged(int index);
    void updateTabButtons();

private:
    void setupUI();
    void createMenuBar();
    void createMainTab();
    void createListsTabs();
    void createStatisticsPanel();
    void updateBooksTable(const QList<Book>& books); // теперь обновляет карточки
    void updateStatistics();
    void updateAllLists();
    void fillTableWithBooks(QTableWidget* table, const QList<Book>& books);
    QIcon makeCoverIcon(const Book& book) const;
};

#endif // MAINWINDOW_H
