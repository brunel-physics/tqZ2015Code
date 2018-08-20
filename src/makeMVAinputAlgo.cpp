#include "makeMVAinputAlgo.hpp"

#include "MvaEvent.hpp"
#include "TLorentzVector.h"
#include "TMVA/Timer.h"
#include "TTree.h"
#include "config_parser.hpp"

#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/program_options.hpp>

MakeMvaInputs::MakeMvaInputs()
    : jetUnc(JetCorrectionUncertainty(
          "scaleFactors/2016/Summer16_23Sep2016V4_MC_Uncertainty_AK4PFchs.txt"))
    , inputVars{}
    , ttbarControlRegion{false}
    , useSidebandRegion{false}
    , inputDir{"mvaTest/"}
    , outputDir{"mvaInputs/"}
{
}

MakeMvaInputs::~MakeMvaInputs()
{
}

void MakeMvaInputs::parseCommandLineArguements(int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this message.")(
        "ttbar", po::bool_switch(&ttbarControlRegion), "Make ttbar CR stuff")(
        "inputDir,i",
        po::value<std::string>(&inputDir),
        "Mva skims input directory")("outputDir,o",
                                     po::value<std::string>(&outputDir),
                                     "Mva inputs output directory")(
        "sideband,s",
        po::bool_switch(&useSidebandRegion),
        "Make side band CR plots");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc;
            std::exit(0);
        }

        po::notify(vm);
    }

    catch (const std::logic_error& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << "Use -h or --help for help." << std::endl;
        std::exit(1);
    }
}

void MakeMvaInputs::runMainAnalysis()
{
    //  std::map< std::string, std::string > listOfMCs = {{"WWW","WWW"},
    //  {"WWZ","WWZ"}, {"WZZ","WZZ"},
    //  {"ZZZ","ZZZ"},{"sChannel","TsChan"},{"tChannel","TtChan"},{"tbarChannel","TbartChan"},{"tWInclusive","TtW"},{"tbarWInclusive","TbartW"},{"tZq","tZq"},{"tHq","THQ"},{"ttbarInclusivePowerheg","TT"},{"tWZ","TWZ"},{"wPlusJets","Wjets"},{"DYJetsToLL_M-50","DYToLL_M50"},{"DYJetsToLL_M-10To50","DYToLL_M10To50"}};
    //  std::map< std::string, std::string > listOfMCs = {{"ttHTobb","ttH",
    //  "ttHToNonbb","ttH", "WWW","WWW", "WWZ","WWZ", "WZZ","WZZ", "ZZZ","ZZZ",
    //  "WW1l1nu2q","WW",
    //  "WW2l2nu","WW"},{"ZZ4l","ZZ"},{"ZZ2l2nu","ZZ"},{"ZZ2l2q","ZZ"},{"WZjets","WZ"},{"WZ2l2q","WZ"},{"WZ1l1nu2q","WZ"},{"sChannel","TsChan"},{"tChannel","TtChan"},{"tbarChannel","TbartChan"},{"tWInclusive","TtW"},{"tbarWInclusive","TbartW"},{"tZq","tZq"},{"tHq","THQ"},{"ttWlnu","TTW"},{"ttW2q","TTW"},{"ttZ2l2nu","TTZ"},{"ttZ2q","TTZ"},{"ttbarInclusivePowerheg","TT"},{"tWZ","TWZ"},{"wPlusJets","Wjets"},{"DYJetsToLL_M-50","DYToLL_M50"},{"DYJetsToLL_M-10To50","DYToLL_M10To50"}};
    //  std::map< std::string, std::string > listOfMCs =
    //  {{"DYJetsToLL_M-50_amcatnlo","DYToLL_M50_aMCatNLO"},{"DYJetsToLL_M-10To50_amcatnlo","DYToLL_M10To50_aMCatNLO"}};
    //  std::map< std::string, std::string > listOfMCs =
    //  {{"ttbarInclusivePowerheg_hdampUP","TT__hdampUp"},{"ttbarInclusivePowerheg_hdampDown","TT__hdampDown"},{"ttbarInclusivePowerheg_fsrup","TT__fsrUp"},{"ttbarInclusivePowerheg_fsrdown","TT__fsrDown"},{"ttbarInclusivePowerheg_isrup","TT__isrUp"},{"ttbarInclusivePowerheg_isrdown","TT__isrDown"}};
    //  std::map< std::string, std::string > listOfMCs =
    //  {{"tChannel_scaleup","TtChan__scaleUp"},{"tChannel_scaledown","TtChan__scaleDown"},{"tChannel_hdampup","TtChan__hdampUp"},{"tChannel_hdampdown","TtChan__hdampDown"},{"tbarChannel_scaleup","TbartChan__scaleUp"},{"tbarChannel_scaledown","TbartChan__scaleDown"},{"tbarChannel_hdampup","TbartChan__hdampUp"},{"tbarChannel_hdampdown","TbartChan__hdampDown"}};
    //  std::map< std::string, std::string > listOfMCs =
    //  {{"tWInclusive_scaleup","TtW__scaleUp"},{"tWInclusive_scaledown","TtW__scaleDown"},{"tbarWInclusive_scaleup","TbartW__scaleUp"},{"tbarWInclusive_scaledown","TbartW__scaleDown"}};
    //  std::map< std::string, std::string > listOfMCs =
    //  {{"tZq_scaleup","tZq__scaleUp"},{"tZq_scaledown","tZq__scaleDown"}};
    std::map<std::string, std::string> listOfMCs = {{"ttHTobb", "ttH"}};
    //  std::map< std::string, std::string > listOfMCs = {};

    std::map<std::string, std::string> channelToDataset{
        {"ee", "DataEG"}, {"mumu", "DataMu"}, {"emu", "MuonEG"}};

    std::vector<std::string> channels;
    if (!ttbarControlRegion)
    {
        channels = {"ee", "mumu"};
    }
    else
    {
        channels = {"emu"};
    }

    std::vector<std::string> systs = {"",
                                      "__trig__plus",
                                      "__trig__minus",
                                      "__jer__plus",
                                      "__jer__minus",
                                      "__jes__plus",
                                      "__jes__minus",
                                      "__pileup__plus",
                                      "__pileup__minus",
                                      "__bTag__plus",
                                      "__bTag__minus",
                                      "__met__plus",
                                      "__met__minus",
                                      "__pdf__plus",
                                      "__pdf__minus",
                                      "__ME_PS__plus",
                                      "__ME_PS__minus",
                                      "__alphaS__plus",
                                      "__alphaS__minus"};

    std::map<std::string, float> mvaMap = setupInputVars();

    std::string treeNamePostfixSig{""}, treeNamePostfixSB{""};
    if (useSidebandRegion)
    {
        std::cout << "Using control region stuff" << std::endl;
        treeNamePostfixSig = "sig_";
        treeNamePostfixSB = "ctrl_";
    }

    // loop over nominal samples
    for (auto sampleIt = listOfMCs.begin(); sampleIt != listOfMCs.end();
         ++sampleIt)
    {
        std::string sample = sampleIt->first;
        std::string outSample = sampleIt->second;

        std::cout << "Doing " << sample << " : " << std::endl;

        TFile* outFile = new TFile(
            (outputDir + "histofile_" + listOfMCs[sample] + ".root").c_str(),
            "RECREATE");

        // loop over systematics
        for (auto syst = systs.begin(); syst != systs.end(); ++syst)
        {
            std::string systName{*syst};
            TTree* outTreeSig = new TTree(
                ("Ttree_" + treeNamePostfixSig + outSample + systName).c_str(),
                ("Ttree_" + treeNamePostfixSig + outSample + systName).c_str());
            TTree* outTreeSdBnd{};
            setupBranches(outTreeSig, mvaMap);

            if (useSidebandRegion)
            {
                outTreeSdBnd = new TTree(
                    ("Ttree_" + treeNamePostfixSB + outSample + systName)
                        .c_str(),
                    ("Ttree_" + treeNamePostfixSB + outSample + systName)
                        .c_str());
                setupBranches(outTreeSdBnd, mvaMap);
            }

            // loop over channels
            for (auto channel = channels.begin(); channel != channels.end();
                 channel++)
            {
                TFile* inFile = new TFile(
                    (inputDir + sample + (*channel) + "mvaOut.root").c_str(),
                    "READ");
                TTree* tree;
                if (systName == "__met__plus" || systName == "__met__minus")
                {
                    tree = (TTree*)inFile->Get("tree");
                }
                else
                {
                    tree = (TTree*)inFile->Get(("tree" + systName).c_str());
                }

                //        TChain* tree;
                //        if ( systName == "__met__plus" || systName ==
                //        "__met__minus" ) tree = new TChain("tree"); else tree
                //        = new TChain(("tree"+systName).c_str());
                //        tree->Add((inputDir+sample+(*channel)+"mvaOut.root").c_str());

                std::cout << systName << " : " << tree->GetEntries()
                          << std::endl;
                MvaEvent* event = new MvaEvent(true, "", tree, true);

                long long numberOfEvents{tree->GetEntries()};
                TMVA::Timer* lEventTimer{
                    new TMVA::Timer{boost::numeric_cast<int>(numberOfEvents),
                                    "Running over dataset ...",
                                    false}};
                lEventTimer->DrawProgressBar(0);

                // loop over events
                for (int i{0}; i < numberOfEvents; i++)
                {
                    lEventTimer->DrawProgressBar(i);
                    event->GetEntry(i);

                    bool SameSignMC = false;
                    if (SameSignMC == true && *channel == "ee"
                        && (event->genElePF2PATPromptFinalState[0] == 0
                            || event->genElePF2PATPromptFinalState[1] == 0))
                    {
                        continue;
                    }
                    if (SameSignMC == true && *channel == "mumu"
                        && (event->genMuonPF2PATPromptFinalState[0] == 0
                            || event->genMuonPF2PATPromptFinalState[1] == 0))
                    {
                        continue;
                    }

                    fillTree(outTreeSig,
                             outTreeSdBnd,
                             event,
                             outSample + systName,
                             (*channel));
                } // end event loop
                inFile->Close();
            } // end channel loop
            outFile->cd();
            outTreeSig->Write();
            delete outTreeSig;
            delete outTreeSdBnd;
            if (useSidebandRegion)
            {
                outTreeSdBnd->Write();
            }
        } // end systematic loop
        outFile->Write();
        outFile->Close();
    } // end sample loop
}

