# Astryn-JS

> 集内存读写、内存检索、鼠标宏、键盘宏为一身的脚本开发工具。  
> 适用于 Windows 平台。

## 开发环境

|     名称     |         版本          |
| :----------: | :-------------------: |
| Windows 版本 | Windows 10 家庭中文版 |
|   系统类型   |          x64          |
|     node     |       v18.16.0        |
|   node-gyp   |        v9.4.0         |
|    Python    |        3.11.4         |

## 前言

        本项目的名称叫Astryn-JS，意为“星光一现”。我在一次平常的失眠夜晚，想出了本项目的雏形，就像在银河星海中又添加了一颗闪光点。原本我打算将其命名为MemoryTools，不过随着想法的递进和功能的丰富，这个朴素的名字无法全满地表达项目特色，于是我选择了这个浪漫的名字。

        在本项目开工之前，已有一个备受好评的类似项目：MemoryJS。MemoryJS提供了很多基础的内存相关的API接口。但在很多我了解的情况中，MemoryJS并不能满所有需求。我意图通过更多、更自由的API来实现大家已熟悉的基础功能、我先前较为受用的功能，以及来自我创意的新功能。更重要的是，我将会努力使得用户更容易借由此项目发挥自己的思路和创意。

        我的构想是使用本项目的开发者能够使用Javascript构建一个完整的游戏脚本，包含了内存读写、图色识别、键鼠控制等操作。我希望我的用户能够使用本项目来配合MemoryJS、CheatEngine、IDA等等成熟的软件快速构建自己的脚本。

        诚然，由于能力和团队限制，项目还并不完善，而且实际涉及的工程量已经远超我初始的预估。我很期待可以借由开源的力量来推动和维持本项目。

## 快速入门

        当前已经实现了两批基础API，各自有其特点。此处要介绍的函数是面向基础略低的用户，更自由和更复杂的API会在之后介绍。

## 基础 API

        基础API旨在提供给使用者自由封装方法，为避免混淆命名，所有函数名都以下划线开头。

        函数名结尾的N是数字(例如1、2、4、8)的统一表示。例如插件提供了_read1、_read2、_read4、_read8 四种函数，统一表示为_readN。

        函数说明中，@paramN后标注函数接收的参数类型与参数描述，@returns后标注函数的返回值类型与返回值描述。

### memory.node

#### \_readN

    @param1 Number: PID
    @param2 BigInt: address

    @returns Number(124)/BigInt(8): result
    @Error :null

    最简单的内存读操作，根据进程PID和给定地址读取默认字节数的值，并将其输出为整数。

| 可使用的函数 | \_read1 | \_read2 | \_read4 | \_read8 |
| ------------ | :-----: | :-----: | :-----: | :-----: |
| 读取字节数   |    1    |    2    |    4    |    8    |
| 返回值类型   | Number  | Number  | Number  | BigInt  |

```js
//假设前文已经取得了_read4函数
var pid = 4556; //通过任务管理器或其他API获得进程PID
var memoryAddress = 0x26d0ee414e8n; //内存地址一律为BigInt
var value = _read4(pid, memoryAddress); //读取成功则返回数值
console.log(value); //114514
```

#### \_readBuf

    @param1 Number: PID
    @param2 BigInt: address
    @param3 Number: length

    @returns Buffer: result
    @Error :null

    稍微灵活一些的内存读操作，根据进程PID和给定地址与给定的字节数，并将其输出为指定长度的Buffer。读一段连续的内存时，使用Buffer传输数据会比多次使用_readN效率更高。若想读取utf-8等类型，可以使用此函数。

#### \_writeN

    @param1 Number: PID
    @param2 BigInt: address
    @param3 Number(124)/BigInt(8): value

    @returns true
    @Error false

    最简单的内存写操作，根据进程PID和给定地址写入默认字节数的给给定值，若成功则返回true，失败返回false。

| 可使用的函数 | \_write1 | \_write2 | \_write4 | \_write8 |
| ------------ | :------: | :------: | :------: | :------: |
| 写入字节数   |    1     |    2     |    4     |    8     |
| 参数 3 类型  |  Number  |  Number  |  Number  |  BigInt  |

