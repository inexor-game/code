from conans import ConanFile, CMake, tools, model
import os, sys

class InexorgluegenConan(ConanFile):
    name = "InexorGlueGen"
    version = "0.3.0alpha"
    description = """This is the conan.io package for the Inexor game gluecode generator, which generates our network code (which is also our scripting binding)
                     to sync variables/classes/lists without writing extra code.
                     Since its a requirement for Inexors build, but we do not want to split it from our source repo (since it depends on some modules there),
                     we always export this package from the Inexor core conanfile.."""
    license = "ZLIB"
    url = "https://github.com/inexor-game/code/"
    settings = "os", "compiler", "build_type", "arch"
    options = {"release_only": [True, False]} # For a buildtool (an executable) the build type does not need to match (so a debug build can use a release GlueGen). But release is faster.
                                              # We default to False though, since otherwise you have the dependencies list doubled (since the deps of GlueGen are those of Inexor itself)
    default_options = '''release_only=False'''
    generators = "cmake"
    # exporting ../.. is really slow and just works for some stuff..
                      # so as a workaround we (this conanfile.py) get copied in a temporary folder above the Inexor conanfile.py to export the contents..
    exports = "inexor*", "cmake*.cmake", "CMakeLists.txt", "dependencies.py"

    def config(self):
        if self.options.release_only:
            self.settings.build_type.replace("Debug", "Release")

        # append the filepath to this conanfile to your pythonpath so we can import files next to it.
        dir_path = os.path.dirname(os.path.realpath(__file__))
        sys.path.append(dir_path)

        # Get the dependency list from github
#        url = "https://raw.githubusercontent.com/inexor-game/code/{}/dependencies.py".format(self.options.commit)
#        filename = "{}/dependencies.py".format(dir_path)
#        self.output.info("Downloading dependency list from inexor-game/code/ repo: {}".format(url))
#        if os.path.exists(filename):
#            os.remove(filename) # We need to remove this file first, since otherwise we always append..
#        tools.download(url, filename)
        from dependencies import requires, options
        self.requires.__init__(*requires)
        self.default_options = '''{}
{}'''.format(self.default_options, options)

    def source(self):
        self.run("git clone https://github.com/inexor-game/code/ && cd code && git checkout {}".format(self.options.commit))
        # This is a very dirty workaround: Although not needed all modules get always built. we'll need a different solution for that.
        cmake_file = "code/inexor/CMakeLists.txt"
        tools.replace_in_file(cmake_file, "add_subdirectory(", "#add_subdirectory(")
        tools.replace_in_file(cmake_file, "#add_subdirectory(texture)", '''#add_subdirectory(texture)
add_subdirectory(util)
add_subdirectory(filesystem)''') # Just readd module_filesystem and module_util again..

    def build(self):
        cmake = CMake(self.settings)
        args = ["-DBUILD_CLIENT=OFF", "-DBUILD_MASTER=OFF", "-DBUILD_TEST=OFF", "-DBUILD_SERVER=OFF", "-DBUILD_GLUEGEN=ON"]
        # args += ["-DBUILD_SHARED_LIBS={}".format('ON' if self.options.shared else 'OFF')]
        self.run('cmake code {} {}'.format(cmake.command_line, " ".join(args)))
        self.run("cmake --build . {}".format(cmake.build_config))

    def package(self):
        self.copy("*gluecodegenerato*", dst="bin", src="bin", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)

