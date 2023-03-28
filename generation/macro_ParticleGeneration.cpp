// Daniel Michelin

#include "ParticleType.hpp"
#include "ResonanceType.hpp"
#include "Particle.hpp"
#include <iostream>
#include <vector>
#include <string>

//ROOT headers
#include "TMath.h"
#include "TH1F.h"
#include "TFile.h"
#include "TRandom.h" //needed for gRandom
#include "TSystem.h" //needed for gSystem
#include "TROOT.h"
#include "TBenchmark.h"


Int_t executionCounter = 0;

// Adds all the following particles to the table and returns how many there are
Int_t FillParticleTable()
{
    /*
    This goddamn function executes twice everytime I compile it via LoadMacro().
    That's the reason for the executionCounter and the if{} block.
    It literally executes twice for no reason. It gets called only once, to
    initialize NumParticleType (see below the function end), because I tested it:
    removing the only call (again, see below), it won't execute at all.
    WHY DOES IT EXECUTE TWICE!??!?!??!?!?
    I don't know what else to do. Several minutes of web search diddn't help, either.
    */
    if(executionCounter == 0)
    {
        //std::cout << "FillParticleTable() is executed" << '\n'; //FOR TESTING PURPOSES
        std::cout << '\n';

        //add new types here
        Particle::AddParticleType("Pion(+)", 0.13957, 1);
        Particle::AddParticleType("Pion(-)", 0.13957, -1);
        Particle::AddParticleType("Kaon(+)", 0.49367, 1);
        Particle::AddParticleType("Kaon(-)", 0.49367, -1);
        Particle::AddParticleType("Proton(+)", 0.93827, 1);
        Particle::AddParticleType("Proton(-)", 0.93827, -1);
        Particle::AddParticleType("K*", 0.89166, 0, 0.050);


        //std::cout << "FillParticleTable() will soon return: " << Particle::getNumParticleType() << '\n' << '\n'; //FOR TESTING PURPOSES
        
        ++executionCounter;
        
        return Particle::getNumParticleType();
    }

    //std::cout << "2nd execution happened\n"; //FOR TESTING PURPOSES
    return Particle::getNumParticleType();

}

Int_t const NumParticleType = FillParticleTable(); //number of particles in the table



/////////////
// Histograms
/////////////

TH1F* histo_ParticleAbundancies = new TH1F{"histo_ParticleAbundancies", "Number of particles per type", NumParticleType, 0, 10};
TH1F* histo_Theta = new TH1F{"histo_Theta_Distribution", "Distribution of azimutal coordinate theta", 500, 0, TMath::Pi()}; //suggested bin #: 100--1000
TH1F* histo_Phi = new TH1F{"histo_Phi_Distribution", "Distribution of polar coordinate phi", 500, 0, 2*TMath::Pi()}; //suggested bin #: 100--1000
TH1F* histo_Impulse = new TH1F{"histo_Impulse_Distribution", "Impulse distribution", 1000, 0, 10};
TH1F* histo_TransverseImpulse = new TH1F{"histo_TransverseImpulse_Distribution", "Transverse impulse distribution", 1000, 0, 10};
TH1F* histo_Energy = new TH1F{"histo_Energy_Distribution", "Particle energy distribution", 1000, 0, 10};


Int_t const InvMassNbins = 80;
Int_t const InvMassXmax = 2;

//In the general InvariantMass histogram ([0]) decayment products MUST NOT be included
std::vector<TH1F*> invMassHistograms{
new TH1F{"histo_InvariantMass", "Invariant mass between every particle", InvMassNbins, 0, InvMassXmax},							// 0
new TH1F{"histo_InvMass_SameSign", "Invariant mass: concordant charge", InvMassNbins, 0, InvMassXmax},						    // 1
new TH1F{"histo_InvMass_SameSign_PionKaon", "Invariant mass: Pion-Kaon concordant charge", InvMassNbins, 0, InvMassXmax},		// 2
new TH1F{"histo_InvMass_OppositeSign", "Invariant mass: discordant charge", InvMassNbins, 0, InvMassXmax},						// 3
new TH1F{"histo_InvMass_OppositeSign_PionKaon", "Invariant mass: Pion-Kaon discordant charge", InvMassNbins, 0, InvMassXmax},	// 4
new TH1F{"histo_InvMass_SameKProducts", "Invariant mass: same K* decay products", InvMassNbins, 0, InvMassXmax}					// 5
};



////////////
// Functions
////////////


// Sets bin labels for Abundancies histogram, which are used as reference in the filling process
void MakeBinLabelsParticleNames()
{
    for(Int_t i = 0; i < NumParticleType; ++i)
    {
        histo_ParticleAbundancies->GetXaxis()->SetBinLabel(i+1, (Particle::getParticleType(i)).c_str());
    }
}