##### 示例 \_writeN

```js
//假设前文已经取得了_write4函数和encodeFloatBytes函数
//如果试图写入一个Float类型的数，我们只需将其转换为单精度表示值即可
var pid = 4556; //通过任务管理器或其他API获得进程PID
var memoryAddress = 0x26d0ee414e8n; //内存地址一律为BigInt
var value = 3.1415926;
var isWritten = _write4(pid, memoryAddress, encodeFloatBytes(value));
console.log(isWritten); //true
```

#### \_writeBuf

    @param1 Number: PID
    @param2 BigInt: address
    @param3 Buffer: buffer

    @returns true
    @Error false

    稍微灵活一些的内存写操作，根据进程PID，给定地址，将给定的一整个buffer从该地址开始写入内存。写一段连续的内存时，使用Buffer传输数据会比频繁使用_writeN效率更高。若想写入utf-8等类型，可以使用此函数。

#### \_searchN

    @param1 Number: PID
    @param2 Number(4)/BigInt(8): targetValue
    @param3 BigInt: startAddress
    @param4 BigInt: endAddress
    @param5 Number: memProtection
    @param6 Number: step
    @param7 Number: stopThreshold
    @param8 Function: callback(Number: index, BigInt: address, Bigint: totalSearchedSize)

    @returns Buffer: resultBuf

    基础的内存搜索操作，在指定内存段中搜索目标值。内部自带内存块属性检测，因此封装的时候可以将startAddress与endAddress默认置为极限值。若自定义搜索地址，请注意函数在搜索某一内存块时，总会完整地读取从入口地址指向的内存块，因此读取到的值的两端会出现些许冗余，这是特性，请在JS层自行过滤。1比特和2比特的搜索实用性较低，如有需求，请使用_searchTemplate函数，参见下文。

| 可使用的函数 | \_search4 | \_search8 |
| ------------ | :-------: | :-------: |
| 参数 2 类型  |  Number   |  BigInt   |

| 参数              | 含义                                                                                                                                          |    建议的默认值     |
| ----------------- | :-------------------------------------------------------------------------------------------------------------------------------------------- | :-----------------: |
| param3            | 起始地址                                                                                                                                      | 0x0000000000000000n |
| param4            | 终止地址                                                                                                                                      | 0x7FFFFFFFFFFFFFFFn |
| param5            | 内存块保护属性                                                                                                                                |          4          |
| param6            | 内存对齐，可以理解为正常一步 4 格                                                                                                             |          4          |
| param7            | 停止阈值，搜索到一定数量结果后放弃搜索 <br> 若传入 0 则相当于不设阈值                                                                         |          0          |
| param8            | 处理搜索过程的回调函数，可以实现进度条等功能 <br> 积极的返回值(如 NaN, true)会立即终止搜索 <br> 消极的返回值(如 undefined, false)则不打断搜索 |       ()=>{}        |
| index             | 已搜索到目标值的数量<br>注意 index 的增量不为 1，判断时不建议使用==                                                                           |
| address           | 当前搜索的内存块的虚拟内存基址                                                                                                                |                     |
| totalSearchedSize | 当前已经搜索完成的内存字节数                                                                                                                  |

#### \_searchTemplate

    @param1 Number: PID
    @param2 Array: template[Number]
    @param3 BigInt: startAddress
    @param4 BigInt: endAddress
    @param5 Number: memProtection
    @param6 Number: step
    @param7 Number: stopThreshold
    @param8 Function: callback(Number: index, BigInt: address, Bigint: totalSearchedSize)

    @returns Array: [Bigint: address]

    搜索特征码模板 支持包含未知值的模板匹配 比纯特征码搜索更自由，比群组搜索更精准。
    你可以搜索这样一段内存：9A 02 00 00 33 33 33 33 09 ?? ?? ?? ?? 00 00 00 00 6D 02 00 00 ?? ?? ?? ?? 6D 02 00 00
    在template中，0~255的值会被当做一个字节，超过255的值会被视为未知值，即??。特别注意，以未知值作为template的开头和末尾是毫无意义的，请勿这样做。

