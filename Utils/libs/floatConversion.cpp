#include <node.h>

namespace numtools {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::BigInt;
using v8::Object;
using v8::String;
using v8::Value;

void encodeFloatBits(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  float floatVal = args[0].As<Number>()->Value();

  int value = (*(long *)&floatVal);
  Local<Number> result = Number::New(isolate, value);
  args.GetReturnValue().Set(result);
}

void decodeFloatBits(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  float intVal = args[0].As<Number>()->Value();

  float value = (*(float *)&intVal);
  Local<Number> result = Number::New(isolate, value);
  args.GetReturnValue().Set(result);
}

void encodeDoubleBits(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  double doubleVal = args[0].As<Number>()->Value();

  long long value = (*(long long *)&doubleVal);
  Local<BigInt> result = BigInt::NewFromUnsigned(isolate, value);
  args.GetReturnValue().Set(result);
}

void decodeDoubleBits(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned long long longVal = args[0].As<BigInt>()->Uint64Value();

  double value = (*(double *)&longVal);
  Local<Number> result = Number::New(isolate, value);
  args.GetReturnValue().Set(result);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "_encodeFloatBits", encodeFloatBits);
  NODE_SET_METHOD(exports, "_decodeFloatBits", decodeFloatBits);
  NODE_SET_METHOD(exports, "_encodeDoubleBits", encodeDoubleBits);
  NODE_SET_METHOD(exports, "_decodeDoubleBits", decodeDoubleBits);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}