from distutils.core import setup, Extension
from distutils.command.build_ext import build_ext


class BuildExt(build_ext):
    def get_source_files(self):
        filenames = build_ext.get_source_files(self)
        for ext in self.extensions:
            filenames.extend(ext.depends)
        return filenames


lzcomp = Extension("lzcomp",
                    sources=[
                        "lzcompmodule.c",
                        "ahuff.c",
                        "bitio.c",
                        "lzcomp.c",
                        "mtxmem.c",
                    ],
                    depends=[
                        "ahuff.h",
                        "bitio.h",
                        "config.h",
                        "errcodes.h",
                        "lzcomp.h",
                        "mtxmem.h",
                    ],
                    language="c",
                    )

setup(
    name="LZComp",
    version="0.1",
    url="https://www.daltonmaag.com/",
    description="Python binding of the LZCOMP compression algorithm",
    author="Cosimo Lupo",
    author_email="cosimo.lupo@daltonmaag.com",
    license="Apache 2.0",
    ext_modules=[lzcomp],
    cmdclass={
        'build_ext': BuildExt,
        },
)