```js
//假设前文已经取得了_searchTemplate函数
const mySearchTemplate =(pid, template, startAddr = 0x0n, endAddr = 0x7fffffffffffffffn, memProtection = 4, step = 1,  stopThreshold = 1, callback = (a,b,c)=>{
    console.log(`progress: ${(c / totalReadableSize).toFixed(2)}%`);//假设前文已经获取了该进程的全部可读字节总数为 totalReadableSize
}){
    return _searchTemplate(pid, template, startAddr, endAddr, memProtection, step, stopThreshold, callback);
}
//假设这是一个可以生成某个游戏的技能特征码的函数
function getSkillTemplate(id) {
  let baseTemplate = [
    0xff, 0xff, 0xff, 0xff, 6666, 6666, 6666, 6666, 0xf2, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0000, 0000, 0000, 0000, 0xf2, 0xff, 0xff, 0xff,
  ];
  baseTemplate[19] = id >>> 24;
  baseTemplate[18] = (id << 8) >>> 24;
  baseTemplate[17] = (id << 16) >>> 24;
  baseTemplate[16] = (id << 24) >>> 24;
  return baseTemplate;
}
var pid = 4556; //通过任务管理器或其他API获得进程PID
var resultArr = mySearchTemplate(pid, getSkillTemplate(300522));
if(resulrArr.length){
    console.log(`first result: 0x${resultArr[0].toString(16)}`);
}
```

#### \_queryAllProcesses

    @param -
    @returns Array:[{Number: PID, String: name}]
    @Error null

    获取全部进程信息

#### \_openProcessByName

    @param1 String: name
    @returns Number: ProcessHandle
    @Error null

    暂不可用，请在js脚本中配合_queryAllProcesses实现此功能

#### \_openProcess

    @param1 Number: desiredAccess
    @param2 Number: inheritHandle
    @param3 Number: pid

    returns Number: processHandle

    以指定的权限打开进程，返回进程句柄。

```js
//假设前文已经取得_openProcess函数
function myOpenProcess(pid) {
  const PROCESS_ALL_ACCESS = 0x000f0000 | 0x00100000 | 0xfff;
  return _openProcess(PROCESS_ALL_ACCESS, false, pid);
}

var pid = 0x22d4;
var processHandle = myOpenProcess(pid);
```

#### \_closeHandle

    @param1 Number: processHandle

    returns true(success)/false(failed)

    关闭句柄，避免浪费内存。

```js
//假设前文已经取得_closeHandle函数、进程句柄processHandle
_closeHandle(processHandle);
```

#### \_hread

    @param1 Number: processHanndle
    @param2 Number: valueSize
    @param3 Buffer: addressBuffer

    @returns Object: { Buffer: indexBuf, Buffer: resultBuf, Number: valueSize}

    批量内存读操作，根据进程句柄、指定的值大小、装载地址列表的buffer批量读取指定未知的数值，返回一个对象，该对象包含装载标记是否成功读取的buffer、装载读取所得值列表的buffer、所得值列表对齐值，分别为indexBuf、resultBuf、valueSize。addressBuffer每8比特表示一个地址，按低位优先写入。indexBuf的第i比特表示第i个值是否读取成功，成功为0x01，失败为0x00。resultBuf每valueSize个比特代表一个值，按低位优先读取。
    例如：
    {
        indexBuf: <Buffer 01 00>,
        resultBuf: <Buffer 9a 02 00 00 00 00 00 00>,
        valueSize: 4
    }
    由valueSize得知resultBuf每4位为一个值，由indexBuf得知第一个值读取成功，而第二个值读取失败。由resultBuf，9a 02 00 00 对应4字节整数666。后续的第二个值 00 00 00 00 为无效值。
    在大型应用中，一个内存块的大小可能大于2GB，这超过了Buffer的承载上限，请在编写脚本时根据实际情况考虑应对措施。

