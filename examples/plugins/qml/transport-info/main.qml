import QtQuick 2.1
import QtQuick.Controls 2.1
import clap 1.0

Rectangle {
    width: 400
    height: 600
    color: "#f8f8f8"

    Text {
        text: "Has transport: " + transport.hasTransport + "\n"
            + (!transport.hasTransport ? "" :
                + "Is playing: " + transport.isPlaying + "\n"
                + "Is recording: " + transport.isRecording + "\n"
                + "Is loop active: " + transport.isLoopActive + "\n"
                + (!(transport.isLoopActive && transport.hasBeatsTimeline) ? "" :
                    + "Loop (beats): " + transport.loopStartBeats + " .. " + transport.loopEndBeats)
                + (!(transport.isLoopActive && transport.hasSecondsTimeline) ? "" :
                    + "Loop (seconds): " + transport.loopStartSeconds + " .. " + transport.loopEndSeconds)
                + "Is within preroll: " + transport.isWithinPreroll + "\n"
                + "Has tempo: " + transport.hasTempo + "\n"
                + (!transport.hasTempo ? "" :
                    + "Tempo (bpm): " + transport.tempo)
                + "Has beats timeline: " + transport.hasBeatsTimeline + "\n"
                + (!transport.hasBeatsTimeline ? "" :
                    + "song position (beats): " + transport.songPositionBeats)
                + (!transport.hasSecondsTimeline ? "" :
                    + "song position (seconds): " + transport.songPositionSeconds)
                + "Has Time Signature: " + transport.hasTimeSignature + "\n"
                + (!transport.hasTimeSignature ? "" :
                    + "Time Signature: " + transport.timeSignatureNumerator + "/" + transport.timeSignatureDenominator)
            )
    }

    Component.onCompleted: {
        transport.isSubscribed = true;
    }
}
