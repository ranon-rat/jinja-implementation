#https://github.com/ranon-rat/moonmake
import moonmake as mmake
import platform
import sys

dir_path = mmake.get_dir(__file__)
def join (*dir, separator="/"): 
    return f"{separator}".join(dir).replace(r"\\","/")

def install():
    pass
def execute():
    #Configures and executes the build process.#
    # Directories and configuration
    MOONMAKE_DIR = ".moonmake"
    PROJECT_NAME = "msrc"
    CPP_VERSION = "2b"
    EXTENSION = mmake.get_extension()
    
    # Important paths
    include_paths = [
        join(".", MOONMAKE_DIR, "dependencies", "headers"),
        join(".", dir_path, "src", "include")
    ]
    
    lib_paths = [
        join(MOONMAKE_DIR, "dependencies", "lib"),
        join( MOONMAKE_DIR, "lib")
    ]
    
 
    # Static libraries
    static_a_files = mmake.discover(join(".", MOONMAKE_DIR, "dependencies", "lib"), ".a")
    static_libs = [f"-l{mmake.strip_lib_prefix(a).replace('.a', '')}" for a in static_a_files]
    
    # Compilation flags
    INCLUDE_FLAGS = mmake.join_with_flag(include_paths, "-I")
    LINK_FLAGS = mmake.join_with_flag(lib_paths, "-L")
    STATIC_LIBRARY = " ".join(static_libs)
    COMPILER_FLAGS = f"-Wall -Wextra -std=c++{CPP_VERSION} -Werror -O2"
    IGNORE_FLAGS = "-Wno-unused-parameter -Wno-return-type"  
    OBJ_FLAGS= "-MMD -MP"
    # Files to watch for changes
    static_watch_files = [join(MOONMAKE_DIR, "dependencies", "lib", a) for a in static_a_files]
    
    # Library files
    lib_files = [f for f in mmake.discover(join(dir_path, "src", "lib"), ".cpp")]
    lib_obj = mmake.change_extension(
        lib_files, 
        join(dir_path, MOONMAKE_DIR, "obj", "lib"), 
        old=".cpp", 
        new=".o"
    )
    lib_static = join(dir_path, MOONMAKE_DIR, "lib", f"lib{PROJECT_NAME}.a")
    # Target files (binaries)
    target_files = [f for f in mmake.discover(join(dir_path, "src", "target"), ".cpp")]
    target_obj = mmake.change_extension(
        target_files, 
        join(dir_path, MOONMAKE_DIR, "obj", "target"), 
        old=".cpp", 
        new=".o"
    )
    target_bin = mmake.change_extension(
        target_files, 
        join(dir_path, MOONMAKE_DIR, "bin"), 
        old=".cpp", 
        new=EXTENSION
    )
    
    # Configure the builder
    builder = mmake.Builder()
    
    # Rule to generate the final binaries
    builder.watch(
        target_bin, 
        target_obj, 
        f"g++ $< -o $@ {COMPILER_FLAGS} {LINK_FLAGS} {STATIC_LIBRARY} -l{PROJECT_NAME}",
        extra_dependencies=[lib_static]  # <-- AGREGAR ESTO
    )
    
    # Rule to compile target object files
    builder.watch(
        target_obj, 
        [join(".", "src", "target", f) for f in target_files],
        f"g++ -c $< -o $@ {COMPILER_FLAGS} {INCLUDE_FLAGS} {IGNORE_FLAGS} {OBJ_FLAGS}",
        dependency_file=True
    )
    
    # Rule to create the static library
    builder.watch(
        [lib_static], 
        lib_obj, 
        "ar rcs $@ $^"
    )
    
    # Rule to compile library object files
    builder.watch(
        lib_obj, 
        [join(".", "src", "lib", f) for f in lib_files],
        f"g++ {COMPILER_FLAGS} {IGNORE_FLAGS} -c $< -o $@ {INCLUDE_FLAGS} {OBJ_FLAGS}",
        dependency_file=True
    )
    
    # Execute all build rules
    builder.compile_all()

if __name__ == "__main__":
    mmake.arguments_cmd(sys.argv, execute, install)

