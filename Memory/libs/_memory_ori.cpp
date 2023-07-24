#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

#include <iostream>
// using namespace v8;

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
//
#define _H_SEARCH_N_JUDGE_                                                                                                                           \
  if (value == targetValue) {                                                                                                                        \
    Local<BigInt> addr = BigInt::NewFromUnsigned(isolate, startAddress + i);                                                                         \
    addrArray->Set(isolate->GetCurrentContext(), index++, addr).ToChecked();                                                                         \
    if (stopThreshold != 0 && stopThreshold == index) {/* 设置了有效的停止阈值 且搜索到的数量足够停止阈值 停止搜索 */     \
      break;                                                                                                                                         \
    }                                                                                                                                                \
  }
//
#define _H_SEARCH_CB_N_ARG_PREPARE_                                                                                                                  \
  Local<Function> checkValue = Local<Function>::Cast(args[1]);                                                                                       \
  _H_SEARCH_N_ARG_PREPARE_
//
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

namespace memory {

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

//为什么是4 8 bufer? 因为根据我多年开挂经验 DWORD/FLOAT/DOUBLE是最多的 包起来性能好
//至于这里只写整数不单独做浮点搜索 是因为我写的纯js浮点数转换二进制太牛逼了
void read4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  unsigned int value = 0;

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, 4, 0)) {
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

  if (ReadProcessMemory(hProcess, (LPCVOID)memoryAddress, &value, 8, 0)) {
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

void write4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long memoryAddress = args[1].As<BigInt>()->Uint64Value();
  unsigned int value = args[2].As<Number>()->Value();

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, 4, 0)) {
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

  if (WriteProcessMemory(hProcess, (LPVOID)memoryAddress, &value, 8, 0)) {
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

void hread4(const FunctionCallbackInfo<Value> &args) {}

void BufferFreeCallback(char *data, void *hint) {
  // You can perform any necessary cleanup here (if required)
  // For example, if you have dynamically allocated 'data' in the plugin code
  // you can free it here using 'delete[]' or 'free()' based on how it was allocated.
  // In this example, since 'sampleData' is a string literal and not dynamically allocated,
  // no action is required here.
}

void search4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  DWORD targetValue = args[1].As<Number>()->Value();
  unsigned long long startAddress = args[2].As<BigInt>()->Uint64Value();
  unsigned long long endAddress = args[3].As<BigInt>()->Uint64Value();
  int memProtection = args[4].As<Number>()->Value(); //内存块的保护属性 0x04读写 0x02只读
  int step = args[5].As<Number>()->Value();          // 对齐位数 1、2、3、4..
  int stopThreshold = args[6].As<Number>()->Value(); // 停止阈值 找到多少结果之后就直接放弃 在特定情况可以极大提高效率 0表示不设阈值
  Local<Function> callBack = Local<Function>::Cast(args[7]); // js回调函数处理搜索数量 当前搜索基址 已搜总大小

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid); //打开进程

  char *bufferData = new char[0x2FFFFFF * 8]; // 383Mb 储存搜索到的地址的缓冲区 最多存50,331,647个地址
  int index = 0;                              // bufferData的实时有效长度
  unsigned int totalSearchedSize;             // 已搜索过的总体积

  MEMORY_BASIC_INFORMATION memInfo;      // 储存内存块的信息
  LPVOID address = (LPVOID)startAddress; // 初始化起始地址
  LPVOID endAddr = (LPVOID)endAddress;   // 终止地址 强转类型使用

  while (address <= endAddr && (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo)) != 0)) { /* 到指定的结束位置或进程的地址末尾停止 */

    if (memInfo.Protect & memProtection) { /* 判断内存块的保护属性 */
      // DWORD *data = (DWORD *)malloc(sizeof(char) * memInfo.RegionSize); /* 动态分配内存块同等大小的空间 将内存块拷贝进来在其中搜索 */
      unsigned char *data = (unsigned char *)malloc(sizeof(char) * memInfo.RegionSize); /* 动态分配内存块同等大小的空间 将内存块拷贝进来在其中搜索 */

      ReadProcessMemory(hProcess, address, data, memInfo.RegionSize, 0); /* 读取整个内存块 现在这个玩意最慢 */

      /* 在当前内存块内搜索 */
      unsigned long long baseAddr = reinterpret_cast<intptr_t>(memInfo.BaseAddress);

      // for (unsigned long long i = 0; i < memInfo.RegionSize / 4; i += 1) {
      // if (data[i] == targetValue) {
      // unsigned long long addr = baseAddr + i * 4; //符合条件的地址
      for (unsigned long long i = 0; i <= memInfo.RegionSize - 4; i += step) { //搜索 uint是4byte 只读到data的倒数第四项
        unsigned int value = *(unsigned int *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
        if (value == targetValue) {
          unsigned long long addr = baseAddr + i; //符合条件的地址

          // std::cout << baseAddr << "--" << memProtection << "--" << addr << "--" << value << std::endl;
          //将读到的64位address转换成buffer的8个char值 写入bufferData 小端
          for (int j = 0; j < 8; j++) {
            char value = (addr >> (j * 8)) & 0xff;
            bufferData[index * 8 + j] = value;
          }

          index++; /* 已搜到的数量+1 */
        }
      }
    }

    /* 将搜索数量 当前搜索基址 已搜总大小反馈给回调函数 */
    totalSearchedSize += memInfo.RegionSize;
    Local<Value> argv[3] = {Number::New(isolate, index), BigInt::NewFromUnsigned(isolate, (unsigned long long)address),
                            Number::New(isolate, totalSearchedSize)};
    MaybeLocal<Value> jsCbResult = callBack->Call(isolate->GetCurrentContext(), Null(isolate), 3, argv).ToLocalChecked();

    /* 处理回调函数的运行结果 如果为是则退出 */
    Local<Value> resultValue;
    jsCbResult.ToLocal(&resultValue);
    if (resultValue->BooleanValue(isolate)) { /* 考虑性能而且暂时没有发现缺点 暂时不一定要求Local */
      return;
    }

    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); /* 转入下一内存块 */
  }

  Local<Object> buffer = node::Buffer::New(isolate, bufferData, index * 8, BufferFreeCallback, nullptr).ToLocalChecked();

  args.GetReturnValue().Set(buffer);
  CloseHandle(hProcess);
}

