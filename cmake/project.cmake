cmake_minimum_required(VERSION 3.9.4)

function(lsystem_configure_target target)
  # target_compile_features(${target} PUBLIC cxx_std_11)
  set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)

  target_compile_options(${target} PRIVATE
    -Wuninitialized
    -Wmaybe-uninitialized
    -Wreorder
    -Wpedantic
    -Wmisleading-indentation
    -Wmissing-include-dirs
    -Wunused
    -Wsuggest-override
    -Wbool-compare
    -Wbool-operation
    -Wduplicated-branches
    -Wduplicated-cond
    -Wtautological-compare
    -Wcast-qual
    -Wcast-align
    -Wcast-function-type
    -Wconversion
    -Wuseless-cast
    -Wempty-body
    -Wextra-semi
    -Wsign-compare
    -Wsign-conversion
    -Wfloat-conversion)

endfunction()
