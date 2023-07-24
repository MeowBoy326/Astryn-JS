#include "hreadwrite.h"
#include <iostream>

namespace hreadwrite {

void BufferFreeCallback(char *data, void *hint) {
  // You can perform any necessary cleanup here (if required)
  // For example, if you have dynamically allocated 'data' in the plugin code
  // you can free it here using 'delete[]' or 'free()' based on how it was allocated.
  // In this example, since 'sampleData' is a string literal and not dynamically allocated,
  // no action is required here.
}

void hRead(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int processHandle = args[0].As<Number>()->Value();
  int resultValueSize = args[1].As<Number>()->Value(); /* resultBuffer中一个值的空间 */
  Local<Object> js_addrBuf = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  uintptr_t intValuePtr = static_cast<uintptr_t>(processHandle); /* 传入的jsNumber强转HANDLE */
  HANDLE hProcess = reinterpret_cast<HANDLE>(intValuePtr);

  char *addressBuffer = node::Buffer::Data(js_addrBuf); /* 取出js传入的地址buffer */
  size_t addressBufLength = node::Buffer::Length(js_addrBuf);
  int maxIndex = addressBufLength / sizeof(long long); /* 需要读取的地址的总数 */

  char *resultBuffer = new char[maxIndex * resultValueSize]; /* 读取结果的buffer 严格按照index顺序 凡读取失败的会在indexBuffer中指示 */
  char *indexBuffer = new char[maxIndex];                    /* 每个byte表示该index的resultValue否读取成功 成功为1 失败为0 */

  /* 按给定地址顺序读取每个值 */
  for (int i = 0; i < maxIndex; i++) {
    unsigned long long memoryAddress = *(unsigned long long *)(*(&addressBuffer) + i * sizeof(long long));
    indexBuffer[i] = ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, resultBuffer + i * resultValueSize, resultValueSize, 0);
  }

  Local<Object> js_indexBuf = node::Buffer::New(isolate, indexBuffer, maxIndex * sizeof(char), BufferFreeCallback, nullptr).ToLocalChecked();
  Local<Object> js_resultBuf = node::Buffer::New(isolate, resultBuffer, maxIndex * resultValueSize, BufferFreeCallback, nullptr).ToLocalChecked();
  Local<Object> resultObj = Object::New(isolate);
  resultObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "valueSize").ToLocalChecked(), Number::New(isolate, resultValueSize))
      .ToChecked();
  resultObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "indexBuf").ToLocalChecked(), js_indexBuf).ToChecked();
  resultObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "resultBuf").ToLocalChecked(), js_resultBuf).ToChecked();

  args.GetReturnValue().Set(resultObj);
}

void hWrite(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int processHandle = args[0].As<Number>()->Value();
  int valueSize = args[1].As<Number>()->Value(); /* valueBuffer中一个值的空间 */
  Local<Object> js_addrBuf = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
  Local<Object> js_valBuf = args[3]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
}
}
