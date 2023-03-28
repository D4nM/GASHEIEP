// Daniel Michelin

#include "Particle.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cmath> //also for M_PI
#include <cstdlib> //for RAND_MAX


double ModuleOf3DVector(double x, double y, double z)
{
    return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
}


/////////////////////
// PUBLIC ELEMENTS //

int Particle::f_NumParticleType = 0; //initialising static counter


// GETTERS //

double Particle::getImpulse(char component) const
{
    switch(component)
    {
    case 'x': return f_P.x;
        break;
    case 'y': return f_P.y;
        break;
    case 'z': return f_P.z;
        break;
    default:
        {
            std::cout << "Invalid char in getting impulse component: insert either 'x', 'y' or 'z'\n";
            return -1;
        }
        break;
    }
}

std::string Particle::getName() const
{
    return f_ParticleType[f_IndexParticle]->getName();
}

double Particle::getMass() const
{
    return f_ParticleType[f_IndexParticle]->getMass();
}

int Particle::getCharge() const
{
    return f_ParticleType[f_IndexParticle]->getCharge();
}

int Particle::getIndex() const { return f_IndexParticle; }

int Particle::getNumParticleType() { return f_NumParticleType; }

std::string Particle::getParticleType(const int index)
{
    if(index < 0 || index >= f_NumParticleType)
    {
        return std::string{"ERROR: corresponding type not found"};
    }
    else
    {
        return Particle::f_ParticleType[index]->getName();
    }
}

// SETTERS //

void Particle::setImpulse(double px, double py, double pz)
{
    f_P.x = px;
    f_P.y = py;
    f_P.z = pz;
}

void Particle::setImpulse(char component, double value)
{
    switch(component)
    {
    case 'x': f_P.x = value;
        break;
    case 'y': f_P.y = value;
        break;
    case 'z': f_P.z = value;
        break;
    default:
        {
            std::cout << "Invalid char in setting impulse component: insert either 'x', 'y' or 'z'\n";
        }
        break;
    }
}

void Particle::setIndex(std::string const& name)
{
    int index = FindParticle(name);
    
    if(index >= 0 && index < ((int)(f_ParticleType.size())))
    {
        f_IndexParticle = index; //if a match is found, the particle's index is assigned
    }
    else
    {
        std::cout << "Cannot set index: the particle \"" << name << "\" (which is calling this method) is not present in the table\n";
        // should this output be omitted? Nowhere is mentioned to implement it ------------------
    }
}

void Particle::setIndex(int newIndex)
{
    f_IndexParticle = newIndex;
}


// CONSTRUCTOR //

Particle::Particle(std::string const& name, double Px, double Py, double Pz) : f_P{Px, Py, Pz}
{
    int index = FindParticle(name);
    // Remember that 'index == -1' means that the particle has no correspondence in the table/vector (of types)
    
    if(index >= 0 && index < ((int)(f_ParticleType.size()))) // '&& index < f_ParticleType.size()' is perhaps superflous;
    {                                               // 'index < f_ParticleType.size()' may be switched with 'index < f_NumParticleType'
        f_IndexParticle = index;
        //std::cout << "Assignment of particle index/ID: matching type found\n"; //FOR TESTING PURPOSES
    }
    else
    {
        std::cout << "Assignment of particle index/ID: NO matching type found\n";
    }
};


// FUNCTIONS //

void Particle::AddParticleType(std::string const& name, double mass, int charge, double resonanceWidth)
{
    int index = FindParticle(name);

    //std::cout << "index = " << index << '\n'; //FOR TESTING PURPOSES

    if(index >= 0 && index < f_NumParticleType) //if the index already exists
    {
        std::cout << "Particle type \""<< name << "\" is already present\n";
    }
    else //if it's a new type
    {
        if(f_NumParticleType < f_MaxNumParticleType) //if the 'table' of particle types isn't full
        {
            std::cout << " Adding particle \"" << name << "\" to the table...\n";

            if(resonanceWidth > 0)
            {
                f_ParticleType.push_back(new ResonanceType{name, mass, charge, resonanceWidth});
            }
            else if(resonanceWidth == 0.)
            {
                f_ParticleType.push_back(new ParticleType{name, mass, charge});
            }
            
            ++f_NumParticleType;
        }
        else //if table is full
        {
            std::cout << "Cannot add particle type: table is full\n";
        }
    }
}

void Particle::PrintParticleTable() //const //removed the 'const' because it's static
{
    for(unsigned int i=0; i < f_ParticleType.size(); ++i)
    {
        f_ParticleType[i]->Print();
    }
    std::cout << '\n';
}

void Particle::PrintParticleDetails() const
{
    std::cout << '\n' << "Index = " << f_IndexParticle
              << '\n' << "Particle: " << f_ParticleType[f_IndexParticle]->getName()
              << '\n' << "Px = " <<  getImpulse('x')
              << '\n' << "Py = " << getImpulse('y')
              << '\n' << "Pz = " << getImpulse('z')
              << '\n';
}

