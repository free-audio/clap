#include "transport-proxy.hh"

TransportProxy::TransportProxy(QObject *parent) : QObject(parent) {}

void TransportProxy::update(bool hasTransport, const clap_event_transport &transport) {
   update(_hasTransport, hasTransport, &TransportProxy::hasTransportChanged);

   update<bool>(_hasBeatsTimeline,
                transport.flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE,
                &TransportProxy::hasBeatsTimelineChanged);
   update<bool>(_hasSecondsTimeline,
                transport.flags & CLAP_TRANSPORT_HAS_SECONDS_TIMELINE,
                &TransportProxy::hasSecondsTimelineChanged);
   update<bool>(_hasTimeSignature,
                transport.flags & CLAP_TRANSPORT_HAS_TIME_SIGNATURE,
                &TransportProxy::hasTimeSignatureChanged);
   update<bool>(
      _isPlaying, transport.flags & CLAP_TRANSPORT_IS_PLAYING, &TransportProxy::isPlayingChanged);
   update<bool>(_isRecording,
                transport.flags & CLAP_TRANSPORT_IS_RECORDING,
                &TransportProxy::isRecordingChanged);
   update<bool>(_isLoopActive,
                transport.flags & CLAP_TRANSPORT_IS_LOOP_ACTIVE,
                &TransportProxy::isLoopActiveChanged);
   update<bool>(_isWithinPreRoll,
                transport.flags & CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL,
                &TransportProxy::isWithinPreRollChanged);

   update(_songPositionBeats,
          transport.song_pos_beats / double(CLAP_BEATTIME_FACTOR),
          &TransportProxy::songPositionBeatsChanged);
   update(_songPositionSeconds,
          transport.song_pos_seconds / double(CLAP_SECTIME_FACTOR),
          &TransportProxy::songPositionSecondsChanged);

   update(_tempo, transport.tempo, &TransportProxy::tempoChanged);

   update(_barStart,
          transport.bar_start / double(CLAP_BEATTIME_FACTOR),
          &TransportProxy::barStartChanged);
   update(_barNumber, transport.bar_number, &TransportProxy::barNumberChanged);

   update(_loopStartBeats,
          transport.loop_start_beats / double(CLAP_BEATTIME_FACTOR),
          &TransportProxy::loopStartBeatsChanged);
   update(_loopEndBeats,
          transport.loop_end_beats / double(CLAP_BEATTIME_FACTOR),
          &TransportProxy::loopEndBeatsChanged);
   update(_loopStartSeconds,
          transport.loop_start_seconds / double(CLAP_SECTIME_FACTOR),
          &TransportProxy::loopStartSecondsChanged);
   update(_loopEndSeconds,
          transport.loop_end_seconds / double(CLAP_SECTIME_FACTOR),
          &TransportProxy::loopEndSecondsChanged);

   update<int>(
      _timeSignatureNumerator, transport.tsig_num, &TransportProxy::timeSignatureNumeratorChanged);
   update<int>(_timeSignatureDenominator,
               transport.tsig_denom,
               &TransportProxy::timeSignatureDenominatorChanged);
}