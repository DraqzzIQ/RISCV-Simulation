#include "MainWindow.h"

// QCodeEditor
#include <QCodeEditor>
#include <QSyntaxStyle>

// Qt
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <iostream>

#include "../parser/Parser.h"
#include "../parser/ParsingResult.h"
#include "../simulator/CPUUtil.h"
#include "../simulator/Simulator.h"
#include "ErrorParser.h"
#include "completers/QRiscvAsmCompleter.h"
#include "highlighters/QRiscvAsmHighlighter.h"


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_setupLayout(nullptr), m_themeCombobox(nullptr), m_codeEditor(nullptr), m_completer(nullptr),
    m_highlighter(nullptr), m_file(nullptr), m_saveAction(nullptr), m_openAction(nullptr), m_showMemoryAction(nullptr),
    m_showRegistersAction(nullptr), m_showAddressAction(nullptr), m_spacer(nullptr), m_pcData(0), m_pcValue(nullptr),
    m_registerFormatComboBox(nullptr), m_memoryLayout(nullptr), m_memoryFormatComboBox(nullptr),
    m_monoFont(new QFont("Courier", 11)), m_registerPanel(nullptr), m_memoryPanel(nullptr), m_instructionMap(nullptr),
    m_hasStarted(false), m_simulator(nullptr), m_speed(1000), m_simulationThread(nullptr), m_configData(nullptr),
    m_helpWindow(nullptr)
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
    m_completer = new QRiscvAsmCompleter(this);
    m_highlighter = new QRiscvAsmHighlighter(m_completer);
    m_themes = {};
    m_registerMap = vector<QLineEdit*>(32);
    m_memoryLayout = new QVBoxLayout;
    m_spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_simulator = new Simulator;
    m_registerData = vector<int32_t>(32);
    m_configData = new ConfigData(Config::deserialize());

    loadStyle(":/styles/light.xml");
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
    m_newFileAction = fileMenu->addAction("New");
    m_newFileAction->setShortcut(QKeySequence("Ctrl+N"));
    m_saveAsAction = fileMenu->addAction("Save As");
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_saveAction = fileMenu->addAction("Save");
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    m_openAction = fileMenu->addAction("Open");
    m_openAction->setShortcut(QKeySequence("Ctrl+O"));

    QMenu* viewMenu = menuBar->addMenu("View");
    QAction* action = viewMenu->addAction("Increase Font Size", this, &MainWindow::increaseFontSize);
    action->setShortcut(QKeySequence("Ctrl++"));
    action = viewMenu->addAction("Decrease Font Size", this, &MainWindow::decreaseFontSize);
    action->setShortcut(QKeySequence("Ctrl+-"));
    m_showRegistersAction = new QAction("Show Register Panel", this);
    m_showRegistersAction->setCheckable(true);
    m_showRegistersAction->setChecked(m_configData->registersShown);
    m_showRegistersAction->setShortcut(QKeySequence("Ctrl+R"));
    viewMenu->addAction(m_showRegistersAction);
    m_showMemoryAction = new QAction("Show Memory Panel", this);
    m_showMemoryAction->setCheckable(true);
    m_showMemoryAction->setChecked(m_configData->memoryShown);
    m_showMemoryAction->setShortcut(QKeySequence("Ctrl+M"));
    viewMenu->addAction(m_showMemoryAction);
    m_showAddressAction = new QAction("Show Instruction Addresses", this);
    m_showAddressAction->setCheckable(true);
    m_showAddressAction->setChecked(m_configData->addressesShown);
    m_showAddressAction->setShortcut(QKeySequence("Ctrl+I"));
    viewMenu->addAction(m_showAddressAction);

    QMenu* helpWindow = menuBar->addMenu("Help");
    m_helpAction = helpWindow->addAction("View Instructions");
    m_helpAction->setShortcut(QKeySequence("Ctrl+H"));

    createToolbar();

    // Layout
    const auto container = new QWidget(this);
    setCentralWidget(container);

    const auto mainLayout = new QHBoxLayout;

    // Registers
    m_registerPanel = createRegisterPanel();
    m_registerPanel->setFixedWidth(m_configData->registersShown ? 360 : 0);
    mainLayout->addWidget(m_registerPanel);

    // CodeEditor
    m_codeEditor = new QCodeEditor(this);
    mainLayout->addWidget(m_codeEditor);

    // Memory
    m_memoryPanel = createMemoryPanel();
    m_memoryPanel->setFixedWidth(m_configData->memoryShown ? 300 : 0);
    mainLayout->addWidget(m_memoryPanel);

    container->setLayout(mainLayout);
}

void MainWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Main");
    toolbar->setMovable(false);

    const auto centerContainer = new QWidget();
    const auto layout = new QHBoxLayout();
    centerContainer->setProperty("cssClass", "themedBackground");
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    m_stopButton = new QPushButton();
    m_stopButton->setIcon(QIcon(":/images/stop.png"));
    m_stopButton->setIconSize(QSize(20, 20));
    m_stopButton->setToolTip("Stop and reset the simulation");
    m_stopButton->setProperty("cssClass", "themedBackground");

    m_stepButton = new QPushButton();
    m_stepButton->setIcon(QIcon(":/images/forward.png"));
    m_stepButton->setIconSize(QSize(20, 20));
    m_stepButton->setToolTip("Step through the simulation");
    m_stepButton->setProperty("cssClass", "themedBackground");

    m_runButton = new QPushButton();
    m_runButton->setIcon(QIcon(":/images/fast_forward.png"));
    m_runButton->setIconSize(QSize(20, 20));
    m_runButton->setToolTip("Run the simulation");
    m_runButton->setProperty("cssClass", "themedBackground");

    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(50, 3000);
    m_speedSlider->setValue(m_speed);
    m_speedSlider->setToolTip("Set execution speed (ms)");

    // Add spacer widgets to toolbar for centering
    const auto spacer = new QWidget();
    spacer->setProperty("cssClass", "themedBackground");
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    const auto toolSpacer = new QWidget();
    toolSpacer->setProperty("cssClass", "themedBackground");
    toolSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(m_stopButton);
    layout->addWidget(m_stepButton);
    layout->addWidget(m_runButton);
    layout->addWidget(new QLabel("Speed:"));
    layout->addWidget(m_speedSlider);

    centerContainer->setLayout(layout);

    // Theme selection
    const auto themeWidget = new QWidget();
    const auto themeLayout = new QHBoxLayout();
    themeWidget->setProperty("cssClass", "themedBackground");
    themeLayout->setProperty("cssClass", "themedBackground");

    m_themeCombobox = new QComboBox();
    themeLayout->addWidget(toolSpacer);
    themeLayout->addWidget(new QLabel("Theme:"));
    themeLayout->addWidget(m_themeCombobox);
    themeWidget->setLayout(themeLayout);

    const auto toolWidget = new QWidget();
    const auto toolLayout = new QHBoxLayout();
    toolWidget->setProperty("cssClass", "themedBackground");
    toolLayout->setProperty("cssClass", "themedBackground");

    toolLayout->addWidget(spacer, 1);
    toolLayout->addWidget(centerContainer, 1);
    toolLayout->addWidget(themeWidget, 1);

    toolWidget->setLayout(toolLayout);
    toolbar->addWidget(toolWidget);
}

QWidget* MainWindow::createRegisterPanel()
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
    scrollContent->setProperty("cssClass", "themedBackground");
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

        if (i == 0) {
            regLabel->setToolTip("read-only");
            regValue->setToolTip("read-only");
        }

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

QWidget* MainWindow::createMemoryPanel()
{
    const auto rightPanel = new QWidget;
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
    memContentWidget->setProperty("cssClass", "themedBackground");

    m_memoryData = m_simulator->GetMemory();

    // Set up scrollable content for memory
    memContentWidget->setLayout(m_memoryLayout);
    scrollArea->setWidget(memContentWidget);
    scrollArea->setWidgetResizable(true);

    layout->addWidget(scrollArea);
    rightPanel->setLayout(layout);

    return rightPanel;
}

