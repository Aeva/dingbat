from setuptools import setup, Extension
import subprocess
import glob


USE_ES3 = False
USE_DEBUGGING = False


linker_args = subprocess.check_output(
    'pkg-config --static --libs glfw3'.split(' '))
linker_args = linker_args.decode().strip().split(' ')
defines = []


if USE_ES3:
    linker_args += "-lEGL -lGLESv2".split(' ')
    defines.append(("USING_GL_ES_3", None))
    defines.append(("GLFW_INCLUDE_ES31", None))
else:
    linker_args += "-lGL -lGLEW".split(' ')
    defines.append(("USING_GL_4_2", None))

compiler_args = [
    '-std=c++14',
    '-I/usr/include/glm',
]

if USE_DEBUGGING:
    compiler_args += [
        '-O0',
        '-g',
    ]
    defines.append(("DEBUG_BUILD", None))

module = Extension(
    'dingbat.backend',
    sources = glob.glob('src/*.cpp'),
    extra_compile_args = compiler_args,
    extra_link_args = linker_args,
    define_macros = defines)


setup(name = 'dingbat',
      version = '0.0',
      description = '',
      packages = ['dingbat'],
      ext_modules = [module])
