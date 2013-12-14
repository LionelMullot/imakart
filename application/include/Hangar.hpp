#ifndef HANGAR_HPP
#define HANGAR_HPP

#include <vector>
#include <string>

class Kart;

/**
 * @brief The Hangar class représente un hangar qui gère les Kart.
 * Concrètement c'est lui qui va créer des Karts et gérer leur allocation mémoire.
 * Ca peut etre là aussi qu'on gère le chargement des types de Kart etc...
 */
// Cette classe est à modifier pour l'instant c'est juste un embryon qui offre une méthode utilitaire :
// => getPlayerKart() et gère ce Kart. Il faudra qu'elle soit capable de gérer un nombre indéfini de Kart
// par la suite.
class Hangar
{
public:
  Hangar();
  ~Hangar();

  Kart& getPlayerKart() const;

private:
  Kart* playerKart;
  std::vector<std::string> findKartFiles();
  std::vector<std::string> kartNames;
  std::vector<Kart*> playerKarts;
};

#endif // HANGAR_HPP
