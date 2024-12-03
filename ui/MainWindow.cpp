#include "MainWindow.h"

// QCodeEditor
#include <QCXXHighlighter>
#include <QCodeEditor>
#include <QGLSLHighlighter>
#include <QSyntaxStyle>
#include "QLuaCompleter.hpp"

// Qt
#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QStyleHints>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

#include "../parser/Parser.h"
#include "../parser/ParsingResult.h"
#include "../simulator/CPUUtil.h"
#include "../simulator/Simulator.h"
#include "ErrorParser.h"


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_setupLayout(nullptr), m_themeCombobox(nullptr), m_codeEditor(nullptr), m_completer(nullptr),
    m_highlighter(nullptr), m_file(nullptr), m_saveAction(nullptr), m_openAction(nullptr), m_spacer(nullptr),
    m_pcValue(nullptr), m_registerFormatComboBox(nullptr), m_memoryLayout(nullptr), m_memoryFormatComboBox(nullptr),
    m_monoFont(new QFont("Courier", 11)), m_simulator(nullptr), m_running(false), m_speed(1000)
{
    initData();
    createWidgets();
    setupWidgets();
    performConnections();

    QFont font = QApplication::font();
    font.setPointSize(10);
    QApplication::setFont(font);
}

void MainWindow::initData()
{
    m_highlighter = new QCXXHighlighter;
    m_themes = {{"light", QSyntaxStyle::defaultStyle()}};
    m_completer = new QLuaCompleter(this);
    m_registerMap = vector<QLineEdit*>(32);
    m_memoryLayout = new QVBoxLayout;
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_simulator = new Simulator;
    m_registerData = m_simulator->GetCpuStatus().registers;

    loadStyle(":/styles/drakula.xml");
}

QString MainWindow::loadCode(const QString& path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly)) {
        return QString();
    }

    return fl.readAll();
}

void MainWindow::loadStyle(const QString& path)
{
    QFile fl(path);

    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    auto style = new QSyntaxStyle(this);

    if (!style->load(fl.readAll())) {
        delete style;
        return;
    }

    m_themes.append({style->name(), style});
}

void MainWindow::createWidgets()
{
    const auto menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu("File");
    m_saveAsAction = fileMenu->addAction("Save As");
    m_saveAction = fileMenu->addAction("Save");
    m_openAction = fileMenu->addAction("Open");

    QMenu* viewMenu = menuBar->addMenu("View");
    viewMenu->addAction("Increase Font Size", this, &MainWindow::increaseFontSize);
    viewMenu->addAction("Decrease Font Size", this, &MainWindow::decreaseFontSize);

    createToolbar();

    // Theme selection
    const auto themeWidget = new QWidget(menuBar);
    const auto themeLayout = new QHBoxLayout(themeWidget);
    themeLayout->setContentsMargins(0, 0, 0, 0);
    themeLayout->addWidget(new QLabel("Theme:"));
    m_themeCombobox = new QComboBox();
    themeLayout->addWidget(m_themeCombobox);
    menuBar->setCornerWidget(themeWidget, Qt::TopRightCorner);

    // Layout
    const auto container = new QWidget(this);
    setCentralWidget(container);

    const auto mainLayout = new QHBoxLayout;

    // Registers
    QWidget* memoryPane = createRegisterPane();
    memoryPane->setFixedWidth(330);
    mainLayout->addWidget(memoryPane);

    // CodeEditor
    m_codeEditor = new QCodeEditor(this);
    mainLayout->addWidget(m_codeEditor);

    // Memory
    QWidget* memoryPanel = createMemoryPane();
    memoryPanel->setFixedWidth(300);
    mainLayout->addWidget(memoryPanel);

    container->setLayout(mainLayout);
}

