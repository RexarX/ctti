from conan import ConanFile

class Ctti(ConanFile):
    name = "ctti"
    url = "https://github.com/RexarX/ctti"
    description = "Compile Time Type Information for C++23"
    license = "MIT"
    version = "1.0.0"
    homepage = "https://github.com/RexarX/ctti"

    # No source method needed for header-only libraries
    exports_sources = "include/*"
    no_copy_source = True

    # Modern build system settings
    settings = "compiler"

    def validate(self):
        if self.settings.compiler.cppstd:
            if int(str(self.settings.compiler.cppstd).replace("gnu", "")) < 23:
                raise ConanInvalidConfiguration(f"{self.name} requires C++23 support")

    def package(self):
        self.copy("*.hpp", dst="include", src="include")

    def package_id(self):
        self.info.clear()  # Header-only libraries don't influence the package ID
