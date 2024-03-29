#include <iostream>

#include "sparsebase/feature/degree_distribution.h"
#include "sparsebase/feature/degrees.h"
#include "sparsebase/feature/feature_extractor.h"
#include "sparsebase/format/csr.h"
#include "sparsebase/format/format.h"
#include "sparsebase/format/format_order_one.h"
#include "sparsebase/format/format_order_two.h"
#include "sparsebase/io/mtx_reader.h"

using namespace std;
using namespace sparsebase;
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

  string file_name = argv[1];
  sparsebase::io::MTXReader<vertex_type, edge_type, value_type> reader(
      file_name);
  COO<vertex_type, edge_type, value_type> *coo = reader.ReadCOO();
  context::CPUContext cpu_context;

  {
    sparsebase::feature::Extractor engine =
        sparsebase::feature::FeatureExtractor<vertex_type, edge_type,
                                              value_type, feature_type>();
    engine.PrintFuncList();
    engine.Add(feature::Feature(degrees{}));
    engine.Subtract(feature::Feature(degrees{}));
    try {
      engine.Add(
          feature::Feature(feature::DegreeDistribution<vertex_type, edge_type,
                                                       value_type, float>{}));
    } catch (utils::FeatureException &ex) {
      cout << ex.what() << endl;
    }
    engine.Add(feature::Feature(degrees{}));
    engine.Add(feature::Feature(degree_dist{}));
    // engine.Add(feature::Feature(
    //     feature::Degrees_DegreeDistribution<vertex_type, edge_type,
    //                                            value_type, feature_type>{}));

    // print features to be extracted
    auto fs = engine.GetList();
    cout << endl << "Features that will be extracted: " << endl;
    for (auto f : fs) {
      cout << f.name() << endl;
    }
    cout << endl;

    // extract features
    auto raws = engine.Extract(coo, {&cpu_context}, true);
    cout << "#features extracted: " << raws.size() << endl;
    auto dgrs = std::any_cast<vertex_type *>(raws[degrees::get_id_static()]);
    auto dst =
        std::any_cast<feature_type *>(raws[degree_dist::get_id_static()]);
    cout << "vertex 0 => degree: " << dgrs[2] << endl;
    cout << "dst[0] " << dst[2] << endl;
  }
  return 0;
}
