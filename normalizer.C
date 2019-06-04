#include "TH1F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"

//Normalizador de histogramas, se le pasa un histograma(1D) como referencia y 
//un double como factor de normalización. Duda: ¿esto toma los de 2D correctamente?
void Normalize (TH1 *h1, double scale){
  
  Double_t ScaleFactor = 1.0;                       //double de normalización. Se reasigna después.
  if(h1->Integral() != 0) {                         //Se revisa que el histograma tenga bines/datos.
    ScaleFactor = scale/(h1->Integral());           //Se halla el factor de normalización. 
  }
  else{
    std::cout << h1->GetName() << "Non-normalizable" << std::endl; //Si el histograma esta vacío, imprime.
    return;                                                        //Y termina.
  }
  h1->Scale(ScaleFactor);                                          //Normaliza el histograma.
}

void normalizer(){

  
  TFile *file = TFile::Open("Test.root");                          //Abre el archivo ".root". Asigna puntero file.
  if (!file){
    std::cout << "File not found!" << std::endl;                   //Si no lo abre, termina el programa. Y muestra un msj.
    exit(1);
  }

  TCanvas c1;                                                      //Se crea un TCanvas para guardar los hist.
  c1.Print("Test.ps[");                                            //Se crea el archivo donde se quiere guardar.
  TDirectory *dirsav = gDirectory;                                 //Se crea un objeto TDirectory y se guarda en el
                                                                   //la estructura de carpetas del archivo.

  TIter next(dirsav->GetListOfKeys());                             //Se crea un iterador que irá por todas las carpetas. 
                                                                   //y obtendrá sus llaves en memoria.
  TKey *key;                                                       //Se crea un objeto TKey.
  
  while((key = (TKey*) next() )){                                  //Se hace un loop que itera en todas las llaves que 
                                                                   //obtiene next.
    if(key->IsFolder()){                                           //Revisa si la llave que se obtuvo es una carpeta.
      dirsav->cd(key->GetName());                                  //Va a la carpeta, obteniendo su nombre.
      TIter next_folder(gDirectory->GetListOfKeys());              //Se crea otro iterador que obtiene las llaves de la
                                                                   //carpeta.
      TKey *key_folder;                                            //Se crea un objeto TKey.
      while((key_folder = (TKey*) next_folder())){                 //Se hace un loop que itera en todas las llaves que
	                                                           //obtiene next_folder
	TClass *cl = gROOT->GetClass(key_folder->GetClassName());  //Se crea un objeto TClass y se le asigna la clase 
	                                                           //de la llave del objeto leído por medio del nombre.
	if(!cl->InheritsFrom("TH1")) continue;                     //Se verifica la herencia de la clase TH1.
	TH1 *h = (TH1*)key_folder->ReadObj();                      //Crea un objeto TH1 y se le asigna el histograma al
	                                                           //que corresponde la llave.	
	Normalize(h , 1.0);                                        //Normaliza el histograma a 1.
	h->Draw();                                                 //Lo dibuja al canvas. 
	c1.Print("Test.ps");                                       //Imprime el canvas al archivo.
      }
    }
  }
  c1.Print("Test.ps]");                                            //Cierra el archivo.
}

int main(){                                                        //La función principal
  normalizer();                                                        //Se incluye el macro.
  return 0;
}
