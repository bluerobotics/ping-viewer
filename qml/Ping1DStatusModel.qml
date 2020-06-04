import QtQml.Models 2.12
import QtQuick 2.7

Item {
    property var title: "Ping1D specific data:"
    property var sensor: null
    // It's not possible to use DelegateModel without parent
    property var model

    model: DelegateModel {
        id: delegateModel

        delegate: Text {
            text: modelData
            color: "white"
            font.pointSize: 8
        }

    }

    Timer {
        interval: 200
        running: true
        repeat: true
        // Limit the frequency update of the model from the sensor properties
        onTriggered: {
            if (!sensor)
                return ;

            delegateModel.model = ["Distance (mm): " + sensor.distance, "Auto (bool): " + sensor.mode_auto, "Scan Start (mm): " + sensor.start_mm, "Scan Length (mm): " + sensor.length_mm, "Ping (#): " + sensor.ping_number, "Transmit duration (μs): " + sensor.transmit_duration, "Ping interval (ms): " + sensor.ping_interval, "Gain (setting): " + sensor.gain_setting, "Confidence (%): " + sensor.confidence, "Speed of sound (mm/s): " + sensor.speed_of_sound, "Processor temperature (C): " + (sensor.processor_temperature / 100).toFixed(1), "PCB temperature (C): " + (sensor.pcb_temperature / 100).toFixed(1), "Board voltage (V): " + (sensor.board_voltage / 1000).toFixed(2)];
        }
    }

}
