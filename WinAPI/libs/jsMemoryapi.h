#include <Windows.h>
#include <node.h>
#include <node_buffer.h>

namespace memoryapi {
using v8::Array;
using v8::ArrayBuffer;
using v8::BigInt;
using v8::Boolean;
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

/*
https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory

@param1 Number: processHandle
@param2 BigInt: memoryAddress
@param3 Function: handleDataBuf(Buffer: data)
@param4 Number: length
@param5 Function: handleBytesRead(Number: bytesRead)

@returns true(success)/false(failed)
*/
void _readProcessMemory(const FunctionCallbackInfo<Value> &args);

/*
// https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex
@param1 Number: processHandle
@param2 BigInt: memoryAddress
@param3 Function: handleMemInfo(Object: memInfo {
    BigInt: baseAddress, //指向页面区域的基址的指针。
    BigInt: allocationBase, //指向 VirtualAlloc 函数分配的页范围的基址的指针。 BaseAddress 成员指向的页面包含在此分配范围内。
    Number: allocationProtect, //最初分配区域时的内存保护选项。 如果调用方没有访问权限，则此成员可以是 内存保护常量 之一或 0。
    Number: partitionId
    BigInt: regionSize, //从所有页面具有相同属性（以字节为单位）的基址开始的区域大小。
    Number: state, //见下表
    Number: protect,
    Number: type, //见下表
    })
//@param4 Number: length //弃用 涉及到内存管理 函数内部自动填写了dwLength

@returns Number: bufferLength(success)/0(failed)

MemInfo.Type可以取以下几个可能的值，它们是在Windows系统中定义的内存类型标志：
MEM_IMAGE（0x1000000）：表示内存区域包含映像或模块的代码、数据或资源。通常用于可执行文件或共享库等映像文件。
MEM_MAPPED（0x40000）：表示内存区域通过文件映射（file mapping）创建，并与磁盘上的文件关联。
MEM_PRIVATE（0x20000）：表示内存区域为私有内存，不与其他进程共享。

MemInfo.State可以取以下几个可能的值，它们是在Windows系统中定义的内存状态标志：
MEM_COMMIT（0x1000）：表示内存区域是已提交的，即已分配给进程使用的内存。该内存区域可供读取和写入。
MEM_FREE（0x10000）：表示内存区域是空闲的，即未被分配给进程使用的内存。该内存区域可以被分配给进程或其他目的。
MEM_RESERVE（0x2000）：表示内存区域是保留的，即已为进程保留但尚未分配的内存。该内存区域暂时不可供读取或写入，直到被提交（commit）后才能使用。
MEM_RESET（0x80000）：表示内存区域已被重置，即其内容被重置为零。该内存区域仍然是已提交的，但其内容已被清空。
MEM_LARGE_PAGES（0x20000000）：表示内存区域是使用大页（large
page）分配的。大页是一种操作系统提供的较大内存页，通常是2MB或更大，可以提供更高的性能和效率。

*/
void _virtualQueryEx(const FunctionCallbackInfo<Value> &args);

/*
// https://learn.microsoft.com/zh-cn/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory

@param1 Number: processHandle
@param2 BigInt: memoryAddress
@param3 Buffer: dataBuffer
@param4 Number: length
@param5 Function: handleBytesWritten(Number: bytesWritten)

@returns true(success)/false(failed)
*/
void _writeProcessMemory(const FunctionCallbackInfo<Value> &args);
}