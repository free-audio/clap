#pragma once

#include "private/std.h"
#include "private/macros.h"

static CLAP_CONSTEXPR const char CLAP_PROPERTY_BOOL[] = "clap:bool";
static CLAP_CONSTEXPR const char CLAP_PROPERTY_INT64[] = "clap:int64";
static CLAP_CONSTEXPR const char CLAP_PROPERTY_DOUBLE[] = "clap:double";
static CLAP_CONSTEXPR const char CLAP_PROPERTY_STRING[] = "clap:string";
static CLAP_CONSTEXPR const char CLAP_PROPERTY_COLOR[] = "clap:color";

typedef struct clap_properties {
   void *ctx; // reserved pointer for the object

   // Gets a property value
   // [in]  prop_id: the property identifier
   // [in]  prop_type: the property type, see CLAP_PROPERTY_BOOL, ...
   // [in]  value: pointer to memory to store the value
   // [in]  value_capacity: amount of memory in bytes which can be used to store the value
   // [out] value_size: the effective value size in bytes, necessary to know if the capacity was enought
   // returns true on success
   bool(CLAP_ABI *get)(const struct clap_properties *props,
                       const char                   *prop_id,
                       const char                   *prop_type,
                       void                         *value,
                       uint32_t                      value_capacity,
                       uint32_t                     *value_size);

   // Sets a property value
   // [in] prop_id: the property identifier
   // [in] prop_type: the property type, see CLAP_PROPERTY_BOOL, ...
   // [in] value: pointer to memory to store the value
   // [in] value_capacity: amount of memory which can be used to store the value
   // [in] value_size: the value size in bytes
   // returns true on success
   bool(CLAP_ABI *set)(const struct clap_properties *props,
                       const char                   *prop_id,
                       const char                   *prop_type,
                       void                         *value,
                       uint32_t                      value_size);
} clap_properties_t;
