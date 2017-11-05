from setuptools import setup, Extension
import subprocess


linker_args = subprocess.check_output(
    'pkg-config --static --libs glfw3'.split(' '))
linker_args = linker_args.decode().strip().split(' ')


module = Extension('glfw_test',
                   sources = ['src/test.cpp'],
                   extra_compile_args = ['-std=c++11'],
                   extra_link_args = linker_args)


setup(name = 'GLFW build test',
      version = '0.0',
      description = '',
      ext_modules = [module])
