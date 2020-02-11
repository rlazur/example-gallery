//////////////////////////////////////////////////////////////////////////////////////////////////
//
// This script is an example of a study I did on ICARUS and SBND to validate spacecharge implementation.
// This script grabs track information from Reco Base and analyzes the endpoints as well as track trjectory.
// This code is in working order, with the addition of excess comments to remind myself how gallery works when I need it for whatever reason in like 3.7 years.
// 
// author: rlazur@colostate.edu
///////////////////////////////////////////////////////////////////////////////////////////////////

//some standard C++ includes                             
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <fstream>

//some ROOT includes                                                           
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStyle.h"
#include "TCanvas.h"

//"art" includes (canvas, and gallery)                                                    
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindOne.h"

//"larsoft" object includes
#include "lardataobj/RecoBase/Track.h" //focus of this study


using namespace art;
using namespace std;
using namespace std::chrono;

//Module Label(s)
string fTrackModuleLabel = "pandoraKalmanTrackICARUSCryo0"; //this is the MODULE LABEL of interest

//use "lar -c eventdump.fcl -n 1 <art-root-file.root>" to see the data products available to you.
//The output is in the form | PROCESS NAME | MODULE LABEL | PRODUCT INSTANCE | DATA PRODUCT TYPE | SIZE. Honestly, the module label, data product type, and size are the only important fields.
//Go ahead and google "larsoft <module label> <data product type>" and look through the documentation for that product to get a feel for what is available to you


//Loop control
const int maxEvents = 99999999; //how many events to look at 
const int maxEventsPerRun = 999999999; //how many events from each run   
const int maxTicks = 9594; //time ticks in a waveform           

