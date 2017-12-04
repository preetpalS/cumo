require 'rbconfig.rb'
require 'mkmf'
require "erb"

if RUBY_VERSION < "2.0.0"
  puts "Numo::NArray requires Ruby version 2.0 or later."
  exit(1)
end

rm_f 'include/numo/extconf.h'

#$CFLAGS="-g3 -O0 -Wall"
#$CFLAGS=" $(cflags) -O3 -m64 -msse2 -funroll-loops"
#$CFLAGS=" $(cflags) -O3"
$INCFLAGS = "-Iinclude -Inarray #$INCFLAGS"

$INSTALLFILES = Dir.glob(%w[include/numo/*.h include/numo/types/*.h include/numo/cuda/*.h]).map{|x| [x,'$(archdir)'] }
$INSTALLFILES << ['include/numo/extconf.h','$(archdir)']
if /cygwin|mingw/ =~ RUBY_PLATFORM
  $INSTALLFILES << ['libnumo.a', '$(archdir)']
end

srcs = %w(
numo
narray/narray
narray/array
narray/step
narray/index
narray/ndloop
narray/data
narray/types/bit
narray/types/int8
narray/types/int16
narray/types/int32
narray/types/int64
narray/types/uint8
narray/types/uint16
narray/types/uint32
narray/types/uint64
narray/types/sfloat
narray/types/dfloat
narray/types/scomplex
narray/types/dcomplex
narray/types/robject
narray/math
narray/SFMT
narray/struct
narray/rand
cuda/runtime
cuda/nvrtc
)

if RUBY_VERSION[0..3] == "2.1."
  puts "add kwargs"
  srcs << "kwargs"
end

if have_header("stdbool.h")
  stdbool = "stdbool.h"
else
  stdbool = nil
end

if have_header("stdint.h")
  stdint = "stdint.h"
elsif have_header("sys/types.h")
  stdint = "sys/types.h"
else
  stdint = nil
end

have_type("bool", stdbool)
unless have_type("u_int8_t", stdint)
  have_type("uint8_t",stdint)
end
unless have_type("u_int16_t", stdint)
  have_type("uint16_t",stdint)
end
have_type("int32_t", stdint)
unless have_type("u_int32_t", stdint)
  have_type("uint32_t",stdint)
end
have_type("int64_t", stdint)
unless have_type("u_int64_t", stdint)
  have_type("uint64_t", stdint)
end
have_func("exp10")

have_var("rb_cComplex")

$objs = srcs.collect{|i| i+".o"}

create_header('include/numo/extconf.h')

depend_path = File.join(__dir__, "depend")
File.open(depend_path, "w") do |depend|
  depend_erb_path = File.join(__dir__, "depend.erb")
  File.open(depend_erb_path, "r") do |depend_erb|
    erb = ERB.new(depend_erb.read)
    erb.filename = depend_erb_path
    depend.print(erb.result)
  end
end

HEADER_DIRS = (ENV['CPATH'] || '').split(':')
LIB_DIRS = (ENV['LIBRARY_PATH'] || '').split(':')
dir_config('numo', HEADER_DIRS, LIB_DIRS)

have_library('cuda')
have_library('cudart')
have_library('nvrtc')
# have_library('cublas')
# have_library('cusolver')
# have_library('curand')

create_makefile('numo')
