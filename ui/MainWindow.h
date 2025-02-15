#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCodeEditor>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMutex>
#include <QString>

#include "../simulator/Simulator.h"
#include "Config.h"
#include "SimulationThread.h"
#include "highlighters/QRiscvAsmHighlighter.h"


using std::string;
using std::vector;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void newFile();
    void saveAsFile();
    void saveFile(bool openDialog = true);
    void openFile();
    void run();
    void step();
    void stop();
    void setSpeed(int speed);
    void setTheme(int index);

private:
    void loadStyle(const QString& path);
    QString loadCode(const QString& path);
    void initData();
    void createWidgets();
    QWidget* createRegisterPanel();
    QWidget* createMemoryPanel();
    void createToolbar();
    void updateRegisterWithFormat(const QString& format) const;
    void updateMemoryWithFormat(const QString& format);
    void setupWidgets();
    void performConnections();
    void setRegisterPanelShown(bool shown) const;
    void setMemoryPanelShown(bool shown) const;
    void setAddressesShown(bool shown) const;
    void increaseFontSize() const;
    void decreaseFontSize() const;
    void wheelEvent(QWheelEvent* event) override;
    void ensureMemoryMapCapacity();
    bool errorPopup(const string& message, bool yesNoButtons = false);
    bool parseAndSetInstructions();
    void reset();
    void setResult(const ExecutionResult& result);
    void executionError(const ExecutionResult& result);
    void executionFinished();
    void highlightMemoryLabel(QLabel* label) const;
    void highlightRegisterLineEdit(QLineEdit* lineEdit) const;
    uint32_t calculateErrorLine(int instruction) const;
    void saveConfig() const;
    void closeEvent(QCloseEvent* event) override;

    QSlider* m_speedSlider;
    QHBoxLayout* m_setupLayout;
    QComboBox* m_themeCombobox;
    QCodeEditor* m_codeEditor;
    QRiscvAsmCompleter* m_completer;
    QRiscvAsmHighlighter* m_highlighter;
    QVector<QPair<QString, QSyntaxStyle*>> m_themes;
    QFile* m_file;
    QAction* m_newFileAction;
    QAction* m_saveAsAction;
    QAction* m_saveAction;
    QAction* m_openAction;
    QAction* m_showMemoryAction;
    QAction* m_showRegistersAction;
    QAction* m_showAddressAction;
    QPushButton* m_runButton;
    QPushButton* m_stepButton;
    QPushButton* m_stopButton;
    QSpacerItem* m_spacer;
    vector<int32_t> m_registerData;
    vector<QLineEdit*> m_registerMap;
    uint32_t m_pcData;
    QLineEdit* m_pcValue;
    QComboBox* m_registerFormatComboBox;
    QVBoxLayout* m_memoryLayout;
    vector<uint32_t> m_memoryData;
    vector<QLabel*> m_memoryMap;
    QComboBox* m_memoryFormatComboBox;
    QFont* m_monoFont; // Monospace font for memory and register values
    QWidget* m_registerPanel;
    QWidget* m_memoryPanel;

    vector<uint32_t>* m_instructionMap;
    bool m_hasStarted;
    Simulator* m_simulator;
    int m_speed;
    SimulationThread* m_simulationThread;

    ConfigData* m_configData;
};


#endif // MAINWINDOW_H
