# RTK.md — project usage notes

`rtk` is a CLI output-condensing proxy (v0.50.0, installed via Homebrew at
`/opt/homebrew/bin/rtk`). The global `~/.claude/RTK.md` defines the general
contract: condensed output is complete, truncated output states its recovery
path, and a command whose result is unusable is re-run with `rtk proxy <cmd>`.
This file only adds recipes that are useful in this repository; do not duplicate
the global rules.

## Common commands here

| Command | Use |
| --- | --- |
| `rtk git status` | Compact working-tree state. |
| `rtk git diff` | Condensed diff of uncommitted changes. |
| `rtk git log --oneline -10` | Recent commit history and message style. |
| `rtk grep <pattern>` | Compact repo-wide literal/regex search, grouped by file. |
| `rtk rg <pattern> src` | Native ripgrep scoped to `src/` with the compact filter. |
| `rtk read src/main.cpp` | Filtered read of a source file (drop boilerplate). |
| `rtk find -name "*.h"` | Locate headers while keeping a compact tree. |
| `rtk ls` / `rtk tree` | Inspect `src/` and `data/` layout. |
| `rtk err pio run -e kc868a2` | Build, surfacing only errors/warnings. |
| `rtk err pio run -e kc868a2_dev` | Build the dev environment, errors only. |
| `rtk summary pio run -e kc868a2 -t upload` | USB upload with a short result summary. |
| `rtk summary pio run -e kc868a2 -t uploadfs` | LittleFS upload with a short result summary. |
| `rtk summary pio run -e kc868a2_ota -t upload` | OTA update with a short result summary. |

Example workflow:

```sh
rtk git status
rtk rg "withMQTT" src
rtk read src/water/water_automation.cpp
rtk err pio run -e kc868a2
```

## Migration checks

These searches make the post-migration invariants quick to confirm:

```sh
# Expected: no matches anywhere in src/ or platformio.ini
rtk rg "FastLED|SPIFFS|EEPROM" src platformio.ini

# Remaining dependencies should all be tags or full SHAs
rtk read platformio.ini

# Entity names/topics come from these calls
rtk rg "withMQTT" src

# LittleFS state files
rtk rg "\.bin" src
```

`rtk rg "FastLED|SPIFFS|EEPROM" src platformio.ini` printing nothing is the
expected result for this firmware. If a match appears, the change reintroduced
legacy storage or an addressable-strip dependency and needs review.

## Getting full output

- A line like `[+N hidden: rtk recall <hash>]` means output was elided. Run
  `rtk recall <hash>` to print the omitted part.
- If output is empty when it clearly should not be, contradicts the exit code,
  or is garbled, bypass filtering with `rtk proxy <original cmd>` (tracked) or
  `rtk run <cmd>` (raw, no tracking).

## Repo-specific warnings

- Never wrap interactive sessions: run `pio device monitor -b 115200` plainly,
  without `rtk`, or the live serial stream and exception decoder are unusable.
- Do not hide build/upload failures: `pio run` exit code is the contract, and
  `rtk err` / `rtk summary` must still be checked against it.
- `pio run -e kc868a2 -t uploadfs` erases the LittleFS partition, including
  `/config.bin`, `/water_counter.bin` and `/light_state.bin`. A successful
  compile says nothing about the filesystem image; check the upload exit code
  and expect to reconfigure the device.
- `rtk deps` does not summarize PlatformIO's `lib_deps` here; read
  `platformio.ini` with `rtk read platformio.ini` instead.
- For commands with no dedicated filter (for example unusual `pio` subcommands
  or `pio run -e kc868a2_ota -t upload`), use `rtk run` or `rtk proxy` rather
  than guessing at a filter.
