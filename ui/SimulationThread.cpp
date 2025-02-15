#include "SimulationThread.h"
SimulationThread::SimulationThread(Simulator* simulator, QObject* parent, int speed) :
    QThread(parent), m_simulator(simulator), m_running(false), m_speed(speed), m_hasStarted(false)
{
}

void SimulationThread::run()
{
    m_running = true;
    m_simulator->Reset();

    while (m_running) {
        if (!m_hasStarted && m_simulator->GetCpuStatus().pc == 0) {
            emit resultReady({true, ExecutionError::NONE, false, {0, 0}, false, {0, 0}, 0, 0});
            sleep();
        }

        const ExecutionResult result = m_simulator->Step();
        if (!result.success) {
            m_running = false;
            if (result.error == ExecutionError::PC_OUT_OF_BOUNDS) {
                emit finished();
            }
            else if (result.error != ExecutionError::NONE && result.error != ExecutionError::PC_OUT_OF_BOUNDS) {
                emit errorOccurred(result);
            }
            return;
        }
        emit resultReady(result);

        sleep();
    }
}

void SimulationThread::sleep() const
{
    int elapsed = 0;
    while (m_running && elapsed < m_speed) {
        constexpr int sleepInterval = 50;
        msleep(sleepInterval);
        elapsed += sleepInterval;
    }
}

void SimulationThread::stop() { m_running = false; }
void SimulationThread::setSpeed(const int speed) { m_speed = speed; }