double MakeMvaInputs::deltaR(float eta1, float phi1, float eta2, float phi2)
{
    double dEta{eta1 - eta2};
    double dPhi{phi1 - phi2};
    while (std::abs(dPhi) > M_PI)
    {
        dPhi += (dPhi > 0. ? -2 * M_PI : 2 * M_PI);
    }
    return std::sqrt((dEta * dEta) + (dPhi * dPhi));
}

std::pair<TLorentzVector, TLorentzVector>
    MakeMvaInputs::sortOutLeptons(MvaEvent* tree, std::string channel)
{
    TLorentzVector zLep1;
    TLorentzVector zLep2;

    int zlep1Index = tree->zLep1Index;
    int zlep2Index = tree->zLep2Index;

    if (channel == "ee")
    {
        zLep1.SetPxPyPzE(tree->elePF2PATPX[zlep1Index],
                         tree->elePF2PATPY[zlep1Index],
                         tree->elePF2PATPZ[zlep1Index],
                         tree->elePF2PATE[zlep1Index]);
        zLep2.SetPxPyPzE(tree->elePF2PATPX[zlep2Index],
                         tree->elePF2PATPY[zlep2Index],
                         tree->elePF2PATPZ[zlep2Index],
                         tree->elePF2PATE[zlep2Index]);
    }
    if (channel == "mumu")
    {
        zLep1.SetPxPyPzE(tree->muonPF2PATPX[zlep1Index],
                         tree->muonPF2PATPY[zlep1Index],
                         tree->muonPF2PATPZ[zlep1Index],
                         tree->muonPF2PATE[zlep1Index]);
        zLep2.SetPxPyPzE(tree->muonPF2PATPX[zlep2Index],
                         tree->muonPF2PATPY[zlep2Index],
                         tree->muonPF2PATPZ[zlep2Index],
                         tree->muonPF2PATE[zlep2Index]);
    }
    if (channel == "emu")
    {
        zLep1.SetPxPyPzE(tree->elePF2PATPX[zlep1Index],
                         tree->elePF2PATPY[zlep1Index],
                         tree->elePF2PATPZ[zlep1Index],
                         tree->elePF2PATE[zlep1Index]);
        zLep2.SetPxPyPzE(tree->muonPF2PATPX[zlep2Index],
                         tree->muonPF2PATPY[zlep2Index],
                         tree->muonPF2PATPZ[zlep2Index],
                         tree->muonPF2PATE[zlep2Index]);
    }

    return {zLep1, zLep2};
}

