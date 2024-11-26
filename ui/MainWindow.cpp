#include "MainWindow.h"

// QCodeEditor
#include <QCXXHighlighter>
#include <QCodeEditor>
#include <QGLSLHighlighter>
#include <QSyntaxStyle>

// Qt
#include <QApplication>
#include <QComboBox>
#include <QFile>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QStyleHints>
#include <QVBoxLayout>

#include "QLuaCompleter.hpp"


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), m_setupLayout(nullptr), m_themeCombobox(nullptr), m_codeEditor(nullptr), m_completer(nullptr),
    m_highlighter(nullptr), m_saveAction(nullptr), m_openAction(nullptr), m_pcValue(nullptr),
    m_registerFormatComboBox(nullptr)
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
    m_saveAction = fileMenu->addAction("Save");
    m_openAction = fileMenu->addAction("Open");

    QMenu* viewMenu = menuBar->addMenu("View");
    viewMenu->addAction("Increase Font Size", this, &MainWindow::increaseFontSize);
    viewMenu->addAction("Decrease Font Size", this, &MainWindow::decreaseFontSize);

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
    memoryPane->setFixedWidth(310);
    mainLayout->addWidget(memoryPane);

    // CodeEditor
    m_codeEditor = new QCodeEditor(this);
    mainLayout->addWidget(m_codeEditor);

    // Memory
    QWidget* memoryPanel = createMemoryPane();
    memoryPanel->setFixedWidth(320);
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
    const auto spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    scrollLayout->addSpacerItem(spacer);

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
    const auto memLayout = new QVBoxLayout;

    // Generate example memory data (using std::vector<uint32_t>)
    m_memoryData = generateMemoryData();

    // Displaying memory addresses and values
    const int memorySize = m_memoryData.size();
    QMap<int, QLineEdit*> memoryMap;

    for (int i = 0; i < memorySize; i++) {
        // Get the memory address as a hexadecimal string
        QString addressText = QString("%1").arg(i * 4, 8, 16, QChar('0')); // i * 4 for byte addressing

        // Split uint32_t into uint8_t values (4 bytes)
        const uint32_t value = m_memoryData[i];
        QString memoryRowText = addressText + "  "; // Start with the address

        // Group the bytes (4 bytes = 32 bits)
        for (int j = 0; j < 4; j++) {
            const uint8_t byteValue = (value >> (8 * j)) & 0xFF;
            memoryRowText += QString("%1 ").arg(byteValue, 2, 16, QChar('0')); // Add each byte as hex
        }

        // Create a label to display the address and grouped bytes
        const auto memoryRowLabel = new QLabel(memoryRowText);
        memoryRowLabel->setFont(*m_monoFont);
        memLayout->addWidget(memoryRowLabel);
    }
    // push content up
    const auto spacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    memLayout->addSpacerItem(spacer);

    // Set up scrollable content for memory
    memContentWidget->setLayout(memLayout);
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
    connect(m_memoryFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateMemoryFormat);
    connect(m_registerFormatComboBox, &QComboBox::currentTextChanged, this, &MainWindow::updateRegisterFormat);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_themeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](const int index) { m_codeEditor->setSyntaxStyle(m_themes[index].second); });
}

void MainWindow::saveFile()
{
    const QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QTextStream out(&file);
    out << m_codeEditor->toPlainText();
    file.close();
}

void MainWindow::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QTextStream in(&file);
    const QString text = in.readAll();
    m_codeEditor->setPlainText(text);
    file.close();
}

void MainWindow::updateRegisterWithFormat(const QString& format)
{
    ensureMapCapacity(m_registerMap, m_registerData);

    const bool toHex = (format == "Hexadecimal");

    // pc
    const int pcValueInt = m_pcValue->text().toInt(nullptr, toHex ? 16 : 10);
    m_pcValue->setText(toHex ? QString("%1").arg(pcValueInt, 8, 16, QChar('0')) : QString::number(pcValueInt));

    // registers
    for (int i = 0; i < 32; i++) {
        const int regValueInt = m_registerMap[i]->text().toInt(nullptr, toHex ? 16 : 10);
        m_registerMap[i]->setText(toHex ? QString("%1").arg(regValueInt, 8, 16, QChar('0'))
                                        : QString::number(regValueInt));
    }
}

void MainWindow::updateMemoryWithFormat(const QString& format)
{
    ensureMapCapacity(m_memoryMap, m_memoryData);

    const bool toHex = (format == "Hexadecimal");

    for (const auto& memValue : m_memoryMap) {
        const int byteValueInt = memValue->text().toInt(nullptr, toHex ? 16 : 10);
        memValue->setText(toHex ? QString("%1").arg(byteValueInt, 2, 16, QChar('0')) : QString::number(byteValueInt));
    }
}

void MainWindow::ensureMapCapacity(vector<QLineEdit*>& map, const vector<uint32_t>& data)
{
    const int sizeDiff = map.size() - data.size();
    if (sizeDiff > 0) {
        for (int i = map.size() - 1; i > map.size() - sizeDiff; i--) {
            delete map[i];
            map.pop_back();
        }
    }
    else if (sizeDiff < 0) {
        map.reserve(data.size());
        for (int i = map.size(); i < data.size(); i++) {
            auto memValue = new QLineEdit("");
            memValue->setFont(*m_monoFont);
            memValue->setReadOnly(true);
            map.push_back(memValue);
        }
    }
}

std::vector<uint32_t> MainWindow::generateMemoryData()
{
    constexpr uint32_t memorySize = 256;
    std::vector<uint32_t> data;
    for (int i = 0; i < memorySize; i++) {
        data.push_back(0);
    }
    return data;
}

void MainWindow::increaseFontSize()
{
    QFont font = m_codeEditor->font();
    font.setPointSize(font.pointSize() + 1);
    m_codeEditor->setFont(font);
}

void MainWindow::decreaseFontSize()
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
