#include "readwrite.h"

namespace readwrite {

void read1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  unsigned char value = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, sizeof(char), 0)) {
    Local<Number> result = Number::New(isolate, value);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void read2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  unsigned short value = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, sizeof(short), 0)) {
    Local<Number> result = Number::New(isolate, value);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void read4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  unsigned int value = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, sizeof(int), 0)) {
    Local<Number> result = Number::New(isolate, value);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void read8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  unsigned long long value = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, sizeof(long long), 0)) {
    Local<BigInt> result = BigInt::NewFromUnsigned(isolate, value);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

void readBuf(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned int length = args[2].As<Number>()->Value(); //注意js缓冲区大小最大2GB 而一个内存块的大小可能超过2GB 这需要js层去做区分和切割

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  char *data = (char *)malloc(sizeof(char) * length);

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, data, length, 0)) {
    Local<Object> js_buffer = node::Buffer::Copy(isolate, (const char *)data, length).ToLocalChecked();
    args.GetReturnValue().Set(js_buffer);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void write1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned char value = args[2].As<Number>()->Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, sizeof(char), 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void write2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned short value = args[2].As<Number>()->Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, sizeof(short), 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void write4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned int value = args[2].As<Number>()->Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, sizeof(int), 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void write8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned long long value = args[2].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, sizeof(long long), 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}

void writeBuf(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  Local<Object> bufferObj = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

  char *data = node::Buffer::Data(bufferObj);
  size_t length = node::Buffer::Length(bufferObj);
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, data, length, 0)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
  CloseHandle(hProcess);
}
}