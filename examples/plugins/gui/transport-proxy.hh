#pragma once

#include <QObject>

#include <clap/all.h>

class TransportProxy : public QObject {
   Q_OBJECT
   Q_PROPERTY(bool hasTransport READ hasTransport NOTIFY hasTransportChanged)

   Q_PROPERTY(bool hasBeatsTimeline READ hasBeatsTimeline NOTIFY hasBeatsTimelineChanged)
   Q_PROPERTY(bool hasSecondsTimeline READ hasSecondsTimeline NOTIFY hasSecondsTimelineChanged)
   Q_PROPERTY(bool hasTimeSignature READ hasTimeSignature NOTIFY hasTimeSignatureChanged)
   Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
   Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)
   Q_PROPERTY(bool isLoopActive READ isLoopActive NOTIFY isLoopActiveChanged)
   Q_PROPERTY(bool isWithinPreRoll READ isWithinPreRoll NOTIFY isWithinPreRollChanged)

   Q_PROPERTY(double songPositionBeats READ getSongPositionBeats NOTIFY songPositionBeatsChanged)
   Q_PROPERTY(
      double songPositionSeconds READ getSongPositionSeconds NOTIFY songPositionSecondsChanged)

   Q_PROPERTY(double tempo READ getTempo NOTIFY tempoChanged)

   Q_PROPERTY(double barStart READ getBarStart NOTIFY barStartChanged)
   Q_PROPERTY(int barNumber READ getBarNumber NOTIFY barNumberChanged)

   Q_PROPERTY(double loopStartBeats READ getLoopStartBeats NOTIFY loopStartBeatsChanged)
   Q_PROPERTY(double loopEndBeats READ getLoopEndBeats NOTIFY loopEndBeatsChanged)
   Q_PROPERTY(double loopStartSeconds READ getLoopStartSeconds NOTIFY loopStartSecondsChanged)
   Q_PROPERTY(double loopEndSeconds READ getLoopEndSeconds NOTIFY loopEndSecondsChanged)

   Q_PROPERTY(int timeSignatureNumerator READ getTimeSignatureNumerator NOTIFY
                 timeSignatureNumeratorChanged)
   Q_PROPERTY(int timeSignatureDenominator READ getTimeSignatureDenominator NOTIFY
                 timeSignatureDenominatorChanged)

public:
   explicit TransportProxy(QObject *parent = nullptr);

   void update(bool hasTransport, const clap_event_transport &transport);

   [[nodiscard]] bool hasTransport() const noexcept { return _hasTransport; }

   [[nodiscard]] bool hasBeatsTimeline() const noexcept { return _hasBeatsTimeline; }

   [[nodiscard]] bool hasSecondsTimeline() const noexcept { return _hasSecondsTimeline; }

   [[nodiscard]] bool hasTimeSignature() const noexcept { return _hasTimeSignature; }

   [[nodiscard]] bool isPlaying() const noexcept { return _isPlaying; }

   [[nodiscard]] bool isRecording() const noexcept { return _isRecording; }

   [[nodiscard]] bool isLoopActive() const noexcept { return _isLoopActive; }

   [[nodiscard]] bool isWithinPreRoll() const noexcept { return _isWithinPreRoll; }

   [[nodiscard]] double getSongPositionBeats() const noexcept { return _songPositionBeats; }

   [[nodiscard]] double getSongPositionSeconds() const noexcept { return _songPositionSeconds; }

   [[nodiscard]] double getTempo() const noexcept { return _tempo; }

   [[nodiscard]] double getBarStart() const noexcept { return _barStart; }

   [[nodiscard]] int getBarNumber() const noexcept { return _barNumber; }

   [[nodiscard]] double getLoopStartBeats() const noexcept { return _loopStartBeats; }

   [[nodiscard]] double getLoopEndBeats() const noexcept { return _loopEndBeats; }

   [[nodiscard]] double getLoopStartSeconds() const noexcept { return _loopStartSeconds; }

   [[nodiscard]] double getLoopEndSeconds() const noexcept { return _loopEndSeconds; }

   [[nodiscard]] int getTimeSignatureNumerator() const noexcept { return _timeSignatureNumerator; }

   [[nodiscard]] int getTimeSignatureDenominator() const noexcept {
      return _timeSignatureDenominator;
   }

signals:
   void updated();

   void hasTransportChanged();

   void hasBeatsTimelineChanged();
   void hasSecondsTimelineChanged();
   void hasTimeSignatureChanged();
   void isPlayingChanged();
   void isRecordingChanged();
   void isLoopActiveChanged();
   void isWithinPreRollChanged();

   void songPositionBeatsChanged();
   void songPositionSecondsChanged();

   void tempoChanged();

   void barStartChanged();
   void barNumberChanged();

   void loopStartBeatsChanged();
   void loopEndBeatsChanged();
   void loopStartSecondsChanged();
   void loopEndSecondsChanged();

   void timeSignatureNumeratorChanged();
   void timeSignatureDenominatorChanged();

private:

    using NotifyType = void (TransportProxy::*)();

    template <typename T>
    void update(T& attr, T value, NotifyType notify)
    {
        if (value == attr)
            return;
        attr = value;
        (this->*notify)();
    }

   bool _hasTransport = false;

   bool _hasBeatsTimeline = false;
   bool _hasSecondsTimeline = false;
   bool _hasTimeSignature = false;
   bool _isPlaying = false;
   bool _isRecording = false;
   bool _isLoopActive = false;
   bool _isWithinPreRoll = false;

   double _songPositionBeats = 0;
   double _songPositionSeconds = 0;

   double _tempo = 0;

   double _barStart = 0;
   int _barNumber = 0;

   double _loopStartBeats = 0;
   double _loopEndBeats = 0;
   double _loopStartSeconds = 0;
   double _loopEndSeconds = 0;

   int _timeSignatureNumerator = 0;
   int _timeSignatureDenominator = 0;
};