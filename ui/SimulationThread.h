#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>

#include "../simulator/Simulator.h"
#include "ErrorParser.h"


class SimulationThread : public QThread {
    Q_OBJECT

public:
    explicit SimulationThread(Simulator* simulator, QObject* parent = nullptr, int speed = 1000);
    void run() override;
    void stop();
    void setSpeed(int speed) { m_speed = speed; }

signals:
    void resultReady(const ExecutionResult& result);
    void errorOccurred(const ExecutionResult& error);

private:
    Simulator* m_simulator;
    bool m_running;
    int m_speed;
};



#endif //SIMULATIONTHREAD_H
