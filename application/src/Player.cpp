#include "Player.hpp"
#include "Kart.hpp"
#include "ItemLogic.hpp"
#include "ItemBox.hpp"
#include <iostream>

const unsigned int Player::MAX_LAP = 2;

Player::Player(Kart& kart, std::stack<GameEvent>& eventStack)
  :myKart(kart), eventStack(eventStack), myCurrentItem(nullptr), currentLap(0), newLapNextTime(false), progression(0.f), nextCheck(0), rank(0)
{
  checkpoints.resize(0);
  myCurrentItem = nullptr;
  myKart.setEventStack(&eventStack);
}

const Kart& Player::getKart() const
{
  return myKart;
}

Kart& Player::getKart()
{
  return myKart;
}

const ItemLogic& Player::getItem() const
{
  return *myCurrentItem;
}

ItemLogic& Player::getItem()
{
  return *myCurrentItem;
}

const bool Player::hasItem() const{
  if(myCurrentItem == nullptr)
    return false;
  return true;
}

const unsigned int& Player::getCurrentLap() const
{
  return currentLap;
}

unsigned int& Player::getCurrentLap()
{
  return currentLap;
}

void Player::fillCheckpoints(const std::vector<Checkpoint> checkpoints)
{
  this->checkpoints = checkpoints;
}

void Player::fillOpponentCheckpoints(const std::vector<Checkpoint> checkpoints)
{
  this->opponentCheckpoints = checkpoints;
}

void Player::validateCheckpoints()
{
  bool allChecked = true;
  bool newLap = false;
  int cpt = 0;
  int nbChecked = 0;
  for (auto it = checkpoints.begin(); it != checkpoints.end(); ++it)
  {
    if (newLapNextTime && !it->start)
      continue;
    if (it->contains(myKart.getPosition()))
    {
      it->checked = true;
      if(it->start && (newLapNextTime || currentLap == 0))
        newLap = true;
    }
    if ( !it->checked)
      allChecked = false;
    else
      nbChecked++;
    cpt++;
  }

  //Si tous les checkpoints sont check : reset
  if (allChecked)
  {
    newLapNextTime = true;
    for (auto it = checkpoints.begin(); it != checkpoints.end(); ++it)
    {
      it->checked = false;
    }
  }

  if (newLap)
  {
    currentLap++;
    //On émet un event nouveau tour
    if (currentLap > Player::MAX_LAP)
    {
      eventStack.push(GameEvent(RACE_FINISHED, GameEventData()));
    }
    else
    {
    GameEventData data;
    data.lapNumber = currentLap;
    eventStack.push(GameEvent(NEW_LAP, data));
    }
    newLapNextTime = false;
  }

  unsigned int cptOpp = 0;
  for (auto it = opponentCheckpoints.begin(); it != opponentCheckpoints.end(); ++it)
  {
    if (it->contains(myKart.getPosition()))
    {
      if(cptOpp == checkpoints.size()-1){
        cptOpp = 0;
      }else{
        cptOpp++;
      }
      nextCheck = cptOpp;
    }
    cptOpp++;
    progression = - sqrt( pow(opponentCheckpoints[nextCheck].position.x - myKart.getPosition().x, 2) + pow(opponentCheckpoints[nextCheck].position.z - myKart.getPosition().z, 2));
  }
  cptOpp = 0;

}

void Player::moveForward() const
{
  	myKart.moveForward();
}

void Player::moveBackward() const
{
  	myKart.moveBackward();
}

void Player::turnLeft() const
{
  myKart.turnLeft();
}

void Player::turnRight() const
{
  myKart.turnRight();
}

void Player::stopMove() const
{
  myKart.stopMove();
}

void Player::stopTurning() const
{
  myKart.stopTurning();
}

void Player::brake() const
{
  myKart.brake();
}

void Player::drift() const
{
  myKart.drift();
}

void Player::setItem(ItemLogic* newItem)
{
  myCurrentItem = newItem;
  myCurrentItem->update();
}

void Player::useItem()
{
  if(myCurrentItem != nullptr){
    std::string effectItem = myCurrentItem->getEffect();
    if(effectItem == "boost()"){
      myKart.boost();
    }else if(effectItem == "launch()"){
      launchItem();
    }
    
    myCurrentItem->noInterface();
    delete myCurrentItem;
    myCurrentItem = nullptr;
  } 
}

void Player::launchItem()
{
  myCurrentItem->launch(myKart.getPosition(), myKart.getOrientation(), myKart.getDirectionAngle());
  GameEventData data;
  data.itemLogicOnMap = new ItemLogic(*myCurrentItem);
  eventStack.push(GameEvent(NEW_ITEM_ON_MAP, data));
}
