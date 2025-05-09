void overlay() {
	TFile *f1 = TFile::Open("Ep2600.root");
	TH1F *h1 = (TH1F*)f1->Get("h1");
	TFile *f2 = TFile::Open("Ep2500.root");
	TH1F *h2 = (TH1F*)f2->Get("h1");
/*
	TFile *f3 = TFile::Open("nothing.root");
	TH1F *h3 = (TH1F*)f3->Get("h1");
	TFile *f4 = TFile::Open("nolead.root");
	TH1F *h4 = (TH1F*)f4->Get("h1");
*/
	h1->Rebin(2);
	h2->Rebin(2);
//	h3->Rebin(2);
//	h4->Rebin(2);
	h1->SetLineColor(kBlue);
	h1->Draw("HIST");
	h2->SetLineColor(kRed);
	h2->Draw("SAMEHIST");
/*
	h3->SetLineColor(kBlack);
	h3->Draw("SAMEHIST");
	h4->SetLineColor(kMagenta);
	h4->Draw("SAMEHIST");
*/
	TLegend * leg = new TLegend(0.7,0.9,0.7,0.9);
	leg->AddEntry(h2,"2.5 MeV");
	leg->AddEntry(h1,"2.6 MeV");
//	leg->AddEntry(h3,"No Ti");
//	leg->AddEntry(h4,"No Pb");
	TH1F *hd = (TH1F*)h1->Clone();
	hd->Add(h2,-1);
	hd->SetLineWidth(2);
	hd->SetLineColor(kBlack);
//	TCanvas *c = new TCanvas();
	hd->Draw("HISTSAME");
	leg->AddEntry(hd,"Difference");
	leg->Draw("SAME");

}