void search8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  unsigned long long targetValue = args[1].As<BigInt>()->Uint64Value();
  unsigned long long startAddress = args[2].As<BigInt>()->Uint64Value();
  unsigned long long endAddress = args[3].As<BigInt>()->Uint64Value();
  int memProtection = args[4].As<Number>()->Value(); // 内存块的保护属性 0x04读写 0x02只读
  int step = args[5].As<Number>()->Value();          // 对齐位数 1、2、3、4..
  int stopThreshold = args[6].As<Number>()->Value(); // 停止阈值 找到多少结果之后就直接放弃 在特定情况可以极大提高效率 0表示不设阈值
  Local<Function> callBack = Local<Function>::Cast(args[7]); // js回调函数处理搜索数量 当前搜索基址 已搜总大小

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid); //打开进程

  char *bufferData = new char[0x2FFFFFF * 8]; // 383Mb 储存搜索到的地址的缓冲区 最多存50,331,647个地址
  int index = 0;                              // bufferData的实时有效长度
  unsigned int totalSearchedSize;             // 已搜索过的总体积

  MEMORY_BASIC_INFORMATION memInfo;      //储存内存块的信息
  LPVOID address = (LPVOID)startAddress; //初始化起始地址
  LPVOID endAddr = (LPVOID)endAddress;   //终止地址 强转类型使用

  while (address <= endAddr && (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo)) != 0)) { //到指定的结束位置或进程的地址末尾停止

    if (memInfo.Protect & memProtection) {                              // 判断内存块的保护属性
      DWORD *data = (DWORD *)malloc(sizeof(char) * memInfo.RegionSize); //动态分配内存块同等大小的空间 将内存块拷贝进来在其中搜索
      ReadProcessMemory(hProcess, address, data, memInfo.RegionSize, 0); //读取内存

      /* 在当前内存块内搜索 */
      unsigned long long baseAddr = reinterpret_cast<intptr_t>(memInfo.BaseAddress);
      for (unsigned long long i = 0; i < memInfo.RegionSize / 4 - 1; i += 1) { //搜索 因为是8字节 长度-1

        unsigned long long value = *(unsigned long long *)(*(&data) + i); // QWORD长度位8 但是搜索要求间隔4 所以这里先按4字节得到偏移再强转为8字节指针
        if (value == targetValue) {
          unsigned long long addr = baseAddr + i * 4; //符合条件的地址
          //将读到的64位address转换成buffer的8个char值 写入bufferData 小端
          for (int j = 0; j < 8; j++) {
            char value = (addr >> (j * 8)) & 0xff;
            bufferData[index * 8 + j] = value;
          }
        }
      }
    }

    /* 将搜索数量 当前搜索基址 已搜总大小反馈给回调函数 */
    totalSearchedSize += memInfo.RegionSize;
    Local<Value> argv[3] = {Number::New(isolate, index), BigInt::NewFromUnsigned(isolate, (unsigned long long)address),
                            Number::New(isolate, totalSearchedSize)};
    MaybeLocal<Value> jsCbResult = callBack->Call(isolate->GetCurrentContext(), Null(isolate), 3, argv).ToLocalChecked();

    /* 处理回调函数的运行结果 如果为是则退出 */
    Local<Value> resultValue;
    jsCbResult.ToLocal(&resultValue);
    if (resultValue->BooleanValue(isolate)) { /* 考虑性能而且暂时没有发现缺点 暂时不一定要求Local */
      return;
    }

    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); /* 转入下一内存块 */
  }
  Local<Object> buffer = node::Buffer::New(isolate, bufferData, index * 8, BufferFreeCallback, nullptr).ToLocalChecked();

  args.GetReturnValue().Set(buffer);
  CloseHandle(hProcess);
}

