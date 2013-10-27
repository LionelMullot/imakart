#ifndef RACEEVENTHANDLER_HPP
#define RACEEVENTHANDLER_HPP

#include "EventHandler.hpp"

class GameEngine;

class RaceEventHandler : public EventHandler
{

public:
  RaceEventHandler(GameEngine* game);

  virtual void up() const;
  virtual void down() const;
  virtual void left() const;
  virtual void right() const;

private:
  GameEngine* game;

};



#endif // RACEEVENTHANDLER_HPP