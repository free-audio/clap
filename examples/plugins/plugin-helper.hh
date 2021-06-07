#pragma once

#include <clap-plugin.hh>

#include "parameters.hh"

namespace clap {
   class PluginHelper : public Plugin {
   public:
      PluginHelper(const clap_plugin_descriptor *desc, const clap_host *host);

   protected:
      // clap_plugin
      bool init() noexcept override;
      void initTrackInfo() noexcept;

      // clap_plugin_track_info
      bool implementsTrackInfo() const noexcept override { return true; }
      void trackInfoChanged() noexcept override;

      // clap_plugin_audio_ports
      bool implementsAudioPorts() const noexcept override;
      uint32_t audioPortsCount(bool is_input) const noexcept override;
      bool audioPortsInfo(uint32_t index,
                          bool is_input,
                          clap_audio_port_info *info) const noexcept override;
      uint32_t audioPortsConfigCount() const noexcept override;
      bool audioPortsGetConfig(uint32_t index,
                               clap_audio_ports_config *config) const noexcept override;
      bool audioPortsSetConfig(clap_id config_id) noexcept override;

      // clap_plugin_params
      bool implementsParams() const noexcept override { return true; }

      uint32_t paramsCount() const noexcept override { return parameters_.count(); }

      bool paramsInfo(int32_t paramIndex, clap_param_info *info) const noexcept override {
         *info = parameters_.getByIndex(paramIndex)->info;
         return true;
      }

      bool paramsEnumValue(clap_id paramId,
                           int32_t valueIndex,
                           clap_param_value *value) noexcept override {
         value->i = parameters_.getById(paramId)->enumDefinition.entries[valueIndex].value;
         return true;
      }

      virtual bool paramsValue(clap_id paramId, clap_param_value *value) noexcept override {
         *value = parameters_.getById(paramId)->value;
         return true;
      }

      virtual bool paramsSetValue(clap_id paramId,
                                  clap_param_value value,
                                  clap_param_value modulation) noexcept override {
         auto p = parameters_.getById(paramId);
         p->value = value;
         p->modulation = modulation;
         return false;
      }

      virtual bool paramsValueToText(clap_id paramId,
                                     clap_param_value value,
                                     char *display,
                                     uint32_t size) noexcept override {
         // TODO
         return false;
      }

      virtual bool paramsTextToValue(clap_id param_id,
                                     const char *display,
                                     clap_param_value *value) noexcept override {
         // TODO
         return false;
      }

      //////////////////////
      // Cached Host Info //
      //////////////////////
      bool hasTrackInfo() const noexcept { return hasTrackInfo_; }
      const clap_track_info &trackInfo() const noexcept {
         assert(hasTrackInfo_);
         return trackInfo_;
      }
      uint32_t trackChannelCount() const noexcept {
         return hasTrackInfo_ ? trackInfo_.channel_count : 2;
      }
      clap_chmap trackChannelMap() const noexcept {
         return hasTrackInfo_ ? trackInfo_.channel_map : CLAP_CHMAP_STEREO;
      }

   protected:
      bool hasTrackInfo_ = false;
      clap_track_info trackInfo_;

      std::vector<clap_audio_port_info> audioInputs_;
      std::vector<clap_audio_port_info> audioOutputs_;
      std::vector<clap_audio_ports_config> audioConfigs_;

      Parameters parameters_;
   };
} // namespace clap