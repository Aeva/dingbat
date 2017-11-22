from setuptools import setup, Extension
import subprocess
import glob


linker_args = subprocess.check_output(
    'pkg-config --static --libs glfw3'.split(' '))
linker_args = linker_args.decode().strip().split(' ')
linker_args += "-lEGL -lGLESv2".split(' ')

module = Extension(
    'dingbat',
    sources = glob.glob('src/*.cpp'),
    extra_compile_args = [
        '-std=c++14',
        '-I/usr/include/glm',
    ],
    extra_link_args = linker_args)


setup(name = 'dingbat',
      version = '0.0',
      description = '',
      ext_modules = [module])
