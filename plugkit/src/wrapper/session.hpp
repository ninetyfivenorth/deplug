#ifndef PLUGKIT_SESSION_WRAPPER_H
#define PLUGKIT_SESSION_WRAPPER_H

#include <memory>
#include <nan.h>

namespace plugkit {

class Session;

class SessionWrapper final : public Nan::ObjectWrap {
public:
  ~SessionWrapper();
  static void init(v8::Isolate *isolate);
  static v8::Local<v8::Object> wrap(const std::shared_ptr<Session> &session);
  static std::shared_ptr<const Session> unwrap(v8::Local<v8::Value> value);
  static NAN_METHOD(New);
  static NAN_GETTER(networkInterface);
  static NAN_GETTER(promiscuous);
  static NAN_GETTER(snaplen);
  static NAN_METHOD(startPcap);
  static NAN_METHOD(stopPcap);
  static NAN_GETTER(id);
  static NAN_METHOD(destroy);
  static NAN_METHOD(getFilteredFrames);
  static NAN_METHOD(getFrames);
  static NAN_METHOD(importFile);
  static NAN_METHOD(exportFile);
  static NAN_GETTER(inspectors);
  static NAN_METHOD(sendInspectorMessage);
  static NAN_METHOD(setDisplayFilter);
  static NAN_METHOD(setStatusCallback);
  static NAN_METHOD(setFilterCallback);
  static NAN_METHOD(setFrameCallback);
  static NAN_METHOD(setLoggerCallback);
  static NAN_METHOD(setEventCallback);
  static NAN_METHOD(setInspectorCallback);

private:
  SessionWrapper(const std::shared_ptr<Session> &session);
  SessionWrapper(const SessionWrapper &) = delete;
  SessionWrapper &operator=(const SessionWrapper &) = delete;

private:
  std::shared_ptr<Session> session;
  v8::UniquePersistent<v8::Function> statusCallback;
  v8::UniquePersistent<v8::Function> filterCallback;
  v8::UniquePersistent<v8::Function> frameCallback;
  v8::UniquePersistent<v8::Function> loggerCallback;
  v8::UniquePersistent<v8::Function> eventCallback;
  v8::UniquePersistent<v8::Function> inspectorCallback;
};
} // namespace plugkit

#endif