```js
//假设前文已经取得了_hread函数与进程句柄processHandle
function convertAddrArrayToBuffer(arr) {
  let buffer = Buffer.alloc(arr.length * 8, 0x00);
  arr.forEach((addr, index) => {
    const L = Number(addr & 0xffffffffn) >> 0;
    const B = Number(addr >> 32n) >> 0;
    buffer.writeInt32LE(L, 8 * index);
    buffer.writeInt32LE(B, 8 * index + 4);
  });
  return buffer;
}
let addressArray = [0x26d0ee414e8n, 0x26d29451570n];
let addressBuffer = convertAddrArrayToBuffer(addressArray); //<Buffer e8 14 e4 0e 6d 02 00 00 70 15 45 29 6d 02 00 00>
let result = memory._hread(processHandle, 4, buffer);
```

#### \_hwrite

    @param1 Number: processHanndle
    @param2 Number: valueSize
    @param3 Buffer: addressBuffer
    @param4 Buffer: valueBuffer

    @returns Object: {Buffer: indexBuf}

    批量内存写操作，根据进程句柄、指定的值大小、装载地址列表的buffer、装载值列表的buffer，将一组值批量写入内存，返回一个对象，该对象包含装载标记是否成功写入的buffer，即indexBuf。addressBuffer每8比特表示一个地址，按低位优先写入。valueBuffer每valueSize个比特代表一个值，按低位优先写入。indexBuf的第i比特表示第i个值是否写入成功，成功为0x01，失败为0x00。

#### \_hsearchN

    @param1 Number: processHandle
    @param2 Number(124)/BigInt(8): targetValue
    @param3 BigInt: startAddress
    @param4 BigInt: endAddress
    @param5 Number: step
    @param6 Number: stopThreshold

    @returns Array: [Bigint: address]

    专用于已经打开的进程且句柄权限足够、已知安全且可读的内存范围。
    注意内存范围必须完整包含所有数据占用的空间，而不是地址的起始和终止简单相减。
    例如：
        地址         值         值大小
        0x04        666          4
        0x08        888          4
    设置startAddress不应大于0x04，endAddress不应小于0x12，否则搜不到结果。
    注意与_searchN的区别，_hsearchN没有内置内存块保护属性检查，因此不可以将起始地址与终止地址设置为0x0与0x7fffffffffffffff。
    _searchN常用于对整个进程大范围搜索，数据交互较为庞大，因此传参使用Buffer。_hsearchN常用于已知小范围频繁搜索，因此参数1使用进程句柄，返回值是数组。两者各有特点，请酌情使用。
    一般js层调用此函数会配合winapi.node的vritualQqueryEx函数，组装成一个整体的search方法。传入_searchN的起始和终止地址是可读内存块的起始和终止，此时不会出现上述问题。

| 可使用的函数 | \_hsearch1 | \_hsearch2 | \_hsearch4 | \_hsearch8 |
| ------------ | :--------: | :--------: | :--------: | :--------: |
| 搜索字节数   |     1      |     2      |     4      |     8      |
| 参数 2 类型  |   Number   |   Number   |   Number   |   BigInt   |

#### \_hsearchcbN

    @param1 Number: processHandle
    @param2 Function: checkValue(Number(124)/BigInt(8): value)
    @param3 BigInt: startAddress
    @param4 BigInt: endAddress
    @param5 Number: step
    @param6 Number: stopThreshold

    @returns Array: [Bigint: address]

    与_hsearchN的区别是，参数2传入的不是一个精确值，而是一个回调函数，用于自定义判断，实现扩大或过滤搜索。例如可以传入(v)=>{return v > 4 && v < 8}来搜索4~8范围内的整数。

| 可使用的函数 | \_hsearchcb1 | \_hsearchcb2 | \_hsearchcb4 | \_hsearchcb8 |
| ------------ | :----------: | :----------: | :----------: | :----------: |
| 搜索字节数   |      1       |      2       |      4       |      8       |
| 回调参数类型 |    Number    |    Number    |    Number    |    BigInt    |

