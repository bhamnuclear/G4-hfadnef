void onehisto(TString,int);
TCanvas *c1 = new TCanvas("c1","7Li(p,n)",0,0,1600,800);
TCanvas *c2 = new TCanvas("c2","7Li(p,n)",800,0,1600,800);

TH1F *h1[10]={0};
TH2F *h2[10]={0};
void histothat() {
	onehisto("attenuate",0);
//	onehisto("Ep2000",1);
//	onehisto("Ep1912",2);
//	onehisto("Ep2000");
}
void onehisto(TString directory,int indx) {
	for(int i=0;i<3;i++) {
		indx=i;
		h1[indx] = new TH1F("h1"+directory,";Energy (MeV);Counts/x keV",100,0,0);
		h1[indx]->SetDirectory(0);
		h2[indx] = new TH2F("h2"+directory,";Angle (deg);Energy (MeV)",90,0,180,100,0,0);
		h2[indx]->SetDirectory(0);
		ifstream in;
		in.open(directory+Form("/pneutrons_%d.txt",i));
		while(in.good()) {
			int proc;
			double En,th;
			in>>En>>th;
			h1[indx]->Fill(En);
			h2[indx]->Fill(th*180./(4*atan(1.)),En);
		}
		double division=h1[indx]->GetXaxis()->GetBinWidth(1)*1000;
		h1[indx]->SetTitle(Form(";Energy (MeV);Counts/%g keV/1 billion protons",division));
		c1->cd();
		h1[indx]->SetMaximum(200);
		if(indx==1) h1[indx]->SetLineColor(kRed);
		if(indx==2) h1[indx]->SetLineColor(kBlue);
		if(indx==3) h1[indx]->SetLineColor(kMagenta);
//		if(indx>0) TCanvas *cnew = new TCanvas();
		if(indx>0) {h1[indx]->Draw("SAME");cout<<indx<<endl;}
		if(indx==0) h1[indx]->Draw();
//		h1[indx]->SetMaximum(800);
		h1[indx]->GetXaxis()->SetRangeUser(0,1);
		c1->Update();
		c2->cd();
		h2[indx]->Draw("COLZ");
		c2->Update();
		cout<<"Integral: "<<h1[indx]->Integral(0,1000)/(1e7*1.6e-16)<<"/mC"<<endl;
	}

}
