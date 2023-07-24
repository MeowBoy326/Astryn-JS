#include <node.h>
#include "jsMemoryapi.h"
#include "jsProcessthreadsapi.h"

namespace winapi {

void Init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "_readProcessMemory", memoryapi::_readProcessMemory);
  NODE_SET_METHOD(exports, "_virtualQueryEx", memoryapi::_virtualQueryEx);
  NODE_SET_METHOD(exports, "_writeProcessMemory", memoryapi::_writeProcessMemory);
}
NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}