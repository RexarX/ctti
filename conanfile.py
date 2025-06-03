from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
import os

class Ctti(ConanFile):
    name = "ctti"
    version = "1.0.0"
    url = "https://github.com/RexarX/ctti"
    description = "Compile Time Type Information for C++23"
    license = "MIT"
    homepage = "https://github.com/RexarX/ctti"
    topics = ("cpp23", "reflection", "metaprogramming", "header-only")

    # Header-only library settings
    package_type = "header-library"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True

    # Build options
    options = {
        "build_tests": [True, False],
        "build_examples": [True, False]
    }
    default_options = {
        "build_tests": False,
        "build_examples": False
    }

    exports_sources = "CMakeLists.txt", "cmake/*", "include/*", "tests/*", "examples/*"

    def configure(self):
        # Header-only library doesn't need these
        self.settings.rm_safe("build_type")

    def layout(self):
        cmake_layout(self)

    def validate(self):
        check_min_cppstd(self, "23")

    def build_requirements(self):
        if self.options.build_tests:
            self.test_requires("catch2/[>=3.0.0]")

    def generate(self):
        if self.options.build_tests or self.options.build_examples:
            deps = CMakeDeps(self)
            deps.generate()
            tc = CMakeToolchain(self)
            tc.variables["CTTI_BUILD_TESTS"] = self.options.build_tests
            tc.variables["CTTI_BUILD_EXAMPLES"] = self.options.build_examples
            tc.variables["CTTI_INSTALL"] = False  # Don't install when building tests
            tc.generate()

    def build(self):
        if self.options.build_tests or self.options.build_examples:
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

    def test(self):
        if self.options.build_tests:
            cmake = CMake(self)
            cmake.test()

    def package(self):
        copy(self, "*.hpp",
             dst=os.path.join(self.package_folder, "include"),
             src=os.path.join(self.source_folder, "include"))
        copy(self, "LICENSE*",
             dst=os.path.join(self.package_folder, "licenses"),
             src=self.source_folder)

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.cppstd = "23"
        self.cpp_info.includedirs = ["include"]

    def package_id(self):
        self.info.clear()  # Header-only: no dependencies on settings/options
