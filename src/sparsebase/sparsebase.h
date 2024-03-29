/*******************************************************
 * Copyright (c) 2022 SparCity, Amro Alabsi Aljundi, Taha Atahan Akyildiz, Arda
 *Sener All rights reserved.
 *
 * This file is distributed under MIT license.
 * The complete license agreement can be obtained at:
 * https://sparcityeu.github.io/sparsebase/pages/license.html
 ********************************************************/
#ifndef SPARSEBASE_SPARSEBASE_H_
#define SPARSEBASE_SPARSEBASE_H_
#include "sparsebase/bases/graph_feature_base.h"
#include "sparsebase/bases/iobase.h"
#include "sparsebase/bases/reorder_base.h"
#include "sparsebase/config.h"
#include "sparsebase/context/context.h"
#include "sparsebase/context/cpu_context.h"
#include "sparsebase/converter/converter.h"
#include "sparsebase/converter/converter_order_one.h"
#include "sparsebase/converter/converter_order_two.h"
#include "sparsebase/converter/converter_store.h"
#include "sparsebase/experiment/concrete_experiment.h"
#include "sparsebase/experiment/experiment_helper.h"
#include "sparsebase/experiment/experiment_type.h"
#include "sparsebase/format/array.h"
#include "sparsebase/format/coo.h"
#include "sparsebase/format/csc.h"
#include "sparsebase/format/csr.h"
#include "sparsebase/format/format.h"
#include "sparsebase/format/format_implementation.h"
#include "sparsebase/format/format_order_one.h"
#include "sparsebase/format/format_order_two.h"
#include "sparsebase/io/binary_reader_order_one.h"
#include "sparsebase/io/binary_reader_order_two.h"
#include "sparsebase/io/binary_writer_order_one.h"
#include "sparsebase/io/binary_writer_order_two.h"
#include "sparsebase/io/edge_list_reader.h"
#include "sparsebase/io/mtx_reader.h"
#include "sparsebase/io/pigo_edge_list_reader.h"
#include "sparsebase/io/pigo_mtx_reader.h"
#include "sparsebase/io/reader.h"
#include "sparsebase/io/sparse_file_format.h"
#include "sparsebase/io/writer.h"
#include "sparsebase/object/object.h"
#include "sparsebase/partition/metis_partition.h"
#include "sparsebase/partition/partitioner.h"
#include "sparsebase/partition/patoh_partition.h"
#include "sparsebase/partition/pulp_partition.h"
#include "sparsebase/permute/permute_order_one.h"
#include "sparsebase/permute/permute_order_two.h"
#include "sparsebase/permute/permuter.h"
#include "sparsebase/reorder/amd_reorder.h"
#include "sparsebase/reorder/degree_reorder.h"
#include "sparsebase/reorder/gray_reorder.h"
#include "sparsebase/reorder/metis_reorder.h"
#include "sparsebase/reorder/rabbit_reorder.h"
#include "sparsebase/reorder/rcm_reorder.h"
#include "sparsebase/reorder/reorder_heatmap.h"
#include "sparsebase/reorder/reorderer.h"
#include "sparsebase/utils/exception.h"
#include "sparsebase/utils/logger.h"
#include "sparsebase/utils/utils.h"
#ifdef USE_CUDA
#include "sparsebase/context/cuda_context_cuda.cuh"
#include "sparsebase/format/cuda_array_cuda.cuh"
#include "sparsebase/format/cuda_csr_cuda.cuh"
#include "sparsebase/utils/utils_cuda.cuh"
#endif
#endif