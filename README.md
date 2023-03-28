# Generation and Analysis of a Simulated High Energy Impact between Elementary Particles (G.A.S.H.E.I.E.P)
The program is comprised of two distinct parts:
- the first one simulates the particles that get released/produced following a high energy impact between two elementary particles, such as two ions or two atomic nuclei, all according some predetermined parameters;
- the second one analysises the generated particles through quantitative and statistical methods.

Specifically, the objective of the analysis is to highlight the presence after the impact of an unstable particle which decays too quickly (i.e. its mean lifetime is too short) in order to be observed or detected directly.

In terms of coding and programming, the aim of the project was to practise object-oriented programming, generating pRNGs via Monte Carlo methods in ROOT, and analysing data in ROOT.


## ROOT installation
Install ROOT following the instructions present on [the official site](https://root.cern/install/).



# Running the C++/ROOT files via the included scripts (Ubuntu)

## General information
There are two executable scripts available which employ the `expect` command to automate the compiling and loading of the C++ libraries & ROOT macros necessary for either the generation of the particles or their analysis. Because of that, if you were to find them not executable via terminal, it's suggested to make them so, i.e. doing `$ chmod +x filename.ext`.

The two script are recognisable by their `.expect` "extension" (which isn't actually an extension).

To install the `expect` command and its automation capabilities, for Debian-based OSs (such as Ubuntu),  
execute `$ sudo apt-get install expect` in the command terminal.  

Check its installation directory with `$ whereis expect` and, if needed, modify the `.expect` files by putting the correct installation location path in the shebang at the start of the script.


## Running the scripts
Run the scripts only from the folder you found them in, i.e. the directory which contains the folders `generation` and `analysis`.  

Run the scripts from the OS command terminal only: they will start a ROOT session on their own, send some commands and then let you interact with the console, but they have no way of knowing where you're trying to run them from, if from the command terminal or from elsewhere.

Typically, one would want to execute the generation one first and then the analysis one. But since the second script cannot be launched from the ROOT terminal (which will be open after the first script), to compile & load the analysis-related files the function `LoadAnalysisMacro()` can be run.  
In case the mentioned function gives a compilation error, it won't be able to be executed again, or rather it'll think that it has already been run. Thus it will be necessary to exit the ROOT session or input the compilation command manually  
(e.g. `gROOT->LoadMacro("./analysis/macro_HistogramAnalysis.cpp+")` ).


## Recommended order of actions
It is assumed that ROOT, `expect` and other dependencies have already been installed, and that the scripts have been made executable and correctly configured for your system.
1. Open the command terminal and navigate to the directory where the contents of the downloaded archive have been extracted to.
2. Execute `$ ./macroLoader_generation.expect`: a ROOT session will start.
3. Now in the ROOT console, execute `LoadAnalysisMacro()`.
4. Execute `VerifyData()`: a ROOT canvas with analysed data will pop up.
5. Execute `AnalyseHistograms(true)`: another ROOT canvas with analysed data will pop up.
6. Quit ROOT by executing `.q` when you're done.



# Running the C++/ROOT files without the scripts (other OSs)
It is assumed that ROOT has already been installed for your system.
1. Open the command terminal and navigate to the directory where the contents of the downloaded archive have been extracted to.
2. Launch ROOT by executing `> root`.
3. Now in the ROOT console, while making sure that no errors are reported, execute in order:  
`gROOT->LoadMacro("./generation/ParticleType.cpp+")`  
`gROOT->LoadMacro("./generation/ResonanceType.cpp+")`  
`gROOT->LoadMacro("./generation/Particle.cpp+")`  
`gROOT->LoadMacro("./generation/macro_ParticleGeneration.cpp+")`

Now the methods for the generation of the particles can be called, such as `GenerateEvents()`.  

Once the generation has finished and data has been produced, you can load the analysis macro by calling `LoadAnalysisMacro()` (or alternatively by executing `gROOT->LoadMacro("./analysis/macro_HistogramAnalysis.cpp+")` in the ROOT console).  

Start analysing the data by calling `VerifyData()` and `AnalyseHistograms(true)`.



# Regarding the ROOT macros
- Always remain in the parent directory containing the `generation` and `analysis` folders while running the ROOT macros, for they assume that's where `gSystem` is pointing to.

- If `macro_ParticleGeneration.cpp` is loaded, you can run the functions:
  - `GenerateParticleName()` to check how the particle generation works;
  - `GenerateEvents()` to generate the default number of events and particles per event (it will take a while);
  - `SetGenerationParameters()` to use a "more interactive" way to launch GenerateEvents() with custom generation parameters;
  - `LoadAnalysisMacro()` to compile and load `macro_HistogramAnalysis.cpp`.

- If `macro_HistogramAnalysis.cpp` is loaded, you can run the functions:
  - `VerifyAbundancies()` to see the proportions of generated particles per type;
  - `VerifyData()` to analyse the distributions of particle abundancies, impulse and both angles, and print to the screen & terminal their results;
  - `AnalyseHistograms(bool const zoomAroundMax)` to analyse (and calculate) the three invariant mass distributions. Pass `true` as the parameter in order to have the pads zoomed in on the resulting peaks.

