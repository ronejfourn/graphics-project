#pragma once

#include "math/matrix.hpp"

enum CameraMovement
{
    FORWARD  = 1 << 0,
    BACKWARD = 1 << 1,
    LEFT     = 1 << 2,
    RIGHT    = 1 << 3,
};

class Camera{
    private:
        //camera characteristics
        Vec3 m_position;
        Vec3 m_front;
        Vec3 m_up;
        Vec3 m_right;
        Vec3 m_worldUp;

        f32 m_znear, m_zfar;

        //Determining euler's angle
        f32 m_yaw;
        f32 m_pitch;

        //controlling movement with precision
        f32 m_mouseSensitivity;
        f32 m_zoom;
        f32 m_movementSpeed;

        f32  m_hfov, m_ar;
        Mat4 m_projection;

    //Camera methods:
    public:

        Camera(Vec3 pos   = Vec3(0, 0, 0),
               f32  fov   = 90,
               f32  ar    = 1,
               Vec3 up    = Vec3(0, 1, 0),
               f32  yaw   = 0,
               f32  pitch = 0,
               f32  znear = 0.01f,
               f32  zfar  = 1000);

        Vec3 getPosition() {return m_position;}
        void setFOV(f32 fov);
        void setAspectRatio(f32 ar);
        Mat4 getViewMatrix();
        Mat4 getProjectionMatrix();
        void processKeyboard(CameraMovement, f32);
        void processMouseMovement(f32 xoffset, f32 yoffset);
        void processMouseScroll(f32);
        void updateCamera();
};
