#pragma once

#include <clap/plugin.h>
#include <clap/string-sizes.h>

static CLAP_CONSTEXPR const char CLAP_EXT_LICENSE[] = "clap.license/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page License
///
/// This extension lets a host show what a plugin's licence is, and lets the user register a
/// commercial plugin that runs as a demo from within the host, without opening the plugin's own
/// UI. It is meant for hosts where the plugin's UI is not available or not practical (embedded
/// devices, hardware instruments, headless or remote hosts), and for hosts that list plugins and
/// want to tag them free, open source, demo or registered.
///
/// A plugin without this extension is shown with no licence information at all.
///
/// The plugin side, called by the host:
/// - get(): free, open source (with the licence's SPDX identifier and, optionally, a URL), or
///   commercial, in demo, registered or expired. Where to buy a commercial plugin is the plugin
///   descriptor's url.
/// - install(): a commercial plugin that is not registered is handed what the user brought: a
///   file they picked (its path and its contents), or a key they typed (a serial number, a key in
///   base64). The plugin installs it wherever it keeps its licences; the host never writes it.
///   Whether it worked is get()'s to say: the host calls get() again, and REGISTERED is the only
///   proof.
///
/// The host side, called by the plugin:
/// - expired(): the demo ran out while the plugin runs, with a message for the user.
///
/// Showing a licence: get() may name the installed licence file. A host may show it: as an image
/// when it is one, as text when it is text, and otherwise as its first bytes in hex. A licence
/// file can carry personal data (a name, an email, an address), so a host should show it only
/// when the user asks, and confirm the first time.
///
/// Under discussion, and called by no host yet: registering by challenge and response (a code
/// shown on the device, finished on a phone), registering online, and the host giving the plugin
/// an ID for the machine it runs on. Each is marked "Draft, not settled" below. A plugin may
/// implement them; a host calls them only once they are settled.

enum clap_license_kind {
   CLAP_LICENSE_FREE = 0,        // free of charge, on terms of its own
   CLAP_LICENSE_OPEN_SOURCE = 1, // an open source licence
   CLAP_LICENSE_COMMERCIAL = 2,  // sold
};

enum clap_license_state {
   CLAP_LICENSE_DEMO = 0,        // runs, with the limits of a demo
   CLAP_LICENSE_REGISTERED = 1,
   CLAP_LICENSE_EXPIRED = 2,     // a demo whose time ran out: registering is all it offers
};

// What a commercial plugin that is not registered takes to register. A bit field.
enum clap_license_register_with {
   CLAP_LICENSE_WITH_FILE = 1 << 0, // install() with a file the user picks
   CLAP_LICENSE_WITH_KEY = 1 << 1,  // install() with a string the user types

   // Draft, not settled: offline challenge and response. The host shows get_challenge()'s text,
   // also as a QR code, to be finished on a phone at the vendor's site; the response the user
   // brings back goes to install() as a key. For a device that is never online, the response
   // should be short enough to type without a keyboard: base32 in groups rather than base64.
   CLAP_LICENSE_WITH_CHALLENGE = 1 << 2,

   // Draft, not settled: online. The user's email or customer number and the machine's ID go to
   // the vendor, after a screen that says what goes where, and the licence comes back. Open:
   // whether the plugin makes the request itself, or the host carries it (online_request() builds
   // it, the host sends it to the plugin's URL and hands the reply to install()). The latter keeps
   // TLS, proxies and "no network" in the host, and lets the consent screen say exactly where the
   // data goes.
   CLAP_LICENSE_WITH_ONLINE = 1 << 3,
};

