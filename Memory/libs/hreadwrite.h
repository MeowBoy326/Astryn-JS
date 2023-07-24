
#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

namespace hreadwrite {
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

/*
@param1 Number: processHanndle
@param2 Number: valueSize
@param3 Buffer: addressBuffer

@returns Object: {Number: valueSize, Buffer: indexBuf, Buffer: resultBuf}
*/
void hRead(const FunctionCallbackInfo<Value> &args);

/*
@param1 Number: processHanndle
@param2 Number: valueSize
@param3 Buffer: addressBuffer
@param4 Buffer: valueBuffer

@returns Object: {Buffer: indexBuf}
*/
void hWrite(const FunctionCallbackInfo<Value> &args);
}