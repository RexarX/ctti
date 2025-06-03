"""
CTTI Symbol Generator

Invoke as follows:
    python inception.py path/to/project/sources [libclang.so full path]

This script generates symbol definitions for the CTTI library by parsing
C++ source files using libclang.
"""

from clang.cindex import Index, Cursor, CursorKind, Config
import os
import sys
import re
import fnmatch
from pathlib import Path
from typing import Dict, List, Set, Optional

class SymbolGenerator:
    """Generate CTTI symbol definitions from C++ source code."""

    def __init__(self, project_dir: str, libclang_path: Optional[str] = None):
        self.project_dir = Path(project_dir).resolve()
        self.symbols: Dict[str, dict] = {}
        self.processed_files: Set[str] = set()

        if libclang_path:
            Config.set_library_file(libclang_path)

        self.index = Index.create()

        # Valid cursor kinds for symbol generation
        self.valid_kinds = {
            CursorKind.NAMESPACE,
            CursorKind.CLASS_DECL,
            CursorKind.STRUCT_DECL,
            CursorKind.UNION_DECL,
            CursorKind.ENUM_DECL,
            CursorKind.FIELD_DECL,
            CursorKind.TYPE_ALIAS_DECL,
            CursorKind.NAMESPACE_ALIAS,
            CursorKind.CLASS_TEMPLATE,
            CursorKind.CXX_METHOD,
            CursorKind.FUNCTION_DECL,
            CursorKind.ENUM_CONSTANT_DECL,
            CursorKind.VAR_DECL,
            CursorKind.PARM_DECL
        }

        # Names to ignore during symbol generation
        self.invalid_names = {
            "", "static_assert", "std", "hash", "get_member", "value_type",
            "member_type", "__builtin_va_list"
        }

        # Add operator names to ignore
        operators = [
            "operator()", "operator+", "operator++", "operator+=", "operator-",
            "operator--", "operator-=", "operator<", "operator<=", "operator<<",
            "operator<<=", "operator>", "operator>=", "operator>>", "operator>>=",
            "operator=", "operator*=", "operator->", "operator*", "operator==",
            "operator!=", "operator~", "operator^", "operator^=", "operator%",
            "operator%=", "operator|", "operator|=", "operator||", "operator&",
            "operator&=", "operator&&", "operator[]", "operator/", "operator/=",
            "operator!", "operator!="
        ]
        self.invalid_names.update(operators)

        # Add mock method names
        mock_methods = [f"MOCK_METHOD{i}" for i in range(11)]
        mock_const_methods = [f"MOCK_CONST_METHOD{i}" for i in range(11)]
        self.invalid_names.update(mock_methods + mock_const_methods)

    def parse_cursor(self, cursor: Cursor) -> dict:
        """Parse a cursor into symbol information."""
        return {
            'file': str(cursor.location.file) if cursor.location.file else "",
            'line': cursor.location.line,
            'spelling': cursor.spelling,
            'kind': str(cursor.kind),
            'cursor': cursor,
            'qualified_name': self._get_qualified_name(cursor)
        }

    def _get_qualified_name(self, cursor: Cursor) -> str:
        """Get the fully qualified name of a cursor."""
        names = []
        current = cursor

        while current and current.kind != CursorKind.TRANSLATION_UNIT:
            if current.spelling:
                names.append(current.spelling)
            current = current.semantic_parent

        return "::".join(reversed(names))

    def _is_in_project(self, file_path: str) -> bool:
        """Check if a file is within the project directory."""
        if not file_path:
            return False

        try:
            abs_file = Path(file_path).resolve()
            return self.project_dir in abs_file.parents or abs_file == self.project_dir
        except (OSError, ValueError):
            return False

    def _should_process_cursor(self, cursor: Cursor) -> bool:
        """Determine if a cursor should be processed for symbol generation."""
        if cursor.spelling in self.invalid_names:
            return False

        if cursor.kind not in self.valid_kinds:
            return False

        if cursor.spelling in self.symbols:
            return False

        if not self._is_in_project(str(cursor.location.file) if cursor.location.file else ""):
            return False

        # Skip private/protected members in some cases
        if cursor.access_specifier and cursor.access_specifier.name in ['PRIVATE', 'PROTECTED']:
            return False

        return True

    def visit_cursor(self, cursor: Cursor) -> None:
        """Recursively visit cursors to find symbols."""
        if self._should_process_cursor(cursor):
            print(f"{len(self.symbols)}# {cursor.location.file}:{cursor.location.line}: "
                  f"{cursor.spelling} (Kind={cursor.kind})")
            self.symbols[cursor.spelling] = self.parse_cursor(cursor)

        for child in cursor.get_children():
            self.visit_cursor(child)

    def generate_symbols_header(self, output_file: str = "symbols.hpp") -> None:
        """Generate the symbols header file."""
        header_content = self._generate_header_content()

        with open(output_file, 'w', encoding='utf-8') as file:
            file.write(header_content)

        print(f"\nGenerated {len(self.symbols)} symbols in {output_file}")

    def _generate_header_content(self) -> str:
        """Generate the content for the symbols header file."""
        header = '''/*
 * Generated from sources
 * Symbolic introspection the lazy way
 *
 * This file was automatically generated by inception.py
 * Do not edit manually.
 */

#ifndef CTTI_SYMBOLS_HPP
#define CTTI_SYMBOLS_HPP

#include <ctti/symbol.hpp>

'''

        # Generate symbol definitions
        for symbol_name, element in sorted(self.symbols.items()):
            header += f'''#ifndef CTTI_SYMBOLS_{symbol_name.upper()}_DEFINED
#define CTTI_SYMBOLS_{symbol_name.upper()}_DEFINED
CTTI_DEFINE_SYMBOL({symbol_name}); // from {element['file']}:{element['line']} ({element['kind']})
#endif // CTTI_SYMBOLS_{symbol_name.upper()}_DEFINED

'''

        header += '''
#endif // CTTI_SYMBOLS_HPP'''

        return header

    def parse_project(self) -> None:
        """Parse the entire project for symbols."""
        print(f'Project include dir: {self.project_dir}')

        patterns = ['*.h', '*.hpp', '*.hxx', '*.c', '*.cpp', '*.cxx']
        exclude_patterns = ['*.pb.h', '*.pb.cc', '*test*', '*Test*', '*TEST*']

        processed_files = set()

        for pattern in patterns:
            for file_path in self.project_dir.rglob(pattern):
                # Skip if matches exclude patterns
                if any(fnmatch.fnmatch(file_path.name, exclude) for exclude in exclude_patterns):
                    continue

                if str(file_path) in processed_files:
                    continue

                processed_files.add(str(file_path))

                try:
                    print(f"Parsing: {file_path}")
                    translation_unit = self.index.parse(str(file_path))

                    if translation_unit.diagnostics:
                        for diag in translation_unit.diagnostics:
                            if diag.severity >= 3:  # Error or Fatal
                                print(f"Warning: {diag.spelling} in {file_path}")

                    self.visit_cursor(translation_unit.cursor)

                except Exception as e:
                    print(f"Error parsing {file_path}: {e}")
                    continue

def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: python inception.py <project_sources_path> [libclang_path]")
        sys.exit(1)

    project_dir = sys.argv[1]
    libclang_path = sys.argv[2] if len(sys.argv) > 2 else None

    if not os.path.exists(project_dir):
        print(f"Error: Project directory '{project_dir}' does not exist")
        sys.exit(1)

    try:
        generator = SymbolGenerator(project_dir, libclang_path)
        generator.parse_project()
        generator.generate_symbols_header()

        print(f"\nSuccessfully generated symbols for {len(generator.symbols)} elements")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