void MainWindow::setupWidgets()
{
    setWindowTitle("RISCV-Simulator");

    const bool isDark = m_configData->theme == 1;
    setTheme(isDark);

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

    // Register Format
    m_registerFormatComboBox->setCurrentText(m_configData->regFormat);
    updateRegisterWithFormat(m_configData->regFormat);

    // Memory Format
    m_memoryFormatComboBox->setCurrentText(m_configData->memFormat);
    updateMemoryWithFormat(m_configData->memFormat);

    // Set the speed
    m_speed = m_configData->speed;
    m_speedSlider->setValue(3050 - m_speed);

    // Set the font size
    QFont font = m_codeEditor->font();
    font.setPointSize(m_configData->fontSize);
    m_codeEditor->setFont(font);

    // Set address visibility
    m_codeEditor->setAddressesVisible(m_configData->addressesShown);

    // Load last opened file
    if (m_configData->lastOpenFile.isEmpty() || !QFile::exists(m_configData->lastOpenFile)) {
        return;
    }
    m_file = new QFile(m_configData->lastOpenFile);
    if (!m_file->open(QIODevice::ReadWrite)) {
        return;
    }
    QTextStream in(m_file);
    const QString text = in.readAll();
    m_codeEditor->setPlainText(text);
}

void MainWindow::performConnections()
{
    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::run);
    connect(m_stepButton, &QPushButton::clicked, this, &MainWindow::step);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::stop);
    connect(m_speedSlider, &QSlider::valueChanged, this, &MainWindow::setSpeed);
    connect(m_memoryFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateMemoryWithFormat);
    connect(m_registerFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateRegisterWithFormat);
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(m_saveAction, &QAction::triggered, this, [this]() { saveFile(true); });
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_themeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::setTheme);
    connect(m_showRegistersAction, &QAction::triggered, this, &MainWindow::setRegisterPanelShown);
    connect(m_showMemoryAction, &QAction::triggered, this, &MainWindow::setMemoryPanelShown);
    connect(m_showAddressAction, &QAction::triggered, this, &MainWindow::setAddressesShown);
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::showHelp);
}

void MainWindow::setRegisterPanelShown(const bool shown) const
{
    const int width = shown ? 360 : 0;
    m_registerPanel->setFixedWidth(width);
    m_configData->registersShown = shown;
    saveConfig();
}

void MainWindow::setMemoryPanelShown(const bool shown) const
{
    const int width = shown ? 300 : 0;
    m_memoryPanel->setFixedWidth(width);
    m_configData->memoryShown = shown;
    saveConfig();
}

void MainWindow::setAddressesShown(const bool shown) const
{
    m_codeEditor->setAddressesVisible(shown);
    m_configData->addressesShown = shown;
    saveConfig();
}

void MainWindow::newFile()
{
    saveFile();
    m_file->close();
    delete m_file;
    m_file = nullptr;
    m_codeEditor->setPlainText("");
    m_configData->lastOpenFile = "";
    saveConfig();
}

void MainWindow::saveAsFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    if (m_file == nullptr) {
        m_file = new QFile(fileName);
        if (!m_file->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, tr("Unable to open file"), m_file->errorString());
            return;
        }
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
    m_configData->lastOpenFile = m_file->fileName();
    saveConfig();
}

void MainWindow::saveFile(const bool openDialog)
{
    if (m_file == nullptr && !openDialog) {
        return;
    }
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
    m_configData->lastOpenFile = m_file->fileName();
    saveConfig();
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
    }
    if (!m_file->open(QIODevice::ReadWrite)) {
        QMessageBox::information(this, tr("Unable to open file"), m_file->errorString());
        return;
    }

    QTextStream in(m_file);
    const QString text = in.readAll();
    m_codeEditor->setPlainText(text);
    m_configData->lastOpenFile = m_file->fileName();
    saveConfig();
}

