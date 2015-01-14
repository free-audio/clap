static inline bool
clap_plugin_params_save(struct clap_plugin *plugin,
                        uint8_t            *buffer,
                        uint32_t           *size)
{
  struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
  if (!params) {
    *size = 0;
    return true;
  }

  struct clap_serializer s;
  s.out     = buffer;
  s.out_end = buffer + *size;

  if (!clap_serializer_dict(&s))
    return false;

  uint32_t count = params->count(plugin);
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_param param;

    if (!params->get(plugin, i, &param))
      continue;

    if (param.type == CLAP_PARAM_GROUP)
      continue;

    clap_serializer_str(&s, param.id, strlen(param.id));
    switch (param.type) {
    case CLAP_PARAM_BOOL:
      if (!clap_serializer_bool(&s, param.value.b))
        return false;
      break;

    case CLAP_PARAM_INT:
    case CLAP_PARAM_ENUM:
      if (!clap_serializer_int32(&s, param.value.i))
        return false;
      break;

    case CLAP_PARAM_FLOAT:
      if (!clap_serializer_float(&s, param.value.f))
        return false;
      break;

    default:
      return false;
    }
  }

  if (!clap_serializer_end(&s))
    return false;

  *size = s.out - buffer;
  return true;
}

static inline void
clap_plugin_params_restore(struct clap_plugin *plugin,
                           const uint8_t      *buffer,
                           uint32_t            size,
                           uint64_t            steady_time)
{
  struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
  if (!params)
    return;

  // allocate the ids
  uint32_t count = params->count(plugin);
  char     ids[count][CLAP_ID_SIZE];
  memset(ids, 0, sizeof (ids));

  // fill ids
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_param param;
    if (params->get(plugin, i, &param))
      memcpy(ids[i], param.id, sizeof (param.id));
  }

  struct clap_deserializer d;
  d.in     = buffer;
  d.in_end = buffer + size;

  if (!clap_deserialize(&d) || d.type != CLAP_SERIALIZE_DICT)
    return;

  while (true) {
    // get param id
    if (!clap_deserialize(&d) || d.type != CLAP_SERIALIZE_STR)
      return;

    // find param index
    uint32_t index;
    for (index = 0; index < count; ++index)
      if (!strncmp(ids[index], d.s, d.slen))
        break;

    if (index == count) {
      // index not found, pull value and continue
      clap_deserialize(&d);
      continue;
    }

    struct clap_event ev;
    ev.next        = NULL;
    ev.type        = CLAP_EVENT_PARAM_SET;
    ev.steady_time = steady_time;
    ev.param.is_global = true;
    ev.param.key = 0;
    ev.param.index = index;

    struct clap_process process;
    process.inputs        = NULL;
    process.outputs       = NULL;
    process.samples_count = 0;
    process.steady_time   = steady_time;
    process.events        = &ev;

    switch (d.type) {
    case CLAP_SERIALIZE_BOOL:
      ev.param.value.b = d.b;
      break;

    case CLAP_SERIALIZE_INT32:
      ev.param.value.i = d.i;
      break;

    case CLAP_SERIALIZE_FLOAT:
      ev.param.value.f = d.f;
      break;

    default:
      return;
    }

    plugin->process(plugin, &process);
  }

  return;
}
