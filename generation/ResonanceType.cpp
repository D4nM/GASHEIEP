// Daniel Michelin

#include "ResonanceType.hpp"
#include <iostream>
#include <string>

ResonanceType::ResonanceType(std::string const& name, double mass, int charge, double resonanceWidth) : 
  ParticleType(name, mass, charge),
  f_Width{resonanceWidth}
  {}

double ResonanceType::getWidth() const { return f_Width; }

void ResonanceType::Print() const
{
  ParticleType::Print();
  std::cout << "Resonance width = " << f_Width << '\n';
}