#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

namespace memoryapi {
using v8::Array;
using v8::ArrayBuffer;
using v8::BigInt;
using v8::Context;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void _openProcess(const FunctionCallbackInfo<Value> &args);
}