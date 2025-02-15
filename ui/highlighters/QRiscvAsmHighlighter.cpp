#include "QRiscvAsmHighlighter.h"
#include <QFile>
#include <QLanguage>
#include <QStyleSyntaxHighlighter>
#include <QSyntaxStyle>
#include <regex>

QRiscvAsmHighlighter::QRiscvAsmHighlighter(QRiscvAsmCompleter* completer, QTextDocument* document) :
    QStyleSyntaxHighlighter(document), m_completer(completer)
{
    Q_INIT_RESOURCE(qcodeeditor_resources);
    QFile fl(":/languages/riscv-asm.xml");

    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    QLanguage language(&fl);

    if (!language.isLoaded()) {
        return;
    }

    // Everything
    m_highlightRules.append({QRegularExpression(R"(\w*)"), "Text"});


    auto keys = language.keys();
    for (auto&& key : keys) {
        auto names = language.names(key);
        for (auto&& name : names) {
            m_highlightRules.append({QRegularExpression(QString(R"(\b%1\b)").arg(name)), key});
        }
    }

    // Registers
    m_highlightRules.append({QRegularExpression(R"(\bx[0-9]+\b)"), "Register"});

    // Numbers
    m_highlightRules.append(
        {QRegularExpression(
             R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:'\d+)*)?\.(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)\.(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:'\d+)*)|(?:0[0-7]*(?:'[0-7]+)*)|(?:0x[0-9a-f]+(?:'[0-9a-f]+)*)|(?:0b[01]+(?:'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))"),
         "Number"});

    // Single line Comment
    m_highlightRules.append({QRegularExpression(R"(;[^\n]*)"), "Comment"});

    // Labels
    m_highlightRules.append({QRegularExpression(QString::fromStdString(m_labelRegex)), "Label"});
}

void QRiscvAsmHighlighter::setDocument(QTextDocument* doc)
{
    if (doc) {
        connect(doc, &QTextDocument::contentsChange, this, &QRiscvAsmHighlighter::onContentsChange);
    }
    QSyntaxHighlighter::setDocument(doc);
}

void QRiscvAsmHighlighter::onContentsChange(const int position, const int charsRemoved, const int charsAdded)
{
    Q_UNUSED(position);
    if (charsRemoved != 0 || charsAdded != 0) {
        m_errorLine = -1;
        m_executingLine = -1;
        rehighlight();
        m_completer->setLabels(m_labels);
        calculateAddresses();
    }
}

void QRiscvAsmHighlighter::calculateAddresses() const
{
    auto block = document()->begin();
    int address = 0;
    while (block.isValid()) {
        const auto text = block.text().toStdString();
        if (isValidLabel(text)) {
            block.setUserState(address);
        }
        else if (isValidInstruction(text)) {
            block.setUserState(address);
            address += 4;
        }
        else {
            block.setUserState(-1);
        }
        block = block.next();
    }
}

void QRiscvAsmHighlighter::highlightLine(const int lineNumber)
{
    m_executingLine = lineNumber;
    rehighlight();
}

void QRiscvAsmHighlighter::highlightError(const int lineNumber)
{
    m_errorLine = lineNumber;
    rehighlight();
}

void QRiscvAsmHighlighter::highlightBlock(const QString& text)
{
    const auto highlightFormat = syntaxStyle()->getFormat("Highlight");
    const auto labelFormat = syntaxStyle()->getFormat("Label");
    const auto errorFormat = syntaxStyle()->getFormat("Error");
    // highlight the error line
    const auto errorBlock = document()->findBlockByLineNumber(m_errorLine);
    // highlight the current line
    const auto executingBlock = document()->findBlockByLineNumber(m_executingLine);

    // check if every label in m_labels is in the text
    foreach (const auto& label, m_labels) {
        if (!document()->toPlainText().toLower().contains(QString::fromStdString(label + ":"))) {
            m_labels.erase(std::ranges::remove(m_labels, label).begin(), m_labels.end());
        }
        break;
    }

    for (auto& rule : m_highlightRules) {
        auto matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            // check for label definitions
            if (rule.formatName == "Label") {
                QString label = match.captured();
                if (label.size() > 1) {
                    label = label.removeLast().toLower();
                }
                const string labelStr = label.toStdString();
                if (std::ranges::find(m_labels, labelStr) == m_labels.end()) {
                    m_labels.push_back(labelStr);
                }
            }
            // check for label references
            else if (std::find(m_labels.begin(), m_labels.end(), match.captured().toLower()) != m_labels.end()) {
                setFormat(match.capturedStart(), match.capturedEnd(), labelFormat);
                continue;
            }

            auto format = syntaxStyle()->getFormat(rule.formatName);

            if (executingBlock.blockNumber() == currentBlock().blockNumber()) {
                format.setForeground(highlightFormat.foreground());
                format.setFontItalic(highlightFormat.fontItalic());
                format.setFontWeight(highlightFormat.fontWeight());
                setFormat(0, text.length(), format);
            }
            else if (errorBlock.blockNumber() == currentBlock().blockNumber()) {
                format = errorBlock.charFormat();
                format.setUnderlineColor(errorFormat.underlineColor());
                format.setUnderlineStyle(errorFormat.underlineStyle());
                setFormat(0, text.length(), format);
            }
            else {
                setFormat(match.capturedStart(), match.capturedLength(), format);
            }
        }
    }
}

bool QRiscvAsmHighlighter::isValidInstruction(string instruction) const
{
    while (instruction[0] == ' ') {
        instruction.erase(0, 1);
    }
    if (instruction.empty() || instruction[0] == ';') {
        return false;
    }
    return true;
}

bool QRiscvAsmHighlighter::isValidLabel(string label) const
{
    if (label.empty()) {
        return false;
    }
    if (!std::regex_match(label, std::regex(R"(^\s*[a-zA-Z_][a-zA-Z0-9_]*:\s*(;.*)?$)"))) {
        return false;
    }
    return true;
}
