#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QThread>

#include "../simulator/Simulator.h"
#include "ErrorParser.h"


class SimulationThread : public QThread
{
    Q_OBJECT

public:
    explicit SimulationThread(Simulator* simulator, QObject* parent = nullptr, int speed = 1000);
    void run() override;
    void stop();
    void setSpeed(int speed);
    void sleep() const;

signals:
    void resultReady(const ExecutionResult& result);
    void errorOccurred(const ExecutionResult& error);
    void finished();

private:
    Simulator* m_simulator;
    bool m_running;
    int m_speed;
    bool m_hasStarted;
};


#endif // SIMULATIONTHREAD_H
