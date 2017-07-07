#ifndef PLUGKIT_CHUNK_H
#define PLUGKIT_CHUNK_H

#include <memory>
#include <string>
#include <vector>
#include "export.hpp"
#include "strns.hpp"

namespace plugkit {

class Slice;

class Property;
using PropertyConstPtr = const Property *;

class Layer;
using LayerConstPtr = const Layer *;

class PLUGKIT_EXPORT Chunk final {
public:
  Chunk();
  Chunk(const strns &ns, const std::string &id, const Slice &payload);
  Chunk(Chunk &&chunk);
  ~Chunk();

  strns streamNs() const;
  void setStreamNs(const strns &ns);
  std::string streamId() const;
  void setStreamId(const std::string &id);

  const Slice &payload() const;
  void setPayload(const Slice &payload);

  const std::vector<PropertyConstPtr> &properties() const;
  PropertyConstPtr propertyFromId(strid id) const;
  void addProperty(const PropertyConstPtr &prop);

  LayerConstPtr layer() const;

private:
  Chunk(const Chunk &chunk) = delete;
  Chunk &operator=(const Chunk &chunk) = delete;

public:
  class Private;
  std::unique_ptr<Private> d;
};
}

#endif
