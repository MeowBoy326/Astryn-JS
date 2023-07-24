#include <Windows.h>
#include <node.h>
#include <iostream>
#include <node_buffer.h>
#include <string>
#include <cstdint>

namespace winapi {

using v8::BigInt;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-findwindowa
void _findWindowA(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::String::Utf8Value str(isolate, args[0]->ToString(context).ToLocalChecked());
  v8::String::Utf8Value str1(isolate, args[1]->ToString(context).ToLocalChecked());
  //转换类型调用
  LPCSTR lpClassName = NULL;
  LPCSTR lpWindowName = NULL;
  std::string strClassName(*str);
  std::string strWindowName(*str1);
  if (strClassName.length()) {
    lpClassName = strClassName.c_str(); //类名
  }
  if (strWindowName.length()) {
    lpWindowName = strWindowName.c_str(); //窗口标题
  }
  HWND windowHandle = FindWindowA(lpClassName, lpWindowName);

  if (windowHandle) {
    //将HWND强转为uint并返回
    Local<Number> result = Number::New(isolate, reinterpret_cast<intptr_t>(windowHandle));
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-getwindowthreadprocessid
void _getWindowThreadProcessId(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int handleValue = args[0].As<Number>()->Value();

  uintptr_t handlePtr = static_cast<uintptr_t>(handleValue);
  HWND windowHandle = reinterpret_cast<HWND>(handlePtr);
  DWORD pid = 0;
  int tid = GetWindowThreadProcessId(windowHandle, &pid);

  if (tid) {
    Local<Number> result = Number::New(isolate, pid);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
void _openProcess(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD dwDesiredAccess = args[0].As<Number>()->Value(); //访问权限
  BOOL bInheritHandle = args[1].As<Number>()->Value();   //一般都是FALSE
  DWORD dwProcessId = args[2].As<Number>()->Value();     // PID
  //将 HANDLE 强转为 uint 然后返回
  HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
  unsigned int processHandle = reinterpret_cast<intptr_t>(hProcess);

  if (processHandle) {
    Local<Number> result = Number::New(isolate, processHandle);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
void _closeHandle(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD dwProcessId = args[0].As<Number>()->Value();
  //将 uint 转换为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(dwProcessId);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  if (CloseHandle(hProcess)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory
void _readProcessMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned int length = args[2].As<Number>()->Value(); //注意js缓冲区大小最大2GB 而一个内存块的大小可能超过2GB 这需要js层去做区分和切割
  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  char *data = (char *)malloc(sizeof(char) * length);

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, data, length, 0)) {
    Local<Object> js_buffer = node::Buffer::Copy(isolate, (const char *)data, length).ToLocalChecked();
    args.GetReturnValue().Set(js_buffer);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
void _writeProcessMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  Local<Object> bufferObj = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  char *data = node::Buffer::Data(bufferObj);
  size_t length = node::Buffer::Length(bufferObj);
  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, data, length, 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
}

// https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex
void _virtualQueryEx(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long address = args[1].As<BigInt>()->Uint64Value();

  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  MEMORY_BASIC_INFORMATION memInfo;                                                    //储存内存块信息的变量
  int success = VirtualQueryEx(hProcess, (LPCVOID)address, &memInfo, sizeof(memInfo)); //读取内存块信息
  // VirtualQueryEx返回值是信息缓冲区中返回的实际字节数。如果函数失败，则返回值为零。
  if (success) {
    unsigned long long allocationBase = reinterpret_cast<unsigned long long>(memInfo.AllocationBase); //内存块分配基址 PVOID 强转 ull
    unsigned long long baseAddress = reinterpret_cast<unsigned long long>(memInfo.BaseAddress);       //内存块虚拟地址 PVOID 强转 ull
    unsigned long long regionSize = memInfo.RegionSize;                                               //内存块大小
    int allocationProtect = memInfo.AllocationProtect;                                                //内存区域的保护属性
    int memProtect = memInfo.Protect;                                                                 //内存区域当前的访问保护属性。
    int state = memInfo.State;                                                                        //内存区域的状态
    int type = memInfo.Type;                                                                          //内存区域的类型

    // Type可以取以下几个可能的值，它们是在Windows系统中定义的内存类型标志：
    // MEM_IMAGE（0x1000000）：表示内存区域包含映像或模块的代码、数据或资源。通常用于可执行文件或共享库等映像文件。
    // MEM_MAPPED（0x40000）：表示内存区域通过文件映射（file mapping）创建，并与磁盘上的文件关联。
    // MEM_PRIVATE（0x20000）：表示内存区域为私有内存，不与其他进程共享。

    // State可以取以下几个可能的值，它们是在Windows系统中定义的内存状态标志：
    // MEM_COMMIT（0x1000）：表示内存区域是已提交的，即已分配给进程使用的内存。该内存区域可供读取和写入。
    // MEM_FREE（0x10000）：表示内存区域是空闲的，即未被分配给进程使用的内存。该内存区域可以被分配给进程或其他目的。
    // MEM_RESERVE（0x2000）：表示内存区域是保留的，即已为进程保留但尚未分配的内存。该内存区域暂时不可供读取或写入，直到被提交（commit）后才能使用。
    // MEM_RESET（0x80000）：表示内存区域已被重置，即其内容被重置为零。该内存区域仍然是已提交的，但其内容已被清空。
    // MEM_LARGE_PAGES（0x20000000）：表示内存区域是使用大页（large
    // page）分配的。大页是一种操作系统提供的较大内存页，通常是2MB或更大，可以提供更高的性能和效率。

    Local<Object> jsMemBlockObj = Object::New(isolate);
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "allocationProtect").ToLocalChecked(),
                       Number::New(isolate, allocationProtect))
        .ToChecked();
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "state").ToLocalChecked(), Number::New(isolate, state)).ToChecked();
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type").ToLocalChecked(), Number::New(isolate, type)).ToChecked();
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "baseAddress").ToLocalChecked(), BigInt::New(isolate, baseAddress))
        .ToChecked();
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "regionSize").ToLocalChecked(), BigInt::New(isolate, regionSize))
        .ToChecked();
    jsMemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "allocationBase").ToLocalChecked(),
                       BigInt::New(isolate, allocationBase))
        .ToChecked();
    args.GetReturnValue().Set(jsMemBlockObj);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

// void temp(const FunctionCallbackInfo<Value> &args) { Isolate *isolate = args.GetIsolate(); }
// void temp(const FunctionCallbackInfo<Value> &args) { Isolate *isolate = args.GetIsolate(); }
void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "_findWindowA", _findWindowA);
  NODE_SET_METHOD(exports, "_getWindowThreadProcessId", _getWindowThreadProcessId);
  NODE_SET_METHOD(exports, "_openProcess", _openProcess);
  NODE_SET_METHOD(exports, "_closeHandle", _closeHandle);
  NODE_SET_METHOD(exports, "_readProcessMemory", _readProcessMemory);
  NODE_SET_METHOD(exports, "_writeProcessMemory", _writeProcessMemory);
  NODE_SET_METHOD(exports, "_virtualQueryEx", _virtualQueryEx);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}