require 'mkmf'

class NumBuffer
  TOP_DIR = File.expand_path('../../..', __FILE__)
  INCLUDE_DIR = File.join(TOP_DIR, 'include')
  append_cppflags("-I#{INCLUDE_DIR}")
end
