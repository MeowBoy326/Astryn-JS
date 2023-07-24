#include <Windows.h>
#include <node.h>

// https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes

namespace keyboard {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void KeyEvent(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int wVk = args[0].As<Number>()->Value();
  keybd_event(wVk, 0, 0, 0);
  keybd_event(wVk, 0, KEYEVENTF_KEYUP, 0);
}

void KeyDown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int wVk = args[0].As<Number>()->Value();
  keybd_event(wVk, 0, 0, 0);
}

void KeyUP(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int wVk = args[0].As<Number>()->Value();
  keybd_event(wVk, 0, KEYEVENTF_KEYUP, 0);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "_keyEvent", KeyEvent);
  NODE_SET_METHOD(exports, "_keyDown", KeyDown);
  NODE_SET_METHOD(exports, "_keyUP", KeyUP);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}
