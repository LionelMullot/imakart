#include "Kart.hpp"
#include <iostream>

Kart::Kart():
			position(0., 0., 0.), speed(0.f), direction((GLfloat)0.f,(GLfloat)1.f){
				
}
Kart::Kart(glm::vec3 pos_, float speed_, glimac::Vertex2D dir_):
			position(pos_), speed(speed_), direction(dir_.x, dir_.y){
	
}

// The Kart goes forward : its position changes
void Kart::moveForward(){
	std::cout << "J'avance" << std::endl;
	direction.y = 1.0;
	position.y ++; 
}

//The Kart goes back : its position changes
void Kart::moveBackward(){
	std::cout << "Je recule" << std::endl;
	direction.y = -1.0;
	position.y --;
}

// The Kart goes left : its direction changes
void Kart::turnLeft(){
	std::cout << "Je tourne à droite" << std::endl;
	direction.x = -1.0;
	position.x --;
}

// The Kart goes left : its direction changes
void Kart::turnRight(){
	std::cout << "Je tourne à gauche" << std::endl;
	direction.x = 1.0;
	position.x ++;
}
