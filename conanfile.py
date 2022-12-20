
from conans import ConanFile, CMake, tools


class EnttSnapshot(ConanFile):
    name = "entt_snapshot"
    version = "0.1"
    author = "Leon Fretter"
    description = "Snapshot-implementation using the entt-library"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "only_lib": [True, False]}
    default_options = {"shared": True, "only_lib": True}
    generators = "cmake", "VSCodeProperties"
    requires = ["code_cpp_props/0.1", "cereal/1.3.1", "entt/3.10.3"]
    exports_sources = "include*", "src*", "CMakeLists.txt"

    def build(self):
        cmake = CMake(self)
        cmake.definitions["only_lib"] = self.options.only_lib
        cmake.configure(source_folder="")
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include", keep_path=True)
        self.copy("*.so", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["entt_snapshot"]
        self.cpp_info.includedirs = ["include"]
