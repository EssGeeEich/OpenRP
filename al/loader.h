#ifndef LUAAL_LOADER_H
#define LUAAL_LOADER_H
#include "shared.h"
#include "emitter.h"

namespace LuaApi {
    namespace SoundLoader {
        SoundEmitter LoadFile(std::string const&);
    }
}

#endif
