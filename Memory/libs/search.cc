

#include "search.h"

namespace search {

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
  unsigned long long totalSearchedSize = 0;   // 已搜索过的总体积

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

      for (unsigned long long i = 0; i <= memInfo.RegionSize - 4; i += step) { //搜索 uint是4byte 只读到data的倒数第四项
        unsigned int value = *(unsigned int *)(*(&data) + i); // data为char数组 所以这里先按1字节得到偏移再强转为对应字节指针
        if (value == targetValue) {
          unsigned long long addr = baseAddr + i; //符合条件的地址

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
                            BigInt::NewFromUnsigned(isolate, totalSearchedSize)};
    MaybeLocal<Value> jsCbResult = callBack->Call(isolate->GetCurrentContext(), Null(isolate), 3, argv).ToLocalChecked();

    /* 处理回调函数的运行结果 如果为是则退出 */
    Local<Value> cbResultValue;
    jsCbResult.ToLocal(&cbResultValue);
    if (cbResultValue->BooleanValue(isolate)) { /* 考虑性能而且暂时没有发现缺点 暂时不一定要求Local */
      return;
    }

    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); /* 转入下一内存块 */
  }

  Local<Object> buffer = node::Buffer::New(isolate, bufferData, index * 8, BufferFreeCallback, nullptr).ToLocalChecked();
  // delete[] bufferData; // 释放动态分配的内存
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
  unsigned long long totalSearchedSize = 0;   // 已搜索过的总体积

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
                            BigInt::NewFromUnsigned(isolate, totalSearchedSize)};
    MaybeLocal<Value> jsCbResult = callBack->Call(isolate->GetCurrentContext(), Null(isolate), 3, argv).ToLocalChecked();

    /* 处理回调函数的运行结果 如果为是则退出 */
    Local<Value> cbResultValue;
    jsCbResult.ToLocal(&cbResultValue);
    if (cbResultValue->BooleanValue(isolate)) { /* 考虑性能而且暂时没有发现缺点 暂时不一定要求Local */
      return;
    }

    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); /* 转入下一内存块 */
  }
  Local<Object> buffer = node::Buffer::New(isolate, bufferData, index * 8, BufferFreeCallback, nullptr).ToLocalChecked();
  // delete[] bufferData; // 释放动态分配的内存
  args.GetReturnValue().Set(buffer);
  CloseHandle(hProcess);
}

void searchTemplate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int pid = args[0].As<Number>()->Value();
  Local<Array> js_array = Local<Array>::Cast(args[1]); // buffer数组 有效值为0x00-0xff 超出范围的值视作??
  unsigned long long startAddress = args[2].As<BigInt>()->Uint64Value();
  unsigned long long endAddress = args[3].As<BigInt>()->Uint64Value();
  int memProtection = args[4].As<Number>()->Value(); //内存块的保护属性 0x04读写 0x02只读
  int step = args[5].As<Number>()->Value();          //对齐位数 1、2、3、4..
  int stopThreshold = args[6].As<Number>()->Value(); //停止阈值 找到多少结果之后就直接放弃 在特定情况可以极大提高效率 0表示不设阈值
  Local<Function> callBack = Local<Function>::Cast(args[7]); // js回调函数处理搜索数量 当前搜索基址 已搜总大小

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
  unsigned long long totalSearchedSize = 0;     // 已搜索过的总体积

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

    /* 将搜索数量 当前搜索基址 已搜总大小反馈给回调函数 */
    totalSearchedSize += memInfo.RegionSize;
    Local<Value> argv[3] = {Number::New(isolate, index), BigInt::NewFromUnsigned(isolate, (unsigned long long)address),
                            BigInt::NewFromUnsigned(isolate, totalSearchedSize)};
    MaybeLocal<Value> jsCbResult = callBack->Call(isolate->GetCurrentContext(), Null(isolate), 3, argv).ToLocalChecked();

    /* 处理回调函数的运行结果 如果为是则退出 */
    Local<Value> cbResultValue;
    jsCbResult.ToLocal(&cbResultValue);
    if (cbResultValue->BooleanValue(isolate)) { /* 考虑性能而且暂时没有发现缺点 暂时不一定要求Local */
      return;
    }

    address = (LPVOID)((DWORD_PTR)memInfo.BaseAddress + memInfo.RegionSize); //转入下一内存块
  }

TemplateSearchComplate:
  delete[] templateArray; // 释放动态分配的内存
  args.GetReturnValue().Set(addrArray);
}
}