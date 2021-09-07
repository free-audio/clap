#pragma once

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

#include <clap/stream.h>

namespace clap {
   class Source : public boost::iostreams::source {
   public:
      explicit Source(clap_istream *is) : _is(is) {}

      std::streamsize read(char *s, std::streamsize n) noexcept { return _is->read(_is, s, n); }

   private:
      clap_istream *_is;
   };

   using IStream = boost::iostreams::stream<Source>;

   class Sink : public boost::iostreams::sink {
   public:
      explicit Sink(clap_ostream *os) : _os(os) {}

      std::streamsize write(const char *s, std::streamsize n) noexcept {
         return _os->write(_os, s, n);
      }

   private:
      clap_ostream *_os;
   };

   using OStream = boost::iostreams::stream<Sink>;
} // namespace clap