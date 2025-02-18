#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QTableWidget>

class HelpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget* parent = nullptr);

private:
    QTableWidget* tableWidget;
    void populateTable() const;
};

#endif // HELPWINDOW_H
