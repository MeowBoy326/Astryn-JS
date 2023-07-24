
#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

namespace readwrite {
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

//按长度分类 float/double 与 uint/ull 的相互转换放在js层即可

/*
@param1 Number: PID
@param2 BigInt: address

@returns Number(124)/BigInt(8): result
@Error :null
*/
void read1(const FunctionCallbackInfo<Value> &args);
void read2(const FunctionCallbackInfo<Value> &args);
void read4(const FunctionCallbackInfo<Value> &args);
void read8(const FunctionCallbackInfo<Value> &args);
/*
@param1 Number: PID
@param2 BigInt: address
@param3 Number: length

@returns Buffer: result
@Error :null
*/
void readBuf(const FunctionCallbackInfo<Value> &args);
/*
@param1 Number: PID
@param2 BigInt: address
@param3 Number(124)/BigInt(8): value

@returns true
@Error false
*/
void write1(const FunctionCallbackInfo<Value> &args);
void write2(const FunctionCallbackInfo<Value> &args);
void write4(const FunctionCallbackInfo<Value> &args);
void write8(const FunctionCallbackInfo<Value> &args);
/*
@param1 Number: PID
@param2 BigInt: address
@param3 Buffer: buffer

@returns true
@Error false
*/
void writeBuf(const FunctionCallbackInfo<Value> &args);
}