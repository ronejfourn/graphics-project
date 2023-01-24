#include <math.h>
#include "camera.hpp"
#include "math.hpp"

constexpr f32 SPEED       =  2.5f ;
constexpr f32 SENSITIVITY =  0.1f ;
constexpr f32 ZOOM        =  20.0f;

Camera::Camera(Vec4 pos, f32  fov, f32  ar, Vec4 up, f32  yaw, f32 pitch)
{
    m_mouseSensitivity = SENSITIVITY;
    m_zoom = ZOOM;
    m_movementSpeed = SPEED;

    m_position = pos;
    m_worldUp  = up;
    m_yaw      = yaw;
    m_pitch    = pitch;

    m_hfov = DEG2RAD(fov) / 2;
    m_ar  = ar;
    m_projection = mat4Perspective(0.01f, 1000, fov, ar);

    updateCamera();
}

void Camera::updateCamera()
{
    Vec4 front_;
    float cosYaw = cos(m_yaw);
    front_.x = cosYaw * sin(m_pitch);
    front_.y = sin(m_yaw);
    front_.z = cosYaw * cos(m_pitch);
    m_front = normalize(front_);
    m_right = normalize(cross(m_worldUp, m_front));
    m_up = normalize(cross(m_front, m_right));
}

void Camera::processKeyboard(CameraMovement direction, f32 deltaTime)
{
    Vec4 velocity;
    if (direction & FORWARD ) velocity += m_front;
    if (direction & BACKWARD) velocity -= m_front;
    if (direction & LEFT    ) velocity -= m_right;
    if (direction & RIGHT   ) velocity += m_right;
    velocity.w = 0;
    if (squareMagnitude(velocity) == 0) return;
    m_position += normalize(velocity) * m_movementSpeed * deltaTime;
}

void Camera::processMouseMovement(f32 xoffset, f32 yoffset)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw   += yoffset;
    m_pitch += xoffset;

    if (m_yaw > DEG2RAD( 89.0f))
        m_yaw = DEG2RAD( 89.0f);
    if (m_yaw < DEG2RAD(-89.0f))
        m_yaw = DEG2RAD(-89.0f);

    updateCamera();
}

void Camera::processMouseScroll(f32 yoffset)
{
    m_zoom -= (f32)yoffset;
    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

Mat4 Camera::getViewMatrix() {
    return lookAt(m_position, m_front , m_worldUp);
}

Mat4 Camera::getProjectionMatrix() {
    return m_projection;
}

void Camera::setFOV(f32 fov) {
    m_hfov = DEG2RAD(fov) / 2;
    f32 tanhfov = tan(m_hfov);
    m_projection[0][0] = 1 / (tanhfov * m_ar);
    m_projection[1][1] = 1 / tanhfov;
}

void Camera::setAspectRatio(f32 ar) {
    m_ar = ar;
    f32 tanhfov = tan(m_hfov);
    m_projection[0][0] = 1 / (tanhfov * m_ar);
}
