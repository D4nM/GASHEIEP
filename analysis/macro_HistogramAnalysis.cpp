// Daniel Michelin

#include <iostream>
#include <vector>
#include <string>

//ROOT headers
#include "TMath.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TRandom.h" //needed for gRandom
#include "TSystem.h" //needed for gSystem
#include "TStyle.h" //needed fot gStyle


// NOTE: variables starting with 'h_' are histograms which may be duplicates
// 		 of histograms already loaded in memory by the generation macro.
//		 This way, the analysis macro can function independently of also having
//		 the generation macro loaded. 



/////////////////////////
// Getting the histograms
/////////////////////////

TFile* histoFile = new TFile{"./particles_output/particleHistograms.root", "READ"};

TH1F* h_ParticleAbundancies = histoFile->Get<TH1F>("histo_ParticleAbundancies");
TH1F* h_Theta = histoFile->Get<TH1F>("histo_Theta_Distribution");
TH1F* h_Phi = histoFile->Get<TH1F>("histo_Phi_Distribution");
TH1F* h_Impulse = histoFile->Get<TH1F>("histo_Impulse_Distribution");
// TH1F* h_TransverseImpulse = histoFile->Get<TH1F>("histo_TransverseImpulse_Distribution"); //no analysis on transverse impulse
// TH1F* h_Energy = histoFile->Get<TH1F>("histo_Energy_Distribution"); //no analysis on energy

std::vector<TH1F*> h_InvMass{
histoFile->Get<TH1F>("histo_InvMass_OppositeSign"),					// 0 -- histogram 1)
histoFile->Get<TH1F>("histo_InvMass_SameSign"),						// 1 -- histogram 2)
histoFile->Get<TH1F>("histo_InvMass_OppositeSign_PionKaon"),		// 2 -- histogram 3)
histoFile->Get<TH1F>("histo_InvMass_SameSign_PionKaon"),			// 3 -- histogram 4)
histoFile->Get<TH1F>("histo_InvMass_SameKProducts")					// 4 -- histogram 5)
};



//////////////////////
// HISTOGRAMS GRAPHICS
//////////////////////

bool SetGraphicsStatus = false;

void SetGraphics() //must be executed once at the start 
{
	gStyle->SetOptStat(11);
	gStyle->SetOptFit(1111);

	// Particle abundancies
	//h_ParticleAbundancies->SetTitle("");
	h_ParticleAbundancies->GetYaxis()->SetTitle("Entries");
	TAxis* partAb_Xaxis = h_ParticleAbundancies->GetXaxis();
	partAb_Xaxis->SetTitle("Particle type");
	partAb_Xaxis->SetNdivisions(10, partAb_Xaxis->GetNbins(), 0, kFALSE);
	partAb_Xaxis->SetLabelSize(0.045);
	h_ParticleAbundancies->SetFillColor(kCyan+1);
	h_ParticleAbundancies->SetLineColor(kBlack);
	h_ParticleAbundancies->SetLineWidth(1);

	// Theta coordinate distribution
	//h_Theta->SetTitle("");
	h_Theta->GetYaxis()->SetTitle("Entries");
	h_Theta->GetXaxis()->SetTitle("Theta, #Theta [rad]");
	h_Theta->SetFillColor(kGreen-3);
	h_Theta->SetLineWidth(0);

	// Phi coordinate distribution
	//h_Phi->SetTitle("");
	h_Phi->GetYaxis()->SetTitle("Entries");
	h_Phi->GetXaxis()->SetTitle("Phi, #Phi [rad]");
	h_Phi->SetFillColor(kGreen+3);
	h_Phi->SetLineWidth(0);

	// Impulse distribution
	//h_Impulse->SetTitle("");
	h_Impulse->GetYaxis()->SetTitle("Entries");
	h_Impulse->GetXaxis()->SetTitle("Impulse, P [GeV c^{-1} ]");
	h_Impulse->SetFillColor(kOrange-5);
	h_Impulse->SetLineWidth(0);
	
	/* // Energy distribution
	//h_Energy->SetTitle("");
	h_Energy->GetYaxis()->SetTitle("Entries");
	h_Energy->GetXaxis()->SetTitle("Energy, E [GeV]");
	h_Energy->SetFillColor(kYellow-3);
	h_Energy->SetLineWidth(0);	*/
	
	// Invariant mass histograms, in case anybody needs to take a look at them before analysing their data
	for(UInt_t i = 0; i < h_InvMass.size(); ++i)
	{
		h_InvMass[i]->GetYaxis()->SetTitle("Entries");
		h_InvMass[i]->GetXaxis()->SetTitle("Mass, M [GeV c^{-2} ]");
		h_InvMass[i]->SetFillColor(kAzure-(i+4));
		h_InvMass[i]->SetLineColor(kBlack);
		h_InvMass[i]->SetLineWidth(1);
	}

	SetGraphicsStatus = true;
	std::cout << " Initial graphics & style set.\n";
}