### floatConversion.node

    遵循IEEE754标准的浮点数编码与解码。如果需要读取一个float类型数值，只需要以4字节整数类型读取，然后将读取到的值转换为小数即可。相应的，如果要写入一个double类型数值，需要将小数转换为双精度浮点表示法所对应的整数(此时应使用bigInt)，然后执行写操作即可。

    以下函数的参数和返回值类型均与下述所表示相同：

    @param1 Number: value
    @returns Number: Number: result

    值得一提的是，高级API中纯JS实现的小数与单精度浮点数表示法的相互转换效率要高于调用C++的基础API，而双精度则相反。

#### \_encodeFloatBits

    将小数转换为单精度浮点表示法对应的4字节整数

#### \_decodeFloatBits

    将一个4字节整数解析为单精度浮点表示法对应的小数

#### \_encodeDoubleBits

    将小数转换为双精度浮点表示法对应的 8 字节整数

#### \_decodeDoubleBits

    将一个8字节整数解析为双精度浮点表示法对应的小数

### mouse.node

#### \_mouseEvent

    一次自定义的鼠标事件

#### \_move

    鼠标移动到指定位置

#### \_leftClick

    鼠标左键单击

#### \_rightClick

    鼠标右键单击

#### \_leftDown

    鼠标左键按下

#### \_leftUp

    鼠标右键按下

#### \_scroll

    鼠标滚轮滚动

#### \_scrollUp

    鼠标滚轮向上滚动

#### \_scrollDown

    鼠标滚轮向下滚动

### keyboard.node

#### \_keyEvent

    一次自定义的键盘输入

#### \_keyDown

    键盘指定建按下

#### \_keyUP

    键盘指定键抬起

## 高级 API

    高级API是项目附带的一些默认函数，相对常用，功能性更强，方便使用者直接调用。同时作为文档的延伸与基础API具体使用示例，帮助使用者更好地理解各API的作用与搭配关系。

### NumberRepresentation.js

#### encodeXXBits/decodeXXBits

    注意，在JavaScript中，所有的Number数字本质都是双精度小数。因此涉及到计算大于 2^53 - 1 的数字时，应使用BigInt。

| 可使用的函数       | 描述                                                      | 传入值类型 | 返回值类型 |
| ------------------ | --------------------------------------------------------- | :--------: | :--------: |
| encodeByteBits     | 将传入的数转为无符号 1 字节整数                           |   Number   |   Number   |
| decodeByteBits     | 将传入的数转为 1 字节整数                                 |   Number   |   Number   |
| encodeWordBits     | 将传入的数转为无符号 2 字节整数                           |   Number   |   Number   |
| decodeWordBits     | 将传入的数转为 2 字节整数                                 |   Number   |   Number   |
| encodeIntBits      | 将传入的数转为无符号 4 字节整数                           |   Number   |   Number   |
| decodeIntBits      | 将传入的数转为 4 字节整数                                 |   Number   |   Number   |
| encodeLongLongBits | 将传入的数转为无符号 8 字节整数                           |   BigInt   |   BigInt   |
| decodeLongLongbits | 将传入的数转为 8 字节整数                                 |   BigInt   |   BigInt   |
| encodeFloatBits    | 将传入的小数转为单精度浮点数表示法对应的无符号 4 字节整数 |   Number   |   Number   |
| decodeFloatBits    | 将传入的数转换为单精度浮点数表示法对应的小数              |   Number   |   Number   |
| encodeDoubleBits   | 将传入的小数转为双精度浮点数表示法对应的无符号 8 字节整数 |   BigInt   |   BigInt   |
| decodeDoubleBits   | 将传入的数转换为双精度浮点数表示法对应的小数              |   BigInt   |   BigInt   |

### memory.js

#### readMemoryFromPID

#### writeMemoryFromPID

#### searchMemoryFromHandle

#### searchMemoryFromPID

#### searchTemplateFromPID

#### analyseGroupSearchStatement

#### groupSearchFromPID

### process.js

#### queryAllProcesses
