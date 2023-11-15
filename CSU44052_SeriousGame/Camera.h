#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct ObjectBounding 
{
	glm::vec3 position;
	float radius;
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	GLfloat MoveSpeed;

	Camera();

	Camera(glm::vec3 position, glm::vec3 up);

	void keyControl(Camera_Movement direction, GLfloat deltaTime);

	void mouseControl(GLdouble xChange, GLdouble yChange);

	glm::mat4 calculateViewMatrix();

	glm::vec3 getCameraPosition();

	std::vector<ObjectBounding> objectBoundings;

	~Camera();

private:

	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat MouseSensitivity;

	void updateCameraVectors();

	bool CheckCollision(const glm::vec3& updatedPosition);
};

