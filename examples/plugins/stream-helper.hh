#pragma once

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

#include <clap/stream.h>

namespace clap {
   class Source : public boost::iostreams::source {
   public:
      explicit Source(clap_istream *is) : is_(is) {}

      std::streamsize read(char *s, std::streamsize n) noexcept { return is_->read(is_, s, n); }

   private:
      clap_istream *is_;
   };

   using IStream = boost::iostreams::stream<Source>;

   class Sink : public boost::iostreams::sink {
   public:
      explicit Sink(clap_ostream *os) : os_(os) {}

      std::streamsize write(const char *s, std::streamsize n) noexcept {
         return os_->write(os_, s, n);
      }

   private:
      clap_ostream *os_;
   };

   using OStream = boost::iostreams::stream<Sink>;
} // namespace clap