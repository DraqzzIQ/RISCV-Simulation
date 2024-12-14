#include "QRiscvAsmHighlighter.h"
#include <QSyntaxStyle>
#include <QLanguage>
#include <QStyleSyntaxHighlighter>
#include <QFile>


QRiscvAsmHighlighter::QRiscvAsmHighlighter(QTextDocument* document) :
    QStyleSyntaxHighlighter(document)
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

    auto keys = language.keys();
    for (auto&& key : keys) {
        auto names = language.names(key);
        for (auto&& name : names) {
            m_highlightRules.append({
                QRegularExpression(QString(R"(\b%1\b)").arg(name)),
                key
            });
        }
    }

    // Numbers
    m_highlightRules.append({
        QRegularExpression(
            R"((?<=\b|\s|^)(?i)(?:(?:(?:(?:(?:\d+(?:'\d+)*)?\.(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)\.(?:e[+-]?(?:\d+(?:'\d+)*))?)|(?:(?:\d+(?:'\d+)*)(?:e[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)?\.(?:[0-9a-f]+(?:'[0-9a-f]+)*)(?:p[+-]?(?:\d+(?:'\d+)*)))|(?:0x(?:[0-9a-f]+(?:'[0-9a-f]+)*)\.?(?:p[+-]?(?:\d+(?:'\d+)*))))[lf]?)|(?:(?:(?:[1-9]\d*(?:'\d+)*)|(?:0[0-7]*(?:'[0-7]+)*)|(?:0x[0-9a-f]+(?:'[0-9a-f]+)*)|(?:0b[01]+(?:'[01]+)*))(?:u?l{0,2}|l{0,2}u?)))(?=\b|\s|$))"),
        "Number"
    });

    // Single line Comment
    m_highlightRules.append({
        QRegularExpression(R"(//[^\n]*)"),
        "Comment"
    });
}

void QRiscvAsmHighlighter::highlightBlock(const QString& text)
{
    for (auto& rule : m_highlightRules) {
        auto matchIterator = rule.pattern.globalMatch(text);

        while (matchIterator.hasNext()) {
            auto match = matchIterator.next();

            setFormat(
                match.capturedStart(),
                match.capturedLength(),
                syntaxStyle()->getFormat(rule.formatName)
                );
        }
    }

    setCurrentBlockState(0);
}