////////////
// FUNCTIONS
////////////


struct AxisEdges
{
	Double_t low;
	Double_t up;
};


AxisEdges GetEdges(TH1* const histo)
{
	TAxis* Xaxis = histo->GetXaxis();
	Double_t lowEdge = Xaxis->GetBinLowEdge(Xaxis->GetFirst());
	Double_t upEdge = Xaxis->GetBinUpEdge(Xaxis->GetLast());
	return AxisEdges{lowEdge, upEdge};
}


// Simple function that checks for an overlap between two ranges of values, or if a range comprises a certain value
bool OverlapCheck(Double_t const val1, Double_t const err1, Double_t const val2, Double_t const err2 = 0.) // val2 ought to be considered as the expected value
{
	if( (val1 + err1 >= val2 - err2) && (val1 - err1 <= val2 + err2) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void VerifyAbundancies()
{	
	Double_t const barWidth = 0.8;
	h_ParticleAbundancies->SetBarWidth(barWidth);
	h_ParticleAbundancies->SetBarOffset((1 - barWidth)/2);

	h_ParticleAbundancies->Draw("bar1");

	
	std::cout << '\n' << " = Abundancies distribution = " << '\n';
	
	const Int_t totalEntries = h_ParticleAbundancies->GetEntries();
	std::cout << "> Total number of entries: " << totalEntries << '\n'; //is there an error of the N# of entries? I'd say not... -------------------------
	
	TAxis* Xaxis = h_ParticleAbundancies->GetXaxis();

	for(Int_t i = 1; i <= Xaxis->GetNbins() ; ++i) // alphanumeric labels start from 1
	{
		std::string const name = Xaxis->GetBinLabel(i);
		Int_t const binEntries = h_ParticleAbundancies->GetBinContent(i);
		Int_t const binError = h_ParticleAbundancies->GetBinError(i);
		Double_t const entriesPercentage = ((Double_t)binEntries / (Double_t)totalEntries) * 100;
		Double_t const errorPercentage = ((Double_t)binError / (Double_t)totalEntries) * 100;
		std::cout << "> N# of " << name << " : " << binEntries << " +/- " << binError;
		std::cout << " ==> (" << entriesPercentage << " +/- " << errorPercentage << ")% of the total" << '\n';
		
		//not-'switch case' consistency verification not possible since expected particle generation probabilities aren't hardcoded
	}
	std::cout << '\n';
}


void VerifyAngularCoord(TH1* histo, std::string const& fittingFunction = "pol1", Double_t const expectedSlope = 0., bool const doPrint = false)
{
	std::string drawOption = "";
	if(doPrint == false) { drawOption = "Q"; } //options: 0 -> doesn't draw anything ; Q -> doesn't print anything
	
	UInt_t fitColor = kRed;

	std::cout << '\n' << " = " << histo->GetName() << " =";

	AxisEdges const edges = GetEdges(histo);
	histo->Fit(fittingFunction.c_str(), drawOption.c_str(), "", edges.low, edges.up);
	
	TF1* fit = histo->GetFunction(fittingFunction.c_str());
	fit->SetLineColor(fitColor);

	Double_t const Chisquare = fit->GetChisquare();
	Int_t const DOF = fit->GetNDF();

	fit->SetParName(0, "intercept"); //no need to redraw
	
	Double_t const intercept = fit->GetParameter(0);
	Double_t const interceptError = fit->GetParError(0);
	std::cout << "> Intercept = " << intercept << " +/- " << interceptError << '\n';

	Double_t slope = 0.;
	Double_t slopeError = 0.;
	if(fittingFunction == "pol1")
	{
		fit->SetParName(1, "slope");
		slope = fit->GetParameter(1);
		slopeError = fit->GetParError(1);
		std::cout << "> Slope = " << slope << " +/- " << slopeError << '\n';
	}
	
	std::cout << "> Chi-squared = " << Chisquare << '\n';
	std::cout << "> DOF = " << DOF << '\n';
	std::cout << "> Reduced Chi-squared = " << Chisquare / DOF << '\n';
	std::cout << "> Prob. that an observed chi^2 exceeds the one obtained = " << TMath::Prob(Chisquare, DOF) << '\n';
	
	if(fittingFunction == "pol1")
	{
		if(OverlapCheck(slope, slopeError, expectedSlope) == true)
		{
			std::cout << "=> Data [IS CONSISTENT] with uniform distribution (observed range overlaps the expected value)" << '\n';
		}
		else
		{
			std::cout << "=> Data [IS NOT CONSISTENT] with uniform distribution (observed range does not overlap the expected value)" << '\n';
		}
		//another way to check consistency would consist in using the reduced Chi-squared integral function	
	}

	std::cout << '\n';
}


void VerifyImpulse(Double_t const expectedImpulse = 1., bool const doPrint = false)
{
	std::string drawOption = "";
	if(doPrint== false) { drawOption = "Q"; } //options: 0 -> doesn't draw anything ; Q -> doesn't print anything
	
	std::cout << '\n';
	std::cout << " = Impulse module distribution = " << '\n';
	
	TF1* customExponential = new TF1{"customExponential","[0]*exp(-[1]*x)"};
	
	AxisEdges const edges = GetEdges(h_Impulse);
	h_Impulse->Fit("customExponential", drawOption.c_str(), "", edges.low, edges.up); //fitting in the interval where the histogram is defined (i.e. 0--10)
	
	TF1* fitImpulse = h_Impulse->GetFunction("customExponential");
	fitImpulse->SetLineColor(kBlue); //does it update it in real time or does it remain the default fit color (red)? -> it gets drawn blue
	
	fitImpulse->SetParName(0, "intercept");
	fitImpulse->SetParName(1, "1/mean");
	
	Double_t const impulseMean = 1./fitImpulse->GetParameter(1); // mean = 1/p1
	Double_t const impulseError = TMath::Abs(impulseMean) * fitImpulse->GetParError(1) / fitImpulse->GetParameter(1);

	Double_t const Chisquare = fitImpulse->GetChisquare();
	Int_t const DOF = fitImpulse->GetNDF();
	
	std::cout << "> Mean = " << impulseMean << " +/- " << impulseError << '\n';
	std::cout << "> Chi-squared = " << Chisquare << '\n';
	std::cout << "> DOF = " << DOF << '\n';
	std::cout << "> Reduced Chi-squared = " << Chisquare / DOF << '\n';
	std::cout << "> Prob. that an observed chi^2 exceeds the one obtained = " << TMath::Prob(Chisquare, DOF) << '\n';
	
	if(OverlapCheck(impulseMean, impulseError, expectedImpulse) == true)
	{
		std::cout << "=> Data [IS CONSISTENT] with exponential distrib. with mean = "<< expectedImpulse << " GeV/c" << '\n';
	}
	else
	{
		std::cout << "=> Data [IS NOT CONSISTENT] with exponential distrib. with mean = "<< expectedImpulse << " GeV/c" << '\n';
	}

	std::cout << '\n';
}



TF1* AnalyseHistoDifference(TH1* histo,
							UInt_t const minuendIndex,
							UInt_t const subtrahendIndex,
							std::string const& histoName,
							std::string const& histoTitle,
							bool const zoomAroundMax = false)
{
	if(minuendIndex >= h_InvMass.size() || subtrahendIndex >= h_InvMass.size())
	{
		std::cout << "Error: at least one of the passed indexes would access an element beyond the vector bounds; the h_InvMass vector isn't that big" << '\n';
		TF1* errorFunc = new TF1{"errorFunc", "gaus", -5, 5};
		return errorFunc;
	}
	else
	{
		histo = static_cast<TH1F*>(h_InvMass[minuendIndex]->Clone());
		
		histo->Sumw2(); // required, as the description of TH1::Add() dictates
		histo->Add(h_InvMass[minuendIndex], h_InvMass[subtrahendIndex], 1, -1);
		histo->SetEntries(histo->Integral());
		
		histo->SetName(histoName.c_str());
		histo->SetTitle(histoTitle.c_str());
		histo->GetYaxis()->SetTitle("Entries difference");
		histo->GetXaxis()->SetTitle("Mass, M [GeV c^{-2} ]");
		histo->SetMarkerColor(kBlack);
		histo->SetMarkerStyle(kCircle);
		histo->SetMarkerSize(0.7);
		//histo->SetFillColor(kTeal-8);
		//histo->SetFillStyle(/**/);
		//histo->SetLineColor(kBlack);
		
		std::cout << '\n';
		std:cout << " = Fitting the resulting histogram of #" << minuendIndex + 1 << "-#" << subtrahendIndex + 1 << " =" << '\n';

		AxisEdges const edges = GetEdges(histo);
		
		Double_t const xMaxBin = histo->GetXaxis()->GetBinCenter(histo->GetMaximumBin());
		Double_t const width = 0.3;

		//turns out that fitting only around the peak gives a worse fit :-/
		//edges = {xMaxBin - width, xMaxBin + width};
		

		//edges = GetEdges(histo); //if using this, remove 'const' from its declaration above

		/*
		//fitting instead from the first non-empty bin doesn't change anything --at this point it's better to fit in the entire range of the histogram
		Double_t xFirstFilledBin;
		for(Int_t d=1; d < histo->GetXaxis()->GetNbins(); ++d)
		{
			if(histo->GetBinContent(d) != 0)
			{
				xFirstFilledBin = histo->GetXaxis()->GetBinLowEdge(d);
				break;
			}
		}
		//std::cout << "xFirstFilledBin = " << xFirstFilledBin << '\n'; //FOR TESTING PURPOSES
		edges.low = xFirstFilledBin;
		*/

		histo->Fit("gaus", "M", "P", edges.low, edges.up);

		TF1* fit = histo->GetFunction("gaus");
		fit->SetLineColor(kRed);

		if(zoomAroundMax == true)
		{
			histo->GetXaxis()->SetRangeUser(xMaxBin - width, xMaxBin + width);
		}

		Double_t const Chisquare = fit->GetChisquare();
		Int_t const DOF = fit->GetNDF();

		std::cout << "> Chi-squared = " << Chisquare << '\n';
		std::cout << "> DOF = " << DOF << '\n';
		std::cout << "> Reduced Chi-squared = " << Chisquare / DOF << '\n';
		std::cout << "> Prob. that an observed chi^2 exceeds the one obtained = " << TMath::Prob(Chisquare, DOF) << '\n';

		return fit;
	}
}


// 5) : histo_InvMass_SameKProducts
TF1* AnalyseHisto_SameKProducts(TH1* histo_sKp, bool const zoomAroundMax = false)
{
	histo_sKp = static_cast<TH1F*>(h_InvMass[4]->Clone());

	histo_sKp->GetYaxis()->SetTitle("Entries");
	histo_sKp->GetXaxis()->SetTitle("Mass, M [GeV c^{-2} ]");
	//histo_sKp->SetMarkerColor(kBlack);
	//histo_sKp->SetMarkerStyle(kCircle); // Put 'P' in goption of Fit() to use markers
	//histo->SetMarkerSize(0.7);
	//histo_sKp->SetFillColor(/**/); //color is already set as kAzure-8
	//histo_sKp->SetFillStyle(/**/);

	std::cout << '\n';
	std:cout << " = Fitting the histogram of the K* decayment products =" << '\n';

	if(zoomAroundMax == true)
	{
		Double_t const xMaxBin = histo_sKp->GetXaxis()->GetBinCenter(histo_sKp->GetMaximumBin());
		Double_t const width = 0.3;
		histo_sKp->GetXaxis()->SetRangeUser(xMaxBin - width, xMaxBin + width);
	}

	AxisEdges const edges = GetEdges(histo_sKp);
	histo_sKp->Fit("gaus", "M", "", edges.low, edges.up);
	
	TF1* fit_sKp = histo_sKp->GetFunction("gaus");
	fit_sKp->SetLineColor(kRed);

	Double_t const Chisquare = fit_sKp->GetChisquare();
	Int_t const DOF = fit_sKp->GetNDF();

	std::cout << "> Chi-squared = " << Chisquare << '\n';
	std::cout << "> DOF = " << DOF << '\n';
	std::cout << "> Reduced Chi-squared = " << Chisquare / DOF << '\n';
	std::cout << "> Prob. that an observed chi^2 exceeds the one obtained = " << TMath::Prob(Chisquare, DOF) << '\n';

	return fit_sKp;
}


// Analyse & print the invariant mass histograms
void AnalyseHistograms(bool const zoomAroundMax = false)
{
	if(SetGraphicsStatus == false) { SetGraphics(); }

	gSystem->cd("particles_output");
	gSystem->mkdir("./invmass_ouput");
	gSystem->cd("invmass_ouput");

	//std::cout << gSystem->pwd() << '\n'; //FOR TESTING PURPOSES

	TFile* output = new TFile{"particleAnalysis.root", "RECREATE"};
	
	TCanvas* canvas = new TCanvas{"canvas_particleHistogramsAnalysis", "Comparing the K* signals", 1700, 700};
	canvas->Divide(3, 1); // 3 columns, 1 row


	canvas->cd(1);

	std::string histoName_1m2 = "histo_DifferenceOfChargedParticlesHistos";
	std::string histoTitle_1m2 = "Inv. mass difference: discordant - concordant charge particles";
	TH1F* histo_1m2 = new TH1F{};
	TF1* fit_1m2 = AnalyseHistoDifference(histo_1m2, 0, 1, histoName_1m2, histoTitle_1m2, zoomAroundMax);
	
	Double_t const mean_1m2 = fit_1m2->GetParameter(1); // Parameters: [1] -> mean, [2] -> sigma
	Double_t const devstd_1m2 = fit_1m2->GetParameter(2);


	canvas->cd(2);

	std::string histoName_3m4 = "histo_DifferenceOfPionKaonHistos";
	std::string histoTitle_3m4 = "Inv. mass difference: discordant - concordant charge Pion-Kaon";
	TH1F* histo_3m4 = new TH1F{};
	TF1* fit_3m4 = AnalyseHistoDifference(histo_3m4, 2, 3, histoName_3m4, histoTitle_3m4, zoomAroundMax);
	
	Double_t const mean_3m4 = fit_3m4->GetParameter(1);
	Double_t const devstd_3m4 = fit_3m4->GetParameter(2);


	canvas->cd(3);

	TH1F* histo_sKp = new TH1F{};
	TF1* fit_sKp = AnalyseHisto_SameKProducts(histo_sKp, zoomAroundMax);
	Double_t const mean_sKp = fit_sKp->GetParameter(1);
	Double_t const devstd_sKp = fit_sKp->GetParameter(2);


	//expected values: mass = 0.89166, std.dev. = 0.05

	std::cout << '\n';
	std::cout << " = Comparing the extracted signals = " << '\n';
	std::cout << "> Histogram #1-#2: (" << mean_1m2 << " +/- " << devstd_1m2 << ") GeV c^-2" << '\n';
	std::cout << "> Histogram #3-#4: (" << mean_3m4 << " +/- " << devstd_3m4 << ") GeV c^-2" << '\n';
	std::cout << "> Histogram of K* products: (" << mean_sKp << " +/- " << devstd_sKp << ") GeV c^-2" << '\n'; 
	
	if(OverlapCheck(mean_1m2, devstd_1m2, mean_sKp, devstd_sKp) == true)
	{
		std::cout << "==> The signals of H_1minus2 & H_KProducts are [COMPATIBLE]" << '\n';
	}
	else
	{
		std::cout << "==> The signals of H_1minus2 & H_KProducts are [NOT COMPATIBLE]" << '\n';
	}

	if(OverlapCheck(mean_3m4, devstd_3m4, mean_sKp, devstd_sKp) == true)
	{
		std::cout << "==> The signals of H_3minus4 & H_KProducts are [COMPATIBLE]" << '\n';
	}
	else
	{
		std::cout << "==> The signals of H_3minus4 & H_KProducts are [NOT COMPATIBLE]" << '\n';
	}
	
	if(OverlapCheck(mean_3m4, devstd_3m4, mean_1m2, devstd_1m2) == true)
	{
		std::cout << "==> The signals of H_3minus4 & H_1minus2 are [COMPATIBLE]" << '\n';
	}
	else
	{
		std::cout << "==> The signals of H_3minus4 & H_1minus2 are [NOT COMPATIBLE]" << '\n';
	}

	std::cout << '\n';

	

	// PRINT the canvas in .pdf, .root, .C
	canvas->Print("Invariant-Mass-Histograms_Analysis.pdf");
	canvas->Print("Invariant-Mass-Histograms_Analysis.png");
	canvas->Print("Invariant-Mass-Histograms_Analysis.cpp");
	canvas->Print("Invariant-Mass-Histograms_Analysis.root");
	
	std::cout << "\n Printing finished.\n";

	
	output->Write(); //does it save the stuff or nah? --it does, but only the pointers declared as 'new'
	
	/*
	output->Close(); //closing TFile* output removes the graphs from the canvas on the screen
	delete output;
	*/

	gSystem->cd("..");
	gSystem->cd("..");

	//std::cout << gSystem->pwd() << '\n'; //FOR TESTING PURPOSES
}


// Checks data consistency for the other histograms
void VerifyData()
{
	if(SetGraphicsStatus == false) { SetGraphics(); }

	gSystem->cd("particles_output");
	gSystem->mkdir("./dataVerif_output");
	gSystem->cd("dataVerif_output");
	TFile* output_Data = new TFile{"dataVerification.root", "RECREATE"};

	std::cout << '\n' << " VERIFICATION OF HISTOGRAM DATA" << '\n';
	
	TCanvas* canvas_Data = new TCanvas{"canvas_DataVerification","Verification of histogram data", 1550, 900};
	canvas_Data->Divide(2,2);

	canvas_Data->cd(1);
	VerifyAbundancies();
	
	canvas_Data->cd(2);
	VerifyImpulse(1., true);

	canvas_Data->cd(3);
	VerifyAngularCoord(h_Theta, "pol0", 0., true);

	canvas_Data->cd(4);
	VerifyAngularCoord(h_Phi, "pol0", 0., true);


	canvas_Data->Print("AbundanciesAnglesImpulse.pdf");
	canvas_Data->Print("AbundanciesAnglesImpulse.png");
	canvas_Data->Print("AbundanciesAnglesImpulse.cpp");
	canvas_Data->Print("AbundanciesAnglesImpulse.root");

	std::cout << "\n Printing finished.\n";


	h_ParticleAbundancies->Write();
	h_Impulse->Write();
	h_Theta->Write();
	h_Phi->Write();

	gSystem->cd("..");
	gSystem->cd("..");
}




