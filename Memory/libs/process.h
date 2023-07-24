#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <Windows.h>
#include <TlHelp32.h>

namespace process {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::BigInt;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Array;

/*
获取全部进程信息
@param -
@returns Array:[{PID, name}]
@Error null
*/
void queryAllProcesses(const FunctionCallbackInfo<Value> &args);

/**
@param1 String: name
@returns Number: ProcessHandle
@Error null
*/
void openProcessByName(const FunctionCallbackInfo<Value> &args);
/*
https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
@param1 Number: desiredAccess
@param2 Number: inheritHandle
@param3 Number: pid

returns Number: processHandle
*/
void _openProcess(const FunctionCallbackInfo<Value> &args);

/*
https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
@param1 Number: processHandle

returns true(success)/false(failed)
*/
void _closeHandle(const FunctionCallbackInfo<Value> &args);

// void Init(Local<Object> exports) { NODE_SET_METHOD(exports, "queryAllProcesses", QueryAllProcesses); }

// NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}