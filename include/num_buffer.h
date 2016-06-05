#ifndef NUM_BUFFER_H
#define NUM_BUFFER_H 1

#ifdef __cplusplus
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

enum nb_value_type {
  NB_VALUE_UNKNOWN,
  NB_VALUE_INT8,
  NB_VALUE_SINT8 = NB_VALUE_INT8,
  NB_VALUE_UINT8,
  NB_VALUE_INT16,
  NB_VALUE_SINT16 = NB_VALUE_INT16,
  NB_VALUE_UINT16,
  NB_VALUE_INT32,
  NB_VALUE_SINT32 = NB_VALUE_INT32,
  NB_VALUE_UINT32,
  NB_VALUE_INT64,
  NB_VALUE_SINT64 = NB_VALUE_INT64,
  NB_VALUE_UINT64,
  NB_VALUE_FLOAT16,
  NB_VALUE_FLOAT32,
  NB_VALUE_FLOAT64,
  NB_VALUE_COMPLEX16,
  NB_VALUE_COMPLEX32,
  NB_VALUE_COMPLEX64
};

typedef enum nb_value_type nb_value_type_t;

static inline size_t
nb_value_type_size(nb_value_type_t value_type)
{
  switch (value_type) {
    case NB_VALUE_UNKNOWN:
      return 0;

    case NB_VALUE_INT8:
    case NB_VALUE_UINT8:
      return 1;

    case NB_VALUE_INT16:
    case NB_VALUE_UINT16:
    case NB_VALUE_FLOAT16:
      return 2;

    case NB_VALUE_INT32:
    case NB_VALUE_UINT32:
    case NB_VALUE_FLOAT32:
    case NB_VALUE_COMPLEX16:
      return 4;

    case NB_VALUE_INT64:
    case NB_VALUE_UINT64:
    case NB_VALUE_FLOAT64:
    case NB_VALUE_COMPLEX32:
      return 8;

    case NB_VALUE_COMPLEX64:
      return 16;

    default:
      UNREACHABLE;
  }
}

static inline char const *
nb_value_type_name(nb_value_type_t value_type)
{
  switch (value_type) {
    case NB_VALUE_UNKNOWN:
      return "UNKNOWN";

    case NB_VALUE_INT8:
      return "INT8";

    case NB_VALUE_UINT8:
      return "UINT8";

    case NB_VALUE_INT16:
      return "INT16";

    case NB_VALUE_UINT16:
      return "UINT16";

    case NB_VALUE_FLOAT16:
      return "FLOAT16";

    case NB_VALUE_INT32:
      return "INT32";

    case NB_VALUE_UINT32:
      return "UINT32";

    case NB_VALUE_FLOAT32:
      return "FLOAT32";

    case NB_VALUE_COMPLEX16:
      return "COMPLEX16";

    case NB_VALUE_INT64:
      return "INT64";

    case NB_VALUE_UINT64:
      return "UINT64";

    case NB_VALUE_FLOAT64:
      return "FLOAT64";

    case NB_VALUE_COMPLEX32:
      return "COMPLEX32";

    case NB_VALUE_COMPLEX64:
      return "COMPLEX64";

    default:
      UNREACHABLE;
  }
}

struct nb_num_buffer {
  VALUE flags;
  VALUE origin;

  nb_value_type_t value_type;

  size_t dim;
  size_t *shape;
  size_t *strides;

  size_t data_size;
  struct {
    union {
      void *void_ptr;
      int8_t *i8_ptr;
      uint8_t *u8_ptr;
      int16_t *i16_ptr;
      uint16_t *u16_ptr;
      int32_t *i32_ptr;
      uint32_t *u32_ptr;
      int64_t *i64_ptr;
      uint64_t *u64_ptr;
      float *f32_ptr;
      double *f64_ptr;
    } as;
  } data;
};

typedef struct nb_num_buffer nb_num_buffer_t;

RUBY_EXTERN nb_num_buffer_t *nb_get_num_buffer(VALUE obj);

RUBY_EXTERN VALUE nb_num_buffer_new_with_data(VALUE origin, nb_value_type_t value_type, size_t dim, size_t const *shape, size_t const *strides, void* data, size_t data_size);
RUBY_EXTERN VALUE nb_num_buffer_new(nb_value_type_t value_type, size_t dim, size_t const *shape, size_t const *strides);
RUBY_EXTERN VALUE nb_num_buffer_inspect(VALUE obj);

RUBY_EXTERN VALUE nb_cNumBuffer;
RUBY_EXTERN rb_data_type_t const nb_num_buffer_data_type;

#ifdef __cplusplus
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* NUM_BUFFER_H */