// Randomly generates a particle type (i.e. the name of the particle)
// Used in the main function
std::string GenerateParticleName()
{
	Double_t a = gRandom->Rndm();
	
	std::string name;
	
	if(a<0.01) //K* 1%
    {
    	name = "K*"; 
	}
	else
	{     
		// The other particles
		
		if(a<0.10) //Protons 9%
		{
			name = "Proton";
		}
		else
		if(a<0.20) //Kaons 10%
		{
			name = "Kaon";
		}
		else //Pions 80%
		{
			name = "Pion";
		}
		
		// setting the charge
		std::string charge;		
		if(gRandom->Rndm() < 0.50)
		{
			charge = "(+)";
		}
		else { charge = "(-)"; }
		
		name.append(charge);
	}
	
	return name;
}


// Main function
void GenerateEvents(Int_t const eventsNum = 1e5, Int_t const partPerEventNum = 100) //default settings: 100k events with 100 particles per event
{
    MakeBinLabelsParticleNames();

    gRandom->SetSeed();

    //Before the following lines execute, the current directory should be the one containing the loading script
    gSystem->mkdir("./particles_output");
    gSystem->cd("particles_output");
    
    TFile* file = new TFile{"particleHistograms.root", "RECREATE"};

    std::cout << "\nGenerating events";

    gBenchmark->Start("Events generation");

    //cycle that generates batches of particles and does all calculations EXCEPT the invariant mass 
    for(Int_t eventCounter = 0; eventCounter < eventsNum; ++eventCounter) //event cycle
    {
        // Live progress bar
        if(eventCounter % ((Int_t)(0.05 * eventsNum)) == 0)
        {
            Double_t fraction = ((Double_t)eventCounter / (Double_t)eventsNum) * 100;
            std::cout << "..." << (Int_t)fraction << "%";
            std::cout.flush();
        }

        std::vector<Particle> particles; //vector that will be filled and emptied every event cycle
        
        for(Int_t particleCounter = 0; particleCounter < partPerEventNum; ++particleCounter) //batch of particles cycle
        {
            std::string particleName{GenerateParticleName()};
            histo_ParticleAbundancies->Fill(particleName.c_str(), 1); //FILLING PARTICLE ABUNDANCIES HISTOGRAM through a defined bin label
            
            Double_t theta = gRandom->Rndm() * TMath::Pi(); //azimutal coordinate
            Double_t phi = gRandom->Rndm() * 2 * TMath::Pi(); //polar coordinate
            Double_t P = gRandom->Exp(1.); //impulse
            
            // Calculating impulse components through spherical coordinates
            Double_t Px = P * TMath::Sin(theta) * TMath::Cos(phi);
            Double_t Py = P * TMath::Sin(theta) * TMath::Sin(phi);
            Double_t Pz = P * TMath::Cos(theta);
            Double_t PTransverse = TMath::Sqrt(Px*Px + Py*Py);


            Particle prtcl{particleName, Px, Py, Pz};
            //prtcl.setIndex(particleName); //sets the ID of the instance --not necessary since ID is set at creation of the instance
            Double_t energy = prtcl.ParticleEnergy();

            histo_Theta->Fill(theta);
            histo_Phi->Fill(phi);
            histo_Impulse->Fill(P);
            histo_TransverseImpulse->Fill(PTransverse);
            histo_Energy->Fill(energy);

            particles.push_back(prtcl); //puts the instance of "chosen" particle into the vector
        }
        
        Int_t const K_ID = Particle::FindParticle_public("K*");
        
        Int_t const p = particles.size(); //p == number of particles present before any decayment
        
        // Cycle that makes K* decay and adds its products at the end of the vector
        for(Int_t i = 0; i < p; ++i) 
        {
            if(particles[i].getIndex() == K_ID) //checks for K*
            {
            	std::string particleName1{"Pion"};
            	std::string particleName2{"Kaon"};
            	
                if(gRandom->Rndm() < 0.50)
                {
                    particleName1.append("(+)");
                    particleName2.append("(-)");
                }
                else
                {
                    particleName1.append("(-)");
                    particleName2.append("(+)");
                }

                Particle dau1{particleName1};
                Particle dau2{particleName2};
                
                particles[i].Decay2Body(dau1, dau2);
 
                particles.push_back(dau1);
                particles.push_back(dau2);
            }
        }

        Int_t const p2 = particles.size(); //p2 == number of particles present after all decayments
     
        // Invariant mass calculation and correspondent histogram filling -- K* must not be considered
        for(Int_t i = 0; i < p2-1; ++i)
        {
        	if(particles[i].getIndex() != K_ID) //skipping K*
        	{
        		for(Int_t j = i+1; j < p2; ++j)
            	{
		        	if(particles[j].getIndex() != K_ID) //skipping K*
		        	{
                        //std::cout << "combination (i,j): " << i << "  " << j << '\n'; //FOR TESTING PURPOSES

		        		Double_t invMass = particles[i].InvMass(particles[j]);
				    	
				    	invMassHistograms[0]->Fill(invMass); //FILLING GENERAL INVARIANT MASS HISTOGRAM

                        Int_t const chargeProduct = particles[i].getCharge() * particles[j].getCharge();

                        std::string particle1_name = particles[i].getName();
                        std::string particle2_name = particles[j].getName();


				    	//cycle that deletes the last three characters of the name, e.g.: "Kaon(+)" --> "Kaon"
				    	for(Int_t g = 0; g < 3; ++g)  //DEPENDENT ON PARTICLE NAME
				    	{
				    		particle1_name.pop_back();
				    		particle2_name.pop_back();
				    	} //this cycle could be made a function: 'void RemoveChargeFromName(std::string& input)'
				    	
				    	if(chargeProduct > 0) //same sign --concordant charges
				    	{
				    		invMassHistograms[1]->Fill(invMass); //FILLING GENERAL SAME SIGN INVARIANT MASS HISTOGRAM
				    		
				    		if( (particle1_name == "Pion" && particle2_name == "Kaon") || (particle1_name == "Kaon" && particle2_name == "Pion") )
				    		{
				    			invMassHistograms[2]->Fill(invMass); //FILLING SAME SIGN PION/KAON HISTOGRAM
				    		}
				    	}
				    	else
				    	if(chargeProduct < 0) //opposite sign --discordant charges
				    	{
				    		invMassHistograms[3]->Fill(invMass); //FILLING GENERAL OPPOSITE SIGN INVARIANT MASS HISTOGRAM
				    		
				    		if( (particle1_name == "Pion" && particle2_name == "Kaon") || (particle1_name == "Kaon" && particle2_name == "Pion") )
				    		{
				    			invMassHistograms[4]->Fill(invMass); //FILLING OPPOSITE SIGN PION/KAON HISTOGRAM
				    		}
		        		}
		        	}	
		        }
        	}
        }
        
        // Invariant mass between decay products of the same K*
        for(Int_t k = partPerEventNum; k < p2; k = k+2) //because the products have been put two by two at the end of the 'particles' vector
        {
        	Double_t invMassDecay = particles[k].InvMass(particles[k+1]);
        	
        	invMassHistograms[5]->Fill(invMassDecay); //FILLING INVARIANT MASS BETWEEN PRODUCTS OF THE SAME K* HISTOGRAM
        }
        
    } //END OF EVENTS GENERATION; END OF THE for loop

    std::cout << "...DONE\n";
    std::cout.flush();

    gBenchmark->Show("Events generation");
    gBenchmark->Reset();
    std::cout << '\n';
    

    //SAVING ALL THE STUFF TO FILE
    histo_ParticleAbundancies->Write();
    histo_Theta->Write();
    histo_Phi->Write();
    histo_Impulse->Write();
    histo_TransverseImpulse->Write();
    histo_Energy->Write();
    
    for(Int_t l = 0; l < ((Int_t)(invMassHistograms.size())); ++l) //saves invariant mass histograms
    {
    	invMassHistograms[l]->Write();
    }

    file->Write(); //Doesn't really seem to work, since the histograms are getting saved to file only through a '->Write()'

    delete file;

    gSystem->cd("..");
}