typedef struct clap_license_info {
   uint32_t kind; // see enum clap_license_kind

   // FREE and OPEN_SOURCE
   // OPEN_SOURCE: the licence as SPDX names it, an identifier from https://spdx.org/licenses/
   // ("MIT", "GPL-3.0-or-later"), or an SPDX license expression ("MIT OR Apache-2.0"), exactly as
   // written there. FREE: the terms' own name, eg "Freeware".
   char name[CLAP_NAME_SIZE];
   char url[CLAP_PATH_SIZE]; // the licence's text; optional, "" for none

   // COMMERCIAL
   uint32_t state;                // see enum clap_license_state
   char     licensee[CLAP_NAME_SIZE]; // REGISTERED: who to, eg "Jane Doe (NFR)"; may be ""
   char     file[CLAP_PATH_SIZE];     // REGISTERED: the installed licence file, for the host to
                                      // show; may be ""

   // COMMERCIAL in DEMO or EXPIRED: how it registers
   uint32_t register_with;                // see enum clap_license_register_with; 0 means every
                                          // way whose function the plugin implements
   char     file_extension[CLAP_NAME_SIZE]; // WITH_FILE: the licence file's extension, so the host
                                            // offers only those: "png", or a list, "png;lic".
                                            // "" offers every file.
   char     key_hint[CLAP_NAME_SIZE];       // WITH_KEY: what a key looks like, shown where it is
                                            // typed: "XXXX-XXXX-XXXX-XXXX"; may be ""

   // COMMERCIAL in DEMO or EXPIRED: what to tell the user, UTF-8, one or two short sentences:
   // "Demo: silent for a moment every minute", "The demo has ended."
   char message[CLAP_NAME_SIZE];
} clap_license_info_t;

typedef struct clap_plugin_license {
   // Fills info. The host zeroes it first, except kind, which it sets to a value that is none of
   // enum clap_license_kind: an answer that does not set kind is no answer.
   // Returns false when the plugin has nothing to say.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, clap_license_info_t *info);

   // COMMERCIAL in DEMO or EXPIRED, and only when the user asked to register: what they brought,
   // which the plugin installs as a licence if it is one.
   //   a file (WITH_FILE)  file_path: the file the user picked, absolute, UTF-8, to read, copy or
   //                       hand to the plugin's own installer; data and size: its contents as
   //                       the host read them, to use instead
   //   a key (WITH_KEY)    file_path: NULL; data: what the user typed, UTF-8, as typed (the plugin
   //                       trims what it likes), size bytes, null-terminated after
   // Returns true when it took it. The host then calls get(), whose REGISTERED, and the licence
   // file it names, are the proof and all the host records. False leaves everything as it was.
   // [main-thread]
   bool(CLAP_ABI *install)(const clap_plugin_t *plugin,
                           const char          *file_path,
                           const void          *data,
                           uint32_t             size);

   // Draft, not settled (WITH_CHALLENGE): the challenge to show, UTF-8, null-terminated, built from
   // the machine's ID (clap_host_license->get_machine_id()) and a nonce the plugin keeps until a
   // response comes back through install(), as a key. Returns false when it cannot make one.
   // [main-thread]
   bool(CLAP_ABI *get_challenge)(const clap_plugin_t *plugin, char *challenge, uint32_t capacity);

   // Draft, not settled (WITH_ONLINE, and only if the host carries the request): the request to
   // send for this customer (an email or customer number, UTF-8): its URL and its body, in the
   // plugin's own format. The host sends it and hands the reply to install().
   // [main-thread]
   bool(CLAP_ABI *online_request)(const clap_plugin_t *plugin,
                                  const char          *customer,
                                  char                *url,
                                  uint32_t             url_capacity,
                                  void                *body,
                                  uint32_t            *body_size);
} clap_plugin_license_t;

typedef struct clap_host_license {
   // The demo ran out while the plugin runs. message is what to tell the user, UTF-8, one or two
   // short sentences; the host shows it, and may call get() again from the main thread. The plugin
   // goes on running, silent or limited as its demo is.
   // [thread-safe]
   void(CLAP_ABI *expired)(const clap_host_t *host, const char *message);

   // Draft, not settled (may be NULL): an ID for the machine the host runs on, for binding a
   // licence to it, as hex, null-terminated. Never the raw hardware identifiers: derived per
   // vendor (eg an HMAC of the hardware's unique ID keyed with the plugin's vendor), so that it is
   // stable for one vendor and two vendors cannot match theirs. A fingerprint, not a proof.
   // Returns false when the machine has no usable ID.
   // [thread-safe]
   bool(CLAP_ABI *get_machine_id)(const clap_host_t *host, char *id, uint32_t capacity);
} clap_host_license_t;

#ifdef __cplusplus
}
#endif
