#ifndef MOVE_H
#define MOVE_H

#include <QString>
#include <QPoint>
#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QVariant>
#include <QMetaType>

class Move
{
public:
    enum class Type
    {
        Unknown,
        Placement, //placing piece
        Movement,  //move piece
        Removal,   //remove piece
        Line,      //draw line        (for 'dots and boxes')
        Capture,   //capturing piece  (for 'fanarona')
        Mill,      //make a mill      (for 'nine mens morris')
    };

private:
    QString playerUsername;
    Type moveType;
    int moveNumber;
    QDateTime timestamp;
    QString description;
    bool isValidMove;
    QMap<QString, QVariant> data;

    //helper_methods
    void generateTimestamp();
    void setDefaultType(Type type);
    QString typeToString(Type type) const;
    Type stringToType(const QString& _string) const;

public:
    //constructors
    Move();
    Move(const QString& _playerUsername, Type _moveType);
    Move(const QString& _playerUsername, int from, int to);
    Move(const QString& _playerUsername, int row, int column, bool horizontal);
    Move(const QString& _playerUsername, const QPoint& from, const QPoint& to);
    Move(const QString& _playerUsername, const QPoint& position);

    //getters
    QString getPlayerUsername() const ;
    Type getMoveType() const ;
    QString getTypeString() const;
    int getMoveNumber() const ;
    QDateTime getTimestamp() const ;
    bool getIsValidMove() const ;
    QString getDescription() const ;

    //setters
    void setPlayerUsername(const QString& _playerUsername) ;
    void setMoveType(Type _moveType) ;
    void setMoveNumber(int _moveNumber) ;
    void setTimestamp(const QDateTime& _timestamp) ;
    void setIsValidMove(bool _isValidMoved) ;
    void setDescription(const QString& _description) ;

    //data_methods
    QVariant getData(const QString& key) const ;
    void setData(const QString& key, const QVariant& value) ;
    bool hasData(const QString& key) const ;
    QMap<QString, QVariant> getAllData() const ;

    //dots_and_boxes_methods
    void setDotsAndBoxesData(int row, int column, bool horizontal);
    bool isHorizontal() const;
    int getRow() const;
    int getColumn() const;

    //morrises_methods
    void setMorrisData(int from, int to, bool isRemoval);
    int getFrom() const;
    int getTo() const;
    bool isRemovalMove() const;
    bool isPlacementMove() const;

    //fanarona_methods
    void setFanoronaData(const QPoint& from, const QPoint& to);
    QPoint getFromPoint() const;
    QPoint getToPoint() const;
    QPoint getDirection() const;

    //working_with_JSON
    QJsonObject toJson() const;
    static Move fromJson(const QJsonObject& jsonObj);

    //serialization
    QByteArray serialize() const;
    static Move deserialize(const QByteArray& _data);

    //operators
    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;

    //to_string_methods
    QString toString() const;
    QString toShortString() const;

};

#endif // MOVE_H