std::pair<TLorentzVector, TLorentzVector>
    MakeMvaInputs::sortOutHadronicW(MvaEvent* tree)
{
    TLorentzVector wQuark1, wQuark2;
    wQuark1.SetPxPyPzE(tree->jetPF2PATPx[tree->wQuark1Index],
                       tree->jetPF2PATPy[tree->wQuark1Index],
                       tree->jetPF2PATPz[tree->wQuark1Index],
                       tree->jetPF2PATE[tree->wQuark1Index]);
    wQuark2.SetPxPyPzE(tree->jetPF2PATPx[tree->wQuark2Index],
                       tree->jetPF2PATPy[tree->wQuark2Index],
                       tree->jetPF2PATPz[tree->wQuark2Index],
                       tree->jetPF2PATE[tree->wQuark2Index]);

    return {wQuark1, wQuark2};
}

std::pair<std::vector<int>, std::vector<TLorentzVector>>
    MakeMvaInputs::getJets(MvaEvent* tree, int syst, TLorentzVector met)
{
    std::vector<int> jetList{};
    std::vector<TLorentzVector> jetVecList{};

    for (int i = 0; i != 15; i++)
    {
        if (tree->jetInd[i] > -1)
        {
            jetList.emplace_back(tree->jetInd[i]);
            jetVecList.emplace_back(getJetVec(tree,
                                              tree->jetInd[i],
                                              tree->jetSmearValue[i],
                                              met,
                                              syst,
                                              true));
        }
        else
            continue;
    }

    return {jetList, jetVecList};
}

std::pair<std::vector<int>, std::vector<TLorentzVector>>
    MakeMvaInputs::getBjets(MvaEvent* tree,
                            int syst,
                            TLorentzVector met,
                            std::vector<int> jets)
{
    std::vector<int> bJetList{};
    std::vector<TLorentzVector> bJetVecList{};

    for (int i = 0; i != 10; i++)
    {
        if (tree->bJetInd[i] > -1)
        {
            bJetList.emplace_back(tree->bJetInd[i]);
            bJetVecList.emplace_back(
                getJetVec(tree,
                          jets[tree->bJetInd[i]],
                          tree->jetSmearValue[tree->bJetInd[i]],
                          met,
                          syst,
                          false));
        }
        else
            continue;
    }

    return {bJetList, bJetVecList};
}

TLorentzVector MakeMvaInputs::getJetVec(MvaEvent* tree,
                                        int index,
                                        float smearValue,
                                        TLorentzVector metVec,
                                        int syst,
                                        bool doMetSmear)
{
    TLorentzVector returnJet;
    returnJet.SetPxPyPzE(tree->jetPF2PATPx[index],
                         tree->jetPF2PATPy[index],
                         tree->jetPF2PATPz[index],
                         tree->jetPF2PATE[index]);
    returnJet *= smearValue;

    if (syst == 16)
    {
        returnJet *=
            1 + jetUnc.getUncertainty(returnJet.Pt(), returnJet.Eta(), 1);
    }
    else if (syst == 32)
    {
        returnJet *=
            1 + jetUnc.getUncertainty(returnJet.Pt(), returnJet.Eta(), 2);
    }

    if (doMetSmear && smearValue > 0.01)
    {
        metVec.SetPx(metVec.Px() + tree->jetPF2PATPx[index]);
        metVec.SetPy(metVec.Py() + tree->jetPF2PATPy[index]);

        metVec.SetPx(metVec.Px() - returnJet.Px());
        metVec.SetPy(metVec.Py() - returnJet.Py());
    }

    return returnJet;
}

TLorentzVector MakeMvaInputs::doUncMet(TLorentzVector met,
                                       TLorentzVector zLep1,
                                       TLorentzVector zLep2,
                                       std::vector<TLorentzVector> jetVecs,
                                       uint syst)
{
    double uncMetX = met.Px() + zLep1.Px() + zLep2.Px();
    double uncMetY = met.Py() + zLep1.Py() + zLep2.Py();

    for (uint i = 0; i != jetVecs.size(); i++)
    {
        uncMetX += jetVecs[i].Px();
        uncMetY += jetVecs[i].Py();
    }

    if (syst == 1024)
    {
        met.SetPx(met.Px() + 0.1 * uncMetX);
        met.SetPy(met.Py() + 0.1 * uncMetY);
    }

    else if (syst == 2048)
    {
        met.SetPx(met.Px() - 0.1 * uncMetX);
        met.SetPy(met.Py() - 0.1 * uncMetY);
    }

    return met;
}

