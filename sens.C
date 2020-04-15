
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int sens(char* filename)
{
  TFile *f = TFile::Open(filename);
  //TFile *f = new TFile("../B1-build/Electrons.root");

  TTree *tree = (TTree*) f->Get("ScorerKin");

  cout << "reading from "<< f->GetName()<<"..."<<endl;
  Double_t ekine;
  Double_t Z;
  Double_t trackID;
  Double_t parentID;
  Double_t eventID;
  Double_t productionVolume; TString volumeAtVertex_;
  Double_t creatorProcess; TString processName_;
  Double_t particleID;


  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("productionVolume",&productionVolume);
  tree->SetBranchAddress("creatorProcess",&creatorProcess);
  tree->SetBranchAddress("particleID",&particleID);

  Double_t dist = 0.1;
  Double_t thick = 0.01;

  // ELECTRON HISTOGRAMS
  TH1F *h_ekine1 = new TH1F("h_ekine1","Energy Spectrum ",1000,0,4);
    h_ekine1->SetAxisRange(0,50,"Y");
    h_ekine1->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine1->GetYaxis()->SetTitle("count");
  TH1F *h_ekine2 = new TH1F("h_ekine2","Energy Spectrum ",1000,0,4);
    h_ekine2->SetAxisRange(0,50,"Y");
    h_ekine2->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine2->GetYaxis()->SetTitle("count");


  Int_t neutrons = 10000;
  Int_t count1_tot = 0;
  Int_t count2_tot = 0;
  Int_t count_totICE = 0;
  TString serialID;
  TString previousSerialID = "0";
  Double_t previousEventID = 0;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
      tree->GetEntry(i);
      if (!(particleID == 1)) {continue;}// 1: electron

        serialID = MakeSerialID(trackID,parentID,eventID);
        if (serialID == previousSerialID) {continue;} // track a particle ONCE

        switch ((Int_t)creatorProcess)
        {
          case 1: processName_= "compt"; break;
          case 2: processName_= "nCapture"; break;
          case 3: processName_= "phot"; break;
          case 4: processName_= "conv"; break;
        }
        switch((Int_t)productionVolume)
        {
          case 1: volumeAtVertex_= "gd"          ; break;
          case 2: volumeAtVertex_= "AlpideSens1" ; break;
          case 3: volumeAtVertex_= "al_1"        ; break;
          case 4: volumeAtVertex_= "AlpideSens2" ; break;
          case 5: volumeAtVertex_= "al_2"        ; break;
        }
        if (Z>0) // ALPIDE1 position
          {
            h_ekine1->Fill(ekine);
            count1_tot++;
          }
        if (Z<0) // ALPIDE2 position
          {
            h_ekine2->Fill(ekine);
            count2_tot++;
          }

        if (processName_ == "nCapture" )
        {
          count_totICE++;
        }
        previousSerialID = serialID;
        previousEventID = eventID;

    }


  TH1F *h1 = (TH1F*) h_ekine1->Clone();
    h1->SetNameTitle("h1","1) electron spectrum");
    h1->SetAxisRange(0,0.5,"X");
    h1->SetAxisRange(0,50,"Y");
    h1->GetXaxis()->SetTitle("Ekine [MeV]");
    h1->GetYaxis()->SetTitle("count");
  TH1F *h2 = (TH1F*) h_ekine2->Clone();
    h2->SetNameTitle("h2","2) electron spectrum");
    h2->SetAxisRange(0,0.5,"X");
    h2->SetAxisRange(0,50,"Y");
    h2->GetXaxis()->SetTitle("Ekine [MeV]");
    h2->GetYaxis()->SetTitle("count");

  TCanvas *c2= new TCanvas("c2"," ");
    c2->Divide(2,1);
    c2->cd(2); h1->Draw();
    c2->cd(1); h2->Draw();




  cout << "ALPIDE" << endl;
  cout << " steps: " << n << endl;
  cout << " 1) count: "<< count1_tot << endl;
  cout << " 2) count: "<< count2_tot << endl;
  cout << " Sum count: "<< count2_tot+count1_tot << endl;
  cout << " ICE count in ALPIDEs: "<< count_totICE << endl;
  cout << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << endl;

  char pdfname[500];
    strcpy(pdfname,filename);
    strcat(pdfname,".pdf");
    cout <<"filename:"<< filename<<endl;
    cout <<"saving pdf as:"<< pdfname << "..."<<endl;
    c2->SaveAs(pdfname);

  char txtname[500];
    strcpy(txtname,filename);
    strcat(txtname,".txt");

  ofstream myfile;
    myfile.open (txtname); // this is a problem
    myfile << "ALPIDE" <<  "\n";
    myfile << " steps: " << n << "\n";
    myfile << " 1) count: "<< count1_tot << "\n";
    myfile << " 2) count: "<< count2_tot << "\n";
    myfile << " Sum count: "<< count2_tot+count1_tot << "\n";
    myfile << " ICE count in ALPIDEs: "<< count_totICE << "\n";
    myfile << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << "\n";
    cout <<"saving terminal output as:"<< txtname << " ..."<<endl;
  myfile.close();

  return 0;

}

TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID)
{

  TString trackID_ = to_string((Int_t)trackID);
  TString parentID_ = to_string((Int_t)parentID);
  TString eventID_ = to_string((Int_t)eventID);
  TString serialID_ = eventID_+parentID_+trackID_;
  //cout << serialID_ <<endl;
  return serialID_;
}