#include "parameter-proxy.hh"

void ParameterProxy::setValue(double value) {
   // TODO: send notification
   value_ = value;
}

void ParameterProxy::setModulation(double mod) {
   // TODO send notification
   modulation_ = mod;
}