QWidget* MainWindow::createRegisterPane()
{
    const auto registerWidget = new QWidget;
    const auto layout = new QVBoxLayout;

    // Format Selector
    const auto formatLayout = new QHBoxLayout;
    const auto formatLabel = new QLabel("Format:");
    m_registerFormatComboBox = new QComboBox();
    m_registerFormatComboBox->addItems({"Hexadecimal", "Decimal"});
    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(m_registerFormatComboBox);
    layout->addLayout(formatLayout);

    // Scrollable Area for PC and Registers
    const auto scrollArea = new QScrollArea;
    const auto scrollContent = new QWidget;
    const auto scrollLayout = new QVBoxLayout;

    // Program Counter (PC)
    const auto pcLayout = new QHBoxLayout;
    const auto pcLabel = new QLabel("Program Counter (PC):");
    m_pcValue = new QLineEdit("00000000");
    m_pcValue->setFont(*m_monoFont);
    m_pcValue->setReadOnly(true);
    pcLayout->addWidget(pcLabel);
    pcLayout->addWidget(m_pcValue);
    scrollLayout->addLayout(pcLayout);

    // Registers (x0 to x31)
    const auto regLayout = new QGridLayout;

    for (int i = 0; i < 32; i++) {
        auto regName = QString("x%1:").arg(i);
        const auto regLabel = new QLabel(regName);
        const auto regValue = new QLineEdit("00000000");
        regValue->setFont(*m_monoFont);
        regValue->setReadOnly(true); // Registers are non-editable

        // Store reference to the QLineEdit for later updates
        m_registerMap[i] = regValue;

        // Add to grid: two registers per row
        const int row = i / 2;
        const int col = (i % 2) * 2;
        regLayout->addWidget(regLabel, row, col);
        regLayout->addWidget(regValue, row, col + 1);
    }
    scrollLayout->addLayout(regLayout);

    // push content up
    scrollLayout->addSpacerItem(m_spacer);

    // Set up scrollable content
    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);

    registerWidget->setLayout(layout);
    return registerWidget;
}

QWidget* MainWindow::createMemoryPane()
{
    const auto rightPane = new QWidget;
    const auto layout = new QVBoxLayout;

    // Memory Viewer
    const auto formatLayout = new QHBoxLayout;
    const auto formatLabel = new QLabel("Format:");
    m_memoryFormatComboBox = new QComboBox();
    m_memoryFormatComboBox->addItems({"Hexadecimal", "Decimal"});
    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(m_memoryFormatComboBox);
    layout->addLayout(formatLayout);

    // Create a scrollable area for memory content
    const auto scrollArea = new QScrollArea;
    const auto memContentWidget = new QWidget;

    m_memoryData = m_simulator->GetMemory();

    updateMemoryWithFormat(m_registerFormatComboBox->currentText());

    // Set up scrollable content for memory
    memContentWidget->setLayout(m_memoryLayout);
    scrollArea->setWidget(memContentWidget);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);
    rightPane->setLayout(layout);

    return rightPane;
}


void MainWindow::setupWidgets()
{
    setWindowTitle("RISCV-Simulator");

    const QStyleHints* styleHints = QGuiApplication::styleHints();
    const bool isDark = styleHints->colorScheme() == Qt::ColorScheme::Dark;

    // CodeEditor
    m_codeEditor->setSyntaxStyle(isDark ? m_themes[1].second : m_themes[0].second);
    m_codeEditor->setCompleter(m_completer);
    m_codeEditor->setHighlighter(m_highlighter);
    m_codeEditor->setTabReplace(true);
    m_codeEditor->setTabReplaceSize(1);
    m_codeEditor->setAutoIndentation(false);
    m_codeEditor->setAutoParentheses(false);
    m_codeEditor->setWordWrapMode(QTextOption::NoWrap);


    QStringList list;
    // Styles
    for (auto&& el : m_themes) {
        list << el.first;
    }

    m_themeCombobox->addItems(list);
    m_themeCombobox->setCurrentIndex(isDark ? 1 : 0);
    list.clear();
}

void MainWindow::performConnections()
{
    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::run);
    connect(m_stepButton, &QPushButton::clicked, this, &MainWindow::step);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stop);
    connect(new QShortcut(QKeySequence("Ctrl+S"), this), &QShortcut::activated, this, &MainWindow::saveFile);
    connect(m_memoryFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateMemoryWithFormat);
    connect(m_registerFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateRegisterWithFormat);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_themeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](const int index) { m_codeEditor->setSyntaxStyle(m_themes[index].second); });
}

void MainWindow::saveAsFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    if (m_file == nullptr) {
        m_file = new QFile(fileName);
    }
    else {
        m_file->close();
        m_file->setFileName(fileName);
        if (!m_file->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, tr("Unable to open file"), m_file->errorString());
            return;
        }
    }

    QTextStream out(m_file);
    m_file->resize(0);
    out << m_codeEditor->toPlainText();
}

