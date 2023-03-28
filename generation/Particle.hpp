// Daniel Michelin

#ifndef PARTICLE_HPP
#define PARTICLE_HPP
#include "ParticleType.hpp"
#include "ResonanceType.hpp"
#include <vector>
#include <string>

struct Impulse
{
  double x;
  double y;
  double z;
};

double ModuleOf3DVector(double x = 0., double y = 0., double z = 0.);

class Particle
{
public:
  Particle(std::string const& name, double Px = 0., double Py = 0., double Pz = 0.); //parametric constructor

  double getImpulse(char component) const;
  std::string getName() const;
  double getMass() const;
  int getCharge() const;
  int getIndex() const;
  static int getNumParticleType();
  static std::string getParticleType(const int index); //returns particle type (i.e. the name) corresponding to the passed index
  
  void setImpulse(double px, double py, double pz);
  void setImpulse(char component, double value);
  void setIndex(std::string const& name);
  void setIndex(int index);

  static void AddParticleType(std::string const& name, double mass, int charge, double resonanceWidth = 0.);

  static void PrintParticleTable();
  //void PrintParticleTable() const; //can this be static? ==> to be so, I need to declare all the functions it calls static, right? (see .cpp)
                                    //if made static, remove 'const' (in .cpp too)

  void PrintParticleDetails() const; //prints index, name and impulse
  
  double ParticleEnergy() const; //calculates the energy of a particle through a determined formula; used in other functions
  
  double InvMass(Particle const& partic2) const; //calculates the invariant mass of between two particles through a determined formula

  int Decay2Body(Particle &dau1, Particle &dau2) const; //makes decay a particle into two particles: dau1 & dau2

  static int FindParticle_public(std::string const& name); //used in the generation macro
  
protected:
  
  
private:
  static std::vector<ParticleType*> f_ParticleType;

  static const int f_MaxNumParticleType = 10;

  static int f_NumParticleType; //number of particle types present in the vector f_ParticleType    // 'initialized' in the .cpp file

  int f_IndexParticle; //index corresponding to a certain particle type in the vector f_ParticleType; in a sense, it's a particle's ID
  
  Impulse f_P;

  static int FindParticle(std::string const& name); //made static so to also be used outside of private methods

  void Boost(double bx, double by, double bz);
};

#endif