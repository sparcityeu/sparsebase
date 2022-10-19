#include "sparsebase/format/format.h"
#include "sparsebase/utils/utils.h"
#include "sparsebase/utils/converter/converter.h"
#include <iostream>
#include <deque>
#include <set>
#include <unordered_set>
#include <algorithm>

using namespace sparsebase::format;

namespace sparsebase::utils::converter {

ConversionMap * Converter::get_conversion_map(bool is_move_conversion) {
  if (is_move_conversion)
    return &move_conversion_map_;
  else
    return &copy_conversion_map_;
}


template <typename IDType, typename NNZType, typename ValueType>
Format *CooCscFunctionConditional(Format *source, context::Context *context) {
  auto *coo = source->AsAbsolute<COO<IDType, NNZType, ValueType>>();

  std::vector<DimensionType> dimensions = coo->get_dimensions();
  IDType n = dimensions[0];
  IDType m = dimensions[1];
  NNZType nnz = coo->get_num_nnz();
  auto coo_col = coo->get_col();
  auto coo_row = coo->get_row();
  auto coo_vals = coo->get_vals();
  NNZType *col_ptr = new NNZType[n + 1];
  NNZType * col_counter = new NNZType[n]();
  IDType *row = new IDType[nnz];
  ValueType *vals;
  if constexpr (!std::is_same_v<void, ValueType>) {
    if (coo->get_vals() != nullptr) {
      vals = new ValueType[nnz];
    } else {
      vals = nullptr;
    }
  } else {
    vals = nullptr;
  }

  std::fill(col_ptr, col_ptr + n + 1, 0);
  std::fill(row, row + nnz, 0);

  for (IDType i = 0; i < nnz; i++) {
    col_ptr[coo_col[i]+1]++;
  }
  for (IDType i = 1; i <= n; i++) {
    col_ptr[i] += col_ptr[i - 1];
  }

  for (IDType i = 0; i < nnz; i++) {
    auto this_nnz_col = coo_col[i];
    auto this_nnz_row = coo_row[i];
    row[col_ptr[this_nnz_col]+col_counter[this_nnz_col]++] = this_nnz_row;
    if constexpr (!std::is_same_v<void, ValueType>) {
      if (coo_vals != nullptr) {
        vals[col_ptr[this_nnz_col]+col_counter[this_nnz_col]-1] = coo_vals[i];
      }
    }
  }
  auto csc =
      new CSC<IDType, NNZType, ValueType>(n, m, col_ptr, row, vals, kOwned, false);
  return csc;
}
template <typename IDType, typename NNZType, typename ValueType>
Format *CsrCooFunctionConditional(Format *source, context::Context *context) {
  auto *csr = source->AsAbsolute<CSR<IDType, NNZType, ValueType>>();

  std::vector<DimensionType> dimensions = csr->get_dimensions();
  IDType n = dimensions[0];
  IDType m = dimensions[1];
  NNZType nnz = csr->get_num_nnz();

  auto col = new IDType[nnz];
  auto row = new IDType[nnz];
  auto csr_row_ptr = csr->get_row_ptr();
  auto csr_col = csr->get_col();

  IDType count = 0;
  for (IDType i = 0; i < n; i++) {
    IDType start = csr_row_ptr[i];
    IDType end = csr_row_ptr[i + 1];

    for (IDType j = start; j < end; j++) {
      row[count] = i;
      count++;
    }
  }

  for (IDType i = 0; i < nnz; i++) {
    col[i] = csr_col[i];
  }

  // if (csr->vals != nullptr)
  ValueType *vals = nullptr;
  if constexpr (!std::is_same_v<void, ValueType>) {
    if (csr->get_vals() != nullptr) {
      vals = new ValueType[nnz];
      auto csr_vals = csr->get_vals();
      for (NNZType i = 0; i < nnz; i++) {
        vals[i] = csr_vals[i];
      }
    } else {
      vals = nullptr;
    }
  }
  auto *coo =
      new COO<IDType, NNZType, ValueType>(n, m, nnz, row, col, vals, kOwned);

  return coo;
}
template <typename IDType, typename NNZType, typename ValueType>
Format *CsrCscFunctionConditional(Format *source, context::Context *context) {
  auto *csr = source->AsAbsolute<CSR<IDType, NNZType, ValueType>>();

  auto coo = CsrCooFunctionConditional<IDType, NNZType, ValueType>(csr, context);


  return CooCscFunctionConditional<IDType, NNZType, ValueType>(coo, context);
}

template <typename IDType, typename NNZType, typename ValueType>
Format *CsrCooMoveConditionalFunction(Format *source, context::Context *) {
  auto *csr = source->AsAbsolute<CSR<IDType, NNZType, ValueType>>();
  auto col = csr->release_col();
  auto vals = csr->release_vals();
  std::vector<DimensionType> dimensions = csr->get_dimensions();
  IDType n = dimensions[0];
  IDType m = dimensions[1];
  NNZType nnz = csr->get_num_nnz();

  auto row = new IDType[nnz];
  auto csr_row_ptr = csr->get_row_ptr();

  IDType count = 0;
  for (IDType i = 0; i < n; i++) {
    IDType start = csr_row_ptr[i];
    IDType end = csr_row_ptr[i + 1];

    for (IDType j = start; j < end; j++) {
      row[count] = i;
      count++;
    }
  }

  // if (csr->vals != nullptr)
  auto *coo =
      new COO<IDType, NNZType, ValueType>(n, m, nnz, row, col, vals, kOwned);

  return coo;
}

template <typename IDType, typename NNZType, typename ValueType>
Format *CooCsrFunctionConditional(Format *source, context::Context *context) {
  auto *coo = source->AsAbsolute<COO<IDType, NNZType, ValueType>>();

  std::vector<DimensionType> dimensions = coo->get_dimensions();
  IDType n = dimensions[0];
  IDType m = dimensions[1];
  NNZType nnz = coo->get_num_nnz();
  auto coo_col = coo->get_col();
  auto coo_row = coo->get_row();
  NNZType *row_ptr = new NNZType[n + 1];
  IDType *col = new IDType[nnz];
  ValueType *vals;

  std::fill(row_ptr, row_ptr + n + 1, 0);
  std::fill(col, col + nnz, 0);

  for (IDType i = 0; i < nnz; i++) {
    col[i] = coo_col[i];
    row_ptr[coo_row[i]]++;
  }

  for (IDType i = 1; i <= n; i++) {
    row_ptr[i] += row_ptr[i - 1];
  }

  for (IDType i = n; i > 0; i--) {
    row_ptr[i] = row_ptr[i - 1];
  }
  row_ptr[0] = 0;

  if constexpr (!std::is_same_v<void, ValueType>) {
    if (coo->get_vals() != nullptr) {
      vals = new ValueType[nnz];
      auto coo_vals = coo->get_vals();
      std::fill(vals, vals + nnz, 0);
      for (NNZType i = 0; i < nnz; i++) {
        vals[i] = coo_vals[i];
      }
    } else {
      vals = nullptr;
    }
  } else {
    vals = nullptr;
  }

  auto csr =
      new CSR<IDType, NNZType, ValueType>(n, m, row_ptr, col, vals, kOwned);
  return csr;
}

template <typename IDType, typename NNZType, typename ValueType>
Format *CooCsrMoveConditionalFunction(Format *source, context::Context *) {
  auto *coo = source->AsAbsolute<COO<IDType, NNZType, ValueType>>();

  std::vector<DimensionType> dimensions = coo->get_dimensions();
  IDType n = dimensions[0];
  IDType m = dimensions[1];
  NNZType nnz = coo->get_num_nnz();
  auto coo_row = coo->get_row();
  NNZType *row_ptr = new NNZType[n + 1];
  IDType *col = coo->release_col();
  ValueType *vals = coo->release_vals();

  std::fill(row_ptr, row_ptr + n + 1, 0);

  for (IDType i = 0; i < nnz; i++) {
    row_ptr[coo_row[i]]++;
  }

  for (IDType i = 1; i <= n; i++) {
    row_ptr[i] += row_ptr[i - 1];
  }

  for (IDType i = n; i > 0; i--) {
    row_ptr[i] = row_ptr[i - 1];
  }
  row_ptr[0] = 0;

  auto csr =
      new CSR<IDType, NNZType, ValueType>(n, m, row_ptr, col, vals, kOwned);
  return csr;
}

template <typename ValueType>
Converter *ConverterOrderOne<ValueType>::Clone() const {
  return new ConverterOrderOne<ValueType>(*this);
}

template <typename ValueType> void ConverterOrderOne<ValueType>::Reset() {
  this->ResetConverterOrderOne();
}
template <typename ValueType> void ConverterOrderOne<ValueType>::ResetConverterOrderOne() {
#ifdef USE_CUDA
  this->RegisterConversionFunction(
      Array<ValueType>::get_format_id_static(),
      format::cuda::CUDAArray<ValueType>::get_format_id_static(),
      converter::cuda::ArrayCUDAArrayConditionalFunction<ValueType>,
      [](context::Context *, context::Context *to) -> bool { return to->get_context_type_member()==context::cuda::CUDAContext::get_context_type(); });
  this->RegisterConversionFunction(
      format::cuda::CUDAArray<ValueType>::get_format_id_static(),
      Array<ValueType>::get_format_id_static(),
      converter::cuda::CUDAArrayArrayConditionalFunction<ValueType>,
      [](context::Context *, context::Context *to) -> bool { return to->get_context_type_member()==context::CPUContext::get_context_type(); });
#endif
}

template <typename ValueType>
ConverterOrderOne<ValueType>::ConverterOrderOne() {
  this->ResetConverterOrderOne();
}

template <typename IDType, typename NNZType, typename ValueType>
Converter *ConverterOrderTwo<IDType, NNZType, ValueType>::Clone() const {
  return new ConverterOrderTwo<IDType, NNZType, ValueType>(*this);
}

template <typename IDType, typename NNZType, typename ValueType>
void ConverterOrderTwo<IDType, NNZType, ValueType>::Reset() {
  this->ResetConverterOrderTwo();
}

template <typename IDType, typename NNZType, typename ValueType>
void ConverterOrderTwo<IDType, NNZType, ValueType>::ResetConverterOrderTwo() {
  this->RegisterConversionFunction(
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      CooCsrFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      });
  this->RegisterConversionFunction(
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CsrCooFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      });
  this->RegisterConversionFunction(
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CSC<IDType, NNZType, ValueType>::get_format_id_static(),
      CooCscFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      });
  this->RegisterConversionFunction(
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      CSC<IDType, NNZType, ValueType>::get_format_id_static(),
      CsrCscFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      });
