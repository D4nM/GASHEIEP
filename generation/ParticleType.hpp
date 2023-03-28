// Daniel Michelin

#ifndef PARTICLETYPE_HPP
#define PARTICLETYPE_HPP
#include <string>

class ParticleType
{
public:
  ParticleType(std::string const& name, double mass, int charge); //parametric constructor
  
  std::string getName() const;
  double getMass() const;
  int getCharge() const;
	  
  virtual double getWidth() const; //returns 0.
  
  //the virtual keywork here will make sure to call the redefined version (of a derived class) of the method
  virtual void Print() const; //print all particle attributes to terminal
	  

protected:
  
	
private:
  std::string const f_Name;
  double const f_Mass;
  int const f_Charge;
	
};

#endif
