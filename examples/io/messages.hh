#include <stdint.h>

namespace clap::messages {

   enum Type : uint32_t {
      // DSP->GUI
      kDefineParameterRequest,

      // GUI->DSP
      kBeginAdjustRequest,
      kAdjustRequest,
      kEndAdjustRequest,

      // Gui, Host->Plugin
      kSetScaleRequest,
      kSizeRequest,
      kSizeResponse,
      kRoundSizeRequest,
      kRoundSizeResponse,
      kSetSizeRequest,
      kSetSizeResponse,
      kShowRequest,
      kShowResponse,
      kHideRequest,
      kHideResponse,
      kCloseRequest,

      // Gui, Plugin->Host
      kResizeRequest,
      kResizeResponse,
   };

   struct SetScaleRequest final {
      static const constexpr Type type = kSetScaleRequest;
      double scale;
   };

   struct SizeRequest final {
      static const constexpr Type type = kSizeRequest;
   };

   struct SizeResponse final {
      static const constexpr Type type = kSizeResponse;
      uint32_t width;
      uint32_t height;
   };

   struct RoundSizeRequest final {
      static const constexpr Type type = kRoundSizeRequest;
      uint32_t width;
      uint32_t height;
   };

   struct RoundSizeResponse final {
      static const constexpr Type type = kRoundSizeResponse;
      uint32_t width;
      uint32_t height;
   };

   struct SetSizeRequest final {
      static const constexpr Type type = kSetSizeRequest;
      uint32_t width;
      uint32_t height;
   };

   struct SetSizeResponse final {
      static const constexpr Type type = kSetSizeResponse;
   };

   struct ShowRequest final {
      static const constexpr Type type = kShowRequest;
   };

   struct ShowResponse final {
      static const constexpr Type type = kShowResponse;
   };

   struct HideRequest final {
      static const constexpr Type type = kHideRequest;
   };

   struct HideResponse final {
      static const constexpr Type type = kHideResponse;
   };

   struct CloseRequest final {
      static const constexpr Type type = kCloseRequest;
   };

   struct ResizeRequest final {
      static const constexpr Type type = kResizeRequest;
      uint32_t width;
      uint32_t height;
   };

   struct ResizeResponse final {
      static const constexpr Type type = kResizeResponse;
   };
} // namespace clap::messages