#ifdef USE_CUDA
  this->RegisterConversionFunction(
      format::cuda::CUDACSR<IDType, NNZType, ValueType>::get_format_id_static(),
      format::cuda::CUDACSR<IDType, NNZType, ValueType>::get_format_id_static(),
      converter::cuda::CUDACsrCUDACsrConditionalFunction<IDType, NNZType,
                                                         ValueType>,
      converter::cuda::CUDAPeerToPeer);
  this->RegisterConversionFunction(
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      format::cuda::CUDACSR<IDType, NNZType, ValueType>::get_format_id_static(),
      converter::cuda::CsrCUDACsrConditionalFunction<IDType, NNZType,
                                                     ValueType>,
      [](context::Context *, context::Context *to) -> bool { return to->get_context_type_member()==context::cuda::CUDAContext::get_context_type(); });
  this->RegisterConversionFunction(
      format::cuda::CUDACSR<IDType, NNZType, ValueType>::get_format_id_static(),
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      converter::cuda::CUDACsrCsrConditionalFunction<IDType, NNZType,
                                                     ValueType>,
      [](context::Context *, context::Context *to) -> bool { return to->get_context_type_member()==context::CPUContext::get_context_type(); });
#endif
  this->RegisterConversionFunction(
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      CooCsrMoveConditionalFunction<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      },
      true);
  this->RegisterConversionFunction(
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CsrCooMoveConditionalFunction<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      },
      true);
  this->RegisterConversionFunction(
      COO<IDType, NNZType, ValueType>::get_format_id_static(),
      CSC<IDType, NNZType, ValueType>::get_format_id_static(),
      CooCscFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      },
      true);
  this->RegisterConversionFunction(
      CSR<IDType, NNZType, ValueType>::get_format_id_static(),
      CSC<IDType, NNZType, ValueType>::get_format_id_static(),
      CsrCscFunctionConditional<IDType, NNZType, ValueType>,
      [](context::Context *, context::Context *to) -> bool {
        return to->get_context_type_member() ==
               context::CPUContext::get_context_type();
      },
      true);
}

