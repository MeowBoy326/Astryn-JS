const winapi = require("../WinAPI/libs/build/Release/winapi.node");

const memoryapi = require("./libs/build/Release/memoryapi.node");

console.log(Object.keys(memoryapi));

var pid = 0x22d4;
var processHandle = winapi._openProcess(0x000f0000 | 0x00100000 | 0xfff, false, pid);
console.log(processHandle);

console.log(
  memoryapi._readProcessMemory(
    processHandle,
    0x26d0ee414e8n,
    (data) => {
      console.log("??", data);
    },
    4,
    (byteRead) => {
      console.log("!!", byteRead);
    }
  )
);

var buffer = Buffer.from([0x9b, 0x20, 0x00, 0x00]);
console.log(
  memoryapi._writeProcessMemory(processHandle, 0x26d0ee414e8n, buffer, 4, (byteWritten) => {
    console.log("!!", byteWritten);
  })
);
