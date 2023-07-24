#include "hsearch.h"

namespace hsearch {

#define _H_SEARCH_N_ARG_PREPARE_                                                                                                                                            \
  unsigned int processHandle = args[0].As<Number>()->Value();                                                                                                               \
  unsigned long long startAddress = args[2].As<BigInt>()->Uint64Value();                                                                                                    \
  unsigned long long endAddress = args[3].As<BigInt>()->Uint64Value();                                                                                                      \
  int step = args[4].As<Number>()->Value(); /* 对齐位数 1、2、3、4.. */                                                                                              \
  int stopThreshold = args[5].As<Number>()->Value(); /* 停止阈值 找到多少结果之后就直接放弃 在特定情况可以极大提高效率 0表示不设阈值 */ \
                                                                                                                                                                            \
  uintptr_t intValuePtr = static_cast<uintptr_t>(processHandle); /* 传入的jsNumber强转HANDLE */                                                                        \
  HANDLE hProcess = reinterpret_cast<HANDLE>(intValuePtr);                                                                                                                  \
                                                                                                                                                                            \
  unsigned long long regionSize = endAddress - startAddress; /* 搜索区域的大小 */                                                                                    \
  Local<Array> addrArray = Array::New(isolate);              /* 要返回的地址数组 */                                                                                 \
  int index = 0;                                             /* addrArray的长度 */                                                                                       \
  unsigned char *data = (unsigned char *)malloc(sizeof(char) * regionSize); /* 动态分配内存块同等大小的空间 将内存块拷贝进来在其中搜索 */        \
  ReadProcessMemory(hProcess, (LPVOID)startAddress, data, regionSize, 0); /* 读取内存 */

#define _H_SEARCH_N_JUDGE_                                                                                                                           \
  if (value == targetValue) {                                                                                                                        \
    Local<BigInt> addr = BigInt::NewFromUnsigned(isolate, startAddress + i);                                                                         \
    addrArray->Set(isolate->GetCurrentContext(), index++, addr).ToChecked();                                                                         \
    if (stopThreshold != 0 && stopThreshold == index) {/* 设置了有效的停止阈值 且搜索到的数量足够停止阈值 停止搜索 */     \
      break;                                                                                                                                         \
    }                                                                                                                                                \
  }

#define _H_SEARCH_CB_N_ARG_PREPARE_                                                                                                                  \
  Local<Function> checkValue = Local<Function>::Cast(args[1]);                                                                                       \
  _H_SEARCH_N_ARG_PREPARE_

#define _H_SEARCH_CB_N_JUEGE_                                                                                                                        \
  MaybeLocal<Value> jsCbResult =                                                                                                                     \
      checkValue->Call(isolate->GetCurrentContext(), Null(isolate), 1, argv).ToLocalChecked(); /* 获得回调函数的执行结果 */               \
  /* 处理回调函数的运行结果 如果为真说明搜索正确 */                                                                             \
  Local<Value> resultValue;                                                                                                                          \
  jsCbResult.ToLocal(&resultValue);                                                                                                                  \
  if (resultValue->BooleanValue(isolate)) {                                                                                                          \
    Local<BigInt> addr = BigInt::NewFromUnsigned(isolate, startAddress + i);                                                                         \
    addrArray->Set(isolate->GetCurrentContext(), index++, addr).ToChecked();                                                                         \
    /* 设置了有效的停止阈值 且搜索到的数量足够停止阈值 停止搜索 */                                                        \
    if (stopThreshold != 0 && stopThreshold == index) {                                                                                              \
      break;                                                                                                                                         \
    }                                                                                                                                                \
  }

//

void hSearch1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned char targetValue = args[1].As<Number>()->Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i < regionSize; i += 1) { // 搜索
    unsigned char value = data[i];
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearch2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned short targetValue = args[1].As<Number>()->Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 2; i += step) { //搜索 ushort是2byte 只读到data的倒数第二项
    unsigned short value = *(unsigned short *)(*(&data) + i);      // data为char数组
                                                                   // 所以这里先按1字节得到偏移再强转为对应字节指针
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearch4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD targetValue = args[1].As<Number>()->Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 4; i += step) { //搜索 uint是4byte 只读到data的倒数第四项
    unsigned int value = *(unsigned int *)(*(&data) + i);          // data为char数组
                                                                   // 所以这里先按1字节得到偏移再强转为对应字节指针
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearch8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned long long targetValue = args[1].As<BigInt>()->Uint64Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 8; i += step) {    //搜索 ull是16byte 只读到data的倒数第八项
    unsigned long long value = *(unsigned long long *)(*(&data) + i); // data为char数组
                                                                      // 所以这里先按1字节得到偏移再强转为对应字节指针
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb1(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i < regionSize; i += step) {
    Local<Value> argv[1] = {Number::New(isolate, data[i])}; // 将读到的值转成jsNumber传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb2(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i < regionSize - 2; i += step) { // 搜索 ushort是2byte 只读到data的倒数第二项
    unsigned short value = *(unsigned short *)(*(&data) + i);     // data为char数组
                                                                  // 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {Number::New(isolate, value)};         // 将读到的值转成jsNumber传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 4; i += step) { // 搜索 uint是4byte 只读到data的倒数第四项
    unsigned int value = *(unsigned int *)(*(&data) + i);          // data为char数组
                                                                   // 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {Number::New(isolate, value)};          // 将读到的值转成jsNumber传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 8; i += step) {    // 搜索 ull是16byte 只读到data的倒数第八项
    unsigned long long value = *(unsigned long long *)(*(&data) + i); // data为char数组
                                                                      // 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {BigInt::NewFromUnsigned(isolate, value)}; // 将读到的值转成jsBigint传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

#undef _H_SEARCH_N_ARG_PREPARE_
#undef _H_SEARCH_N_JUDGE_
#undef _H_SEARCH_CB_N_ARG_PREPARE_
#undef _H_SEARCH_CB_N_ARG_PREPARE_
}