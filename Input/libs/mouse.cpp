#include <Windows.h>
#include <node.h>
// https://www.5axxw.com/questions/simple/zhlieo

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void MouseEvent(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int dx = args[0].As<Number>()->Value();
  int dy = args[1].As<Number>()->Value();
  int mouseData = args[2].As<Number>()->Value();
  int dwFlags = args[3].As<Number>()->Value();
  int time = args[4].As<Number>()->Value();

  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dx = dx;
  input.mi.dy = dy;
  input.mi.mouseData = mouseData;
  input.mi.dwFlags = dwFlags;
  input.mi.time = time;

  SendInput(1, &input, sizeof(INPUT));
}

void Move(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int x = args[0].As<Number>()->Value();
  int y = args[1].As<Number>()->Value();
  SetCursorPos(x, y);
}

void LeftClick(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
  SendInput(1, &input, sizeof(INPUT));
  ZeroMemory(&input, sizeof(INPUT));
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
  SendInput(1, &input, sizeof(INPUT));
}

void RightClick(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
  SendInput(1, &input, sizeof(INPUT));
  ZeroMemory(&input, sizeof(INPUT));
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
  SendInput(1, &input, sizeof(INPUT));
}

void LeftDown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
  SendInput(1, &input, sizeof(INPUT));
}
void LeftUp(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
  SendInput(1, &input, sizeof(INPUT));
}

void Scroll(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int distance = args[0].As<Number>()->Value();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_WHEEL;
  input.mi.mouseData = distance;
  SendInput(1, &input, sizeof(INPUT));
}
void ScrollUp(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_WHEEL;
  input.mi.mouseData = 120;
  SendInput(1, &input, sizeof(INPUT));
}
void ScrollDown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  INPUT input = {0};
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_WHEEL;
  input.mi.mouseData = -120;
  SendInput(1, &input, sizeof(INPUT));
}

/*****************************************************************************/
void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "_mouseEvent", MouseEvent);
  NODE_SET_METHOD(exports, "_move", Move);
  NODE_SET_METHOD(exports, "_leftClick", LeftClick);
  NODE_SET_METHOD(exports, "_rightClick", RightClick);
  NODE_SET_METHOD(exports, "_leftDown", LeftDown);
  NODE_SET_METHOD(exports, "_leftUp", LeftUp);
  NODE_SET_METHOD(exports, "_scroll", Scroll);
  NODE_SET_METHOD(exports, "_scrollUp", ScrollUp);
  NODE_SET_METHOD(exports, "_scrollDown", ScrollDown);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

} // namespace demo
