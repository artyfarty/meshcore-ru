"""PlatformIO pre-build script: replace Adafruit GFX glcdfont.c with Cyrillic version."""
import glob
import os
import shutil

Import("env")

project_dir = env["PROJECT_DIR"]
source = os.path.join(project_dir, "src", "helpers", "ui", "glcdfont_ru.c")

if not os.path.exists(source):
    print("WARNING: glcdfont_ru.c not found")
else:
    pattern = os.path.join(project_dir, ".pio", "libdeps", "**", "glcdfont.c")
    for target in glob.glob(pattern, recursive=True):
        if "Adafruit" in target:
            shutil.copy2(source, target)
            print(f"Patched {target} with Cyrillic font")
