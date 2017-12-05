from setuptools import setup, Extension
import subprocess
import glob


USE_ES3 = False


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


module = Extension(
    'dingbat',
    sources = glob.glob('src/*.cpp'),
    extra_compile_args = [
        '-std=c++14',
        '-I/usr/include/glm',
        # '-O0',
        # '-g',
    ],
    extra_link_args = linker_args,
    define_macros = defines)


setup(name = 'dingbat',
      version = '0.0',
      description = '',
      ext_modules = [module])
