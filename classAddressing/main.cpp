#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

void doTheThing(const string &ipa) {
  stringstream ss(ipa);
  vector<int> octs(5, 0);
  int i = 1;
  char className = '$';
  string oct = "";
  while (getline(ss, oct, '.') && i < 5) {
    octs[i++] = stoi(oct);
  }

  int firstByte = octs[1];

  if (firstByte >= 0) {
    if (firstByte <= 127) {
      className = 'A';
    } else if (firstByte <= 191) {
      className = 'B';
    } else if (firstByte <= 223) {
      className = 'C';
    } else if (firstByte <= 239) {
      className = 'D';
    } else if (firstByte <= 255) {
      className = 'E';
    } else {
      cout << "Invalid IP Address!! Bytes can't be greater than 255!!\n";
      return;
    }
  }

  switch (className) {
  case 'A': {
    cout << "NetId: " << octs[1] << ".0.0.0\n";
    cout << "BroadcastId: " << octs[1] << "255.255.255\n";
    break;
  }
  case 'B': {
    cout << "NetId: " << octs[1] << "." << octs[2] << ".0.0\n";
    cout << "BroadcastId: " << octs[1] << "." << octs[2] << ".255.255\n";
    break;
  }
  case 'C': {
    cout << "NetId: " << octs[1] << "." << octs[2] << "." << octs[3] << ".0\n";
    cout << "BroadcastId: " << octs[1] << "." << octs[2] << "." << octs[3]
         << ".255\n";
    break;
  }
  default: {
    cout << "Invalid IP Address\n";
    break;
  }
  }
}

int main() {
  string ipa;
  cout << "Enter IP Address: ";
  getline(cin, ipa);
  doTheThing(ipa);
}
