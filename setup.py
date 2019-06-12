import os
import sys
import setuptools
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.file_util import copy_file
import pybind11

__version__ = os.environ.get("CI_COMMIT_TAG", '0.0.1')
DEBUG = os.environ.get("DEBUG", "False").lower() == 'true'


def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    """
    if has_flag(compiler, '-std=c++17'):
        return '-std=c++17'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    else:
        raise RuntimeError('Unsupported compiler -- at least C++11 support '
                           'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': ['-march=native'],
    }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        if hasattr(self.compiler, "compiler_so"
                  ) and '-Wstrict-prototypes' in self.compiler.compiler_so:
            self.compiler.compiler_so.remove('-Wstrict-prototypes')
            if not DEBUG:
                self.compiler.compiler_so.remove('-g')
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            #if has_flag(self.compiler, '-fvisibility=hidden'):
            #    opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append(
                '/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
            opts.append('/DWIN32')
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


ext_modules = [
    Extension(
        'sjson',
        [
            'sjson/sjson.cpp',
            #'src/simdjson.cpp',
        ],
        include_dirs=[
            # Path to pybind11 headers
            pybind11.get_include(),
            pybind11.get_include(user=True),
            'singleheader/',
        ],
        library_dirs=[
            "src",
            #  '{}/lib/{}'.format(solclient_path(), solclient_lib_path()),
            #"C:\\Users\\011404\\yvictor\\pybind\\libs\\solclient-win-7.7.1.4\\lib\\Win64\\",
        ],
        #libraries=[
        #    solclient_lib_name(),
        #],  #, 'stdc++', 'm'],
        #extra_compile_args = ['-D_GNU_SOURCE'],
        #extra_link_args=['-lstdc++', '-lm'],
        language='c++'),
]

requirements = []
dev_requirements = ['pybind11>=2.2']

setup(
    name='pysolace',
    version=__version__,
    author='YVictor, TKHuang',
    author_email='yvictor3141@gmail.com, tkhuangs@gmail.com',
    url='https://github.com/Yvictor/pybind_solace',
    description='python solclient',
    long_description='',
    packages=find_packages(exclude=['tests*']),
    #package_data={'pysolace': gen_package_data_list()},
    ext_modules=ext_modules,
    install_requires=requirements,
    extras_require={
        'dev': dev_requirements,
    },
    cmdclass={
        'build_ext': BuildExt,
    },
    zip_safe=False,
)