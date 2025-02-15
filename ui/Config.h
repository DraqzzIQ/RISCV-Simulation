#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <string>

using std::string;

struct ConfigData
{
    int theme = 0;
    int speed = 500;
    QString regFormat = "Hexadecimal";
    QString memFormat = "Hexadecimal";
    QString lastOpenFile = "";
    int fontSize = 10;
    bool memoryShown = true;
    bool registersShown = true;
    bool addressesShown = true;
};

class Config
{
public:
    static ConfigData deserialize();
    static void serialize(const ConfigData& data);

private:
    const static QString path;
};

#endif // CONFIG_H
