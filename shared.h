#ifndef SHARED_H
#define SHARED_H

#include <QOpenGLFunctions_4_4_Core>
#include <QFile>

#include <exception>
#include <memory>

typedef QOpenGLFunctions_4_4_Core GL_t;
class GameWindow;

enum {
    KILOBYTE = 1024 * 1,
    MEGABYTE = 1024 * KILOBYTE,
    GIGABYTE = 1024 * MEGABYTE
};

#endif // SHARED_H
