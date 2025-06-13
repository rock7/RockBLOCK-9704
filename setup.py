import os
import pathlib
import subprocess
import shutil
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.dirname(os.path.realpath(__file__))

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, ext):
        config = "Debug" if self.debug else "Release"
        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        cmake_args = [
            f"-DCMAKE_BUILD_TYPE={config}",
            f"-DBUILD_PYTHON_SHARED_LIBRARY=ON",
            f"-DDEBUG=ON"
        ]

        build_args = ["--config", config]

        # Configure with CMake
        subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=build_temp)

        # Build shared lib aka rockblock
        subprocess.check_call(["cmake", "--build", ".", "--target", "rockblock"] + build_args, cwd=build_temp)

        # Determine file extension
        if sys.platform == "win32":
            built_so_path = os.path.join(build_temp, f"{config}", "rockblock.dll")
        elif sys.platform == "darwin":
            built_so_path = os.path.join(build_temp, "librockblock.dylib")
        else:
            built_so_path = os.path.join(build_temp, "librockblock.so")

        # Final path where Python expects the dynamic module
        target_pyd_path = pathlib.Path(self.get_ext_fullpath(ext.name))
        shutil.copyfile(built_so_path, target_pyd_path)

setup(
    packages=["rockblock9704"],
    ext_modules=[CMakeExtension("rockblock")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
)
