from setuptools import setup, Extension


VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = '.'.join(map(str, [VERSION_MAJOR,
                             VERSION_MINOR,
                             VERSION_PATCH]))


cvarda = Extension('cvarda',
                   sources = ['wrapper.c',
                              '../src/alloc.c',
                              '../src/avl_tree.c',
                              '../src/itv_tree.c',
                              '../src/mnv_index.c',
                              '../src/mnv_table.c',
                              '../src/region_index.c',
                              '../src/region_table.c',
                              '../src/snv_index.c',
                              '../src/snv_table.c',
                              '../src/trie.c'],
                   define_macros = [('VRD_VERSION_MAJOR', VERSION_MAJOR),
                                    ('VRD_VERSION_MINOR', VERSION_MINOR),
                                    ('VRD_VERSION_PATCH', VERSION_PATCH)],
                   extra_compile_args = ['-Wextra',
                                         '-Wpedantic',
                                         '-std=c99'])


setup(name = 'cvarda',
      version = VERSION,
      description = 'Varda2 Variant frequency database C library',
      ext_modules = [cvarda])
