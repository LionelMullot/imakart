#include "World3D.hpp"
#include <assimp/postprocess.h>
#include "Object3D.hpp"
#include <stdexcept>
#include <sstream>
#include "Camera.hpp"
#include <iostream>
#include <cstdio>

Assimp::Importer World3D::import;


World3D::World3D(const unsigned int width, const unsigned int height)
    : camera(new Camera(width,height)), sun(new DirectionalLight()), ambientLight(0.1f,0.1f,0.1f,1.0f), spot(new SpotLight()), day(false)//Un peu degueu, a voir, c'est pour simplifier
{
  //Pour le dessin du monde 3D
  raceProgram.addShader(GL_VERTEX_SHADER, "shaders/Simple3DVS.glsl");
  raceProgram.addShader(GL_FRAGMENT_SHADER, "shaders/SimpleFS.glsl");

  //Pour la skybox
  skyboxProgram.addShader(GL_VERTEX_SHADER, "shaders/Skybox.vs.glsl");
  skyboxProgram.addShader(GL_FRAGMENT_SHADER, "shaders/Skybox.fs.glsl");
}

World3D::~World3D()
{
  //Utiliser un iterator declenche un bug hyper chelou (tente de detruire un deuxieme
  //pointeur vers la classe mere...)
  for (size_t i = 0; i < objects3D.size(); ++i)
  {
    delete objects3D[i];
  }

  objects3D.erase(objects3D.begin(), objects3D.end());
  delete camera;
  delete sun;

}

void World3D::init()
{
  std::string logInfo;
  if (!raceProgram.compileAndLinkShaders(logInfo))
  {
    throw std::runtime_error(logInfo);
  }
  logInfo.clear();

  if (!skyboxProgram.compileAndLinkShaders(logInfo))
  {
    throw std::runtime_error(logInfo);
  }

  if(day)
  {
    /****** SKYBOX JOUR ******/
    if(!skybox.init("textures/skybox/day","_deserted_back.tga","_deserted_front.tga","_deserted_up.tga","_deserted_bottom.tga","_deserted_right.tga","_deserted_left.tga"))
    {
      throw std::runtime_error("Impossible d'initialiser la SkyBox");
    }
  }
  else
  {
    /****** SKYBOX NUIT *****/
    if(!skybox.init("textures/skybox/night","nightsky_north.bmp","nightsky_south.bmp","nightsky_up.bmp","nightsky_down.bmp","nightsky_east.bmp","nightsky_west.bmp"))
    {
      throw std::runtime_error("Impossible d'initialiser la SkyBox");
    }
  }
  
  skybox.setCamera(camera);
}

