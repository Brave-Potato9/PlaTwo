#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H
#include <QObject>
#include <QMap>
#include <QTimer>
class TimerManager : public QObject
{
    Q_OBJECT
private:
    QMap<int, QTimer*> timers;
    QMap<int, int> remainingTimes;
public:
    //constructor_destructor
    explicit TimerManager(QObject * parent = nullptr);
    ~TimerManager();

    //working_with_Timer_and_activity
    void startTimer(int playerIndex, int seconds);
    void stopTimer(int playerIndex);
    void pauseTimer(int playerIndex);
    void resumeTimer(int playerIndex);
    bool isTimerRunning(int playerIndex) const;

    //remaining_time
    int getRemainingTime(int playerIndex) const;
signals:
    void timeUpdated(int playerIndex, int remainingSeconds);
    void timeExpired(int playerIndex);
private slots:
    void onTimerTick();
};

#endif // TIMERMANAGER_H
