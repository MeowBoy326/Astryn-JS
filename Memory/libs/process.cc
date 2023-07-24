#include "process.h"

namespace process {

void queryAllProcesses(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  // 创建一个数组来存储进程信息
  Local<Array> processArray = Array::New(isolate);

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Failed to create snapshot.").ToLocalChecked()));
    return;
  }

  PROCESSENTRY32 processEntry;
  processEntry.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(snapshot, &processEntry)) {
    CloseHandle(snapshot);
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Failed to get process information.").ToLocalChecked()));
    return;
  }

  // 遍历快照中的所有进程，并将信息存入数组

  int index = 0;
  do {
    // 创建一个对象来存储每个进程的 ID 和名称
    Local<Object> processObj = Object::New(isolate);
    processObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "PID").ToLocalChecked(),
                    Number::New(isolate, processEntry.th32ProcessID))
        .ToChecked();
    processObj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name").ToLocalChecked(),
                    String::NewFromUtf8(isolate, processEntry.szExeFile).ToLocalChecked())
        .ToChecked();

    // 将对象添加到数组中
    processArray->Set(isolate->GetCurrentContext(), index++, processObj).ToChecked();
  } while (Process32Next(snapshot, &processEntry));

  CloseHandle(snapshot);

  // 返回进程信息数组
  args.GetReturnValue().Set(processArray);
}

void openProcessByName(const FunctionCallbackInfo<Value> &args) { Isolate *isolate = args.GetIsolate(); }

void _openProcess(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD desiredAccess = args[0].As<Number>()->Value(); //访问权限
  BOOL inheritHandle = args[1].As<Number>()->Value();  //一般都是FALSE
  DWORD processId = args[2].As<Number>()->Value();     // PID
  //将 HANDLE 强转为 uint 然后返回
  HANDLE hProcess = OpenProcess(desiredAccess, inheritHandle, processId);
  unsigned int processHandle = reinterpret_cast<intptr_t>(hProcess);

  if (processHandle) {
    Local<Number> result = Number::New(isolate, processHandle);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::Null(isolate);
    args.GetReturnValue().Set(result);
  }
}

void _closeHandle(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD processHandle = args[0].As<Number>()->Value();
  //将 uint 转换为 HANDLE
  uintptr_t handlePtr = static_cast<uintptr_t>(processHandle);
  HANDLE hProcess = reinterpret_cast<HWND>(handlePtr);

  if (CloseHandle(hProcess)) {
    Local<Value> result = v8::True(isolate);
    args.GetReturnValue().Set(result);
  } else {
    Local<Value> result = v8::False(isolate);
    args.GetReturnValue().Set(result);
  }
}
}