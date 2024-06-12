#include "ping360flasher.h"

Ping360Flasher::Ping360Flasher(QObject* parent)
    : Flasher(parent, {115200})
{
    connect(
        &_worker, &Ping360FlashWorker::flashProgressChanged, this,
        [this](float flashProgressPct) { emit flashProgress(flashProgressPct); }, Qt::QueuedConnection);
    connect(
        &_worker, &Ping360FlashWorker::stateChanged, this, [this](Flasher::States newState) { setState(newState); },
        Qt::QueuedConnection);
    connect(
        &_worker, &Ping360FlashWorker::messageChanged, this, [this](QString message) { setMessage(message); },
        Qt::QueuedConnection);
}

void Ping360Flasher::flash()
{
    _worker.setBaudRate(_baudRate);
    _worker.setFirmwarePath(_firmwareFilePath);
    _worker.setLink(_link);
    _worker.setVerify(_verify);
    _worker.start();
}
