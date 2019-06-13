/////////////////////////////////////////////////////////////////
//Authors: Lina Parra Y Fernando Vergara.                      //
//Date: June 2019                                              // 
//Código para leer un archivo de ROOT, leer sus subdirectorios,//
//normalizar histogramas y escribirlos a un nuevo archivo .root//
//de salida.                                                   //
//                                                             //
/////////////////////////////////////////////////////////////////
#include "TH1.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TCollection.h"
#include "TKey.h"
#include "TClass.h"
#include "TROOT.h"
#include "TEnv.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
void Normalize(TH1 *h, Double_t norm) {
  if ((!h) || (norm == 0.)) return; // just a precaution
  Double_t integral = h->Integral();
  if (integral != 0.) h->Scale(norm / integral);
  else std::cout << h->GetName() << " is non-normalizable." << std::endl;
}

void histo_norm() {
  
  //Open the config file.
  ifstream inFile;
  inFile.open ("config.in", ios::in);
  if (!inFile){
    std::cerr << "Error: Can't open input file:" << std::endl;
    exit(1);
  }
  
  //Open the file.
  
  TEnv *config_file = new TEnv("config_file");
  config_file->ReadFile("config.in", kEnvChange);
  string input_filename = config_file->GetValue("input_filename", "file");
  std::cout <<"Opening:  "  << input_filename << std::endl;
  
  //Create the output file
  TFile *output = TFile::Open(TString::Format("output_%s", (input_filename).c_str()), "RECREATE");
  std::cout << "Opening output: " << output->GetName() << std::endl;
  
  //Open the input root file
  TFile *input_file = TFile::Open((input_filename).c_str());
  TDirectory *input_dir = gDirectory;
  
  TIter next_dir(input_dir->GetListOfKeys());
  TKey *dir_key;
  
  while((dir_key = (TKey*) next_dir())){
    if(dir_key->IsFolder()){ // Warning: TTree and TNtuple are also "folders"
      input_file->cd();
      output->cd();
      TDirectory *output_dir = output->mkdir(dir_key->GetName());                 //Create the directory in the output file.
      output_dir->cd();                                                           //Go to it.
      input_dir->cd(dir_key->GetName());
      TIter next_folder(gDirectory->GetListOfKeys());
      TKey *key_folder;
      
      while((key_folder = (TKey*) next_folder())){
	TClass *cl = gROOT->GetClass(key_folder->GetClassName());
	if(!cl->InheritsFrom("TH1")) continue;
	TH1 *h = (TH1*)key_folder->ReadObj();
	if (h) {
	  h->SetDirectory(output_dir);
	  std::cout << "Normalilzing: " << h->GetName() << "Integral:" << h->Integral() << std::endl;
	  Normalize(h, 1.0);
	  std::cout << "Normalized Integral:  " << h->Integral() << std::endl;         //We check the integral.
	}
      }
      output_dir->Write();                                                    //Write them in memory to the actual directory.
    }
  }
  
  delete input_file;
  std::cout << "Closing output: " << output->GetName() << std::endl;
  delete output;
}

int main(){
  histo_norm();
  return 0;
}