std::map<std::string, float> MakeMvaInputs::setupInputVars()
{
    inputVars["eventWeight"] = {0.0};
    inputVars["eventNumber"] = {0.0};
    inputVars["mTW"] = {0.0};
    inputVars["wQuark1Pt"] = {0.0};
    inputVars["wQuark1Eta"] = {0.0};
    inputVars["wQuark1Phi"] = {0.0};
    inputVars["wQuark2Pt"] = {0.0};
    inputVars["wQuark2Eta"] = {0.0};
    inputVars["wQuark2Phi"] = {0.0};
    inputVars["wPairMass"] = {0.0};
    inputVars["wPairPt"] = {0.0};
    inputVars["wPairEta"] = {0.0};
    inputVars["wPairPhi"] = {0.0};
    inputVars["met"] = {0.0};
    inputVars["nJets"] = {0.0};
    inputVars["leadJetPt"] = {0.0};
    inputVars["leadJetPhi"] = {0.0};
    inputVars["leadJetEta"] = {0.0};
    inputVars["leadJetbTag"] = {0.0};
    inputVars["secJetPt"] = {0.0};
    inputVars["secJetPhi"] = {0.0};
    inputVars["secJetEta"] = {0.0};
    inputVars["secJetbTag"] = {0.0};
    inputVars["thirdJetPt"] = {0.0};
    inputVars["thirdJetPhi"] = {0.0};
    inputVars["thirdJetEta"] = {0.0};
    inputVars["thirdJetbTag"] = {0.0};
    inputVars["fourthJetPt"] = {0.0};
    inputVars["fourthJetPhi"] = {0.0};
    inputVars["fourthJetEta"] = {0.0};
    inputVars["fourthJetbTag"] = {0.0};
    inputVars["nBjets"] = {0.0};
    inputVars["bTagDisc"] = {0.0};
    inputVars["lep1Pt"] = {0.0};
    inputVars["lep1Eta"] = {0.0};
    inputVars["lep1Phi"] = {0.0};
    inputVars["lep1RelIso"] = {0.0};
    inputVars["lep1D0"] = {0.0};
    inputVars["lep2Pt"] = {0.0};
    inputVars["lep2Eta"] = {0.0};
    inputVars["lep2Phi"] = {0.0};
    inputVars["lep2RelIso"] = {0.0};
    inputVars["lep2D0"] = {0.0};
    inputVars["lepMass"] = {0.0};
    inputVars["lepPt"] = {0.0};
    inputVars["lepEta"] = {0.0};
    inputVars["lepPhi"] = {0.0};
    inputVars["zMass"] = {0.0};
    inputVars["zPt"] = {0.0};
    inputVars["zEta"] = {0.0};
    inputVars["zPhi"] = {0.0};
    inputVars["topMass"] = {0.0};
    inputVars["topPt"] = {0.0};
    inputVars["topEta"] = {0.0};
    inputVars["topPhi"] = {0.0};
    inputVars["j1j2delR"] = {0.0};
    inputVars["j1j2delPhi"] = {0.0};
    inputVars["w1w2delR"] = {0.0};
    inputVars["w1w2delPhi"] = {0.0};
    inputVars["zLepdelR"] = {0.0};
    inputVars["zLepdelPhi"] = {0.0};
    inputVars["zl1Quark1DelR"] = {0.0};
    inputVars["zl1Quark1DelPhi"] = {0.0};
    inputVars["zl1Quark2DelR"] = {0.0};
    inputVars["zl1Quark2DelPhi"] = {0.0};
    inputVars["zl2Quark1DelR"] = {0.0};
    inputVars["zl2Quark1DelPhi"] = {0.0};
    inputVars["zl2Quark2DelR"] = {0.0};
    inputVars["zl2Quark2DelPhi"] = {0.0};
    inputVars["zlb1DelR"] = {0.0};
    inputVars["zlb1DelPhi"] = {0.0};
    inputVars["zlb2DelR"] = {0.0};
    inputVars["zlb2DelPhi"] = {0.0};
    inputVars["lepHt"] = {0.0};
    inputVars["wQuarkHt"] = {0.0};
    inputVars["totPt"] = {0.0};
    inputVars["totEta"] = {0.0};
    inputVars["totPhi"] = {0.0};
    inputVars["totPtVec"] = {0.0};
    inputVars["totVecM"] = {0.0};
    inputVars["chan"] = {0.0};
    inputVars["totPt2Jet"] = {0.0};
    inputVars["wZdelR"] = {0.0};
    inputVars["wZdelPhi"] = {0.0};
    inputVars["zQuark1DelR"] = {0.0};
    inputVars["zQuark1DelPhi"] = {0.0};
    inputVars["zQuark2DelR"] = {0.0};
    inputVars["zQuark2DelPhi"] = {0.0};
    inputVars["zTopDelR"] = {0.0};
    inputVars["zTopDelPhi"] = {0.0};
    inputVars["zl1TopDelR"] = {0.0};
    inputVars["zl1TopDelPhi"] = {0.0};
    inputVars["zl2TopDelR"] = {0.0};
    inputVars["zl2TopDelPhi"] = {0.0};
    inputVars["wTopDelR"] = {0.0};
    inputVars["wTopDelPhi"] = {0.0};
    inputVars["w1TopDelR"] = {0.0};
    inputVars["w1TopDelPhi"] = {0.0};
    inputVars["w2TopDelR"] = {0.0};
    inputVars["w2TopDelPhi"] = {0.0};
    inputVars["minZJetR"] = {0.0};
    inputVars["minZJetPhi"] = {0.0};
    inputVars["totHt"] = {0.0};
    inputVars["jetHt"] = {0.0};
    inputVars["jetMass"] = {0.0};
    inputVars["jetPt"] = {0.0};
    inputVars["jetEta"] = {0.0};
    inputVars["jetPhi"] = {0.0};
    inputVars["jetMass3"] = {0.0};
    inputVars["totHtOverPt"] = {0.0};
    inputVars["chi2"] = {0.0};

    return inputVars;
}

