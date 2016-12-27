#ifndef COMMON_H
#define COMMON_H

#include "song.h"
#include "album.h"

#include <boost/variant.hpp>

typedef boost::variant<Album, Song> MusicType;

#endif // COMMON_H
