project('list-caps', 'cpp', version: '1.0.0', default_options : ['warning_level=3', 'werror=false', 'cpp_std=c++20'])
add_project_arguments('-Wfatal-errors', language: 'cpp')
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')

dep = dependency('gstreamer-1.0', version: '>=1.20', required: true)

common_files = files (
    'src/gstutil/pipeline-helper.cpp'
)

list_caps_files = files(
    'src/list-caps.cpp'
) + common_files

executable('list-caps', list_caps_files, dependencies : dep)
