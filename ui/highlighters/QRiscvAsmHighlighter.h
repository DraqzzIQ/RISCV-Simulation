#pragma once

#include <QHighlightRule>
#include <QStyleSyntaxHighlighter>

#include <QRegularExpression>
#include <QVector>

class QSyntaxStyle;

class QRiscvAsmHighlighter : public QStyleSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param document Pointer to document.
     */
    explicit QRiscvAsmHighlighter(QTextDocument* document = nullptr);
    void highlightLine(int lineNumber);

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<QHighlightRule> m_highlightRules;
    int m_executingLine = 0;
};
