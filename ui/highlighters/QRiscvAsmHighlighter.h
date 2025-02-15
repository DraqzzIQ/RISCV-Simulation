#pragma once

#include "../completers/QRiscvAsmCompleter.h"


#include <QHighlightRule>
#include <QStyleSyntaxHighlighter>
#include <QVector>
#include <string>
#include <unordered_set>
#include <vector>

using std::string;
using std::vector;

class QSyntaxStyle;

class QRiscvAsmHighlighter : public QStyleSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit QRiscvAsmHighlighter(QRiscvAsmCompleter* completer, QTextDocument* document = nullptr);
    void highlightLine(int lineNumber);
    void highlightError(int lineNumber);
    void setDocument(QTextDocument* doc);

protected:
    void highlightBlock(const QString& text) override;

private:
    QRiscvAsmCompleter* m_completer;
    QVector<QHighlightRule> m_highlightRules;
    vector<string> m_labels;
    int m_executingLine = -1;
    int m_errorLine = -1;
    const string m_labelRegex = R"(\b\w+:(?=\s|\n|$))";

    void collectLabels();
    void calculateAddresses() const;
    bool isValidInstruction(string instruction) const;
    bool isValidLabel(string label) const;

private slots:
    void onContentsChange(int position, int charsRemoved, int charsAdded);
};
