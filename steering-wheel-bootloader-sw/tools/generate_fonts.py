import os
import hashlib
import sys
from pathlib import Path
from SCons.Script import Import
import logging

Import("env")

json_path = Path("tools/fonts.json")
hash_path = Path("tools/.fonts.json.sha256")
libgen = next(Path(".pio").rglob("generator.py"))


def hash_file(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


logger = logging.getLogger("libraster")
logging.basicConfig(encoding='utf-8', level=logging.INFO)

if not json_path.exists():
    logger.warning("[libraster] fonts.json not found")
    exit(1)
elif not hash_path.exists() or hash_file(json_path) != hash_path.read_text():
    logger.info("[libraster] generating")
    os.system(f"{sys.executable} {libgen} --json {json_path}")
    hash_path.write_text(hash_file(json_path))
else:
    logger.warning("[libraster] fonts.json unchanged, skipping")
