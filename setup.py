import os
import subprocess
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
        ]

        build_temp = os.path.join(self.build_temp, ext.name)
        os.makedirs(build_temp, exist_ok=True)

        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=build_temp
        )
        subprocess.check_call(
            ["cmake", "--build", "."], cwd=build_temp
        )
        # After building, rename the file to what setuptools expects
        built_so = os.path.join(extdir, "libglider_bindings.so")
        target_so = os.path.join(extdir, "glider.cpython-310-x86_64-linux-gnu.so")
        if os.path.exists(built_so):
            os.rename(built_so, target_so)

setup(
    name="glider-bindings",
    version="0.1.0",
    ext_modules=[CMakeExtension("glider_bindings")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
)