#include <iostream>

#include "sparsebase/format/coo.h"
#include "sparsebase/format/csr.h"
#include "sparsebase/format/format.h"
#include "sparsebase/format/format_order_one.h"
#include "sparsebase/format/format_order_two.h"

using namespace std;
using namespace sparsebase;

template <typename IDType, typename NNZType, typename VALType>
format::Format *MyFunction(format::Format *source, context::Context *) {
  return nullptr;
}

int main() {
  int row[6] = {0, 0, 1, 1, 2, 2};
  int col[6] = {0, 1, 1, 2, 3, 3};
  int vals[6] = {10, 20, 30, 40, 50, 60};

  format::COO<int, int, int> *coo =
      new format::COO<int, int, int>(6, 6, 6, row, col, vals);
  context::CPUContext cpu_context;

  auto converter = new converter::ConverterOrderTwo<int, int, int>();

  converter->RegisterConversionFunction(
      format::COO<int, int, int>::get_id_static(),
      format::CSR<int, int, int>::get_id_static(), MyFunction<int, int, int>,
      [](context::Context *, context::Context *) -> bool { return true; });

  auto csr = converter->Convert(
      coo, format::CSR<int, int, int>::get_id_static(), &cpu_context);
  cout << csr << endl;

  delete coo;
  delete converter;
  delete csr;
}