void MainWindow::saveFile()
{
    if (m_file == nullptr) {
        const QString fileName =
            QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
        if (fileName.isEmpty()) {
            return;
        }

        m_file = new QFile(fileName);
        if (!m_file->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, tr("Unable to open file"), m_file->errorString());
            return;
        }
    }

    QTextStream out(m_file);
    m_file->resize(0);
    out << m_codeEditor->toPlainText();
}

void MainWindow::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    if (m_file == nullptr) {
        m_file = new QFile(fileName);
    }
    else {
        m_file->close();
        m_file->setFileName(fileName);
        if (!m_file->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, tr("Unable to open file"), m_file->errorString());
            return;
        }
    }

    QTextStream in(m_file);
    const QString text = in.readAll();
    m_codeEditor->setPlainText(text);
}

void MainWindow::run()
{
    if (m_running) {
        return;
    }
    m_simulator->Reset();
    if (!parseAndSetInstructions()) {
        return;
    }
    m_running = true;

    while (m_running) {
        const ExecutionResult result = m_simulator->Step();
        if (!result.success) {
            m_running = false;
            if (result.error != ExecutionError::NONE && result.error != ExecutionError::PC_OUT_OF_BOUNDS) {
                errorPopup(ErrorParser::ParseError(result.error, result.pc));
            }
            return;
        }
        setResult(result);

        m_mutex.lock();
        m_waitCondition.wait(&m_mutex, m_speed);
        m_mutex.unlock();
    }
}
void MainWindow::step()
{
    if (m_running) {
        return;
    }
    if (!parseAndSetInstructions()) {
        return;
    }

    const ExecutionResult result = m_simulator->Step();
    if (!result.success) {
        errorPopup(ErrorParser::ParseError(result.error, result.errorInstruction));
        return;
    }
    setResult(result);
}

void MainWindow::stop()
{
    m_running = false;

    m_waitCondition.wakeAll();

    reset();
}

void MainWindow::reset()
{
    m_simulator->Reset();
    m_pcData = 0;
    m_registerData = m_simulator->GetCpuStatus().registers;
    m_memoryData = m_simulator->GetMemory();
    updateMemoryWithFormat(m_memoryFormatComboBox->currentText());
    updateRegisterWithFormat(m_registerFormatComboBox->currentText());
}

void MainWindow::setResult(const ExecutionResult& result)
{
    // Update the register and memory values
    m_pcData = result.pc;
    highlightLineEdit(m_pcValue);
    if (result.registerChanged) {
        m_registerData[result.registerChange.reg] = result.registerChange.value;
        updateRegisterWithFormat(m_registerFormatComboBox->currentText());
        highlightLineEdit(m_registerMap[result.registerChange.reg]);
    }
    else if (result.memoryChanged) {
        m_memoryData[result.memoryChange.address] = result.memoryChange.value;
        updateMemoryWithFormat(m_memoryFormatComboBox->currentText());
        highlightLabel(m_memoryMap[result.memoryChange.address]);
    }
}

void MainWindow::updateRegisterWithFormat(const QString& format) const
{
    const bool toHex = (format == "Hexadecimal");

    // pc
    const uint32_t pcValueInt = m_pcData;
    m_pcValue->setText(toHex ? QString("%1").arg(pcValueInt, 8, 16, QChar('0')) : QString::number(pcValueInt));

    // registers
    for (int i = 0; i < 32; i++) {
        const uint32_t regValueInt = m_registerData[i];
        m_registerMap[i]->setText(toHex ? QString("%1").arg(regValueInt, 8, 16, QChar('0'))
                                        : QString::number(regValueInt));
    }
}

void MainWindow::updateMemoryWithFormat(const QString& format)
{
    ensureMemoryMapCapacity();

    const bool toHex = (format == "Hexadecimal");

    for (int i = 0; i < m_memoryData.size(); i++) {
        // Get the memory address as a hexadecimal string
        QString addressText = QString("%1:").arg(i * 4, 8, 16, QChar('0')); // i * 4 for byte addressing

        const uint32_t value = m_memoryData[i];
        QString memoryRowText = addressText + "  ";

        // Group the bytes (4 bytes = 32 bits)
        for (int j = 0; j < 4; j++) {
            const uint8_t byteValue = (value >> (8 * j)) & 0xFF;
            memoryRowText += (toHex ? QString("%1 ").arg(byteValue, 2, 16, QChar('0'))
                                    : QString("%1 ").arg(byteValue, 3, 10, QChar('0')));
        }
        m_memoryMap[i]->setText(memoryRowText);
    }
}

