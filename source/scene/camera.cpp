#include "camera.hpp"
#include "math/matrix.hpp"

constexpr f32 SPEED       =  25 ;
constexpr f32 SENSITIVITY =  0.1f ;

Camera::Camera(Vec3 pos, f32 fov, f32 ar, Vec3 up, f32 yaw, f32 pitch, f32 zn, f32 zf)
{
    m_mouseSensitivity = SENSITIVITY;
    m_movementSpeed = SPEED;

    m_position = pos;
    m_worldUp  = up;
    m_yaw      = yaw;
    m_pitch    = pitch;

    m_hfov  = (f32)(DEG2RAD(fov) / 2.0f);
    m_ar    = ar;
    m_znear = zn;
    m_zfar  = zf;
    m_proj  = mat4Perspective(zn, zf, m_hfov * 2, ar);

    updateCamera();
}

void Camera::updateCamera()
{
    Vec3 front_;
    float cosYaw = cosf(m_yaw);
    front_.x = cosYaw * sinf(m_pitch);
    front_.y = sinf(m_yaw);
    front_.z = cosYaw * cosf(m_pitch);
    m_front = normalize(front_);
    m_right = normalize(cross(m_worldUp, m_front));
    m_up = normalize(cross(m_front, m_right));
    m_view = mat4LookAt(m_position, m_front , m_worldUp);
}

void Camera::processKeyboard(CameraMovement direction, f32 deltaTime)
{
    Vec3 velocity;
    if (direction & FORWARD ) velocity += m_front;
    if (direction & BACKWARD) velocity -= m_front;
    if (direction & LEFT    ) velocity -= m_right;
    if (direction & RIGHT   ) velocity += m_right;
    if (squareMagnitude(velocity) == 0) return;
    m_position += normalize(velocity) * m_movementSpeed * deltaTime;
    m_view = mat4LookAt(m_position, m_front , m_worldUp);
}

void Camera::processMouseMovement(f32 xoffset, f32 yoffset)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw   += yoffset;
    m_pitch += xoffset;

    if (m_yaw > (f32)DEG2RAD( 89.0f))
        m_yaw = (f32)DEG2RAD( 89.0f);
    if (m_yaw < (f32)DEG2RAD(-89.0f))
        m_yaw = (f32)DEG2RAD(-89.0f);

    updateCamera();
}

void Camera::setFOV(f32 fov) {
    m_hfov = (f32)(DEG2RAD(fov) / 2.0f);
    f32 tanhfov = tanf(m_hfov);
    m_proj[0][0] = 1 / (tanhfov * m_ar);
    m_proj[1][1] = 1 / tanhfov;
}

void Camera::setAspectRatio(f32 ar) {
    m_ar = ar;
    f32 tanhfov = tanf(m_hfov);
    m_proj[0][0] = 1 / (tanhfov * m_ar);
}

void Camera::setPlanes(f32 zn, f32 zf) {
    m_proj = mat4Perspective(zn, zf, m_hfov * 2, m_ar);
}