template <typename IDType, typename NNZType, typename ValueType>
ConverterOrderTwo<IDType, NNZType, ValueType>::ConverterOrderTwo() {
  this->ResetConverterOrderTwo();
}

void Converter::RegisterConversionFunction(
    std::type_index from_type, std::type_index to_type,
    ConversionFunction conv_func,
                                           ConversionCondition edge_condition,
    bool is_move_conversion) {
  auto map = get_conversion_map(is_move_conversion);
  if (map->count(from_type) == 0) {
    map->emplace(
        from_type,
        std::unordered_map<
            std::type_index,
            std::vector<
                std::tuple<ConversionCondition, ConversionFunction>>>());
  }

  if ((*map)[from_type].count(to_type) == 0) {
    (*map)[from_type][to_type].push_back(
        std::make_tuple<ConversionCondition, ConversionFunction>(
            std::forward<ConversionCondition>(edge_condition),
            std::forward<ConversionFunction>(conv_func)));
    //(*map)[from_type].emplace(to_type, {make_tuple<EdgeConditional,
    // ConversionFunction>(std::forward<EdgeConditional>(edge_condition),
    // std::forward<ConversionFunction>(conv_func))});
  } else {
    (*map)[from_type][to_type].push_back(
        std::make_tuple<ConversionCondition, ConversionFunction>(
            std::forward<ConversionCondition>(edge_condition),
            std::forward<ConversionFunction>(conv_func)));
  }
}

