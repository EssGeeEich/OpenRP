#ifndef LINK_H
#define LINK_H
#include "shared.h"
#include "state.h"
#include "rp_opengl.h"
#include "rp_openal.h"

namespace LuaApi {

class Link {
    DrawableState m_drawState;
    SquareShape m_square;
    DeviceList m_deviceList;
public:
    Link();
    bool Init(GL_t*, GameWindow*, Lua::State&);
    void registerEnums(Lua::State&);
};

}

#endif // LINK_H
