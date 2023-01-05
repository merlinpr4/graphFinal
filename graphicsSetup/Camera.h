//Camera reference https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVITY = 0.15f;
const float ZOOM = 50.0f;
const float YAW = -90.0f;

class Camera
{
public:
    glm::vec3 Pos;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Pos = pos;
        WUp = up;
        Yaw = yaw;
        Pitch = pitch;
        cameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Pos = glm::vec3(posX, posY, posZ);
        WUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        cameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Pos, Pos + Front, Up);
    }

    //keyboard controls
    void ProcessKeyboard(Movement direction, float dTime)
    {
        float velocity = MovementSpeed * dTime;
        if (direction == FORWARD)
            Pos += Front * velocity;
        if (direction == BACKWARD)
            Pos -= Front * velocity;
        if (direction == LEFT)
            Pos -= Right * velocity;
        if (direction == RIGHT)
            Pos += Right * velocity;
    }

    // mouse inputs
    void MouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 90.0f)
                Pitch = 90.0f;
            if (Pitch < -90.0f)
                Pitch = -90.0f;
        }
        cameraVectors();
    }

    // zoom controls
    void MouseZoom(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    void cameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
   
        Right = glm::normalize(glm::cross(Front, WUp));  
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif

