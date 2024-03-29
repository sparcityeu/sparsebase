#ifndef SPARSEBASE_PROJECT_CONVERTER_ORDER_TWO_CUDA_CUH
#define SPARSEBASE_PROJECT_CONVERTER_ORDER_TWO_CUDA_CUH

#include "converter.h"
#include "converter_cuda.cuh"
#include "sparsebase/config.h"
namespace sparsebase::converter {

template <typename IDType, typename NNZType, typename ValueType>
sparsebase::format::Format *CsrCUDACsrConditionalFunction(
    sparsebase::format::Format *source, sparsebase::context::Context *context);

template <typename IDType, typename NNZType, typename ValueType>
sparsebase::format::Format *CUDACsrCsrConditionalFunction(
    sparsebase::format::Format *source, sparsebase::context::Context *context);

template <typename IDType, typename NNZType, typename ValueType>
sparsebase::format::Format *CUDACsrCUDACsrConditionalFunction(
    sparsebase::format::Format *source, sparsebase::context::Context *context);

}  // namespace sparsebase::converter
#ifdef _HEADER_ONLY
#include "converter_order_two_cuda.cu"
#endif
#endif  // SPARSEBASE_PROJECT_CONVERTER_ORDER_TWO_CUDA_CUH
