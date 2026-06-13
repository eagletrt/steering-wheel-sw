import hashlib
import logging
import os
import re
import sys
from pathlib import Path

from SCons.Script import Import

Import("env")

JSON_PATH = Path("tools/fonts.json")
HASH_PATH = Path("tools/.fonts.json.sha256")
OUTPUT_DIR = Path("tools/generated")


def patch_extended_ascii_literals(path: Path) -> None:
    """Rewrite single-quote literals containing a non-ASCII codepoint into
    `'\\xNN'` escapes.

    libraster's glyph-key column is `char character` and the docs describe
    it as extended ASCII — i.e. a single byte, Latin-1 style. The upstream
    generator emits the source character verbatim, which works for ASCII
    but produces a multi-byte UTF-8 sequence inside `'...'` for things like
    `°` and clang rejects that as a multi-character literal. Rewriting the
    literal to `'\\xB0'` makes the byte value explicit and is exactly what
    the lookup table needs.
    """

    text = path.read_text(encoding="utf-8")

    def repl(match: "re.Match[str]") -> str:
        ch = match.group(1)
        codepoint = ord(ch)
        if codepoint <= 0x7F:
            return match.group(0)
        if codepoint > 0xFF:
            raise ValueError(
                f"Character {ch!r} (U+{codepoint:04X}) in {path} is outside "
                "the extended-ASCII range libraster glyph keys can hold")
        return f"'\\x{codepoint:02X}'"

    patched = re.sub(r"'([^\\'])'", repl, text)
    if patched != text:
        path.write_text(patched, encoding="utf-8")


logging.basicConfig(level=logging.INFO, format="[libraster] %(message)s")
logger = logging.getLogger("libraster")


def hash_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    if not JSON_PATH.exists():
        logger.warning("fonts.json not found at %s", JSON_PATH)
        return 1

    try:
        generator = next(Path(".pio").rglob("generator.py"))
    except StopIteration:
        logger.warning("libraster generator.py not found under .pio/")
        return 1

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    current_hash = hash_file(JSON_PATH)
    output_files_present = (
        OUTPUT_DIR / "raster-fonts.c").exists() and (OUTPUT_DIR / "raster-fonts.h").exists()
    cache_valid = HASH_PATH.exists() and HASH_PATH.read_text() == current_hash

    if cache_valid and output_files_present:
        logger.info("fonts.json unchanged, skipping")
        return 0

    logger.info("regenerating fonts into %s", OUTPUT_DIR)
    cmd = f"{sys.executable} {generator} --json {JSON_PATH} --output {OUTPUT_DIR}"
    if os.system(cmd) != 0:
        logger.error("font generator failed")
        return 1

    # Upstream generator emits raw UTF-8 for non-ASCII glyph keys; force them
    # into `'\xNN'` escapes so the C compiler accepts them.
    patch_extended_ascii_literals(OUTPUT_DIR / "raster-fonts.c")

    HASH_PATH.write_text(current_hash)
    return 0


if main() != 0:
    sys.exit(1)
