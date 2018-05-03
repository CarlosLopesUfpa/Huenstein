#include <iostream>
#include <fstream>
using namespace std;

int main () {
  ofstream myfile ("example.txt");
  if (myfile.is_open())
  {
  	for(int x = 0; x<10; ++x){
    myfile << "This is a line."<< x <<"\n";
    myfile << "This is another line.\n";
	}
    myfile.close();
  }
  else cout << "Unable to open file";
  return 0;
}