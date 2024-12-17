project('FourierIA', 'cpp', default_options:['cpp_std=c++17'])
add_global_arguments(['-std=c++17', '-pthread'], language: 'cpp')
subdir('src')
sndfile_sub = subproject('libsndfile')
sndfile_inc = sndfile_sub.get_variable('incdirs')
sndfile_dep = sndfile_sub.get_variable('libsndfile_dep')

pthread_dep = dependency('threads')

executable('IA', sources, include_directories: ['src', 'Matplot++-1.2.0-Linux/include', sndfile_inc], dependencies : [sndfile_dep, pthread_dep])