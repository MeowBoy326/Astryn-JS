#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

namespace search {
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
@param1 Number: PID
@param2 Number(124)/BigInt(8): targetValue
@param3 BigInt: startAddress
@param4 BigInt: endAddress
@param5 Number: memProtection
@param6 Number: step
@param7 Number: stopThreshold
@param8 Function: callback(Number: index, BigInt: address, Bigint: totalSearchedSize)

@returns Buffer: resultBuf
*/
void search1(const FunctionCallbackInfo<Value> &args);
void search2(const FunctionCallbackInfo<Value> &args);
void search4(const FunctionCallbackInfo<Value> &args);
void search8(const FunctionCallbackInfo<Value> &args);

/*
搜索特征码模板 支持包含未知值的模板匹配 不是联合搜索

@param1 Number: PID
@param2 Array: template
@param3 BigInt: startAddress
@param4 BigInt: endAddress
@param5 Number: memProtection
@param6 Number: step
@param7 Number: stopThreshold
@param8 Function: callback(Number: index, BigInt: address, Bigint: totalSearchedSize)

@returns Array: [Bigint: address]

*/
void searchTemplate(const FunctionCallbackInfo<Value> &args);
}