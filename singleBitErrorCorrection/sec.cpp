#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

string validateFrame(string i_frame){
  // validate here
  return i_frame;
}

string generateParity(string frame, unordered_set<int> Gi){
  int Pi=0;
  // cout<<"\nInside generateParity\n";
  for(int pos:Gi){
    Pi=(Pi ^ (frame[pos-1]-'0')); // frame[pos-1] is the bit and - '0' to get int
  }
  return to_string(Pi);
}

string constructCodeword(string i_frame, vector<unordered_set<int>> Gs){
  int n=Gs.size();
  string codew=i_frame;
  for(int i=0; i<n; i++){
    codew += generateParity(i_frame, Gs[i]);
  }
  cout<<"in the constructCodeword codew = "<<codew<<endl;
  return codew;
}
string simulateError(string codew){
  srand(time(nullptr));
  int n=codew.size();
  int randPos=(rand() % n);
  cout<<"Error created at "<<randPos<<endl;
  char ch = (codew[randPos]=='1') ? '0':'1';
  string err_codew = codew.substr(0,randPos) + ch;
  if(randPos+1 < n) err_codew += codew.substr(randPos+1);

  return err_codew;
}

unordered_set<int> set_intrscn(const unordered_set<int>& set1, const unordered_set<int>& set2){
  // cout<<"\nInside set_intrscn\n";
  unordered_set<int> intersection;
  const auto& smaller = (set1.size() <= set2.size()) ? set1 : set2;
  const auto& larger = (set1.size() <= set2.size()) ? set2 : set1;

  for (const auto& elem : smaller) {
    if (larger.count(elem) != 0){ 
        intersection.insert(elem);
    }
  }
  return intersection;
}

pair<int, char> sec(string &sen_codew, string &rec_codew, vector<unordered_set<int>> Gs){
  // cout<<"\nInside sec\n";
  int n=sen_codew.size();
  int k=Gs.size();
  int m=n-k;
  unordered_set<int> failParity; // only the failing parityBits can isolate the erroneous bit

  pair<int, char> ans;
  for(int i=0; i<k; i++){
    string Pi_r = generateParity(rec_codew, Gs[i]);
    string Pi = string(1, rec_codew[m+i]);
    if (Pi != Pi_r) failParity.insert(i);
  }

  int i;
  if(failParity.size() == 1){
    // that parity bit is wrong itself
    for(auto in : failParity) i=in;
    return {m+i, 'P'};
  }
  else if(failParity.size() == 0){
    cout<<"No Error\n"; // (no error)
  }
  else {
    cout<<"\nfailParitySet size= "<< failParity.size()<<endl;
    // find the common bits in failed parity set
    unordered_set<int> commonBits={};
    for(auto in: failParity){
      cout<<"Fail parity: "<<in<<endl;
      if(commonBits.empty()) 
        commonBits = Gs[in];
      else 
        commonBits = set_intrscn(commonBits, Gs[in]);
    }
    cout<<"\nCommon Bits: (1-indexed) ";
    for(auto ele: commonBits){
      cout<<ele<<" ";
    }
    cout<<endl;

    // remove the bits that are in groups that have correct parity (not in failParity)
    for(int i=0; i<k; i++){ // k = number of groups
      if(failParity.count(i)==0){ // Gs[i] has correct parity
        /*
        for(auto ele: commonBits){
          if(Gs[i].count(ele)!=0) commonBits.erase(ele); // <-- modifying a container while iterating over it can cause segfault
        }
        */

        // remove the bits that are in groups that have correct parity
        for(int i = 0; i < k; i++){ 
          if(failParity.count(i) == 0){ 
            vector<int> toRemove;
            for(auto ele : commonBits){
              if(Gs[i].count(ele) != 0) {
                toRemove.push_back(ele);
              }
            }
            // Now safely erase them
            for(int ele : toRemove) {
                commonBits.erase(ele);
            }
          }
        }
      }
    }
    // finally common bits should have only one bit which is the erroneous bit 
    if(commonBits.size()!=1) {
      cout<<"Common bits doesn't have size 1 after operations\n Most probable a bug in this simulation\n";
    }
    else {
      for(auto in : commonBits) i=in;
      return {i-1, 'D'};
    }
  }
  return ans;
}

int main(){
  // so we are assuming the transfer channel can cause only single bit error
  // we need to detect the error and correct it
  // for this, we divide the frame (input) into groups of bits (positional) (pre decided for now) and assign a parity bit to each group
  // we then construct the codeword by appending the parity bits at the end of input
  // then we randomly change a bit in the codeword to simulate a transmission error (single bit)
  // the task is to find the bit position, correct it and return the {bitPos, correctCodeWord}
  
  // Pre decided info
  int k=3; // number of groups
  vector<unordered_set<int>> Gs(k);
  Gs[0]={1,3,4}; Gs[1]={1,2,3}; Gs[2]={2,3,4}; // only here i am using 1-indexed
  // i started with the though of using 1-indexing, then forgot about it and ended up using 0-indexing everywhere

  string i_frame;
  while(true){
    cout<<"\n----------------------\nSENDER! Enter the dataframe: ";
    getline(cin, i_frame);
    i_frame=validateFrame(i_frame);
    if(i_frame == "X") {
      cout<<"Closing the program!\n-------------";
      return 0;
    }
    // handle the exit
    int flag=0;
    for(char c:i_frame){
      if(c!='0' && c!='1'){
        cout<<"Invalid dataframe (only 1 and 0 are allowed!)"<<endl;
        flag=1; break;
      }
    } 
    if(flag==1) continue;

    int m=i_frame.length();
    // construct codeword
    string codew=constructCodeword(i_frame, Gs);
    // generate a single bit transmission error
    string err_codew=simulateError(codew);
    cout<<"After transmission, receiver received frame: "<<err_codew<<endl;
    // detect and correct the error

    cout<<"Error detected!\n"; // codew != err_codew
    cout<<"Working on correction: ";
    pair<int,char> bitPos=sec(codew, err_codew, Gs);
    if(bitPos.second == 'D'){
      cout<<"Error in the data word\n";
    }
    else if(bitPos.second == 'P'){
      cout<<"Error in the Parity bit\n";
    }
    else cout<<"No Error\nIgnore next line\n"; 
    cout<<"Error at "<<bitPos.first<<" fixed!\n";
  }
}