void MakeMvaInputs::setupBranches(TTree* tree, std::map<std::string, float>)
{
    tree->Branch("EvtWeight", &inputVars["eventWeight"], "EvtWeight/F");
    tree->Branch("EvtNumber", &inputVars["eventNumber"], "EvtNumber/F");
    tree->Branch("mTW", &inputVars["mTW"], "mTW/F");
    tree->Branch("wQuark1Pt", &inputVars["wQuark1Pt"], "wQuark1Pt/F");
    tree->Branch("wQuark1Eta", &inputVars["wQuark1Eta"], "wQuark1Eta/F");
    tree->Branch("wQuark1Phi", &inputVars["wQuark1Phi"], "wQuark1Phi/F");
    tree->Branch("wQuark2Pt", &inputVars["wQuark2Pt"], "wQuark2Pt/F");
    tree->Branch("wQuark2Eta", &inputVars["wQuark2Eta"], "wQuark2Eta/F");
    tree->Branch("wQuark2Phi", &inputVars["wQuark2Phi"], "wQuark2Phi/F");
    tree->Branch("wPairMass", &inputVars["wPairMass"], "wPairMass/F");
    tree->Branch("wPairPt", &inputVars["wPairPt"], "wPairPt/F");
    tree->Branch("wPairEta", &inputVars["wPairEta"], "wPairEta/F");
    tree->Branch("wPairPhi", &inputVars["wPairPhi"], "wPairPhi/F");
    tree->Branch("met", &inputVars["met"], "met/F");
    tree->Branch("nJets", &inputVars["nJets"], "nJets/F");
    tree->Branch("leadJetPt", &inputVars["leadJetPt"], "leadJetPt/F");
    tree->Branch("leadJetEta", &inputVars["leadJetEta"], "leadJetEta/F");
    tree->Branch("leadJetPhi", &inputVars["leadJetPhi"], "leadJetPhi/F");
    tree->Branch("leadJetbTag", &inputVars["leadJetbTag"], "leadJetbTag/F");
    tree->Branch("secJetPt", &inputVars["secJetPt"], "secJetPt/F");
    tree->Branch("secJetEta", &inputVars["secJetEta"], "secJetEta/F");
    tree->Branch("secJetPhi", &inputVars["secJetPhi"], "secJetPhi/F");
    tree->Branch("secJetbTag", &inputVars["secJetbTag"], "secJetbTag/F");
    tree->Branch("thirdJetPt", &inputVars["thirdJetPt"], "thirdJetPt/F");
    tree->Branch("thirdJetEta", &inputVars["thirdJetEta"], "thirdJetEta/F");
    tree->Branch("thirdJetPhi", &inputVars["thirdJetPhi"], "thirdJetPhi/F");
    tree->Branch("thirdJetbTag", &inputVars["thirdJetbTag"], "thirdJetbTag/F");
    tree->Branch("fourthJetPt", &inputVars["fourthJetPt"], "fourthJetPt/F");
    tree->Branch("fourthJetEta", &inputVars["fourthJetEta"], "fourthJetEta/F");
    tree->Branch("fourthJetPhi", &inputVars["fourthJetPhi"], "fourthJetPhi/F");
    tree->Branch(
        "fourthJetbTag", &inputVars["fourthJetbTag"], "fourthJetbTag/F");
    tree->Branch("nBjets", &inputVars["nBjets"], "nBjets/F");
    tree->Branch("bTagDisc", &inputVars["bTagDisc"], "bTagDisc/F");
    tree->Branch("lep1Pt", &inputVars["lep1Pt"], "lep1Pt/F");
    tree->Branch("lep1Eta", &inputVars["lep1Eta"], "lep1Eta/F");
    tree->Branch("lep1Phi", &inputVars["lep1Phi"], "lep1Phi/F");
    tree->Branch("lep1RelIso", &inputVars["lep1RelIso"], "lep1RelIso/F");
    tree->Branch("lep1D0", &inputVars["lep1D0"], "lep1D0/F");
    tree->Branch("lep2Pt", &inputVars["lep2Pt"], "lep2Pt/F");
    tree->Branch("lep2Eta", &inputVars["lep2Eta"], "lep2Eta/F");
    tree->Branch("lep2Phi", &inputVars["lep2Phi"], "lep2Phi/F");
    tree->Branch("lep2RelIso", &inputVars["lep2RelIso"], "lep2RelIso/F");
    tree->Branch("lep2D0", &inputVars["lep2D0"], "lep2D0/F");
    tree->Branch("lepMass", &inputVars["lepMass"], "lepMass/F");
    tree->Branch("lepPt", &inputVars["lepPt"], "lepPt/F");
    tree->Branch("lepEta", &inputVars["lepEta"], "lepEta/F");
    tree->Branch("lepPhi", &inputVars["lepPhi"], "lepPhi/F");
    tree->Branch("zMass", &inputVars["zMass"], "zMass/F");
    tree->Branch("zPt", &inputVars["zPt"], "zPt/F");
    tree->Branch("zEta", &inputVars["zEta"], "zEta/F");
    tree->Branch("zPhi", &inputVars["zPhi"], "zPhi/F");
    tree->Branch("topMass", &inputVars["topMass"], "topMass/F");
    tree->Branch("topPt", &inputVars["topPt"], "topPt/F");
    tree->Branch("topEta", &inputVars["topEta"], "topEta/F");
    tree->Branch("topPhi", &inputVars["topPhi"], "topPhi/F");
    tree->Branch("jjdelR", &inputVars["j1j2delR"], "jjdelR/F");
    tree->Branch("jjdelPhi", &inputVars["j1j2delPhi"], "jjdelPhi/F");
    tree->Branch("wwdelR", &inputVars["w1w2delR"], "wwdelR/F");
    tree->Branch("wwdelPhi", &inputVars["w1w2delPhi"], "wwdelPhi/F");
    tree->Branch("zLepdelR", &inputVars["zLepdelR"], "zLepdelR/F");
    tree->Branch("zLepdelPhi", &inputVars["zLepdelPhi"], "zLepdelPhi/F");
    tree->Branch(
        "zl1Quark1DelR", &inputVars["zl1Quark1DelR"], "zl1Quark1DelR/F");
    tree->Branch(
        "zl1Quark1DelPhi", &inputVars["zl1Quark1DelPhi"], "zl1Quark1DelPhi/F");
    tree->Branch(
        "zl1Quark2DelR", &inputVars["zl1Quark2DelR"], "zl1Quark2DelR/F");
    tree->Branch(
        "zl1Quark2DelPhi", &inputVars["zl1Quark2DelPhi"], "zl1Quark2DelPhi/F");
    tree->Branch(
        "zl2Quark1DelR", &inputVars["zl2Quark1DelR"], "zl2Quark1DelR/F");
    tree->Branch(
        "zl2Quark1DelPhi", &inputVars["zl2Quark1DelPhi"], "zl2Quark1DelPhi/F");
    tree->Branch(
        "zl2Quark2DelR", &inputVars["zl2Quark2DelR"], "zl2Quark2DelR/F");
    tree->Branch(
        "zl2Quark2DelPhi", &inputVars["zl2Quark2DelPhi"], "zl2Quark2DelPhi/F");
    tree->Branch("zlb1DelR", &inputVars["zlb1DelR"], "zlb1DelR/F");
    tree->Branch("zlb1DelPhi", &inputVars["zlb1DelPhi"], "zlb1DelPhi/F");
    tree->Branch("zlb2DelR", &inputVars["zlb2DelR"], "zlb2DelR/F");
    tree->Branch("zlb2DelPhi", &inputVars["zlb2DelPhi"], "zlb2DelPhi/F");
    tree->Branch("lepHt", &inputVars["lepHt"], "lepHt/F");
    tree->Branch("wQuarkHt", &inputVars["wQuarkHt"], "wQuarkHt/F");
    tree->Branch("totPt", &inputVars["totPt"], "totPt/F");
    tree->Branch("totEta", &inputVars["totEta"], "totEta/F");
    tree->Branch("totPhi", &inputVars["totPhi"], "totPhi/F");
    tree->Branch("totPtVec", &inputVars["totPtVec"], "totPtVec/F");
    tree->Branch("totVecM", &inputVars["totVecM"], "totVecM/F");
    tree->Branch("Channel", &inputVars["chan"], "Channel/I");
    tree->Branch("totPt2Jet", &inputVars["totPt2Jet"], "totPt2Jet/F");
    tree->Branch("wzdelR", &inputVars["wZdelR"], "wzdelR/F");
    tree->Branch("wzdelPhi", &inputVars["wZdelPhi"], "wzdelPhi/F");
    tree->Branch("zQuark1DelR", &inputVars["zQuark1DelR"], "zQuark1DelR/F");
    tree->Branch(
        "zQuark1DelPhi", &inputVars["zQuark1DelPhi"], "zQuark1DelPhi/F");
    tree->Branch("zQuark2DelR", &inputVars["zQuark2DelR"], "zQuark2DelR/F");
    tree->Branch(
        "zQuark2DelPhi", &inputVars["zQuark2DelPhi"], "zQuark2DelPhi/F");
    tree->Branch("zTopDelR", &inputVars["zTopDelR"], "zTopDelR/F");
    tree->Branch("zTopDelPhi", &inputVars["zTopDelPhi"], "zTopDelPhi/F");
    tree->Branch("zl1TopDelR", &inputVars["zl1TopDelR"], "zl1TopDelR/F");
    tree->Branch("zl1TopDelPhi", &inputVars["zl1TopDelPhi"], "zl1TopDelPhi/F");
    tree->Branch("zl2TopDelR", &inputVars["zl2TopDelR"], "zl2TopDelR/F");
    tree->Branch("zl2TopDelPhi", &inputVars["zl2TopDelPhi"], "zl2TopDelPhi/F");
    tree->Branch("wTopDelR", &inputVars["wTopDelR"], "wTopDelR/F");
    tree->Branch("wTopDelPhi", &inputVars["wTopDelPhi"], "wTopDelPhi/F");
    tree->Branch("w1TopDelR", &inputVars["w1TopDelR"], "w1TopDelR/F");
    tree->Branch("w1TopDelPhi", &inputVars["w1TopDelPhi"], "w1TopDelPhi/F");
    tree->Branch("w2TopDelR", &inputVars["w2TopDelR"], "w2TopDelR/F");
    tree->Branch("w2TopDelPhi", &inputVars["w2TopDelPhi"], "w2TopDelPhi/F");
    tree->Branch("zjminR", &inputVars["minZJetR"], "zjminR/F");
    tree->Branch("zjminPhi", &inputVars["minZJetPhi"], "zjminPhi/F");
    tree->Branch("totHt", &inputVars["totHt"], "totHt/F");
    tree->Branch("jetHt", &inputVars["jetHt"], "jetHt/F");
    tree->Branch("jetMass", &inputVars["jetMass"], "jetMass/F");
    tree->Branch("jetPt", &inputVars["jetPt"], "jetPt/F");
    tree->Branch("jetEta", &inputVars["jetEta"], "jetEta/F");
    tree->Branch("jetPhi", &inputVars["jetPhi"], "jetPhi/F");
    tree->Branch("jetMass3", &inputVars["jetMass3"], "jetMass3/F");
    tree->Branch("totHtOverPt", &inputVars["totHtOverPt"], "totHtOverPt/F");
    tree->Branch("chi2", &inputVars["chi2"], "chi2/F");
}

