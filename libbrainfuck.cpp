#include <bits/stdc++.h>
using namespace std;

namespace {
vector<char> data(1);
}

extern "C" void dplsh(size_t *dp, size_t shift) {
  *dp -= shift;
  assert(0 <= *dp && *dp < data.size());
}

extern "C" char *dprsh(size_t *dp, size_t shift) {
  *dp += shift;
  if (*dp >= data.size()) data.resize(*dp + 1);
  return data.data();
}
