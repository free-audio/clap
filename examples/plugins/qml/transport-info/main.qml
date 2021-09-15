import QtQuick 2.1
import QtQuick.Controls 2.1
import clap 1.0

Rectangle {
    width: 450
    height: 470
    color: "#f8f8f8"

    Text {
        id: display
        text: computeText()
        font.pointSize: 20
    }

    function computeText() {
        if (!transport.hasTransport)
            return "Free Running";

        var text = "";
        text += "Time Signature: " + computeTimeSignature() + "\n";
        text += "Tempo: " + (transport.hasTempo ? (transport.tempo.toFixed(3) + " (bpm)") : "(none)") + "\n";

        if (transport.hasBeatsTimeline)
            text += "song position (beats): " + transport.songPositionBeats.toFixed(3) + "\n";
        else
            text += "No timeline in beats\n";

        if (transport.hasSecondsTimeline)
            text += "song position (seconds): " + transport.songPositionSeconds.toFixed(3) + "s\n";
        else
            text += "No timeline in seconds\n";

        text += "\n";

        text += "Is playing: " + transport.isPlaying + "\n";
        text += "Is recording: " + transport.isRecording + "\n";
        text += "Is within preroll: " + transport.isWithinPreRoll + "\n";
        text += "\n";

        if (transport.isLoopActive) {
            if (transport.hasBeatsTimeline)
                text += "Loop (beats): " + transport.loopStartBeats.toFixed(3) + " .. " + transport.loopEndBeats.toFixed(3) + "\n";

            if (transport.hasSecondsTimeline)
                text += "Loop (seconds): " + transport.loopStartSeconds.toFixed(3) + "s .. " + transport.loopEndSeconds.toFixed(3) + "s\n";

            text += "\n";
        }

        return text;
    }

    function computeTimeSignature() {
        if (transport.hasTimeSignature)
            return transport.timeSignatureNumerator + "/" + transport.timeSignatureDenominator;
        return "(none)";
    }

    function updateText() {
        display.text = computeText();
    }

    Component.onCompleted: {
        transport.isSubscribed = true;
        transport.updated.connect(updateText);
    }
}