void MakeMvaInputs::fillTree(TTree* outTreeSig,
                             TTree* outTreeSdBnd,
                             MvaEvent* tree,
                             std::string label,
                             std::string channel,
                             bool SameSignMC)
{
    uint syst = 0;

    if (label.find("__met__plus") != std::string::npos)
    {
        syst = 1024;
    }
    if (label.find("__met__minus") != std::string::npos)
    {
        syst = 2048;
    }

    if (channel == "emu")
    {
        inputVars["chan"] = 2;
    }
    if (channel == "ee")
    {
        inputVars["chan"] = 1;
    }
    if (channel == "mumu")
    {
        inputVars["chan"] = 0;
    }

    inputVars["eventNumber"] = tree->eventNum;

    std::pair<TLorentzVector, TLorentzVector> zPairLeptons =
        sortOutLeptons(tree, channel);
    TLorentzVector zLep1 = zPairLeptons.first;
    TLorentzVector zLep2 = zPairLeptons.second;

    TLorentzVector metVec(
        tree->metPF2PATPx, tree->metPF2PATPy, 0, tree->metPF2PATEt);

    std::pair<std::vector<int>, std::vector<TLorentzVector>> jetPair =
        getJets(tree, syst, metVec);
    std::vector<int> jets = jetPair.first;
    std::vector<TLorentzVector> jetVecs = jetPair.second;

    std::pair<std::vector<int>, std::vector<TLorentzVector>> bJetPair =
        getBjets(tree, syst, metVec, jets);
    std::vector<int> bJets = bJetPair.first;
    std::vector<TLorentzVector> bJetVecs = bJetPair.second;

    std::pair<TLorentzVector, TLorentzVector> wQuarkPair =
        sortOutHadronicW(tree);
    TLorentzVector wQuark1 = wQuarkPair.first;
    TLorentzVector wQuark2 = wQuarkPair.second;

    // Do unclustered met stuff here now that we have all of the objects, all
    // corrected for their various SFs etc ...
    if (syst == 1024 || syst == 2048)
    {
        metVec = doUncMet(metVec, zLep1, zLep2, jetVecs, syst);
    }

    // SFs for NPL lepton estimation normilisation
    // mz20 mw 20, ee = 0.958391264995; mumu = 1.02492608673;
    // mz20 mw 50, ee = 1.12750771638; mumu = 0.853155120216
    // mz50 mw 50, ee = 1.2334461839; mumu = 0.997331838956

    if (SameSignMC == true && channel == "ee")
    {
        inputVars["eventWeight"] = tree->eventWeight * 0.958391264995;
    }
    else if (SameSignMC == true && channel == "mumu")
    {
        inputVars["eventWeight"] = tree->eventWeight * 1.02492608673;
    }
    else
    {
        inputVars["eventWeight"] = tree->eventWeight;
    }

    inputVars["leadJetPt"] = jetVecs[0].Pt();
    inputVars["leadJetEta"] = jetVecs[0].Eta();
    inputVars["leadJetPhi"] = jetVecs[0].Phi();

    float totPx{0.0}, totPy{0.0};

    totPx += zLep1.Px() + zLep2.Px();
    totPy += zLep1.Py() + zLep2.Py();
    inputVars["lep1Pt"] = zLep1.Pt();
    inputVars["lep1Eta"] = zLep1.Eta();
    inputVars["lep1Phi"] = zLep1.Phi();
    inputVars["lep2Pt"] = zLep2.Pt();
    inputVars["lep2Eta"] = zLep2.Eta();
    inputVars["lep2Phi"] = zLep2.Phi();

    if (channel == "ee")
    {
        inputVars["lep1RelIso"] = tree->elePF2PATComRelIsoRho[tree->zLep1Index];
        inputVars["lep1D0"] = tree->elePF2PATD0PV[tree->zLep1Index];
        inputVars["lep2RelIso"] = tree->elePF2PATComRelIsoRho[tree->zLep2Index];
        inputVars["lep2D0"] = tree->elePF2PATD0PV[tree->zLep2Index];
    }
    if (channel == "mumu")
    {
        inputVars["lep1RelIso"] =
            tree->muonPF2PATComRelIsodBeta[tree->zLep1Index];
        inputVars["lep1D0"] = tree->muonPF2PATDBPV[tree->zLep1Index];
        inputVars["lep2RelIso"] =
            tree->muonPF2PATComRelIsodBeta[tree->zLep2Index];
        inputVars["lep2D0"] = tree->muonPF2PATDBPV[tree->zLep2Index];
    }
    if (channel == "emu")
    {
        inputVars["lep1RelIso"] = tree->elePF2PATComRelIsoRho[tree->zLep1Index];
        inputVars["lep1D0"] = tree->elePF2PATD0PV[tree->zLep1Index];
        inputVars["lep2RelIso"] =
            tree->muonPF2PATComRelIsodBeta[tree->zLep2Index];
        inputVars["lep2D0"] = tree->muonPF2PATDBPV[tree->zLep2Index];
    }

    inputVars["lepMass"] = (zLep1 + zLep2).M();
    inputVars["lepPt"] = std::sqrt(totPx * totPx + totPy * totPy);
    inputVars["lepEta"] = (zLep1 + zLep2).Eta();
    inputVars["lepPhi"] = (zLep1 + zLep2).Phi();
    inputVars["wQuark1Pt"] = wQuark1.Pt();
    inputVars["wQuark1Eta"] = wQuark1.Eta();
    inputVars["wQuark1Phi"] = wQuark1.Phi();
    inputVars["wQuark2Pt"] = wQuark2.Pt();
    inputVars["wQuark2Eta"] = wQuark2.Eta();
    inputVars["wQuark2Phi"] = wQuark2.Phi();

    float wPairMass = (wQuark1 + wQuark2).M();
    inputVars["wPairMass"] = wPairMass;
    inputVars["wPairPt"] = (wQuark1 + wQuark2).Pt();
    inputVars["wPairEta"] = (wQuark1 + wQuark2).Eta();
    inputVars["wPairPhi"] = (wQuark1 + wQuark2).Phi();
    totPx += jetVecs[0].Px();
    totPy += jetVecs[0].Py();

    if (jetVecs.size() > 1)
    {
        totPx += jetVecs[1].Px();
        totPy += jetVecs[1].Py();
    }
    inputVars["totPt2Jet"] = std::sqrt(totPx * totPx + totPy * totPy);

    for (uint i = 2; i != jetVecs.size(); i++)
    {
        totPx += jetVecs[i].Px();
        totPy += jetVecs[i].Py();
    }

    inputVars["totPt"] = std::sqrt(totPx * totPx + totPy * totPy);
    TLorentzVector totVec = (zLep1 + zLep2);

    for (uint i = 0; i != jetVecs.size(); i++)
    {
        totVec += jetVecs[i];
    }

    inputVars["totEta"] = totVec.Eta();
    inputVars["totEta"] = totVec.Phi();
    inputVars["totPtVec"] = totVec.Pt();
    inputVars["totVecM"] = totVec.M();
    inputVars["mTW"] =
        std::sqrt(2 * tree->jetPF2PATPt[tree->wQuark1Index]
                  * tree->jetPF2PATPt[tree->wQuark2Index]
                  * (1
                     - cos(tree->jetPF2PATPhi[tree->wQuark1Index]
                           - tree->jetPF2PATPhi[tree->wQuark2Index])));
    inputVars["nJets"] = float(jets.size());
    inputVars["nBjets"] = float(bJets.size());
    inputVars["met"] = tree->metPF2PATEt;
    inputVars["bTagDisc"] = tree->jetPF2PATBDiscriminator[jets[bJets[0]]];
    inputVars["leadJetbTag"] = tree->jetPF2PATBDiscriminator[jets[0]];
    inputVars["secJetbTag"] = -1.;
    inputVars["secJetPt"] = -1.;
    inputVars["secJetEta"] = -500.;
    inputVars["secJetPhi"] = -500.;
    inputVars["thirdJetbTag"] = -1.;
    inputVars["thirdJetPt"] = -1.;
    inputVars["thirdJetEta"] = -500.;
    inputVars["thirdJetPhi"] = -500.;
    inputVars["fourthJetbTag"] = -1.;
    inputVars["fourthJetPt"] = -1.;
    inputVars["fourthJetEta"] = -500.;
    inputVars["fourthJetPhi"] = -500.;

    if (jetVecs.size() > 1)
    {
        inputVars["secJetPt"] = jetVecs[1].Pt();
        inputVars["secJetEta"] = jetVecs[1].Eta();
        inputVars["secJetPhi"] = jetVecs[1].Phi();
        inputVars["secJetbTag"] = tree->jetPF2PATBDiscriminator[jets[1]];
    }

    if (jetVecs.size() > 2)
    {
        inputVars["thirdJetPt"] = jetVecs[2].Pt();
        inputVars["thirdJetEta"] = jetVecs[2].Eta();
        inputVars["thirdJetPhi"] = jetVecs[2].Phi();
        inputVars["thirdJetbTag"] = tree->jetPF2PATBDiscriminator[jets[2]];
    }

    if (jetVecs.size() > 3)
    {
        inputVars["fourthJetPt"] = jetVecs[3].Pt();
        inputVars["fourthJetEta"] = jetVecs[3].Eta();
        inputVars["fourthJetPhi"] = jetVecs[3].Phi();
        inputVars["fourthJetbTag"] = tree->jetPF2PATBDiscriminator[jets[3]];
    }

    float topMass = (bJetVecs[0] + wQuark1 + wQuark2).M();
    inputVars["topMass"] = topMass;
    inputVars["topPt"] = (bJetVecs[0] + wQuark1 + wQuark2).Pt();
    inputVars["topEta"] = (bJetVecs[0] + wQuark1 + wQuark2).Eta();
    inputVars["topPhi"] = (bJetVecs[0] + wQuark1 + wQuark2).Phi();
    inputVars["wZdelR"] = (zLep2 + zLep1).DeltaR(wQuark1 + wQuark2);
    inputVars["wZdelPhi"] = (zLep2 + zLep1).DeltaPhi(wQuark1 + wQuark2);

    inputVars["zQuark1DelR"] = (zLep2 + zLep1).DeltaR(wQuark1);
    inputVars["zQuark1DelPhi"] = (zLep2 + zLep1).DeltaPhi(wQuark1);
    inputVars["zQuark2DelR"] = (zLep2 + zLep1).DeltaR(wQuark2);
    inputVars["zQuark2DelPhi"] = (zLep2 + zLep1).DeltaPhi(wQuark2);

    inputVars["zTopDelR"] =
        (zLep2 + zLep1).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["zTopDelPhi"] =
        (zLep2 + zLep1).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["zl1TopDelR"] = (zLep1).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["zl1TopDelPhi"] =
        (zLep1).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["zl2TopDelR"] = (zLep2).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["zl2TopDelPhi"] =
        (zLep2).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);

    inputVars["wTopDelR"] =
        (wQuark1 + wQuark2).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["wTopDelPhi"] =
        (wQuark1 + wQuark2).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["w1TopDelR"] = (wQuark1).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["w1TopDelR"] = (wQuark1).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["w1TopDelPhi"] =
        (wQuark1).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["w2TopDelR"] = (wQuark2).DeltaR(bJetVecs[0] + wQuark1 + wQuark2);
    inputVars["w2TopDelPhi"] =
        (wQuark2).DeltaPhi(bJetVecs[0] + wQuark1 + wQuark2);

    inputVars["j1j2delR"] = -1.;
    inputVars["j1j2delPhi"] = -10.;

    if (jetVecs.size() > 1)
    {
        inputVars["j1j2delR"] = jetVecs[0].DeltaR(jetVecs[1]);
        inputVars["j1j2delPhi"] = jetVecs[0].DeltaPhi(jetVecs[1]);
    }

    inputVars["w1w2delR"] = (wQuark1).DeltaR(wQuark2);
    inputVars["w1w2delPhi"] = (wQuark1).DeltaPhi(wQuark2);
    inputVars["zLepdelR"] = (zLep1).DeltaR(zLep2);
    inputVars["zLepdelPhi"] = (zLep1).DeltaPhi(zLep2);
    inputVars["zl1Quark1DelR"] = (zLep1).DeltaR(wQuark1);
    inputVars["zl1Quark1DelPhi"] = (zLep1).DeltaPhi(wQuark1);
    inputVars["zl1Quark2DelR"] = (zLep1).DeltaR(wQuark2);
    inputVars["zl1Quark2DelPhi"] = (zLep1).DeltaPhi(wQuark2);
    inputVars["zl2Quark1DelR"] = (zLep2).DeltaR(wQuark1);
    inputVars["zl2Quark1DelPhi"] = (zLep2).DeltaPhi(wQuark1);
    inputVars["zl2Quark2DelR"] = (zLep2).DeltaR(wQuark2);
    inputVars["zl2Quark2DelPhi"] = (zLep2).DeltaPhi(wQuark2);
    inputVars["minZJetR"] = 3.0;

    float jetHt = 0.;
    TLorentzVector jetVector;

    for (uint i = 0; i != jetVecs.size(); i++)
    {
        jetHt += jetVecs[i].Pt();
        jetVector += jetVecs[i];
        if (jetVecs[i].DeltaR(zLep2 + zLep1) < inputVars["minZJetR"])
        {
            inputVars["minZJetR"] = jetVecs[i].DeltaR(zLep2 + zLep1);
        }
        if (jetVecs[i].DeltaPhi(zLep2 + zLep1) < inputVars["minZJetR"])
        {
            inputVars["minZJetPhi"] = jetVecs[i].DeltaPhi(zLep2 + zLep1);
        }
    }

    inputVars["zlb1DelR"] = zLep1.DeltaR(bJetVecs[0]);
    inputVars["zlb1DelPhi"] = zLep1.DeltaPhi(bJetVecs[0]);
    inputVars["zlb2DelR"] = zLep2.DeltaR(bJetVecs[0]);
    inputVars["zlb2DelPhi"] = zLep2.DeltaPhi(bJetVecs[0]);

    float ht = 0.;
    ht += zLep1.Pt() + zLep2.Pt();

    inputVars["lepHt"] = ht;
    inputVars["jetHt"] = jetHt;
    inputVars["jetMass"] = jetVector.M();
    inputVars["jetPt"] = jetVector.Pt();
    inputVars["jetEta"] = jetVector.Eta();
    inputVars["jetPhi"] = jetVector.Phi();

    if (channel != "emu")
    {
        inputVars["jetMass3"] = (jetVecs[0] + jetVecs[1] + jetVecs[2]).M();
    }
    else
    {
        inputVars["jetMass3"] = (jetVecs[0] + jetVecs[1]).M();
    }

    inputVars["wQuarkHt"] = wQuark1.Pt() + wQuark2.Pt();

    ht += jetHt;
    inputVars["totHt"] = ht;
    inputVars["totHtOverPt"] = ht / std::sqrt(totPx * totPx + totPy * totPy);
    inputVars["zMass"] = (zLep1 + zLep2).M();
    inputVars["zPt"] = (zLep2 + zLep1).Pt();
    inputVars["zEta"] = (zLep2 + zLep1).Eta();
    inputVars["zPhi"] = (zLep2 + zLep1).Phi();

    float wChi2Term = (wPairMass - 80.3585) / 8.0;
    float topChi2Term = (topMass - 173.21) / 30.0;
    inputVars["chi2"] = wChi2Term * wChi2Term + topChi2Term * topChi2Term;

    if (useSidebandRegion)
    {
        if (inputVars["chi2"] >= 40. and inputVars["chi2"] < 150.)
        {
            outTreeSdBnd->Fill();
        }
        if (inputVars["chi2"] < 40.)
        {
            outTreeSig->Fill();
        }
    }
    else
    {
        outTreeSig->Fill();
    }
}
