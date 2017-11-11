from setuptools import setup, Extension
import subprocess


linker_args = subprocess.check_output(
    'pkg-config --static --libs glfw3'.split(' '))
linker_args = linker_args.decode().strip().split(' ')
linker_args += "-lEGL -lGLESv2".split(' ')

module = Extension('dingbat',
                   sources = [
                       'src/dingbat.cpp',
                       'src/util.cpp',
                       'src/shaders.cpp',
                       'src/buffers.cpp',
                       'src/painter.cpp'],
                   extra_compile_args = ['-std=c++14'],
                   extra_link_args = linker_args)


setup(name = 'dingbat',
      version = '0.0',
      description = '',
      ext_modules = [module])
