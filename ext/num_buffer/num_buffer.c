#include <ruby/ruby.h>
#include <assert.h>

#include "num_buffer.h"

VALUE nb_cNumBuffer;

static void
nb_num_buffer_mark(void *ptr)
{
  nb_num_buffer_t *nb = (nb_num_buffer_t *) ptr;
  rb_gc_mark(nb->origin);
}

static void
nb_num_buffer_free(void *ptr)
{
  nb_num_buffer_t *nb = (nb_num_buffer_t *) ptr;
}

static size_t
nb_num_buffer_memsize(void const *ptr)
{
  return 0;
}

rb_data_type_t const nb_num_buffer_data_type = {
  "num_buffer",
  {
    nb_num_buffer_mark,
    nb_num_buffer_free,
    nb_num_buffer_memsize,
  },
#ifdef RUBY_TYPED_FREE_IMMEDIATELY
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY
#endif
};

static VALUE
nb_num_buffer_alloc(VALUE klass)
{
  nb_num_buffer_t *nb;
  VALUE obj = TypedData_Make_Struct(klass, nb_num_buffer_t, &nb_num_buffer_data_type, nb);
  nb->origin = Qnil;
  nb->value_type = NB_VALUE_UNKNOWN;
  nb->dim = 0;
  nb->shape = NULL;
  nb->strides = NULL;
  nb->data_size = 0;
  nb->data.as.void_ptr = NULL;
  return obj;
}

static inline nb_num_buffer_t *
get_num_buffer(VALUE obj)
{
  nb_num_buffer_t *nb;
  TypedData_Get_Struct(obj, nb_num_buffer_t, &nb_num_buffer_data_type, nb);
  return nb;
}

nb_num_buffer_t *
nb_get_num_buffer(VALUE obj)
{
  return get_num_buffer(obj);
}

static inline size_t
nb_calculate_data_size(nb_value_type_t value_type, size_t dim, size_t const *shape, size_t const *strides)
{
  size_t i, size;

  assert(value_type != NB_VALUE_UNKNOWN);
  assert(strides != NULL);
  assert(dim > 0);

  size_t const value_size = nb_value_type_size(value_type);
  size = value_size;
  for (i = 0; i < dim - 1; ++i) {
    size *= strides[i];
  }
  size *= shape[0];

  return size;
}

static VALUE
nb_num_buffer_new_raw(VALUE origin, nb_value_type_t value_type, size_t dim, size_t *shape, size_t *strides, void* data, size_t data_size)
{
  assert(value_type != NB_VALUE_UNKNOWN);
  assert(dim > 0);
  assert(shape != NULL);
  assert(strides != NULL);
  assert(data != NULL);
  assert(data_size > 0);
  assert(data_size >= nb_calculate_data_size(value_type, dim, shape, strides));

  VALUE obj = nb_num_buffer_alloc(nb_cNumBuffer);
  nb_num_buffer_t *nb = get_num_buffer(obj);

  nb->origin = origin;
  nb->value_type = value_type;
  nb->dim = dim;
  nb->shape = shape;
  nb->strides = strides;
  nb->data_size = data_size;
  nb->data.as.void_ptr = data;

  return obj;
}

VALUE
nb_num_buffer_new_with_data(VALUE origin, nb_value_type_t value_type, size_t dim, size_t const *shape, size_t const *strides, void* data, size_t data_size)
{
  assert(value_type != NB_VALUE_UNKNOWN);
  assert(dim > 0);
  assert(shape != NULL);
  assert(strides != NULL);
  assert(data != NULL);
  assert(data_size > 0);
  assert(data_size >= nb_calculate_data_size(value_type, dim, shape, strides));

  size_t *shape_copied = ALLOC_N(size_t, dim);
  MEMCPY(shape_copied, shape, size_t, dim);

  size_t *strides_copied = ALLOC_N(size_t, dim - 1);
  MEMCPY(strides_copied, strides, size_t, dim - 1);

  return nb_num_buffer_new_raw(origin, value_type, dim, shape_copied, strides_copied, data, data_size);
}

VALUE
nb_num_buffer_new(nb_value_type_t value_type, size_t dim, size_t const *shape, size_t const *strides)
{
  assert(value_type != NB_VALUE_UNKNOWN);
  assert(dim > 0);
  assert(shape != NULL);

  size_t *shape_copied = ALLOC_N(size_t, dim);
  MEMCPY(shape_copied, shape, size_t, dim);

  size_t *strides_copied = ALLOC_N(size_t, dim - 1);
  if (strides != NULL) {
    MEMCPY(strides_copied, strides, size_t, dim - 1);
  }
  else {
    size_t i;
    for (i = 0; i < dim - 1; ++i) {
      strides_copied[i] = shape[i];
    }
  }

  size_t data_size = nb_calculate_data_size(value_type, dim, shape_copied, strides_copied);
  void *data = ALLOC_N(uint8_t, data_size);

  return nb_num_buffer_new_raw(Qnil, value_type, dim, shape_copied, strides_copied, data, data_size);
}

VALUE
nb_num_buffer_inspect(VALUE obj)
{
  nb_num_buffer_t *nb = get_num_buffer(obj);

  VALUE str;
  VALUE cname = rb_class_name(CLASS_OF(obj));
  str = rb_sprintf("#<%"PRIsVALUE":%p dim=%"PRIuSIZE" shape=(", cname, (void*)obj, nb->dim);

  char buf[64];
  snprintf(buf, sizeof(buf), "%"PRIuSIZE, nb->shape[0]);
  rb_str_cat2(str, buf);

  size_t i;
  for (i = 1; i < nb->dim; ++i) {
    snprintf(buf, sizeof(buf), ", %"PRIuSIZE, nb->shape[i]);
    rb_str_cat2(str, buf);
  }
  rb_str_cat2(str, "), strides=(");

  snprintf(buf, sizeof(buf), "%"PRIuSIZE, nb->strides[0]);
  rb_str_cat2(str, buf);

  for (i = 1; i < nb->dim - 1; ++i) {
    snprintf(buf, sizeof(buf), ", %"PRIuSIZE, nb->strides[i]);
    rb_str_cat2(str, buf);
  }
  rb_str_cat2(str, "), value_type=");

  snprintf(buf, sizeof(buf), "%s", nb_value_type_name(nb->value_type));
  rb_str_cat2(str, buf);

  rb_str_cat2(str, ", data_size=");

  snprintf(buf, sizeof(buf), "%"PRIuSIZE, nb->data_size);
  rb_str_cat2(str, buf);

  rb_str_cat2(str, ">");
  rb_obj_infect(str, obj);

  return str;
}

static VALUE
nb_num_buffer_get_shape(VALUE obj)
{
  nb_num_buffer_t *nb = get_num_buffer(obj);

  VALUE ary = rb_ary_new_capa(nb->dim);
  size_t i;
  for (i = 0; i < nb->dim; ++i)
    rb_ary_push(ary, SIZET2NUM(nb->shape[i]));

  return ary;
}

void
Init_num_buffer(void)
{
  nb_cNumBuffer = rb_define_class("NumBuffer", rb_cObject);
  rb_define_alloc_func(nb_cNumBuffer, nb_num_buffer_alloc);
  rb_define_method(nb_cNumBuffer, "inspect", nb_num_buffer_inspect, 0);
  rb_define_method(nb_cNumBuffer, "shape", nb_num_buffer_get_shape, 0);
}
