#include <nan.h>
#include <plugkit/dissector.h>
#include <plugkit/context.h>
#include <plugkit/token.h>
#include <plugkit/property.h>

#include <plugkit/variant.hpp>
#include <plugkit/layer.hpp>
#include <plugkit/fmt.hpp>
#include <unordered_map>

using namespace plugkit;

namespace {

const std::tuple<uint16_t, const char *, Token> flagTable[] = {
    std::make_tuple(0x1, "Reserved", Token_get("reserved")),
    std::make_tuple(0x2, "Don\'t Fragment", Token_get("dontFrag")),
    std::make_tuple(0x4, "More Fragments", Token_get("moreFrag")),
};

const std::unordered_map<uint16_t, std::pair<std::string, Token>> protoTable = {
    {0x01, std::make_pair("ICMP", Token_get("[icmp]"))},
    {0x02, std::make_pair("IGMP", Token_get("[igmp]"))},
    {0x06, std::make_pair("TCP", Token_get("[tcp]"))},
    {0x11, std::make_pair("UDP", Token_get("[udp]"))},
};

const auto ipv4Token = Token_get("ipv4");
const auto versionToken = Token_get("version");
const auto hLenToken = Token_get("hLen");
const auto typeToken = Token_get("type");
const auto tLenToken = Token_get("tLen");
const auto idToken = Token_get("id");
const auto flagsToken = Token_get("flags");
const auto fOffsetToken = Token_get("fOffset");
const auto ttlToken = Token_get("ttl");
const auto protocolToken = Token_get("protocol");
const auto unknownToken = Token_get("[unknown]");
const auto checksumToken = Token_get("checksum");
const auto srcToken = Token_get("src");
const auto dstToken = Token_get("dst");

void analyze(Context *ctx, Worker *data, Layer *layer) {
  fmt::Reader<Slice> reader(layer->payload());
  Layer *child = Layer_addLayer(layer, ipv4Token);
  Layer_addTag(child, ipv4Token);

  uint8_t header = reader.readBE<uint8_t>();
  int version = header >> 4;
  int headerLength = header & 0b00001111;

  Property *ver = Layer_addProperty(child, versionToken);
  *Property_valueRef(ver) = version;
  Property_setRange(ver, reader.lastRange());

  Property *hlen = Layer_addProperty(child, hLenToken);
  *Property_valueRef(hlen) = headerLength;
  Property_setRange(hlen, reader.lastRange());

  Property *tos = Layer_addProperty(child, typeToken);
  *Property_valueRef(tos) = reader.readBE<uint8_t>();
  Property_setRange(tos, reader.lastRange());

  uint16_t totalLength = reader.readBE<uint16_t>();
  Property *tlen = Layer_addProperty(child, tLenToken);
  *Property_valueRef(tlen) = totalLength;
  Property_setRange(tlen, reader.lastRange());

  Property *id = Layer_addProperty(child, idToken);
  *Property_valueRef(id) = reader.readBE<uint16_t>();
  Property_setRange(id, reader.lastRange());

  uint8_t flagAndOffset = reader.readBE<uint8_t>();
  uint8_t flag = (flagAndOffset >> 5) & 0b00000111;

  Property *flags = Layer_addProperty(child, flagsToken);
  *Property_valueRef(flags) = flag;
  std::string flagSummary;
  for (const auto &bit : flagTable) {
    bool on = std::get<0>(bit) & flag;
    Property *flagBit = Property_addProperty(flags, std::get<2>(bit));
    *Property_valueRef(flagBit) = on;
    Property_setRange(flagBit, reader.lastRange());

    if (on) {
      if (!flagSummary.empty())
        flagSummary += ", ";
      flagSummary += std::get<1>(bit);
    }
  }
  Property_setRange(flags, reader.lastRange());

  uint16_t fgOffset =
      ((flagAndOffset & 0b00011111) << 8) | reader.readBE<uint8_t>();
  Property *fragmentOffset = Layer_addProperty(child, fOffsetToken);
  *Property_valueRef(fragmentOffset) = fgOffset;
  Property_setRange(fragmentOffset, Range{6, 8});

  Property *ttl = Layer_addProperty(child, ttlToken);
  *Property_valueRef(ttl) = reader.readBE<uint8_t>();
  Property_setRange(ttl, reader.lastRange());

  uint8_t protocolNumber = reader.readBE<uint8_t>();
  Property *proto = Layer_addProperty(child, protocolToken);
  *Property_valueRef(proto) = protocolNumber;
  const auto &type = fmt::enums(protoTable, protocolNumber,
                                std::make_pair("Unknown", unknownToken));

  Property_setRange(proto, reader.lastRange());

  Layer_addTag(child, type.second);

  Property *checksum = Layer_addProperty(child, checksumToken);
  *Property_valueRef(checksum) = reader.readBE<uint16_t>();
  Property_setRange(checksum, reader.lastRange());

  const auto &srcSlice = reader.slice(4);
  Property *src = Layer_addProperty(child, srcToken);
  *Property_valueRef(src) = srcSlice;
  Property_setRange(src, reader.lastRange());

  const auto &dstSlice = reader.slice(4);
  Property *dst = Layer_addProperty(child, dstToken);
  *Property_valueRef(dst) = dstSlice;
  Property_setRange(dst, reader.lastRange());

  child->setPayload(reader.slice(totalLength - 20));
}
}

void Init(v8::Local<v8::Object> exports) {
  static Dissector diss;
  diss.layerHints[0] = Token_get("[ipv4]");
  diss.analyze = analyze;
  exports->Set(Nan::New("dissector").ToLocalChecked(),
               Nan::New<v8::External>(&diss));
}

NODE_MODULE(dissectorEssentials, Init);
