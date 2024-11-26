#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCodeEditor>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QString>
#include <QVector>

using std::vector;

class QComboBox;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void saveFile();
    void openFile();

private:
    void loadStyle(const QString& path);
    QString loadCode(const QString& path);
    void initData();
    void createWidgets();
    QWidget* createRegisterPane();
    QWidget* createMemoryPane();
    void updateRegisterWithFormat(const QString& format);
    void updateMemoryWithFormat(const QString& format);
    void setupWidgets();
    void performConnections();
    void increaseFontSize();
    void decreaseFontSize();
    void wheelEvent(QWheelEvent* event) override;
    vector<uint32_t> generateMemoryData();
    void ensureMapCapacity(vector<QLineEdit*>& map, const vector<uint32_t>& data);

    QHBoxLayout* m_setupLayout;
    QComboBox* m_themeCombobox;
    QCodeEditor* m_codeEditor;
    QCompleter* m_completer;
    QStyleSyntaxHighlighter* m_highlighter;
    QVector<QPair<QString, QSyntaxStyle*>> m_themes;
    QAction* m_saveAction;
    QAction* m_openAction;
    vector<uint32_t> m_registerData;
    vector<QLineEdit*> m_registerMap;
    QLineEdit* m_pcValue;
    QComboBox* m_registerFormatComboBox;
    vector<uint32_t> m_memoryData;
    vector<QLineEdit*> m_memoryMap;
    QComboBox* m_memoryFormatComboBox;
    QFont* m_monoFont = new QFont("Courier", 11); // Monospace font for register values
};


#endif // MAINWINDOW_H
