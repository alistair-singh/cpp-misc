
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

auto run(int num) {
  vector<int> v;
  v.reserve(num);
  fill_n(back_inserter(v), num, num);
  return v;
}

int main(int argc, char **argv) {
  cout << "vectorint.cc" << endl;
  if (argc != 2) {
    cout << "usage: vectorint [num]" << endl
         << "\t [num] is a signed integer." << endl;
    return 1;
  }

  //  try
  //  {
  int num = stoi(argv[1], nullptr);
  cout << "number of items " << num << endl;
  auto v = run(num);
  cout << "memory reserved, press enter to quit." << endl;
  
  string line;
  getline(cin, line);
  return 0;
  //  }
  //  catch (std::exception& e)
  //  {
  //    cout << "!!! Exception :" << e.what() << endl;
  //    return 2;
  //  }
}