void World3D::draw() const
{
  //Mise a jour matrice ViewProjection
  //Attention : le vertex shader doit contenir les bonnes uniforms
  camera->updateViewProjectionMatrix();
  skyboxProgram.use();
  skybox.render(skyboxProgram);

  raceProgram.use();
  const glm::mat4& viewMatrix = camera->getViewMatrix();
  const glm::mat4& viewProjection = camera->getViewProjectionMatrix();
  GLint viewId = raceProgram.getUniformIndex("uView");
  GLint viewProjectionId = raceProgram.getUniformIndex("viewProjection");
  raceProgram.setUniform(viewId, viewMatrix);
  raceProgram.setUniform(viewProjectionId, viewProjection);

  //Ranger aussi la gestion des lumieres
  //Gestion des lumières ponctuelles


  for (auto oneLight = lights.begin(); oneLight != lights.end(); ++oneLight)
  {
    (*oneLight)->updateLight(viewMatrix);
    const glm::vec4& position = (*oneLight)->getLightPosition();
    const glm::vec3& intensity = (*oneLight)->getLightIntensity();
    GLint lightPosId = raceProgram.getUniformIndex("point.uLightPos");
    GLint lightIntensityId = raceProgram.getUniformIndex("point.uLi");
    raceProgram.setUniform(lightPosId,position);
    raceProgram.setUniform(lightIntensityId, intensity);
  }

  char NamePos [50];
  char NameInt [50];

  for (unsigned int i = 0 ; i < lights.size() ; i++) {
    lights[i]->updateLight(viewMatrix);

    sprintf(NamePos, "points[%u].uLightPos", i);
    sprintf(NameInt, "points[%u].uLi", i);

    GLint lightPosId = raceProgram.getUniformIndex(NamePos);
    GLint lightIntensityId = raceProgram.getUniformIndex(NameInt);
    raceProgram.setUniform(lightPosId,  lights[i]->getLightPosition());
    raceProgram.setUniform(lightIntensityId, lights[i]->getLightIntensity());
  }
/*  GLint lightPosId0 = raceProgram.getUniformIndex("points[0].uLightPos");
  std::cout << lightPosId0 << std::endl;
  GLint lightIntensityId0 = raceProgram.getUniformIndex("points[0].uLi");
  GLint lightPosId1 = raceProgram.getUniformIndex("points[1].uLightPos");
  GLint lightIntensityId1 = raceProgram.getUniformIndex("points[1].uLi");

  raceProgram.setUniform(lightPosId0,  lights[0]->getLightPosition());
  raceProgram.setUniform(lightIntensityId0, lights[0]->getLightIntensity());
  raceProgram.setUniform(lightPosId1,  lights[1]->getLightPosition());
  raceProgram.setUniform(lightIntensityId1, lights[1]->getLightIntensity());
*/
  //Gestion d'une spotLight
  spot->updateLightPosition();
  //spot->updateLightDirection();
  //spot->updateLight(viewMatrix);
  const glm::vec4& spotPos = spot->getLightPosition();
  const glm::vec4& spotDir = spot->getLightDirection();
  const glm::vec3& spotIntensity = spot->getLightIntensity();
  const float& spotCutoff = spot->getLightCutoff();
  GLint spotPosId = raceProgram.getUniformIndex("spot.uLightPos");
  GLint spotDirId = raceProgram.getUniformIndex("spot.uLightDir");
  GLint spotIntensityId = raceProgram.getUniformIndex("spot.uLi");
  GLint spotCutId = raceProgram.getUniformIndex("spot.uCutoff");
  raceProgram.setUniform(spotPosId,spotPos);
  raceProgram.setUniform(spotDirId,spotDir);
  raceProgram.setUniform(spotIntensityId, spotIntensity);
  raceProgram.setUniform(spotCutId,spotCutoff);

  //Gestion de la lumière directionnelle
  sun->updateLight(viewMatrix);
  const glm::vec4& direction = sun->getLightDirection();
  const glm::vec3& intensity = sun->getLightIntensity();
  GLint lightDirId = raceProgram.getUniformIndex("directional.uLightDir");
  GLint lightIntensityId = raceProgram.getUniformIndex("directional.uLi");
  raceProgram.setUniform(lightDirId,direction);
  raceProgram.setUniform(lightIntensityId, intensity);

  const glm::vec4& ambient = getAmbientLight();
  GLint ambientLightId = raceProgram.getUniformIndex("uAmbientLight");
  raceProgram.setUniform(ambientLightId, ambient);

  for (auto object3D = objects3D.begin(); object3D != objects3D.end(); ++object3D)
  {
    (*object3D)->setViewMatrix(camera->getViewMatrix());
    (*object3D)->update();
    (*object3D)->draw(raceProgram);
  }
}

void World3D::setCamera(Camera* newCamera)
{
  delete camera;
  camera = newCamera;
  skybox.setCamera(camera);
}

void World3D::setSpot(SpotLight* newSpot)
{
  delete spot;
  spot = newSpot;
}

void World3D::switchInBackwardView(){
  camera->switchInBackwardView();
}

void World3D::switchInForwardView(){
  camera->switchInForwardView();
}

void World3D::switchInLeftBendView(){
  camera->switchInLeftBendView();
}

void World3D::switchInRightBendView(){
  camera->switchInRightBendView();
}


void World3D::setSize(const unsigned int width, const unsigned int height){
  camera->setSize(width, height);
}

void World3D::addLights(const std::string filePath){
  const aiScene* scene = import.ReadFile( filePath,
             aiProcess_CalcTangentSpace |
             aiProcess_Triangulate |
             aiProcess_JoinIdenticalVertices |
             aiProcess_SortByPType);

  if (!scene)
  {
    std::ostringstream errorMessage;
    errorMessage << "Light : Impossible de trouver le fichier de modele 3D : " << filePath << "." << std::endl;
    throw std::runtime_error(errorMessage.str());
  }

  lights.resize(scene->mNumLights);

  PointLight* l;
  glm::vec4 pos;
  for(unsigned int i = 0; i < scene->mNumLights; ++i)
  {
    const aiLight* const light = scene->mLights[i];
    pos = glm::vec4(light->mPosition.x,light->mPosition.y,light->mPosition.z,1.0);
    l = new PointLight(pos);
    this->addLight(l);
  }
}
