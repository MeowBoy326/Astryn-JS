

#include <node.h>
#include "readwrite.h"
#include "search.h"
#include "hreadwrite.h"
#include "hsearch.h"
#include "process.h"

namespace memory {

void Init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "_read4", readwrite::read4);
  NODE_SET_METHOD(exports, "_read8", readwrite::read8);
  NODE_SET_METHOD(exports, "_readBuf", readwrite::readBuf);
  NODE_SET_METHOD(exports, "_write4", readwrite::write4);
  NODE_SET_METHOD(exports, "_write8", readwrite::write8);
  NODE_SET_METHOD(exports, "_writeBuf", readwrite::writeBuf);
  NODE_SET_METHOD(exports, "_search4", search::search4);
  NODE_SET_METHOD(exports, "_search8", search::search8);
  NODE_SET_METHOD(exports, "_searchTemplate", search::searchTemplate);
  NODE_SET_METHOD(exports, "_queryAllProcesses", process::queryAllProcesses);
  NODE_SET_METHOD(exports, "_openProcessByName", process::openProcessByName);
  NODE_SET_METHOD(exports, "_openProcess", process::_openProcess);
  NODE_SET_METHOD(exports, "_closeHandle", process::_closeHandle);
  NODE_SET_METHOD(exports, "_hread", hreadwrite::hRead);
  NODE_SET_METHOD(exports, "_hwrite", hreadwrite::hWrite);
  NODE_SET_METHOD(exports, "_hsearch1", hsearch::hSearch1);
  NODE_SET_METHOD(exports, "_hsearch2", hsearch::hSearch2);
  NODE_SET_METHOD(exports, "_hsearch4", hsearch::hSearch4);
  NODE_SET_METHOD(exports, "_hsearch8", hsearch::hSearch8);
  NODE_SET_METHOD(exports, "_hsearchcb1", hsearch::hSearchCb1);
  NODE_SET_METHOD(exports, "_hsearchcb2", hsearch::hSearchCb2);
  NODE_SET_METHOD(exports, "_hsearchcb4", hsearch::hSearchCb4);
  NODE_SET_METHOD(exports, "_hsearchcb8", hsearch::hSearchCb8);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}