//搜索特征码模板 支持包含未知值的模板匹配 不是联合搜索
void searchTemplate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  Local<Array> js_array = Local<Array>::Cast(args[1]); // buffer数组 有效值为0x00-0xff 超出范围的值视作??
  unsigned long long startAddress = args[2].As<BigInt>()->Uint64Value();
  unsigned long long endAddress = args[3].As<BigInt>()->Uint64Value();
  int memProtection = args[4].As<Number>()->Value(); //内存块的保护属性 0x04读写 0x02只读
  int step = args[5].As<Number>()->Value();          //对齐位数 1、2、3、4..
  int stopThreshold = args[6].As<Number>()->Value(); //停止阈值 找到多少结果之后就直接放弃 在特定情况可以极大提高效率 0表示不设阈值

  //将传入的js数组转写到c++数组中
  int templateLength = js_array->Length();
  int *templateArray = new int[templateLength];
  for (int i = 0; i < templateLength; i++) {
    Local<Value> element = js_array->Get(isolate->GetCurrentContext(), i).ToLocalChecked();
    if (element->IsNumber()) {
      templateArray[i] = element->Int32Value(isolate->GetCurrentContext()).ToChecked();
    } else {
      templateArray[i] = 666; //只做区分 只要大于0xff就行
    }
  }

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid); //打开进程

  Local<Array> addrArray = Array::New(isolate); //要返回的地址数组
  int index = 0;                                // addrArray的长度

  MEMORY_BASIC_INFORMATION memInfo;      //储存内存块的信息
  LPVOID address = (LPVOID)startAddress; //初始化起始地址
  LPVOID endAddr = (LPVOID)endAddress;   //终止地址 强转类型使用

  while (address <= endAddr && (VirtualQueryEx(hProcess, address, &memInfo, sizeof(memInfo)) != 0)) { //到指定的结束位置或进程的地址末尾停止

    if (memInfo.Protect & memProtection) { // 判断内存块的保护属性

      char *data = (char *)malloc(sizeof(char) * memInfo.RegionSize); //动态分配内存块同等大小的空间 将内存块拷贝进来在其中搜索
      ReadProcessMemory(hProcess, memInfo.BaseAddress, data, memInfo.RegionSize, 0); //读取整块内存 注意不是输入的address

      for (unsigned long long i = 0; i < memInfo.RegionSize - templateLength;
           i += step) { //搜索 这里是byte和传入的数组匹配 注意传入的模板不一定是整齐的4位 所以设置对齐位数

        for (int j = 0; j < templateLength; j++) {
          if (templateArray[j] > 0xff) { //规定大于0xff的数视作未知值 任意匹配 跳过
            continue;
          }
          char value = *(char *)(templateArray + j); //判断是否大于0xff后即可按照char数组处理
          if (value != data[i + j]) {                //遇到一股不匹配的byte就跳出
            break;
          }
          if (j == templateLength - 1) { //匹配成功 记录地址
            unsigned long long baseAddr = reinterpret_cast<intptr_t>(memInfo.BaseAddress);
            // Local<Number> addr = Number::New(isolate, baseAddr + i);
            Local<BigInt> addr = BigInt::NewFromUnsigned(isolate, baseAddr + i);
            addrArray->Set(isolate->GetCurrentContext(), index++, addr).ToChecked();

            // std::cout << memInfo.BaseAddress << "--->" << i << std::endl;
            if (stopThreshold != 0 && stopThreshold == index) { // index 等于数组长度+1 因此==即可
              goto TemplateSearchComplate;
            }
          }
        }
      }
    }

    std::cout.flush();
    // std::cout << (unsigned long long)address - startAddress << "__" << endAddress - startAddress << std::endl;
    std::cout << "Search Progress: " << ((unsigned long long)address - startAddress) * 100 / (endAddress - startAddress) + 1 << "%\r"; //美观
    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); //转入下一内存块
  }