void MainWindow::showHelp()
{
    if (!m_helpWindow) {
        m_helpWindow = new HelpWindow(this);
    }
    m_helpWindow->show();
}

void MainWindow::setSpeed(const int speed)
{
    m_speed = 3050 - speed;
    if (m_simulationThread) {
        m_simulationThread->setSpeed(m_speed);
    }
    m_configData->speed = m_speed;
    saveConfig();
}

void MainWindow::setTheme(const int index)
{
    m_codeEditor->setSyntaxStyle(m_themes[index].second);

    QFile styleSheet(index == 1 ? ":/themes/dark.qss" : ":/themes/light.qss");
    styleSheet.open(QFile::ReadOnly);
    const QString style(styleSheet.readAll());
    qApp->setStyleSheet(style);
    m_configData->theme = index;
    saveConfig();
}

void MainWindow::run()
{
    reset();
    saveFile(false);
    if (!parseAndSetInstructions()) {
        return;
    }

    if (m_simulationThread != nullptr) {
        return;
    }

    m_simulationThread = new SimulationThread(m_simulator, this, m_speed);
    connect(m_simulationThread, &SimulationThread::resultReady, this, &MainWindow::setResult);
    connect(m_simulationThread, &SimulationThread::errorOccurred, this, &MainWindow::executionError);
    connect(m_simulationThread, &SimulationThread::finished, this, &MainWindow::executionFinished);

    m_stepButton->setEnabled(false);
    m_runButton->setEnabled(false);

    m_simulationThread->start();
}

void MainWindow::step()
{
    saveFile(false);
    m_highlighter->highlightError(-1);
    if (m_simulationThread != nullptr) {
        return;
    }
    if (!parseAndSetInstructions()) {
        return;
    }

    // Highlight executing instruction
    if (!m_hasStarted && m_instructionMap->size() > 0) {
        m_pcData = 0;
        updateRegisterWithFormat(m_registerFormatComboBox->currentText());
        m_hasStarted = true;
        m_highlighter->highlightLine(m_instructionMap->at(0));
        return;
    }

    const ExecutionResult result = m_simulator->Step();
    if (result.success) {
        setResult(result);
        return;
    }

    m_hasStarted = false;
    if (result.error == ExecutionError::PC_OUT_OF_BOUNDS) {
        if (errorPopup("Program counter out of bounds. Reset?", true)) {
            this->reset();
        }
        return;
    }
    m_highlighter->highlightError(calculateErrorLine(result.errorInstruction));
    errorPopup(ErrorParser::ParseError(result.error, calculateErrorLine(result.errorInstruction)));
}

void MainWindow::stop()
{
    if (m_simulationThread != nullptr) {
        m_simulationThread->stop();
        m_simulationThread = nullptr;
    }
    m_stepButton->setEnabled(true);
    m_runButton->setEnabled(true);

    reset();
}

void MainWindow::reset()
{
    m_simulator->Reset();
    m_pcData = 0;
    m_registerData = std::vector<int32_t>(32);
    m_memoryData = m_simulator->GetMemory();
    updateMemoryWithFormat(m_memoryFormatComboBox->currentText());
    updateRegisterWithFormat(m_registerFormatComboBox->currentText());
    m_highlighter->highlightLine(-1);
    m_hasStarted = false;
}

void MainWindow::executionError(const ExecutionResult& error)
{
    errorPopup(ErrorParser::ParseError(error.error, calculateErrorLine(error.errorInstruction)));
    m_simulationThread = nullptr;
    m_stepButton->setEnabled(true);
    m_runButton->setEnabled(true);
}

