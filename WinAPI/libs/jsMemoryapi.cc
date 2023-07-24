#include "jsMemoryapi.h"

namespace memoryapi {

void BufferFreeCallback(char *data, void *hint) {
  // You can perform any necessary cleanup here (if required)
  // For example, if you have dynamically allocated 'data' in the plugin code
  // you can free it here using 'delete[]' or 'free()' based on how it was allocated.
  // In this example, since 'sampleData' is a string literal and not dynamically allocated,
  // no action is required here.
}

void _readProcessMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  Local<Function> handleDataBuf = Local<Function>::Cast(args[2]); // js回调函数处理读取到的数据
  unsigned int length = args[3].As<Number>()->Value(); //注意js缓冲区大小最大2GB 而一个内存块的大小可能超过2GB 这需要js层去做区分和切割
  Local<Function> handleBytesRead = Local<Function>::Cast(args[4]); //实际读取的字节数
  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  char *data = (char *)malloc(sizeof(char) * length);
  SIZE_T bytesRead = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, data, length, &bytesRead)) {
    Local<Object> js_dataBuf = node::Buffer::New(isolate, data, length * sizeof(char), BufferFreeCallback, nullptr).ToLocalChecked();
    Local<Value> argv[1] = {js_dataBuf};
    handleDataBuf->Call(isolate->GetCurrentContext(), Null(isolate), 1, argv).ToLocalChecked();

    Local<Value> argv1[1] = {Number::New(isolate, bytesRead)};
    handleBytesRead->Call(isolate->GetCurrentContext(), Null(isolate), 1, argv1).ToLocalChecked();

    Local<Value> result = v8::Boolean::New(isolate, true);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Boolean::New(isolate, false);
    args.GetReturnValue().Set(result);
  }
}

void _virtualQueryEx(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long address = args[1].As<BigInt>()->Uint64Value();
  Local<Function> handleMemInfo = Local<Function>::Cast(args[2]); // js回调函数处理读取到的内存块信息
  unsigned int length = args[3].As<Number>()->Value();            //弃用

  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  MEMORY_BASIC_INFORMATION memInfo;                                                    //储存内存块信息的变量
  int success = VirtualQueryEx(hProcess, (LPCVOID)address, &memInfo, sizeof(memInfo)); //读取内存块信息
  // VirtualQueryEx返回值是信息缓冲区中返回的实际字节数。如果函数失败，则返回值为零。
  if (success) {
    unsigned long long baseAddress = reinterpret_cast<unsigned long long>(memInfo.BaseAddress);       //内存块虚拟地址 PVOID 强转 ull
    unsigned long long allocationBase = reinterpret_cast<unsigned long long>(memInfo.AllocationBase); //内存块分配基址 PVOID 强转 ull
    int allocationProtect = memInfo.AllocationProtect;                                                //内存区域的保护属性
    unsigned long long regionSize = memInfo.RegionSize;                                               //内存块大小
    int state = memInfo.State;                                                                        //内存区域的状态
    int protect = memInfo.Protect;                                                                    //内存区域当前的访问保护属性。
    int type = memInfo.Type;                                                                          //内存区域的类型

    Local<Object> js_MemBlockObj = Object::New(isolate);

    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "baseAddress").ToLocalChecked(), BigInt::New(isolate, baseAddress))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "allocationBase").ToLocalChecked(),
                        BigInt::New(isolate, allocationBase))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "allocationProtect").ToLocalChecked(),
                        Number::New(isolate, allocationProtect))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "regionSize").ToLocalChecked(), BigInt::New(isolate, regionSize))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "state").ToLocalChecked(), Number::New(isolate, state))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "protect").ToLocalChecked(), Number::New(isolate, protect))
        .ToChecked();
    js_MemBlockObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type").ToLocalChecked(), Number::New(isolate, type)).ToChecked();

    // callback
    Local<Value> argv[1] = {js_MemBlockObj};
    handleMemInfo->Call(isolate->GetCurrentContext(), Null(isolate), 1, argv).ToLocalChecked();
  }

  Local<Number> result = Number::New(isolate, success);
  args.GetReturnValue().Set(result);
}

void _writeProcessMemory(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned int processHandle = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  Local<Object> js_dataBuf = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
  unsigned int length = args[3].As<Number>()->Value();                 //虽然可以直接获取length 考虑尊重原API和自由度 保留自定义
  Local<Function> handleBytesWritten = Local<Function>::Cast(args[4]); //实际写入的字节数
  //将uint 转为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  char *data = node::Buffer::Data(js_dataBuf);
  SIZE_T bytesWritten = 0;

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, data, length, &bytesWritten)) {
    Local<Value> argv1[1] = {Number::New(isolate, bytesWritten)};
    handleBytesWritten->Call(isolate->GetCurrentContext(), Null(isolate), 1, argv1).ToLocalChecked();

    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
}

// void Init(v8::Local<v8::Object> exports) {
//   NODE_SET_METHOD(exports, "_readProcessMemory", _readProcessMemory);
//   NODE_SET_METHOD(exports, "_virtualQueryEx", _virtualQueryEx);
//   NODE_SET_METHOD(exports, "_writeProcessMemory", _writeProcessMemory);
// }
// NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}