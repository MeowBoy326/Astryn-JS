const winapi = require("../WinAPI/libs/build/Release/winapi.node");

const memory = require("../Memory/libs/build/Release/memory.node");

// var pid = 0x22d4;
// var processHandle = winapi._openProcess(0x000f0000 | 0x00100000 | 0xfff, false, pid);
// console.log(processHandle);
// var array = [0xe8, 0x14, 0xe4, 0x0e, 0x6d, 0x02, 0x00, 0x00];
// var array = [
//   0xe8, 0x14, 0xe4, 0x0e, 0x6d, 0x02, 0x00, 0x00, 0x70, 0x15, 0x45, 0x29, 0x6d, 0x02, 0x00, 0x00, 0xb8, 0x15, 0x33, 0xff, 0xfb, 0x7f, 0x0, 0x00,
// ];
// var buffer = Buffer.from(array);

let addressArray = [0x26d0ee414e8n, 0x26d29451570n];

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
function convertResultToList(result) {
  const { indexBuf, resultBuf, valueSize } = result;
}
var buffer = convertAddrArrayToBuffer(addressArray);
// let result = memory._hread(processHandle, 4, buffer);
let result = {
  indexBuf: Buffer.from([1, 0]),
  resultBuf: Buffer.from([0x9a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),
  valueSize: 4,
};
console.log(buffer, result);

// console.time("1");
// var test = memory._hread(processHandle, 4, buffer);
// console.timeEnd("1");
// console.log(test);
// // console.log(test.resultBuf.readInt32LE(0));
// console.log(test.resultBuf.readInt32LE(0), test.resultBuf.readInt32LE(4), test.resultBuf.readInt32LE(8));
