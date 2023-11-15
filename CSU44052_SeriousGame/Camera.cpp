#include "Camera.h"

Camera::Camera()
{
    Position = glm::vec3(0.0f, 0.5f, 3.0f);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = YAW;
    Pitch = PITCH;
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    MoveSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    updateCameraVectors();
}

Camera::Camera(glm::vec3 position, glm::vec3 up)
{
    Position = position;
    Up = up;
    Yaw = YAW;
    Pitch = PITCH;
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    MoveSpeed = SPEED;
    MouseSensitivity = SENSITIVITY;
    WorldUp = up;

    updateCameraVectors();
}

glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::vec3 Camera::getCameraPosition()
{
    return Position;
}

void Camera::keyControl(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = MoveSpeed * deltaTime;

    auto y = Position.y;

    if (direction == FORWARD && !CheckCollision(Position + Front * velocity))
    {
        Position += Front * velocity;
    }

    if (direction == BACKWARD && !CheckCollision(Position - Front * velocity))
    {
        Position -= Front * velocity;
    }

    if (direction == LEFT && !CheckCollision(Position - Right * velocity))
    {
        Position -= Right * velocity;
    }

    if (direction == RIGHT && !CheckCollision(Position + Right * velocity))
    {
        Position += Right * velocity;
    }

    Position.y = y;
}

void Camera::mouseControl(GLdouble xChange, GLdouble yChange)
{
    xChange *= MouseSensitivity;
    yChange *= MouseSensitivity;

    Yaw += xChange;
    Pitch += yChange;

    // To make sure the camera follows the physics of a human head
    if (Pitch > 89.0f)
    {
        Pitch = 89.0f;
    }

    if (Pitch < -89.0f)
    {
        Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

bool Camera::CheckCollision(const glm::vec3& updatedPosition)
{
    // Camera boundry radius
    float cameraRadius = 0.0f;

    // Explain this condition 
    for (const auto& object : objectBoundings)
    {
        // Position and the boundry radius an object
        glm::vec3 pos = object.position;
        GLfloat radius = object.radius + 0.01f;

        // Distance between camera and the object
        GLfloat distance = glm::distance(updatedPosition, pos);

        // Check for collision
        if (distance < cameraRadius + radius)
        {
            return true;
        }
    }

    return false;
}

Camera::~Camera()
{
}
