#include "TimerManager.h"

//------------------------------------ constructor_destructor ------------------------------------
TimerManager::TimerManager(QObject * parent): QObject(parent) {}
TimerManager::~TimerManager() {
    for(auto timer : timers) {
        timer->stop();
        delete timer;
    }
    timers.clear();
}

//------------------------------------ working_with_timer_and_activity ------------------------------------
void TimerManager::startTimer(int playerIndex, int seconds) {
    stopTimer(playerIndex); // delete previous timer(if exists)
    remainingTimes[playerIndex] = seconds;
    QTimer * timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &TimerManager::onTimerTick);
    timers[playerIndex] = timer;
    timer->start();
}
void TimerManager::stopTimer(int playerIndex) {
    if(timers.contains(playerIndex)) {
        timers[playerIndex]->stop();
        timers[playerIndex]->deleteLater();
        timers.remove(playerIndex); //delete from map
    }
}
void TimerManager::pauseTimer(int playerIndex) {
    if(timers.contains(playerIndex)) {
        timers[playerIndex]->stop();
    }
}
void TimerManager::resumeTimer(int playerIndex) {
    if(timers.contains(playerIndex)) {
        timers[playerIndex]->start();
    }
}
bool TimerManager::isTimerRunning(int playerIndex) const {
    if(timers.contains(playerIndex)) {
        if(timers[playerIndex]->isActive()) {
            return true;
        }
    }
    return false;
}

//------------------------------------ remaining_time ------------------------------------
int TimerManager::getRemainingTime(int playerIndex) const {
    if(timers.contains(playerIndex)) {
        return remainingTimes[playerIndex];
    } else {
        return -1;
    }

}
//------------------------------------ passing_of_time_slot ------------------------------------
void TimerManager::onTimerTick() {
    QTimer * senderTimer = qobject_cast<QTimer*>(sender());
    if(senderTimer == nullptr) {
        return;
    }
    int playerIndex = -1;
    for(auto it = timers.begin() ; it != timers.end() ; it++) {
        if(it.value() == senderTimer) {
            playerIndex = it.key();
            break;
        }
    }
    if(playerIndex == -1) return;
    remainingTimes[playerIndex]--;
    emit timeUpdated(playerIndex, remainingTimes[playerIndex]);
    if(remainingTimes[playerIndex] <= 0) {
        stopTimer(playerIndex);
        emit timeExpired(playerIndex);
    }
}