void SetGenerationParameters()
{
    Int_t events;
    Int_t particlesPerEvent;

    std::cout << "\nNOTE: don't put expressions such as '1e5', because it'll go in an infinite loop\n";

    do
    {
        std::cout << "\nInsert number of events to be generated: ";
        std::cin >> events;

        if(!(events > 0))
        {
            events = 0;
            std::cout << "<!> Incorrect input: must enter a positive value\n";
        }
    } while (!(events > 0));
    
    do
    {
        std::cout << "Insert how many particles will be generated per event: ";
        std::cin >> particlesPerEvent;
        if(!(particlesPerEvent > 0))
        {
            particlesPerEvent = 0;
            std::cout << "<!> Incorrect input: must enter a positive value\n";
        }
    } while (!(particlesPerEvent > 0));

    GenerateEvents(events, particlesPerEvent);
}


bool isAnalysisMacroCompiled = false;
void LoadAnalysisMacro()
{
    if(isAnalysisMacroCompiled == false)
    {
        isAnalysisMacroCompiled = true;
        gROOT->LoadMacro("./analysis/macro_HistogramAnalysis.cpp+");
        std::cout << " Analysis macro compiled & loaded.\n";
    }
    else
    {
        std::cout << " Analysis macro is already compiled & loaded.\n";
    }
}






