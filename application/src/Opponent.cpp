#include "Opponent.hpp"
#include "Kart.hpp"
#include <iostream>
#include <cmath>

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif

Opponent::Opponent(Kart& kart)
  :opponentKart(kart), angle(0.f), heading(0.f), x(0.f), z(0.f), turnRight(true)
{
  checkpoints.resize(0);
}

const Kart& Opponent::getKart() const
{
  return opponentKart;
}

Kart& Opponent::getKart()
{
  return opponentKart;
}

void Opponent::fillCheckpoints(const std::vector<Checkpoint> checkpoints)
{
  this->checkpoints = checkpoints;
}

void Opponent::validateCheckpoints()
{
  unsigned int cpt = 0;
  for (auto it = checkpoints.begin(); it != checkpoints.end(); ++it)
  {
    if (it->contains(opponentKart.getPosition()))
    {
      if(cpt == checkpoints.size()-1){
        cpt = 0;
      }else{
        cpt++;
      }

      x = checkpoints[cpt].position.x - opponentKart.getPosition().x;
      z = checkpoints[cpt].position.z - opponentKart.getPosition().z;
    }
    cpt++;

  }
  cpt = 0;

  angle = 2* atan(x / (z + sqrt(z*z + x*x)));
  angle = angle * 360.f / (2*M_PI);
  angle = angle -180;
  if(angle < -180){
    angle += 360.f;
  }

  float qx = opponentKart.getOrientation().x;
  float qy = opponentKart.getOrientation().y;
  float qz = opponentKart.getOrientation().z;
  float qw = opponentKart.getOrientation().w;

  heading = (atan2(2*qy*qw-2*qx*qz , 1 - 2*qy*qy - 2*qz*qz)) * 360.f / (2*M_PI);

  opponentKart.moveForward();

  if(angle != 0.f){
      if(heading <= (angle -1.5f) || heading >= (angle +1.5f)){
        if(heading <= 0 && angle <= 0){
          if(fabs(heading) - fabs(angle) <0){
            opponentKart.turnRight();
          }else{
            opponentKart.turnLeft();
          }
        }else if(heading > 0 && angle > 0){
          if(fabs(heading) - fabs(angle) >0){
            opponentKart.turnRight();
          }else{
            opponentKart.turnLeft();
          }
        }else if(heading > 0 && angle < 0){
          if(heading - angle < 180.f){
            opponentKart.turnRight();
          }else{
            opponentKart.turnLeft();
          }
        }else if(heading < 0 && angle > 0){
          if(angle - heading > 180.f){
            opponentKart.turnRight();
          }else{
            opponentKart.turnLeft();
          }
        }
      }else{
        opponentKart.stopTurning();
      }
  }
}

void Opponent::startMovement(){
   //opponentKart.moveForward();
   //opponentKart.turnLeft();
}



