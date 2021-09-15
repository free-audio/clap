import QtQuick 2.1
import QtQuick.Controls 2.1
import clap 1.0

Rectangle {
    width: 400
    height: 600
    color: "#f8f8f8"

    Text {
        id: display
        text: computeText()
    }

    function computeText() {
        var text = "Has transport: " + transport.hasTransport + "\n";
        if (!transport.hasTransport)
            return text;

        text += "Is playing: " + transport.isPlaying + "\n";
        text += "Is recording: " + transport.isRecording + "\n";
        text += "Is loop active: " + transport.isLoopActive + "\n";

        if (transport.isLoopActive && transport.hasBeatsTimeline) {
            text += "Loop (beats): " + transport.loopStartBeats + " .. " + transport.loopEndBeats + "\n";
        }

        if (transport.isLoopActive && transport.hasSecondsTimeline) {
            text += "Loop (seconds): " + transport.loopStartSeconds + " .. " + transport.loopEndSeconds + "\n";
        }

        text += "Is within preroll: " + transport.isWithinPreRoll + "\n";

        text += "Has tempo: " + transport.hasTempo + "\n";
        if (transport.hasTempo)
            text += "Tempo (bpm): " + transport.tempo + "\n";

        text += "Has beats timeline: " + transport.hasBeatsTimeline + "\n";
        if (transport.hasBeatsTimeline)
            text += "song position (beats): " + transport.songPositionBeats + "\n";

        text += "Has seconds timeline: " + transport.hasSecondsTimeline + "\n";
        if (transport.hasSecondsTimeline)
            text += "song position (seconds): " + transport.songPositionSeconds + "\n";

        text += "Has Time Signature: " + transport.hasTimeSignature + "\n";
        if (transport.hasTimeSignature)
            text += "Time Signature: " + transport.timeSignatureNumerator + "/" + transport.timeSignatureDenominator + "\n";

        return text;
    }

    function updateText() {
        display.text = computeText();
    }

    Component.onCompleted: {
        transport.isSubscribed = true;
        transport.updated.connect(updateText);
    }
}
