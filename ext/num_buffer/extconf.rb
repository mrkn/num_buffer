require 'mkmf'

$LOAD_PATH.unshift File.expand_path('../../../lib', __FILE__)
require 'num_buffer/mkmf'

create_makefile('num_buffer')
