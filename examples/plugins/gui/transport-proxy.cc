#include "transport-proxy.hh"
#include "../io/messages.hh"
#include "application.hh"

TransportProxy::TransportProxy(QObject *parent) : QObject(parent) {}

void TransportProxy::update(bool hasTransport, const clap_event_transport &t) {
   update(_hasTransport, hasTransport, &TransportProxy::hasTransportChanged);

   update<bool>(_hasTempo, t.flags & CLAP_TRANSPORT_HAS_TEMPO, &TransportProxy::hasTempoChanged);
   update<bool>(_hasBeatsTimeline,
                t.flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE,
                &TransportProxy::hasBeatsTimelineChanged);
   update<bool>(_hasSecondsTimeline,
                t.flags & CLAP_TRANSPORT_HAS_SECONDS_TIMELINE,
                &TransportProxy::hasSecondsTimelineChanged);
   update<bool>(_hasTimeSignature,
                t.flags & CLAP_TRANSPORT_HAS_TIME_SIGNATURE,
                &TransportProxy::hasTimeSignatureChanged);
   update<bool>(_isPlaying, t.flags & CLAP_TRANSPORT_IS_PLAYING, &TransportProxy::isPlayingChanged);
   update<bool>(
      _isRecording, t.flags & CLAP_TRANSPORT_IS_RECORDING, &TransportProxy::isRecordingChanged);
   update<bool>(
      _isLoopActive, t.flags & CLAP_TRANSPORT_IS_LOOP_ACTIVE, &TransportProxy::isLoopActiveChanged);
   update<bool>(_isWithinPreRoll,
                t.flags & CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL,
                &TransportProxy::isWithinPreRollChanged);

   update<double>(_songPositionBeats,
                  t.song_pos_beats / double(CLAP_BEATTIME_FACTOR),
                  &TransportProxy::songPositionBeatsChanged);
   update<double>(_songPositionSeconds,
                  t.song_pos_seconds / double(CLAP_SECTIME_FACTOR),
                  &TransportProxy::songPositionSecondsChanged);

   update<double>(_tempo, t.tempo, &TransportProxy::tempoChanged);

   update<double>(
      _barStart, t.bar_start / double(CLAP_BEATTIME_FACTOR), &TransportProxy::barStartChanged);
   update<int>(_barNumber, t.bar_number, &TransportProxy::barNumberChanged);

   update<double>(_loopStartBeats,
                  t.loop_start_beats / double(CLAP_BEATTIME_FACTOR),
                  &TransportProxy::loopStartBeatsChanged);
   update<double>(_loopEndBeats,
                  t.loop_end_beats / double(CLAP_BEATTIME_FACTOR),
                  &TransportProxy::loopEndBeatsChanged);
   update<double>(_loopStartSeconds,
                  t.loop_start_seconds / double(CLAP_SECTIME_FACTOR),
                  &TransportProxy::loopStartSecondsChanged);
   update<double>(_loopEndSeconds,
                  t.loop_end_seconds / double(CLAP_SECTIME_FACTOR),
                  &TransportProxy::loopEndSecondsChanged);

   update<int>(_timeSignatureNumerator, t.tsig_num, &TransportProxy::timeSignatureNumeratorChanged);
   update<int>(
      _timeSignatureDenominator, t.tsig_denom, &TransportProxy::timeSignatureDenominatorChanged);

   emit updated();
}

void TransportProxy::setIsSubscribed(bool value) {
   if (value == _isSubscribed)
      return;

   _isSubscribed = value;
   isSubscribedChanged();
   clap::messages::SubscribeToTransportRequest rq{value};
   Application::instance().remoteChannel().sendRequestAsync(rq);
}
