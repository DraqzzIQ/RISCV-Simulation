// K-Editor
#include "QRiscvAsmCompleter.h"
#include <QLanguage>

// Qt
#include <QFile>
#include <QStringListModel>

QRiscvAsmCompleter::QRiscvAsmCompleter(QObject* parent) : QCompleter(parent)
{
    m_list = new QStringList;

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
        m_list->append(names);
    }

    setModel(new QStringListModel(*m_list, this));
    setCompletionColumn(0);
    setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    setCaseSensitivity(Qt::CaseSensitive);
    setWrapAround(true);
}
void QRiscvAsmCompleter::setLabels(const std::vector<std::string>& labels)
{
    QStringList list;
    foreach (const auto& label, labels) {
        list.append(QString::fromStdString(label));
    }
    list.append(*m_list);
    setModel(new QStringListModel(list, this));
}
