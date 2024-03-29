#include "sparsebase/feature/feature_extractor.h"
#include "sparsebase/feature/degrees.h"
#include "sparsebase/feature/degree_distribution.h"
#include "sparsebase/format/format.h"
#include "sparsebase/bases/iobase.h"
#include <iostream>

using namespace std;
using namespace sparsebase;
using namespace bases;
using namespace format;

using vertex_type = unsigned int;
using edge_type = unsigned int;
using value_type = float;
using feature_type = double;

using degrees = feature::Degrees<vertex_type, edge_type, value_type>;
using degree_dist = feature::DegreeDistribution<vertex_type, edge_type,
    value_type, feature_type>;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: ./sparse_feature <matrix_market_format>\n";
    cout << "Hint: You can use the matrix market file: "
            "examples/data/ash958.mtx\n";
    return 1;
  }

  // The name of the matrix-market file
  string file_name = argv[1];

  // Read the matrix in to a COO representation
  COO<vertex_type, edge_type, value_type> *coo = IOBase::ReadMTXToCOO<vertex_type, edge_type, value_type>(file_name);

  // Create an extractor with the correct types of your COO (data) and your expected feature type
  sparsebase::feature::Extractor engine =
      sparsebase::feature::FeatureExtractor<vertex_type, edge_type,
          value_type, feature_type>();

  //add all the feature you want to extract to the extractor
  engine.Add(feature::Feature(degrees{}));
  engine.Add(feature::Feature(degree_dist{}));

  // print features to be extracted
  auto fs = engine.GetList();
  cout << endl << "Features that will be extracted: " << endl;
  for (auto f : fs) {
    cout << utils::demangle(f.name()) << endl;
  }
  cout << endl;

  // Create a context, CPUcontext for this case.
  // The contexts defines the architecture that the computation will take place in.
  context::CPUContext cpu_context;
  // extract features
  auto raws = engine.Extract(coo, {&cpu_context}, true);

  cout << "#features extracted: " << raws.size() << endl;
  auto dgrs =
      std::any_cast<vertex_type *>(raws[degrees::get_id_static()]);
  auto dst = std::any_cast<feature_type *>(
      raws[degree_dist::get_id_static()]);
  cout << "vertex 0 => degree: " << dgrs[0] << endl;
  cout << "dst[0] " << dst[0] << endl;
  return 0;
}