Format *Converter::Convert(Format *source, std::type_index to_type,
                           std::vector<context::Context *>to_contexts,
                           bool is_move_conversion) {
  auto outputs = ConvertCached(source, to_type, to_contexts, is_move_conversion);
  if (outputs.size()>1)
    std::transform(outputs.begin(), outputs.end()-1, outputs.begin(), [](format::Format* f){
      delete f;
      return nullptr;
    });
  return outputs.back();
}

std::vector<Format *>Converter::ConvertCached(Format *source, std::type_index to_type,
                                              std::vector<context::Context *>to_contexts,
                                              bool is_move_conversion) {

  if (to_type == source->get_format_id() &&
      std::find_if(to_contexts.begin(), to_contexts.end(), [&source](context::Context* from){
        return from->IsEquivalent(source->get_context());
      }) != to_contexts.end()) {
    return {source};
  }

  ConversionChain chain = GetConversionChain(source->get_format_id(), source->get_context(),
                                             to_type, to_contexts, is_move_conversion);
  if(!chain)
    throw ConversionException(source->get_format_name(), utils::demangle(to_type));
  auto outputs = ApplyConversionChain(chain, source, is_move_conversion);
  return std::vector<Format*>(outputs.begin()+1, outputs.end());
}

Format *Converter::Convert(Format *source, std::type_index to_type,
                           context::Context *to_context,
                           bool is_move_conversion) {
  return Convert(source, to_type, std::vector<context::Context*>({to_context}), is_move_conversion);
}

std::vector<Format *>Converter::ConvertCached(Format *source, std::type_index to_type,
                           context::Context *to_context,
                           bool is_move_conversion) {
  return ConvertCached(source, to_type, std::vector<context::Context*>({to_context}), is_move_conversion);
}

/*
std::optional<std::tuple<Converter::CostType, context::Context*>> Converter::CanDirectlyConvert(std::type_index from_type, context::Context* from_context, std::type_index to_type, const std::vector<context::Context *> &to_contexts, bool is_move_conversion){
  auto map = get_conversion_map(is_move_conversion);
  if (map->find(from_type) != map->end()) {
    if ((*map)[from_type].find(to_type) != (*map)[from_type].end()) {
      for (auto condition_function_pair : (*map)[from_type][to_type]) {
        for (auto to_context : to_contexts) {
          if (std::get<0>(condition_function_pair)(from_context, to_context)) {
            return std::make_tuple<bool, context::Context *>(
                true, std::forward<context::Context *>(to_context));
          }
        }
      }
    }
  }
  return {};
}
 */

