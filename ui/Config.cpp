#include "Config.h"
#include <QFile>
#include <QStandardPaths>
#include <filesystem>
#include <qdir.h>
#include <qfileinfo.h>
#include <qurl.h>

#include "lib/json/json.hpp"

using json = nlohmann::json;

const QString Config::path =
    QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/RISCV-Simulator/config.json";

ConfigData Config::deserialize()
{
    if (!std::filesystem::exists(path.toStdString())) {
        return {};
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    string json = file.readAll().toStdString();
    const auto data = json::parse(json);

    return ConfigData{
        .theme = data.value<int>("theme", 0),
        .speed = data.value<int>("speed", 500),
        .regFormat = QString::fromStdString(data.value<string>("regFormat", "Hexadecimal")),
        .memFormat = QString::fromStdString(data.value<string>("memFormat", "Hexadecimal")),
        .lastOpenFile = QString::fromStdString(data.value<string>("lastOpenFile", "")),
        .fontSize = data.value<int>("fontSize", 10),
        .memoryShown = data.value<bool>("memoryShown", true),
        .registersShown = data.value<bool>("registersShown", true),
        .addressesShown = data.value<bool>("addressesShown", true),
    };
}

void Config::serialize(const ConfigData& data)
{
    json j;
    j["theme"] = data.theme;
    j["speed"] = data.speed;
    j["regFormat"] = data.regFormat.toStdString();
    j["memFormat"] = data.memFormat.toStdString();
    j["lastOpenFile"] = data.lastOpenFile.toStdString();
    j["fontSize"] = data.fontSize;
    j["memoryShown"] = data.memoryShown;
    j["registersShown"] = data.registersShown;
    j["addressesShown"] = data.addressesShown;

    QFile file(path);
    const QFileInfo info(file);

    if (!std::filesystem::exists(info.absoluteDir().path().toStdString())) {
        std::filesystem::create_directories(info.absoluteDir().path().toStdString());
    }

    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    file.resize(0);
    file.write(j.dump().c_str());
}
