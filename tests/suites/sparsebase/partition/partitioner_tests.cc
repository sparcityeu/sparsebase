#include <memory>

#include "sparsebase/bases/iobase.h"
#include "sparsebase/bases/reorder_base.h"
#include "sparsebase/format/coo.h"
#include "sparsebase/format/csc.h"
#include "sparsebase/format/csr.h"
#include "sparsebase/format/format_order_one.h"
#include "sparsebase/format/format_order_two.h"
#include "sparsebase/partition/partitioner.h"
#include "sparsebase/reorder/degree_reorder.h"
#include "sparsebase/reorder/reorderer.h"
#ifdef USE_CUDA
#include "sparsebase/converter/converter_cuda.cuh"
#include "sparsebase/converter/converter_order_one_cuda.cuh"
#include "sparsebase/converter/converter_order_two_cuda.cuh"
#include "sparsebase/format/cuda_array_cuda.cuh"
#include "sparsebase/format/cuda_csr_cuda.cuh"
#endif

const std::string FILE_NAME = "../../../../examples/data/ash958.mtx";

using namespace sparsebase;
;
using namespace sparsebase::reorder;
using namespace sparsebase::partition;
using namespace sparsebase::bases;
#include "../functionality_common.inc"
