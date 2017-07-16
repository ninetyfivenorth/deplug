#ifndef PLUGKIT_TYPES_H
#define PLUGKIT_TYPES_H

#include <chrono>
#include <memory>

namespace plugkit {

class Layer;
class Property;
class Frame;
class FrameView;
class Slice;
class Chunk;
class Plugin;
struct SessionContext;

class Logger;
using LoggerPtr = std::shared_ptr<Logger>;

using Range = std::pair<uint32_t, uint32_t>;

using Timestamp = std::chrono::time_point<std::chrono::system_clock,
                                          std::chrono::nanoseconds>;
}

#endif