void MainWindow::ensureMemoryMapCapacity()
{
    const int sizeDiff = m_memoryMap.size() - m_memoryData.size();
    if (sizeDiff == 0) {
        return;
    }
    m_memoryLayout->removeItem(m_spacer);

    if (sizeDiff > 0) {
        for (int i = m_memoryMap.size() - 1; i > m_memoryMap.size() - sizeDiff; i--) {
            m_memoryLayout->removeWidget(m_memoryMap[i]);
            delete m_memoryMap[i];
            m_memoryMap.pop_back();
        }
    }
    else if (sizeDiff < 0) {
        m_memoryMap.reserve(m_memoryData.size());
        for (int i = m_memoryMap.size(); i < m_memoryData.size(); i++) {
            auto memValue = new QLabel("");
            memValue->setFont(*m_monoFont);
            m_memoryLayout->addWidget(memValue);
            m_memoryMap.push_back(memValue);
        }
    }

    m_memoryLayout->addItem(m_spacer);
}

void MainWindow::increaseFontSize() const
{
    QFont font = m_codeEditor->font();
    font.setPointSize(font.pointSize() + 1);
    m_codeEditor->setFont(font);
}

void MainWindow::decreaseFontSize() const
{
    QFont font = m_codeEditor->font();
    font.setPointSize(font.pointSize() - 1);
    m_codeEditor->setFont(font);
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            increaseFontSize();
        }
        else {
            decreaseFontSize();
        }
    }
    else {
        event->ignore();
    }
}

void MainWindow::errorPopup(const string& message) const
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString(message));
    msgBox.exec();
}

bool MainWindow::parseAndSetInstructions() const
{
    QStringList lines = m_codeEditor->toPlainText().split('\n');
    std::vector<std::string> stdLines;
    stdLines.reserve(lines.size());
    for (const QString& line : lines) {
        stdLines.push_back(line.toStdString());
    }
    const ParsingResult result = Parser::Parse(stdLines);
    if (!result.success) {
        errorPopup(ErrorParser::ParseError(result.errorType, result.errorLine));
        return false;
    }

    m_simulator->SetInstructions(result.instructions);
    return true;
}

void MainWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Main");
    toolbar->setMovable(false);

    const auto centerContainer = new QWidget();
    const auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    m_stopButton = new QPushButton();
    m_stopButton->setIcon(QIcon(":/images/stop.png"));
    m_stopButton->setIconSize(QSize(20, 20));
    m_stopButton->setToolTip("Stop and reset the simulation");

    m_stepButton = new QPushButton();
    m_stepButton->setIcon(QIcon(":/images/forward.png"));
    m_stepButton->setIconSize(QSize(20, 20));
    m_stepButton->setToolTip("Step through the simulation");

    m_runButton = new QPushButton();
    m_runButton->setIcon(QIcon(":/images/fast_forward.png"));
    m_runButton->setIconSize(QSize(20, 20));
    m_runButton->setToolTip("Run the simulation");

    layout->addWidget(m_stopButton);
    layout->addWidget(m_stepButton);
    layout->addWidget(m_runButton);

    centerContainer->setLayout(layout);

    // Add spacer widgets to toolbar for centering
    const auto spacerLeft = new QWidget();
    spacerLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    const auto spacerRight = new QWidget();
    spacerRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    toolbar->addWidget(spacerLeft);
    toolbar->addWidget(centerContainer);
    toolbar->addWidget(spacerRight);
}

void MainWindow::highlightLabel(QLabel* label) const
{
    label->setStyleSheet("color: green;");
    QTimer::singleShot(300, [label]() { label->setStyleSheet(""); });
}

void MainWindow::highlightLineEdit(QLineEdit* lineEdit) const
{
    lineEdit->setStyleSheet("color: green;");
    QTimer::singleShot(300, [lineEdit]() { lineEdit->setStyleSheet(""); });
}
