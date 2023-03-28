// Daniel Michelin

#ifndef RESONANCETYPE_HPP
#define RESONANCETYPE_HPP
#include "ParticleType.hpp"
#include <string>


//with a public inheritance the derived class can call the methods of the base class only through the base class:
//e.g. BaseClass:BaseClassMethod()
class ResonanceType : public ParticleType
{
public:
  ResonanceType(std::string const& name, double mass, int charge, double resonanceWidth); //no initialization list in prototype
  
  double getWidth() const;
  
  void Print() const;
  

protected:


private:
  double const f_Width;

};

#endif