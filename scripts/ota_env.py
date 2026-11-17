"""Inject OTA upload settings and firmware auth from the untracked project-root .env.

PlatformIO does not load .env files on its own, so this pre-script sets
UPLOAD_PORT / UPLOAD_FLAGS and injects -DNAVIER_OTA_AUTH before the
espressif32 builder runs. The firmware password therefore always matches the
upload password for the kc868a2_ota environment.

The .env file is untracked and must never be committed. See .env.example.
"""

import os
import sys

Import("env")

ENV_NAME = "kc868a2_ota"
REQUIRED_KEYS = ("NAVIER_OTA_IP", "NAVIER_OTA_AUTH")


def _parse_env(path):
    values = {}
    with open(path, "r", encoding="utf-8", errors="replace") as handle:
        for raw_line in handle:
            line = raw_line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, value = line.split("=", 1)
            key = key.strip()
            value = value.strip().rstrip("\r")
            if len(value) >= 2 and value[0] == value[-1] and value[0] in "\"'":
                value = value[1:-1]
            values[key] = value
    return values


def _fail(message):
    sys.stderr.write(message)
    sys.stderr.flush()
    # env.Exit()/sys.exit() is caught by the PlatformIO runner and reported
    # as exit code 1; bypass the wrapper to preserve the documented code 2.
    os._exit(2)


def _ota_auth_define(value):
    # Emit escaped quotes so PlatformIO's flag parser (which strips plain
    # quotes) still hands the compiler a C string literal.
    escaped = value.replace("\\", "\\\\").replace('"', '\\"')
    return '-DNAVIER_OTA_AUTH=\\"%s\\"' % escaped


# This script is referenced only by env:kc868a2_ota; guard defensively so any
# future reuse from another environment is a complete no-op.
env_name = env.subst("$PIOENV") or env.get("ENV_NAME", "")

if env_name == ENV_NAME:
    try:
        from SCons.Script import COMMAND_LINE_TARGETS
    except ImportError:
        COMMAND_LINE_TARGETS = globals().get("COMMAND_LINE_TARGETS", [])
    targets = set(COMMAND_LINE_TARGETS)

    project_dir = env.subst("$PROJECT_DIR")
    env_path = os.path.join(project_dir, ".env")

    values = {}
    if os.path.isfile(env_path):
        values = _parse_env(env_path)

    ota_ip = values.get("NAVIER_OTA_IP", "")
    ota_auth = values.get("NAVIER_OTA_AUTH", "")

    # A double quote or backslash cannot safely round-trip through the
    # -DNAVIER_OTA_AUTH="<value>" define below, so the firmware password
    # would silently differ from the espota upload password. Reject early
    # without echoing the value itself.
    if ota_auth and ('"' in ota_auth or "\\" in ota_auth):
        _fail(
            "\n"
            "NAVIER_OTA_AUTH must not contain double-quote or backslash characters.\n"
            "Such values cannot be injected safely into the firmware define "
            "-DNAVIER_OTA_AUTH, so OTA would upload with a different password "
            "than the device expects.\n"
            "Edit NAVIER_OTA_AUTH in .env (the value itself is never printed).\n"
        )

    if ota_ip:
        env.Replace(UPLOAD_PORT=ota_ip)
    if ota_auth:
        env.Replace(UPLOAD_FLAGS=["--auth=" + ota_auth])
        env.Append(BUILD_FLAGS=[_ota_auth_define(ota_auth)])

    # A plain `pio run -e kc868a2_ota` (no command-line targets) may build
    # without .env: src/defines.h supplies the dev fallback password, so the
    # firmware still compiles. Strict validation applies only when a target is
    # requested (build/upload/etc.), preventing any real build or upload from
    # silently using mismatched credentials.
    if targets:
        missing = [key for key in REQUIRED_KEYS if not values.get(key)]
        if not os.path.isfile(env_path) or missing:
            _fail(
                "\n"
                "OTA upload credentials are missing.\n"
                "Copy .env.example to .env in the project root and set "
                "NAVIER_OTA_IP and NAVIER_OTA_AUTH.\n"
                "The .env file is untracked; never commit it.\n"
            )
    elif not env.subst("$UPLOAD_PORT"):
        # The espressif32 builder warns about a missing upload_port for OTA
        # even on plain builds. Nothing is uploaded here, so fall back to the
        # default esptool protocol to keep plain builds quiet without .env.
        env.Replace(UPLOAD_PROTOCOL="esptool")
