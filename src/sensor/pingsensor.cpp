#include "logger.h"
#include "pingsensor.h"
#include "ping-message-common.h"
#include "ping-message-ping1d.h"

PING_LOGGING_CATEGORY(PING_PROTOCOL_PINGSENSOR, "ping.protocol.pingsensor")

PingSensor::PingSensor()
    :Sensor()
    ,_lostMessages(0)
{
    _parser = new PingParserExt();
    connect(dynamic_cast<PingParserExt*>(_parser), &PingParserExt::newMessage, this, &PingSensor::handleMessagePrivate,
            Qt::DirectConnection);
    connect(dynamic_cast<PingParserExt*>(_parser), &PingParserExt::parseError, this, &PingSensor::parserErrorsUpdate);
}


void PingSensor::request(int id) const
{
    if(!link()->isWritable()) {
        qCWarning(PING_PROTOCOL_PINGSENSOR) << "Can't write in this type of link.";
        return;
    }
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "Requesting:" << id;

    ping_message m(10);
    m.set_payload_length(0);
    m.set_message_id(id);
    m.updateChecksum();
    writeMessage(m);
}

void PingSensor::writeMessage(const ping_message& msg) const
{
    if(link() && link()->isOpen() && link()->isWritable()) {
        link()->write(reinterpret_cast<const char*>(msg.msgData), msg.msgDataLength());
    }
}

void PingSensor::handleMessagePrivate(const ping_message& msg)
{
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "Handling Message:" << msg.message_id();

    if(_dstId != msg.destination_device_id()) {
        _dstId = msg.destination_device_id();
        emit dstIdUpdate();
    }

    if(_srcId != msg.source_device_id()) {
        _srcId = msg.source_device_id();
        emit srcIdUpdate();
    }

    switch (msg.message_id()) {

    case CommonId::ACK: {
        common_ack ackMessage{msg};
        qCDebug(PING_PROTOCOL_PINGSENSOR) << "ACK message:" << ackMessage.acked_id();
        break;
    }

    case CommonId::NACK: {
        common_nack nackMessage{msg};
        qCCritical(PING_PROTOCOL_PINGSENSOR) << "Sensor NACK!";
        _nack_msg = QString("%1: %2").arg(nackMessage.nack_message()).arg(nackMessage.nacked_id());
        qCDebug(PING_PROTOCOL_PINGSENSOR) << "NACK message:" << _nack_msg;
        emit nackMsgUpdate();
        break;
    }

    // needs dynamic-payload patch
    case CommonId::ASCII_TEXT: {
        _ascii_text = common_ascii_text(msg).ascii_message();
        qCInfo(PING_PROTOCOL_PINGSENSOR) << "Sensor status:" << _ascii_text;
        emit asciiTextUpdate();
        break;
    }

    case CommonId::DEVICE_INFORMATION: {
        common_device_information m(msg);

        _device_type = m.device_type();
        _device_revision = m.device_revision();
        _firmware_version_major = m.firmware_version_major();
        _firmware_version_minor = m.firmware_version_minor();
        _firmware_version_patch = m.firmware_version_patch();

        emit deviceTypeUpdate();
        emit deviceRevisionUpdate();
        emit firmwareVersionMajorUpdate();
        emit firmwareVersionMinorUpdate();
        emit firmwareVersionPatchUpdate();
        break;
    }

    case CommonId::PROTOCOL_VERSION: {
        common_protocol_version m(msg);

        _protocol_version_major = m.version_major();
        _protocol_version_minor = m.version_minor();
        _protocol_version_patch = m.version_patch();

        emit protocolVersionMajorUpdate();
        emit protocolVersionMinorUpdate();
        emit protocolVersionPatchUpdate();
        break;
    }

    //Will be deprecated in future firmware versions of Ping1D
    case Ping1dId::FIRMWARE_VERSION: {
        ping1d_firmware_version m(msg);

        m.device_type();
        // Ping1D uses device_model as device_type to specify which sersion is it
        _device_type = m.device_model();
        _firmware_version_major = m.firmware_version_major();
        _firmware_version_minor = m.firmware_version_minor();

        emit deviceTypeUpdate();
        emit firmwareVersionMajorUpdate();
        emit firmwareVersionMinorUpdate();
        break;
    }

    default:
        // Not a common message
        break;
    }

    handleMessage(msg);
}

void PingSensor::printStatus() const
{
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "PingSensor Status:";
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- srcId:" << _srcId;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- dstID:" << _dstId;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- device_type:" << _device_type;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- device_revision:" << _device_revision;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- protocol_version_major:" << _protocol_version_major;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- protocol_version_minor:" << _protocol_version_minor;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- protocol_version_patch:" << _protocol_version_patch;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- firmware_version_major:" << _firmware_version_major;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- firmware_version_minor:" << _firmware_version_minor;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- firmware_version_patch:" << _firmware_version_patch;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- ascii_text:" << _ascii_text;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- nack_msg:" << _nack_msg;
    qCDebug(PING_PROTOCOL_PINGSENSOR) << "\t- lostMessages:" << _lostMessages;
    printSensorInformation();
}

PingSensor::~PingSensor() = default;
