#pragma once

struct GarbageBagProps 
{
	glm::vec3 translation;
	glm::mat4 matrix;
};

struct StarProps 
{
	glm::vec3 translation;
	glm::mat4 matrix;
};

struct BirdProps 
{
	glm::vec3 birdPosition;
	glm::vec3 birdVelocity;
};

int main();