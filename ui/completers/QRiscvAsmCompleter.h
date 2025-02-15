#pragma once

// Qt
#include <QCompleter> // Required for inheritance

class QStringListModel;
/**
 * @brief Class, that describes completer with
 * glsl specific types and functions.
 */
class QRiscvAsmCompleter : public QCompleter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Pointer to parent QObject.
     */
    explicit QRiscvAsmCompleter(QObject* parent = nullptr);

    void setLabels(const std::vector<std::string>& labels);

private:
    QStringList* m_list;
};