std::vector<ConversionStep> Converter::ConversionBFS(std::type_index from_type, context::Context* from_context, std::type_index to_type, const std::vector<context::Context*>& to_contexts, ConversionMap* map){
  std::deque<std::type_index> frontier{from_type};
  std::unordered_map<std::type_index, std::pair<std::type_index, ConversionStep>> seen;
  seen.emplace(from_type, std::make_pair(from_type, std::make_tuple(nullptr, nullptr)));
  int level = 1;
  size_t level_size = 1;
  while (!frontier.empty()){
    for (int i =0; i < level_size; i++){
      auto curr = frontier.back();
      frontier.pop_back();
      for (const auto& neighbor : (*map)[curr]){
        if (seen.find(neighbor.first) == seen.end()){
          for (auto curr_to_neighbor_functions : neighbor.second){ // go over every edge curr->neighbor
            bool found_an_edge = false;
            for (auto to_context : to_contexts) { // check if, with the given contexts, this edge exists
              if (std::get<0>(curr_to_neighbor_functions)(from_context,
                                                          to_context)) {
                seen.emplace(neighbor.first, std::make_pair(curr, std::make_tuple(std::get<1>(curr_to_neighbor_functions), to_context)));
                frontier.emplace_back(neighbor.first);
                if (neighbor.first == to_type) {
                  std::vector<ConversionStep> output(level);
                  std::type_index tip = to_type;
                  for (int j = level-1; j >= 0; j--){
                    output[j] = seen.at(tip).second;
                    tip = seen.at(tip).first;
                  }
                  return output;
                }
                found_an_edge = true;
                break; // no need to check other contexts
              }
            }
            if (found_an_edge) break; // no need to check other edges between curr->neighbor
          }
        }
      }
    }
    level++;
    level_size = frontier.size();
  }
  return {};
}

ConversionChain
Converter::GetConversionChain(std::type_index from_type, context::Context *from_context,
                      std::type_index to_type,
                      const std::vector<context::Context *> &to_contexts,
                      bool is_move_conversion) {
  // If the source doesn't need conversion, return an empty but existing optional
  if (from_type == to_type && std::find(to_contexts.begin(), to_contexts.end(), from_context) != to_contexts.end())
    return ConversionChain(std::in_place);
  auto map = get_conversion_map(is_move_conversion);
  auto conversions =  ConversionBFS(from_type, from_context, to_type, to_contexts, map);
  if (!conversions.empty())
    return std::make_tuple(conversions, conversions.size());
  else
    return {};
}
bool Converter::CanConvert(std::type_index from_type,
                           context::Context *from_context,
                           std::type_index to_type,
                           context::Context *to_context,
                           bool is_move_conversion) {
  return GetConversionChain(from_type, from_context, to_type, {to_context}, is_move_conversion).has_value();
}

bool Converter::CanConvert(std::type_index from_type,
                           context::Context *from_context,
                           std::type_index to_type,
                           const std::vector<context::Context *>&to_contexts,
                           bool is_move_conversion) {
  return GetConversionChain(from_type, from_context, to_type, to_contexts, is_move_conversion).has_value();
}

  void Converter::ClearConversionFunctions(std::type_index from_type, std::type_index to_type, bool move_conversion){
    auto map = get_conversion_map(move_conversion);
    if (map->find(from_type) != map->end()){
      if ((*map)[from_type].find(to_type)!= (*map)[from_type].end()){
        (*map)[from_type].erase(to_type);
        if ((*map)[from_type].size() == 0) map->erase(from_type);
      }
    }
  }
  
  /*! Removes all conversion functions from the current converter
   */
  void Converter::ClearConversionFunctions(bool move_conversion){
    auto map = get_conversion_map(move_conversion);
    map->clear();
  }

std::vector<format::Format*> Converter::ApplyConversionChain(const ConversionChain& chain,
                                            format::Format* input, bool is_move_conversion){
  std::vector<Format*> format_chain{input};
  if (chain) {
    auto conversion_chain = std::get<0>(*chain);
    Format * current_format = input;
    for (const auto& conversion_step : conversion_chain ){
      format_chain.push_back(std::get<0>(conversion_step)(current_format, std::get<1>(conversion_step)));
      current_format = format_chain.back();
    }
  }
  return format_chain;
}

std::vector<std::vector<Format *>>
Converter::ApplyConversionSchema(const ConversionSchema & cs,
                                   const std::vector<Format *> &packed_sfs,
                                 bool is_move_conversion) {
  // Each element in ret is a chain of formats resulting from
  // converting one of the packed_sfs
  std::vector<std::vector<Format *>> ret;
  for (int i = 0; i < cs.size(); i++) {
    const auto& conversion = cs[i];
    std::vector<format::Format*> format_chain = ApplyConversionChain(conversion, packed_sfs[i], is_move_conversion);
    ret.push_back(format_chain);
  }
  return ret;
}
Converter::~Converter() {}

#if !defined(_HEADER_ONLY)
#include "init/converter.inc"
#endif

} // namespace sparsebase::utils::converter
