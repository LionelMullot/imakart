#include "Camera.hpp"
#include "Positionable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(size_t windowWidth, size_t windowHeight)
  : objectToFollow(nullptr), windowWidth(windowWidth), windowHeight(windowHeight), viewThirdPerson(true), viewBackward(false)
{
}

const glm::vec3 Camera::getPosition() const
{
  if (objectToFollow != nullptr)
  {
    glm::vec3 initialDirection;
    if(viewBackward){
      initialDirection = glm::vec3(0.f, 0.f, 3.f);
    }else{
      initialDirection = glm::vec3(0.f, 0.f, -3.f);
    }
    glm::vec3 direction = glm::toMat3(objectToFollow->getOrientation()) * initialDirection;
    return  (objectToFollow->getPosition() - direction) + glm::vec3(0.f, 2.f, 0.f);
  }
  else
    return glm::vec3(0.f, 0.f, 0.f);
}

const glm::vec3 Camera::getWhereILook() const
{
  //TODO : faire regarder la camÃƒÂ©ra le nez un peu relevÃƒÂ© par rapport Ãƒ  l'objet
  //qu'elle suit
  if (objectToFollow != nullptr)
    return objectToFollow->getPosition() + glm::vec3(0.f, 1.6f, 0.f);
  else
    return glm::vec3(0.f, 0.f, -100.f);
}

const glm::mat4&Camera::getViewMatrix() const
{
  return viewMatrix;
}
const glm::mat4&Camera::getProjectionMatrix() const
{
  return projectionMatrix;
}
const glm::mat4&Camera::getViewProjectionMatrix() const
{
  return viewProjection;
}

void Camera::updateViewProjectionMatrix()
{
  //Decommenter cette ligne et commenter celle d'apres pour avoir une camera qui suit le KartCube
  viewMatrix = glm::lookAt(getPosition(), getWhereILook(), glm::vec3(0.f, 1.f, 0.f));
  //viewMatrix = glm::lookAt(glm::vec3(0.f, 2.5f, 2.5f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 1.f, 0.f));
  projectionMatrix =  glm::perspective(90.f, windowWidth / (float) windowHeight, 0.1f, 1000.f);
  viewProjection = projectionMatrix * viewMatrix;
}

void Camera::switchInBackwardView(){
  viewBackward = true;
}

void Camera::switchInForwardView(){
  viewBackward = false;
}