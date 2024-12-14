#pragma once

#include <QStyleSyntaxHighlighter>
#include <QHighlightRule>

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

protected:
    void highlightBlock(const QString& text) override;

private:
    QVector<QHighlightRule> m_highlightRules;
};
