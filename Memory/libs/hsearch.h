#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

#ifndef H_SEARCH_H
#define H_SEARCH_H

namespace hsearch {
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

//专用于已经打开的进程且句柄权限足够 已知安全且可读的内存范围
//注意内存范围必须完整包含所有数据占用的空间 而不是地址的起始和终止简单相减
//一般js层调用会配合winapi的vritualQqueryEx函数 传入的都是内存块的起始和终止 不会出太大问题

/*
@param1 Number: processHandle
@param2 Number(124)/BigInt(8): targetValue
@param3 BigInt: startAddress
@param4 BigInt: endAddress
@param5 Number: step
@param6 Number: stopThreshold

@returns Array: [Bigint: address]
*/
void hSearch1(const FunctionCallbackInfo<Value> &args);
void hSearch2(const FunctionCallbackInfo<Value> &args);
void hSearch4(const FunctionCallbackInfo<Value> &args);
void hSearch8(const FunctionCallbackInfo<Value> &args);

/*
@param1 Number: processHandle
@param2 Function: checkValue(Number(124)/BigInt(8): value)
@param3 BigInt: startAddress
@param4 BigInt: endAddress
@param5 Number: step
@param6 Number: stopThreshold

@returns Array: [Bigint: address]
*/
void hSearchCb1(const FunctionCallbackInfo<Value> &args);
void hSearchCb2(const FunctionCallbackInfo<Value> &args);
void hSearchCb4(const FunctionCallbackInfo<Value> &args);
void hSearchCb8(const FunctionCallbackInfo<Value> &args);
}
#endif // SEARCH_H