void MainWindow::setResult(const ExecutionResult& result)
{
    // Update the register and memory values
    if (result.registerChanged) {
        m_registerData[result.registerChange.reg] = static_cast<int32_t>(result.registerChange.value);
        highlightRegisterLineEdit(m_registerMap[result.registerChange.reg]);
    }
    else if (result.memoryChanged) {
        m_memoryData[result.memoryChange.address] = result.memoryChange.value;
        updateMemoryWithFormat(m_memoryFormatComboBox->currentText());
        highlightMemoryLabel(m_memoryMap[result.memoryChange.address]);
    }
    m_pcData = result.pc;
    updateRegisterWithFormat(m_registerFormatComboBox->currentText());
    highlightRegisterLineEdit(m_pcValue);

    if (m_instructionMap->size() > result.pc / 4) {
        m_highlighter->highlightLine(m_instructionMap->at(result.pc / 4));
    }
    else {
        m_highlighter->highlightLine(-1);
    }
}

void MainWindow::executionFinished()
{
    m_simulationThread = nullptr;
    m_stepButton->setEnabled(true);
    m_runButton->setEnabled(true);
}

void MainWindow::updateRegisterWithFormat(const QString& format) const
{
    const bool toHex = (format == "Hexadecimal");

    // pc
    const uint32_t pcValueInt = m_pcData;
    m_pcValue->setText(toHex ? QString("%1").arg(pcValueInt, 8, 16, QChar('0')) : QString::number(pcValueInt));

    // remaining registers
    for (int i = 0; i < 32; i++) {
        const int32_t regValueInt = m_registerData[i];
        m_registerMap[i]->setText(toHex ? QString("%1").arg(static_cast<uint32_t>(regValueInt), 8, 16, QChar('0'))
                                        : QString::number(regValueInt));
    }

    m_configData->regFormat = format;
    saveConfig();
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

    m_configData->memFormat = format;
    saveConfig();
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
    else {
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
    m_configData->fontSize = font.pointSize();
    saveConfig();
}

void MainWindow::decreaseFontSize() const
{
    QFont font = m_codeEditor->font();
    if (font.pointSize() <= 1)
        return;
    font.setPointSize(font.pointSize() - 1);
    m_codeEditor->setFont(font);
    m_configData->fontSize = font.pointSize();
    saveConfig();
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

bool MainWindow::errorPopup(const string& message, const bool yesNoButtons)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Error");
    msgBox.setText(QString::fromStdString(message));
    if (!yesNoButtons) {
        msgBox.exec();
        return false;
    }
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

bool MainWindow::parseAndSetInstructions()
{
    QStringList lines = m_codeEditor->toPlainText().split('\n');
    std::vector<std::string> stdLines;
    stdLines.reserve(lines.size());
    for (const QString& line : lines) {
        stdLines.push_back(line.toStdString());
    }
    const ParsingResult result = Parser::Parse(stdLines);

    m_simulator->SetInstructions(result.instructions);
    if (m_instructionMap != nullptr) {
        delete m_instructionMap;
        m_instructionMap = nullptr;
    }
    m_instructionMap = new vector(result.instructionMap);

    if (result.success) {
        return true;
    }
    m_highlighter->highlightError(result.errorLine - 1);

    const string format =
        InstructionFormats.contains(result.errorPart) ? "\nUsage: " + InstructionFormats.at(result.errorPart) : "";
    errorPopup(ErrorParser::ParseError(result.errorType, result.errorLine) + format);
    return false;
}

void MainWindow::highlightMemoryLabel(QLabel* label) const
{
    label->setStyleSheet("color: green;");
    QTimer::singleShot(300, [label]() { label->setStyleSheet(""); });
}

void MainWindow::highlightRegisterLineEdit(QLineEdit* lineEdit) const
{
    lineEdit->setStyleSheet("color: green;");
    QTimer::singleShot(300, [lineEdit]() { lineEdit->setStyleSheet(""); });
}

uint32_t MainWindow::calculateErrorLine(const int instruction) const
{
    if (m_instructionMap->size() < instruction) {
        return 0;
    }
    return m_instructionMap->at(instruction - 1) + 1;
}

void MainWindow::saveConfig() const { Config::serialize(*m_configData); }
void MainWindow::closeEvent(QCloseEvent* event)
{
    const QString content = m_codeEditor->toPlainText();
    if (!content.trimmed().isEmpty()) {
        saveFile();
    }
    QMainWindow::closeEvent(event);
}
