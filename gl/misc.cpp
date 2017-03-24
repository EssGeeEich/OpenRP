#include "misc.h"
#include "../gamewindow.h"
namespace LuaApi {

// TimerImpl
TimerImpl::TimerImpl() : m_running(false), m_start(TimerImpl::clock::now()), m_time(m_start) {}
void TimerImpl::update()
{
    if(m_running)
    {
        m_time = TimerImpl::clock::now();
    }
}
std::uint64_t TimerImpl::timei() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_time - m_start).count();
}
static float const inv_1000 = static_cast<float>(1.0 / 1000.0);
float TimerImpl::timef() const {
    return static_cast<float>(timei()) * inv_1000;
}
void TimerImpl::start() {
    m_running = true;
}
void TimerImpl::stop() {
    m_running = false;
}
void TimerImpl::reset() {
    m_time = m_start = TimerImpl::clock::now();
}
bool TimerImpl::running() const {
    return m_running;
}
}