double Particle::ParticleEnergy() const
{
    auto mass = f_ParticleType[f_IndexParticle]->getMass();

    auto impulseModule = ModuleOf3DVector(f_P.x, f_P.y, f_P.z);
    auto energy = sqrt(pow(mass, 2) + pow(impulseModule, 2));
    
    return energy;
}

double Particle::InvMass(Particle const& partic2) const
{
    auto energy1 = ParticleEnergy();
    auto energy2 = partic2.ParticleEnergy();
    auto sumOfPx = f_P.x + partic2.getImpulse('x');
    auto sumOfPy = f_P.y + partic2.getImpulse('y');
    auto sumOfPz = f_P.z + partic2.getImpulse('z');
    
    auto invariantMass = sqrt(pow(energy1 + energy2, 2) - pow(ModuleOf3DVector(sumOfPx, sumOfPy, sumOfPz), 2));
    
    return invariantMass;
}

int Particle::Decay2Body(Particle &dau1, Particle &dau2) const
{
  if(getMass() == 0.0)
  {
      std::cout << "\nDecayment cannot be performed if mass is zero\n";
      return 1;
  }
  
  double massMot = getMass();
  double massDau1 = dau1.getMass();
  double massDau2 = dau2.getMass();

  if(f_IndexParticle > -1) // add width effect
  {
    // gaussian random numbers

    float x1, x2, w, y1, y2;
    
    double invnum = 1./RAND_MAX;
    do {
      x1 = 2.0 * rand()*invnum - 1.0;
      x2 = 2.0 * rand()*invnum - 1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;

    massMot += f_ParticleType[f_IndexParticle]->getWidth() * y1;

  }

  if(massMot < massDau1 + massDau2)
  {
      std::cout << "\nDecayment cannot be performed because mass is too low in this channel\n";
      return 2;
  }
  
  //double pout = sqrt((massMot*massMot - (massDau1+massDau2)*(massDau1+massDau2))*(massMot*massMot - (massDau1-massDau2)*(massDau1-massDau2))) / massMot*0.5;
  double pout = sqrt( (massMot*massMot - pow(massDau1+massDau2, 2)) * (massMot*massMot - pow(massDau1-massDau2, 2)) ) / massMot*0.5;

  double norm = 2*M_PI/RAND_MAX;

  double phi = rand()*norm;
  double theta = rand()*norm*0.5 - M_PI/2.;
  dau1.setImpulse(pout*sin(theta)*cos(phi), pout*sin(theta)*sin(phi), pout*cos(theta));
  dau2.setImpulse(-pout*sin(theta)*cos(phi), -pout*sin(theta)*sin(phi), -pout*cos(theta));

  //double energy = sqrt(fPx*fPx + fPy*fPy + fPz*fPz + massMot*massMot);
  double energy = sqrt(pow(ModuleOf3DVector(f_P.x, f_P.y, f_P.z), 2) + massMot*massMot);

  double bx = f_P.x/energy;
  double by = f_P.y/energy;
  double bz = f_P.z/energy;

  dau1.Boost(bx,by,bz);
  dau2.Boost(bx,by,bz);

  return 0;
}

int Particle::FindParticle_public(std::string const& name)
{
    return Particle::FindParticle(name);
}


/////////////////////
// PRIVATE METHODS //

std::vector<ParticleType*> Particle::f_ParticleType{}; //"initializing" static table of types

int Particle::FindParticle(std::string const& name)
{
    //std::cout << "f_ParticleType.size() = " << f_ParticleType.size() << '\n'; //FOR TESTING PURPOSES

    for(unsigned int i=0; i < f_ParticleType.size(); ++i) // 'i < f_ParticleType.size()' may be switched with 'i < f_NumParticleType'
    {
        if(f_ParticleType[i]->getName() == name)
        {
            return i; // returns index value (i.e. position in the vector) if there's a match,
        }             // that is if the passed particle type ('name') is already present in the vector
    }

    return -1; //'no match' value
}

void Particle::Boost(double bx, double by, double bz)
{
    double energy = ParticleEnergy();

    //Boost this Lorentz vector
    double b2 = bx*bx + by*by + bz*bz;
    double gamma = 1.0 / sqrt(1.0 - b2);
    double bp = bx*f_P.x + by*f_P.y + bz*f_P.z;
    double gamma2 = b2 > 0 ? (gamma - 1.0)/b2 : 0.0;

    f_P.x += gamma2*bp*bx + gamma*bx*energy;
    f_P.y += gamma2*bp*by + gamma*by*energy;
    f_P.z += gamma2*bp*bz + gamma*bz*energy;
}