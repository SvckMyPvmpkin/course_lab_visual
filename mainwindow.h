#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTabWidget>
#include "librarymanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    LibraryManager* m_library;
    
    // Main widgets
    QTabWidget* m_tabWidget;
    
    // All books tab
    QTableWidget* m_booksTable;
    QPushButton* m_addButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_refreshButton;
    
    // Search section
    QLineEdit* m_searchLine;
    QComboBox* m_searchTypeCombo;
    QPushButton* m_searchButton;
    QPushButton* m_clearSearchButton;
    
    // Sort section
    QComboBox* m_sortCombo;
    QPushButton* m_sortButton;
    
    // Lists tabs
    QTableWidget* m_readBooksTable;
    QTableWidget* m_planToReadTable;
    QTableWidget* m_currentlyReadingTable;
    
    // Statistics
    QLabel* m_totalBooksLabel;
    QLabel* m_readBooksLabel;
    QLabel* m_planToReadLabel;
    
    // Import/Export
    QPushButton* m_exportButton;
    QPushButton* m_importButton;

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
    void onTableDoubleClick(int row, int column);

private:
    void setupUI();
    void createMenuBar();
    void createMainTab();
    void createListsTabs();
    void createStatisticsPanel();
    void updateBooksTable(const QList<Book>& books);
    void updateStatistics();
    void updateAllLists();
    void fillTableWithBooks(QTableWidget* table, const QList<Book>& books);
};

#endif // MAINWINDOW_H
