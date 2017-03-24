#ifndef LINK_H
#define LINK_H
#include "shared.h"
#include "state.h"
#include "gl/all.h"
#include "al/all.h"

namespace LuaApi {

class Link {
    DeviceList m_deviceList;
public:
    Link();
    bool Init(GL_t*, GameWindow*, Lua::State&);
    void registerEnums(Lua::State&);
};

}

#endif // LINK_H
