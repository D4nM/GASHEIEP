// Daniel Michelin

#include "ParticleType.hpp"
#include <iostream>
#include <string>

ParticleType::ParticleType(std::string const& name, double mass, int charge) : f_Name{name}, f_Mass{mass}, f_Charge{charge} {};

std::string ParticleType::getName() const { return f_Name; }
double ParticleType::getMass() const { return f_Mass; }
int ParticleType::getCharge() const { return f_Charge; }

double ParticleType::getWidth() const { return 0.; }

void ParticleType::Print() const //do I need to invoke the getter as "ParticleType::getName()" or nah? -> seems not
{
  std::cout << '\n'
            << " Particle: " << getName()
            << '\n'
            << " Mass = " << getMass()
            << '\n'
            << " Charge = " << getCharge()
            << '\n';
}