int main(int argc, char *argv[]){
  
  //load in art root files from the text files in your build directory
  cout << "Reading from filelist.txt" << endl;
  vector<string> filenames; //spacecharge off
  vector<string> filenames_sce; //spacecharge on
  ifstream myfile("filelist.txt"); //ifstreams to be read by gallery
  ifstream myfile_sce("filelist_doublesce.txt");
  copy(istream_iterator<string>(myfile),istream_iterator<string>(),back_inserter(filenames));
  copy(istream_iterator<string>(myfile_sce),istream_iterator<string>(),back_inserter(filenames_sce));
  
  //open output files
  TFile f_output("output.root","RECREATE");
  ofstream soutput;
  ofstream eoutput;
  ofstream soutput_sce;
  ofstream eoutput_sce;
  ofstream cosoutput;
  ofstream cosoutput_sce;
  soutput.open("startpoints.txt");
  eoutput.open("endpoints.txt");
  soutput_sce.open("startpoints_doublesce.txt");
  eoutput_sce.open("endpoints_doublesce.txt");
  cosoutput.open("cosines.txt");
  cosoutput_sce.open("cosines_doublesce.txt");

  //control variables for looping                          
  int totalEventCount = 0;
  int prevRun = -9999;
  int runEventCount = 0;
  int chanNum;

  //begin looping through non-sce data event-by-event
  for (gallery::Event ev(filenames) ; !ev.atEnd(); ev.next()){
    auto t_begin = high_resolution_clock::now();
    int thisRunNum = ev.eventAuxiliary().run();
    int thisSubrunNum = ev.eventAuxiliary().subRun();
    int thisEventNum = ev.eventAuxiliary().event();
    
    //loop control
    if (thisRunNum != prevRun){//new run
      prevRun = thisRunNum;
      runEventCount = 0;}
    //in MC it seems like every evt has the same run number
    if (runEventCount++ > maxEventsPerRun) {break;} //enough of this run
    //toal event count is probably the only thing that matters much
    if (totalEventCount++ > maxEvents){
      cout << "I think you've seen enough...events" << endl;
      break;
    }
    cout << "Processing " << "Run " << thisRunNum << ", " << "Subrun " << thisSubrunNum << ", " << "Event " << thisEventNum << endl;

    try{  //to look for the track object
      auto const& track = *ev.getValidHandle< vector <recob::Track > > (fTrackModuleLabel);
      for (auto const& t : track){ //loop through objects in evt
	const double len = t.Length();
	const recob::Track::Point_t start = t.Start();
	const recob::Track::Point_t end = t.End();
	const recob::Track::Vector_t sdir = t.StartDirection();
	const recob::Track::Vector_t edir = t.EndDirection();
	
	//calculate cosine between start and end of track directions
	double smag = sqrt(pow(sdir.X(),2)+pow(sdir.Y(),2)+pow(sdir.Z(),2));
	double emag = sqrt(pow(edir.X(),2)+pow(edir.Y(),2)+pow(edir.Z(),2));
	double dotp = sdir.X()*edir.X() + sdir.Y()*edir.Y() + sdir.Z()*edir.Z();
	double cos = dotp / smag / emag;

	//save ouput
	cosoutput << cos << endl;
	soutput <<
	  start.X() << ", " <<
	  start.Y() << ", " <<
	  start.Z() << endl;
	eoutput <<
	  end.X() << ", " <<
	  end.Y() << ", " <<
	  end.Z() << endl;	
      }//end track loop
    } catch (const exception& e) {
      cout << "couldn't load track object" << endl;
    }//end try-catch
  }//end event loop

  //control variables for looping                          
  totalEventCount = 0;
  prevRun = -9999;
  runEventCount = 0;
  cout << "starting SCE events" << endl;
  ////////////////////////////////////////////////////////////////////
  /// SCE EVENTS
  ////////////////////////////////////////////////////////////////////
  for (gallery::Event ev(filenames_sce) ; !ev.atEnd(); ev.next()){
    auto t_begin = high_resolution_clock::now();
    int thisRunNum = ev.eventAuxiliary().run();
    int thisSubrunNum = ev.eventAuxiliary().subRun();
    int thisEventNum = ev.eventAuxiliary().event();
    
    if (thisRunNum != prevRun){//new run
      prevRun = thisRunNum;
      runEventCount = 0;}
    //in MC it seems like every evt has the same run number
    if (runEventCount++ > maxEventsPerRun) {break;} //enough of this run
    //toal event count is probably the only thing that matters much
    if (totalEventCount++ > maxEvents){
      cout << "I think you've seen enough...sce events" << endl;
      break;
    }
    cout << "Processing " << "Run " << thisRunNum << ", " << "Subrun " << thisSubrunNum << ", " << "Event " << thisEventNum << endl;
    
    try{
      auto const& track = *ev.getValidHandle< vector <recob::Track > > (fTrackModuleLabel);
      
      for (auto const& t : track){ //loop through objects in evt
	const double len = t.Length();
	const recob::Track::Point_t start = t.Start();
	const recob::Track::Point_t end = t.End();
	const recob::Track::Vector_t sdir = t.StartDirection();
	const recob::Track::Vector_t edir = t.EndDirection();

	//calculate cosine between start and end of track directions
	double smag = sqrt(pow(sdir.X(),2)+pow(sdir.Y(),2)+pow(sdir.Z(),2));
	double emag = sqrt(pow(edir.X(),2)+pow(edir.Y(),2)+pow(edir.Z(),2));
	double dotp = sdir.X()*edir.X() + sdir.Y()*edir.Y() + sdir.Z()*edir.Z();
	double cos = dotp / smag / emag;
	cosoutput_sce << cos << endl;

	soutput_sce <<
	  start.X() << ", " <<
	  start.Y() << ", " <<
	  start.Z() << endl;
	eoutput_sce <<
	  end.X() << ", " <<
	  end.Y() << ", " <<
	  end.Z() << endl;	
      }//end track loop
    } catch (const exception& e) {
      cout << "couldn't load track object" << endl;
    }//end try-catch
  }//end SCE event loop

  //close output files ::broom-sweeping emoji::
  f_output.Write();
  f_output.Close();
  eoutput.close();
  soutput.close();
  eoutput_sce.close();
  soutput_sce.close();
  cosoutput.close();
  cosoutput_sce.close();
}//end main
