const floatConversion = require("./libs/build/Release/floatConversion.node");

const { _encodeFloatBits, _decodeFloatBits, _encodeDoubleBits, _decodeDoubleBits } = floatConversion;

const encodeByteBits = (byte) => {
  return byte & 0xff;
};

const decodeByteBits = (hex) => {
  return (hex << 24) >> 24;
};

const encodeWordBits = (word) => {
  return word & 0xffff;
};

const decodeWordBits = (hex) => {
  return (hex << 16) >> 16;
};

const encodeIntBits = (dword) => {
  return dword >>> 0;
};

const decodeIntBits = (hex) => {
  return hex >> 0;
};

const encodeFloatBits = (float) => {
  if (isNaN(float)) return 0x7fc00000;
  if (1 / float === 0) return float > 0 ? 0x7f800000 : 0xff800000; //Infinity
  if (float === 0) return 1 / float === -Infinity ? 0x80000000 : 0x00000000; //区分正负0

  let S, E, M, HEX;
  S = float >= 0 ? 0 : 1;
  E = Math.floor(Math.log(float >= 0 ? float : -float) / Math.LN2) + 127;
  E = E >= 0xff ? 0xff : E < 0 ? 0 : E;
  M = E >= 0xff ? 0 : Math.round((1 << 23) * ((Math.sign(float) * float) / 2 ** (E - 127 + !E) - !!E));
  HEX = ((S << 31) | (E << 23) | M) >>> 0;
  return HEX;
};

const decodeFloatBits = (int) => {
  let S, E, M, result;
  S = int >>> 31;
  E = (int << 1) >>> 24;
  M = (int << 9) >>> 9;
  if (E === 0xff) {
    if (M === 0x0) {
      return (-1) ** S * Infinity;
    } else {
      return NaN;
    }
  }
  result = (-1) ** S * (M / (1 << 23) + !!E) * 2 ** (E - 127 + !E);
  return result;
};

//c++直接强转指针类型处理边界值还是很给力的 这里只需要稍作处理一下NaN
const encodeDoubleBits = (double) => {
  if (isNaN(double)) return 0x7ff8000000000000n;
  return _encodeDoubleBits(double);
};

const decodeDoubleBits = (hex) => {
  return _decodeDoubleBits(hex);
};

const encodeLongLongBits = (bigInt) => {
  return 0xffffffffffffffffn & bigInt;
};

const decodeLongLongbits = (longhex) => {
  return longhex <= 0x7fffffffffffffffn ? longhex : longhex - 0x10000000000000000n;
};

const float_to_hex_buffer_abandon = (floatValue) => {
  const buffer = Buffer.alloc(4); // 创建一个长度为 4 的 Buffer 对象
  buffer.writeFloatLE(floatValue, 0); // 将浮点数写入到 Buffer 中
  let int = buffer.readInt32LE(0);
  return int;
};

const long_to_double_abandon = (bigInt) => {
  let hex1, S, E, M, result;
  hex1 = Number(bigInt >> 32n);
  S = hex1 >>> 31;
  E = (hex1 << 1) >>> 21;
  M = Number(bigInt & 0x000fffffffffffffn) / 2 ** 52 + !!E; //bigInt是整型
  if (E === 0x7ff) {
    if (M === 0x0) {
      return (-1) ** S * Infinity;
    } else {
      return NaN;
    }
  }
  result = (-1) ** S * M * 2 ** (E - 1023 + !E);
  return result;
};

const double_to_long_abandon = (double) => {
  let S, E, M, HEX, HEX1, HEX2;
  if (isNaN(double)) {
    return 0x7ff8000000000000n;
  }
  S = double >= 0 ? 0 : 1;
  E = Math.floor(Math.log(double >= 0 ? double : -double) / Math.LN2) + 1023;
  E = E >= 0x7ff ? 0x7ff : E < 0 ? 0 : E;
  M = E >= 0x7ff ? 0 : Math.round(2 ** 52 * ((Math.sign(double) * double) / 2 ** (E - 1023 + !E) - !!E));
  HEX = (BigInt((S << 11) | E) << 52n) | BigInt(M);
  return HEX;
};
const long_to_double_buffer_abandon = (long) => {
  const buffer = Buffer.alloc(8); // 创建一个长度为 4 的 Buffer 对象
  buffer.writeBigInt64LE(long, 0);
  let double = buffer.readDoubleLE(0);
  return double;
};
const double_to_long_buffer_abandon = (double) => {
  const buffer = Buffer.alloc(8); // 创建一个长度为 4 的 Buffer 对象
  buffer.writeDoubleLE(double, 0);
  let long = buffer.readBigInt64LE(0);
  return long;
};

exports.numConvert = {
  encodeByteBits,
  decodeByteBits,
  encodeFloatBits,
  decodeFloatBits,
  encodeWordBits,
  decodeWordBits,
  encodeIntBits,
  decodeIntBits,
  encodeDoubleBits,
  decodeDoubleBits,
  encodeLongLongBits,
  decodeLongLongbits,
};
