#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QString>
#include <QJsonObject>

class GameConfig
{
private:
    //general_configurations
    QString gameType;
    bool hasTimeLimit;
    int timeLimit;

    //network_configurations
    int serverPort;
    QString serverIP;

    //dots_and_boxes_configurations
    int dotsAndBoxesRows;
    int dotsAndBoxesColumns;

    //morris_configurations
    bool useFlyingPhase;

    //helper_method
    void validateAndFix();

public:
    //constructors
    GameConfig();

    //getters
    QString getGameType() const ;
    bool getHasTimeLimit() const ;
    int getTimeLimit() const ;
    int getServerPort() const ;
    QString getServerIP() const ;
    int getDotsAndBoxesRows() const ;
    int getDotsAndBoxesColumns() const ;
    bool getUseFlyingPhase() const;

    //setters
    void setGameType(const QString& _gameType);
    void setHasTimeLimit(bool hasTimeLimit);
    void setTimeLimit(int _timeLimit);
    void setServerPort(int _serverPort);
    void setServerIP(const QString& serverIP);
    void setDotsBoardSize(int _rows, int _columns);
    void setDotsAndBoxesRows(int _dotsAndBoxesRows);
    void setDotsAndBoxesColumns(int _dotsAndBoxesColumns);
    void setUseFlyingPhase(bool _useFlyingPhase);

    //operational_methods
    bool isValid() const;
    QString toString() const;
    void resetToDefaults();

    //working_with_JSON
    QJsonObject toJson() const;
    static GameConfig fromJson(const QJsonObject& jsonObj);

    //operators
    bool operator==(const GameConfig& other) const;
    bool operator!=(const GameConfig& other) const;

};

#endif // GAMECONFIG_H