TemplateSearchComplate:
  delete[] templateArray; // 释放动态分配的内存
  std::cout << std::endl; //打印转到下一行
  args.GetReturnValue().Set(addrArray);
}

//纯js实现联合搜索的性能把我感动哭了 遂含泪写了以下4个函数 专用于已经打开的进程且句柄权限足够 已知安全且可读的内存范围
//注意内存范围必须完整包含所有数据占用的空间 而不是地址的起始和终止简单相减
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
    unsigned short value = *(unsigned short *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearch4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  DWORD targetValue = args[1].As<Number>()->Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 4; i += step) { //搜索 uint是4byte 只读到data的倒数第四项
    unsigned int value = *(unsigned int *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
    _H_SEARCH_N_JUDGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearch8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  unsigned long long targetValue = args[1].As<BigInt>()->Uint64Value();
  _H_SEARCH_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 8; i += step) { //搜索 ull是16byte 只读到data的倒数第八项
    unsigned long long value = *(unsigned long long *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
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
    unsigned short value = *(unsigned short *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {Number::New(isolate, value)};     // 将读到的值转成jsNumber传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb4(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 4; i += step) { // 搜索 uint是4byte 只读到data的倒数第四项
    unsigned int value = *(unsigned int *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {Number::New(isolate, value)}; // 将读到的值转成jsNumber传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

void hSearchCb8(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  _H_SEARCH_CB_N_ARG_PREPARE_
  for (unsigned long long i = 0; i <= regionSize - 8; i += step) { // 搜索 ull是16byte 只读到data的倒数第八项
    unsigned long long value = *(unsigned long long *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
    Local<Value> argv[1] = {BigInt::NewFromUnsigned(isolate, value)}; // 将读到的值转成jsBigint传给回调函数
    _H_SEARCH_CB_N_JUEGE_
  }
  args.GetReturnValue().Set(addrArray);
}

// UNDER FUCKING FOREVER PROGRAMMING...
void federatedSearch(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  std::cout << "UNDER PROGRAMMING..." << std::endl;
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "_read4", read4);
  NODE_SET_METHOD(exports, "_read8", read8);
  NODE_SET_METHOD(exports, "_readBuf", readBuf);
  NODE_SET_METHOD(exports, "_write4", write4);
  NODE_SET_METHOD(exports, "_write8", write8);
  NODE_SET_METHOD(exports, "_writeBuf", writeBuf);
  NODE_SET_METHOD(exports, "_search4", search4);
  NODE_SET_METHOD(exports, "_search8", search8);
  NODE_SET_METHOD(exports, "_searchTemplate", searchTemplate);
  NODE_SET_METHOD(exports, "_hsearch1", hSearch1);
  NODE_SET_METHOD(exports, "_hsearch2", hSearch2);
  NODE_SET_METHOD(exports, "_hsearch4", hSearch4);
  NODE_SET_METHOD(exports, "_hsearch8", hSearch8);
  NODE_SET_METHOD(exports, "_hsearchcb1", hSearchCb1);
  NODE_SET_METHOD(exports, "_hsearchcb2", hSearchCb2);
  NODE_SET_METHOD(exports, "_hsearchcb4", hSearchCb4);
  NODE_SET_METHOD(exports, "_hsearchcb8", hSearchCb8);
  // NODE_SET_METHOD(exports, "federatedSearch", federatedSearch);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}